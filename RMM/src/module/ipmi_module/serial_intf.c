/**
 * Copyright (c)  2015-2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <termios.h>

#include "libcfg/cfg.h"
#include "event.h"
#include "ipmi.h"
#include "ipmi_log.h"
#include "libsecurec/safe_lib.h"



/**
 * Parity: N/n - None; O/o - Odd; E/e - Even.
 * Flow_control: 0 - None; 1 - XON/XOFF; 2 - RTS/CTS; 3 - DSR/DTR.
 * By default, baudrate = 115200, databits = 8, stopbits = 1, parity = 'N', flow_control = 0
 */
#define PARITY_NONE			'N'
#define PARITY_ODD			'O'
#define PARITY_EVEN			'E'

#define FLOW_CONTROL_NONE		'0'
#define FLOW_CONTROL_XON_XOFF	'1'
#define FLOW_CONTROL_RTS_CTS	'2'
#define FLOW_CONTROL_DSR_DTR	'3'

#define DATABITS_7			'7'
#define DATABITS_8			'8'

#define STOPBITS_1			'1'
#define STOPBITS_2			'2'

#define DEFAULT_BAUDRATE		115200
#define DEFAULT_DATABITS		DATABITS_8
#define DEFAULT_PARITY			PARITY_NONE
#define DEFAULT_STOPBITS		STOPBITS_1
#define DEFAULT_FLOW_CONTROL	FLOW_CONTROL_NONE

#define SERIAL_MSG_QUEUE_NUM		256

struct serial_recv_msg {
	struct serial_recv_msg *next;	/* free buffer link */

	int len;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];
};

struct serial_msg_head {
	struct serial_recv_msg  *head;
	struct serial_recv_msg **tail;
};

static struct fd_event serial_fd_event[MAX_SERIAL_PORT];

static struct serial_msg_head  serialmsg_head[MAX_SERIAL_PORT];
static struct serial_recv_msg *serialmsg_freelist[MAX_SERIAL_PORT];
static pthread_mutex_t serialmsg_mutex[MAX_SERIAL_PORT];
static pthread_cond_t  serialmsg_cond[MAX_SERIAL_PORT];

static struct _rates {
	int speed;
	int name;
} rates[] = {
	{B300,    300},
	{B1200,   1200},
	{B2400,   2400},
	{B4800,   4800},
	{B9600,   9600},
	{B19200,  19200},
	{B38400,  38400},
	{B115200, 115200},
	{B230400, 230400},
	{B460800, 460800}
};

/**
  *  @brief open the serial port
  *
  *  @param[in] dev serial interface device name
  *  @return serial file fd
  */
static int serial_open(char *dev)
{
	int fd = -1;
	int val = 0;

	/* Here, we make it work with NONBLOCK flag, and sometimes we need to
	 * read several times to get the complete message */
	fd = open(dev, O_RDWR | O_NOCTTY, 0);
	if (fd < 0) {
		perror("serial_open");
		return -1;
	}

	val = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}


/**
  *  @brief This function is used to set the attributes for the serial port.
  *
  *  @param[in] fd serial device file fd
  *  @param[in] rate bit rate, e.g., 9600bps
  *  @param[in] databits data bits, e.g., 7-bits or 8 bits
  *  @param[in] stopbits stop bit, 0 or 1 bit
  *  @param[in] parity N/n - None; O/o - Odd; E/e - Even
  *  @param[in] flow_control NONE, HW or SW flow control
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int serial_set_attr(int fd, unsigned int rate, unsigned char databits,
						   unsigned char stopbits, unsigned char parity,
						   unsigned char flow_control)
{
	struct termios ti = {0};
	int i = 0;

	tcgetattr(fd, &ti);

	for (i = 0; i < sizeof(rates) / sizeof(struct _rates); i++) {
		if (rates[i].name == rate) {
			cfsetispeed(&ti, rates[i].speed);
			cfsetospeed(&ti, rates[i].speed);
			break;
		}
	}

	ti.c_cflag &= ~CSIZE;
	switch (databits) {
	case DATABITS_7:
		ti.c_cflag |= CS7;
		break;
	case DATABITS_8:
		ti.c_cflag |= CS8;
		break;
	default:
		return -1;
	}

	switch (parity) {
	case 'n':
	case 'N':
		ti.c_cflag &= ~PARENB;
		ti.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O':
		ti.c_cflag |= (PARODD | PARENB);
		ti.c_iflag |= INPCK;
		break;
	case 'e':
	case 'E':
		ti.c_cflag |= PARENB;
		ti.c_cflag &= ~PARODD;
		ti.c_iflag |= INPCK;
		break;
	default:
		return -1;
	}

	switch (stopbits) {
	case STOPBITS_1:
		ti.c_cflag &= ~CSTOPB;
		break;
	case STOPBITS_2:
		ti.c_cflag |= CSTOPB;
		break;
	default:
		return -1;
	}

	switch (flow_control) {
	case FLOW_CONTROL_NONE:
		ti.c_cflag &= ~CRTSCTS;
		ti.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case FLOW_CONTROL_RTS_CTS:
		ti.c_cflag |= CRTSCTS;
		ti.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case FLOW_CONTROL_XON_XOFF:
		ti.c_cflag &= ~CRTSCTS;
		ti.c_iflag |= (IXON | IXOFF | IXANY);
		break;
	default:
		break;
	}

	ti.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | INPCK | ISTRIP);
	ti.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	ti.c_oflag &= ~OPOST;

	tcsetattr(fd, TCSAFLUSH, &ti);

	return 0;
} /* End of serial_set_attr(...) */

/**
  *  @brief close serial port
  *
  *  @param[in] fd serial file fd
  *  @return
  */
static int serial_close(int fd)
{
	return close(fd);
}


/**
  *  @brief open the serial port, and read the configuration from the config file "rmm.cfg"
  *
  *  @param[in] dev_name serial device name
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int open_serial_interface(char *dev_name)
{
	int fd = 1;
	int rc = -1;
	unsigned int rate = DEFAULT_BAUDRATE;
	unsigned char databits = DEFAULT_DATABITS;
	unsigned char stopbits = DEFAULT_STOPBITS;
	unsigned char parity = DEFAULT_PARITY;
	unsigned char flow_control = DEFAULT_FLOW_CONTROL;

	/* Load the serial parameters from the config file "ipmi.ini",
	 * if some parameters doesn't be marked in the config file,
	 * use the default values */

	rate = rmm_cfg_get_baudrate();
	databits = rmm_cfg_get_databits();
	stopbits = rmm_cfg_get_stopbits();
	parity = rmm_cfg_get_parity();
	flow_control = rmm_cfg_get_flow_control();

	fd = serial_open(dev_name);
	if (fd < 0) {
		rmm_log(ERROR, "serial open failed\n");
		return -1;
	}
	rmm_log(DBG, "baude rate is %d,  databits is %d, stopbits is %d, parity is %d, flow is %d\n",
		rate, databits, stopbits, parity, flow_control);

	/* Need to set the serial port with the above attributes reading from
	   the config file */
	rc = serial_set_attr(fd, rate, databits, stopbits, parity, flow_control);
	if (rc < 0) {
		rmm_log(ERROR, "serial_set_attr failed\n");
		serial_close(fd);
		return -1;
	}

	return fd;
}

/**
  *  @brief send serial outbound message
  *
  *  @param[in] fd serial file fd
  *  @param[in] msg
  *  @param[in] len msg length
  *  @return
  */
void serial_send_outbound_msg(int fd, unsigned char *msg, int len)
{
#ifdef DEBUG_IPMI
	{
		int __i;
		char __buff[1536];
		char __hdr[64];
		char __body[1280] = {0};
		char __end[64];
		char __tmp[8];

		snprintf(__hdr, sizeof(__hdr), "SERIAL OUT: ");
		for (__i = 0; __i < len; __i++) {
			snprintf_s_i(__tmp, sizeof(__tmp), "%02X ", msg[__i]);
			strncat_s(__body, sizeof(__body), __tmp, 8);
		}
		snprintf(__end, sizeof(__end), "\n");
		snprintf_s_sss(__buff, sizeof(__buff), "%s%s%s", __hdr, __body, __end);

		IPMI_LOG_DEBUG("%s\n", __buff);
	}
#endif
	int cur_serial_port = 0;

	cur_serial_port = get_spec_serial_port_by_fd(fd);
	if (-1 == cur_serial_port)
		return;

	if (write(fd, msg, len) < 0)
		printf("write fail\n");
}

/**
  *  @brief receive message from serial interface
  *
  *  @param[in] fd serial interface file fd
  *  @return
  */
static void serial_recv_inbound_msg(int fd)
{
	int rc = -1;
	int offset = 0;
	struct serial_recv_msg *msg = NULL;
	int cur_serial_port = 0;

	cur_serial_port = get_spec_serial_port_by_fd(fd);
	if (-1 == cur_serial_port)
		return;

	pthread_mutex_lock(&(serialmsg_mutex[cur_serial_port]));

	while (1) {
		msg = serialmsg_freelist[cur_serial_port];
		if (msg == NULL) {
			IPMI_LOG_ERR("SERIAL-Intf: No Recv Buff, adjust #SERIAL_MSG_QUEUE_NUM(%d)!\n",
						 SERIAL_MSG_QUEUE_NUM);
			break;
		}

		/* Be careful here, we need to read several times to get the complete data. */
		offset = 0;
		while (1) {
			rc = read(fd, msg->msg + offset, sizeof(msg->msg) - offset);
			if (rc > 0) {
				offset += rc;
				usleep(1000);
			} else
				break;
		}

		if (0 == offset)
			break;

		msg->len = offset;
		serialmsg_freelist[cur_serial_port] = msg->next;
		msg->next = NULL;
		*(serialmsg_head[cur_serial_port].tail) = msg;
		serialmsg_head[cur_serial_port].tail = &(msg->next);

		#ifdef DEBUG_IPMI
		{
			int __i;
			char __buff[8 * IPMI_MAX_MSG_LENGTH + 256] = {0};
			char __hdr[64] = {0};
			char __body[8 * IPMI_MAX_MSG_LENGTH] = {0};
			char __end[64] = {0};
			char __tmp[8] = {0};

			snprintf(__hdr, sizeof(__hdr), "SERIAL IN: ");
			for (__i = 0; __i < msg->len; __i++) {
				snprintf_s_i(__tmp, sizeof(__tmp), "%02X ", msg->msg[__i]);
				strncat_s(__body, sizeof(__body), __tmp, 8);
			}
			snprintf(__end, sizeof(__end), "\n");
			snprintf_s_sss(__buff, sizeof(__buff), "%s%s%s", __hdr, __body, __end);

			IPMI_LOG_DEBUG("%s\n", __buff);
		}
		#endif

	}

	if (serialmsg_head[cur_serial_port].head != NULL)
		pthread_cond_signal(&(serialmsg_cond[cur_serial_port]));

	pthread_mutex_unlock(&(serialmsg_mutex[cur_serial_port]));
}


/**
  *  @brief The thread for receiving the message from the serial port sent by BMC
  *
  *  @param
  *  @return
  */
static void *serial_intf_thread(void *argv)
{
	struct serial_recv_msg *list;
	struct serial_msg_head freelist;
	int cur_serial_port = 0;

	prctl(PR_SET_NAME, "serial_intf_thread");

	memcpy_s(&cur_serial_port, sizeof(cur_serial_port), (char *)argv, sizeof(int));

	/* Don't forget free it. */
	free(argv);
	argv = NULL;

	freelist.head = NULL;

	while (1) {
		pthread_mutex_lock(&(serialmsg_mutex[cur_serial_port]));

		if (freelist.head != NULL) {
			*(freelist.tail) = serialmsg_freelist[cur_serial_port];
			serialmsg_freelist[cur_serial_port] = freelist.head;

			freelist.head = NULL;
		}

		while (serialmsg_head[cur_serial_port].head == NULL) {
			pthread_cond_wait(&(serialmsg_cond[cur_serial_port]), &(serialmsg_mutex[cur_serial_port]));
		}

		freelist.head = serialmsg_head[cur_serial_port].head;
		freelist.tail = serialmsg_head[cur_serial_port].tail;
		serialmsg_head[cur_serial_port].head = NULL;
		serialmsg_head[cur_serial_port].tail = &(serialmsg_head[cur_serial_port].head);

		pthread_mutex_unlock(&(serialmsg_mutex[cur_serial_port]));

		list = freelist.head;
		while (list != NULL) {
			if (IPMI_MAX_MSG_LENGTH >= list->len)
				handle_serial_msg(cur_serial_port, list->msg, list->len);

			list = list->next;
		}
	} /* End of while (1) */

	serial_close(serial_fd_event[cur_serial_port].fd);

	return NULL;
}

/**
  *  @brief
  *
  *  @param[in] dev serial device name
  *  @param[in] netfn IPMI command net function code
  *  @param[in] cmd IPMI command
  *  @param[in] msgid msg id
  *  @param[in] user_port user application socket post
  *  @return
  */
void start_serial_intf(char *dev, unsigned char netfn, unsigned char cmd,
					   int msgid, unsigned short user_port, ipmi_json_ipc_header_t header)
{
	pthread_t tid;
	int cur_serial_port;
	int *tmp_cur = NULL;
	int rc = -1;
	int fd = -1;
	struct stat sb;

	if (dev == NULL) {
		IPMI_LOG_ERR("Request device is invalid\n");
		return;
	}

	if (stat(dev, &sb) != 0) {
		IPMI_LOG_ERR("stat for %s wrong\n", dev);
		send_back_result_to_app(fd, netfn, cmd, msgid, user_port, header);
		return;
	}

	rc = check_serial_port_by_dev(dev, &cur_serial_port);
	if (SERIAL_PORT_IS_OPENED == rc) {
		if (serial_port_is_updated(cur_serial_port, (long)sb.st_ino)) {
			serial_close(serial_fd_event[cur_serial_port].fd);
			serial_fd_event[cur_serial_port].fd = open_serial_interface(dev);
			serial_port_update(cur_serial_port,
							   serial_fd_event[cur_serial_port].fd,
							   (long)sb.st_ino);
		}
		IPMI_LOG_DEBUG("This serial port has been opened!\n");
		send_back_result_to_app(serial_fd_event[cur_serial_port].fd,
								netfn, cmd, msgid, user_port, header);
		return;
	} else if (SERIAL_PORT_IS_CLOSED == rc) {
		IPMI_LOG_DEBUG("This serial port is exist but closed!\n");
		serial_fd_event[cur_serial_port].fd = open_serial_interface(dev);
		if (serial_fd_event[cur_serial_port].fd == -1) {
			IPMI_LOG_ERR("Failed to open %s\n", dev);
		}

		set_serial_port_used(cur_serial_port,
							 serial_fd_event[cur_serial_port].fd,
							 dev, tid, (long)sb.st_ino);

		send_back_result_to_app(serial_fd_event[cur_serial_port].fd,
								netfn, cmd, msgid, user_port, header);

		IPMI_LOG_DEBUG("SERIAL-Intf is started successfully!\n");
		return;
	} else if (SERIAL_PORT_WAITING == rc) {
		IPMI_LOG_DEBUG("Need to wait...\n");
		send_back_result_to_app(-1, netfn, cmd, msgid, user_port, header);
		return;
	}

	cur_serial_port = get_unused_serial_port();
	if (cur_serial_port == -1) {
		IPMI_LOG_ERR("No more space for serial port\n");

		send_back_result_to_app(fd, netfn, cmd, msgid, user_port, header);
		return;
	}

	pthread_mutex_init(&(serialmsg_mutex[cur_serial_port]), NULL);
	pthread_cond_init(&(serialmsg_cond[cur_serial_port]), NULL);

	init_serial_msg_handler(cur_serial_port);

	serialmsg_head[cur_serial_port].head = NULL;
	serialmsg_head[cur_serial_port].tail = &(serialmsg_head[cur_serial_port].head);

	serialmsg_freelist[cur_serial_port] = mem_freelist_create(SERIAL_MSG_QUEUE_NUM,
															  sizeof(struct serial_recv_msg));
	if (serialmsg_freelist[cur_serial_port] == NULL) {
		send_back_result_to_app(fd, netfn, cmd, msgid, user_port, header);
		return;
	}

	IPMI_LOG_DEBUG("Dev Name: %s\n", dev);
	serial_fd_event[cur_serial_port].fd = open_serial_interface(dev);
	if (serial_fd_event[cur_serial_port].fd == -1) {
		free(serialmsg_freelist[cur_serial_port]);
		rmm_log(ERROR, "open serial interface failed\n");
		send_back_result_to_app(fd, netfn, cmd, msgid, user_port, header);
		return;
	}

	serial_fd_event[cur_serial_port].handle_fd = serial_recv_inbound_msg;
	fd_event_add(&(serial_fd_event[cur_serial_port]));

	tmp_cur = (int *)malloc(sizeof(int));
	if (NULL == tmp_cur)
		return;

	*tmp_cur = cur_serial_port;

	if (pthread_create(&tid, NULL, serial_intf_thread, tmp_cur) != 0) {
		if (NULL != tmp_cur) {
			free(tmp_cur);
			tmp_cur = NULL;
		}

		serial_close(serial_fd_event[cur_serial_port].fd);
		serial_fd_event[cur_serial_port].fd = -1;
		free(serialmsg_freelist[cur_serial_port]);
		send_back_result_to_app(fd, netfn, cmd, msgid, user_port, header);

		IPMI_LOG_ERR("Failed to create serial_intf_thread.\n");
		return;
	}

	set_serial_port_used(cur_serial_port,
						 serial_fd_event[cur_serial_port].fd,
						 dev, tid, (long)sb.st_ino);

	IPMI_LOG_DEBUG("SERIAL-Intf is started successfully!\n");

	send_back_result_to_app(serial_fd_event[cur_serial_port].fd,
							netfn, cmd, msgid, user_port, header);
}

/**
  *  @brief stop serial interface
  *
  *  @param[in] dev serial device name
  *  @param[in] netfn IPMI command net function code
  *  @param[in] cmd IPMI command
  *  @param[in] msgid msg id
  *  @param[in] user_port user application socket post
  *  @return
  */
void stop_serial_intf(char *dev, unsigned char netfn, unsigned char cmd,
					  int msgid, unsigned short user_port, ipmi_json_ipc_header_t header)
{
	int cur_serial_port = 0;
	int rc = -1;

	/* Currently, need to stop the serial when do the upgradation for
	 * the CM firmware, and it need to reboot the CM to make sure it
	 * be upgraded successully; sometimes, the CM won't reboot itself,
	 * so we need to reboot via serial console, wait a few seconds for
	 * the "reboot" process. */
	set_serial_port_timeout_by_dev(dev, 75);

	rc = check_serial_port_by_dev(dev, &cur_serial_port);
	if ((SERIAL_PORT_IS_CLOSED == rc) || (rc < 0)) {
		IPMI_LOG_DEBUG("This serial port has not been opened!\n");

		send_back_result_to_app(-1, netfn, cmd, msgid, user_port, header);
		return;
	}

	serial_close(serial_fd_event[cur_serial_port].fd);

	send_back_result_to_app(serial_fd_event[cur_serial_port].fd,
							netfn, cmd, msgid, user_port, header);

	serial_fd_event[cur_serial_port].fd = -1;
	set_serial_port_unused(cur_serial_port);

	IPMI_LOG_DEBUG("SERIAL-Intf is stopped successfully!\n");
}
