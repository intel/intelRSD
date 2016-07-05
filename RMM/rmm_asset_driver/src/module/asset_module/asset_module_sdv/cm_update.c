/**
 * Copyright (c)  2015, Intel Corporation.
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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <termios.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

#include "libutils/rmm.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libutils/ip.h"
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"
#include "action.h"
#include "general.h"


struct _fm_update_info {
	unsigned int host;
	char dev[32];
	char file[128];
};



enum {
	CM_FW_UPDATE_UNKNOW = 0,
	CM_FW_UPDATE_SUCCESS,
	CM_FW_UPDATE_FAIL,
	CM_FW_UPDATE_ONGOING
};

static int var_firmware_update_status;

void cm_set_firmware_update_status(int status)
{
	var_firmware_update_status = status;
}

int cm_get_firmware_update_status(void)
{
	return var_firmware_update_status;
}

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
 * @brief: Do not block the serial
 */
static int serial_open(char *dev)
{
	int fd = -1;
	int val = 0;

	/* Here, we make it work with BLOCK flag, sometimes we need to
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
 * @brief: This function is used to set the attributes for the serial port.
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

static int serial_close(int fd)
{
	return close(fd);
}

/**
*	Work around after upgrade cm, need reboot the CM before CM v0.9
**/
static int upgraded_serial_inf_close_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;
	struct _fm_update_info *mbp_update = (struct _fm_update_info *)cb_data;
	struct stat sb;
	int fd = -1;
	char file[128] = {0};
	char prefix[64] = {0};
	int ret = -1;
	char *search = NULL;

	printf("SERIAL CLOSE RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	printf("update dev name is %s\n", mbp_update->dev);

	for (i = 0; i < 45; i++) {
		if (stat(mbp_update->dev, &sb) != 0) {
			if (errno == ENOENT) {
				cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
				printf("upgraded_serial_inf_close_rsp_cb failed.\n");
				return 0;
			}
		}

		sleep(1);
	}

	search = strstr(mbp_update->dev, "IPMI");
	if (NULL == search) {
		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		printf("upgraded_serial_inf_close_rsp_cb search failed.\n");
		return -1;
	}

	strncpy_safe(prefix, mbp_update->dev, sizeof(prefix), search - mbp_update->dev);

	snprintf_s_s(file, sizeof(file), "%sConsole", prefix);

	fd = serial_open(file);
	if (fd < 0) {
		printf("serial_open failed: %s\n", file);
		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		return -1;
	}

	unsigned int rate = DEFAULT_BAUDRATE;
	unsigned char databits = DEFAULT_DATABITS;
	unsigned char stopbits = DEFAULT_STOPBITS;
	unsigned char parity = DEFAULT_PARITY;
	unsigned char flow_control = DEFAULT_FLOW_CONTROL;

	ret = serial_set_attr(fd, rate, databits, stopbits, parity, flow_control);
	if (ret < 0) {
		printf("serial_set_attr failed\n");
		serial_close(fd);
		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		return -1;
	}

	ret = write(fd, "reboot\r", 7);

	serial_close(fd);
	cm_set_firmware_update_status(CM_FW_UPDATE_SUCCESS);

	return 0;
}


static int ipmi_upgrade_close_rsp_handler(int result, unsigned char *rsp, int rsp_len,
										  void *cb_data)
{
	struct _fm_update_info *mbp_update = (struct _fm_update_info *)cb_data;

	if (rsp[0] == IPMI_CC_OK) {
		unlink(mbp_update->file);

		struct jipmi_serial_msg req = {};


		memcpy_s(req.msg_base.data, sizeof(req.msg_base.data), mbp_update->dev, strnlen_s(mbp_update->dev, sizeof(mbp_update->dev)-1));


		FILL_INT(req.msg_base.netfn,		OEM_IPMI_NETFN);
		FILL_INT(req.msg_base.cmd,		OEM_SERIAL_CLOSE_CMD);
		FILL_INT(req.msg_base.data_len,	strnlen_s(mbp_update->dev, sizeof(mbp_update->dev)-1));
		req.serial_flag = IPMI_SERIAL_CLOSE_DEV;

		libjipmi_serial_cmd(&req, upgraded_serial_inf_close_rsp_cb, mbp_update, JIPMI_NON_SYNC);

		return 0;
	} else {
		unlink(mbp_update->file);
		free(mbp_update);
		
		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		printf("ipmi_upgrade_close_rsp_handler, rsp failed.\n");
		return -1;
	}

	return 0;
}

static int ipmi_upgrade_start_rsp_handler(int result, unsigned char *rsp, int rsp_len,
										  void *cb_data)
{
	struct _fm_update_info *mbp_update = (struct _fm_update_info *)cb_data;
	struct jipmi_msg ipmi_req = {};

	if (rsp[0] == IPMI_CC_OK) {
		FILL_INT(ipmi_req.netfn,	IPMI_RMCP_CLOSE_SESSION_NETFN);
		FILL_INT(ipmi_req.cmd,		IPMI_RMCP_CLOSE_SESSION_CMD);
		FILL_STR(ipmi_req.name,		am_rmcp_username);
		FILL_STR(ipmi_req.password,	am_rmcp_password);
		FILL_INT(ipmi_req.data_len,	0);
		libjipmi_rmcp_close_session(mbp_update->host, IPMI_RMCP_PORT, &ipmi_req,
								   ipmi_upgrade_close_rsp_handler, mbp_update, JIPMI_NON_SYNC);

		return 0;
	} else {
		unlink(mbp_update->file);
		free(mbp_update);

		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		printf("ipmi_upgrade_start_rsp_handler, rsp failed, rsp is %x.\n", rsp[0]);
		return -1;
	}

	return 0;
}

static int ipmi_upgrade_file_rsp_handler(int result, unsigned char *rsp, int rsp_len,
										 void *cb_data)
{
	struct _fm_update_info *mbp_update = (struct _fm_update_info *)cb_data;

	if (rsp[0] == IPMI_CC_OK) {
		struct jipmi_msg req = {};
		/* Step3: Start Upgrading */
		memset((char *)&req, 0, sizeof(req));


		FILL_INT(req.netfn,		UPDATE_FIRMWARE_NETFN);
		FILL_INT(req.cmd,		UPDATE_FM_START_CMD);
		FILL_STR(req.name,		am_rmcp_username);
		FILL_STR(req.password,	am_rmcp_password);
		FILL_INT(req.data_len,	0x00);

		libjipmi_rmcp_cmd(mbp_update->host, IPMI_RMCP_PORT, &req,
						 ipmi_upgrade_start_rsp_handler, mbp_update, JIPMI_NON_SYNC);
		return 0;
	} else {
		unlink(mbp_update->file);
		free(mbp_update);

		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		printf("ipmi_upgrade_file_rsp_handler, rsp failed, rsp is %x.\n", rsp[0]);
		return -1;
	}

	return 0;
}

static int ipmi_upgrade_ip_rsp_handler(int result, unsigned char *rsp, int rsp_len,
									void *cb_data)
{
	struct _fm_update_info *mbp_update = (struct _fm_update_info *)cb_data;

	if (rsp[0] == IPMI_CC_OK) {
		struct jipmi_msg req = {};
		int len = 0;
		int data_len = 0;
		int i = 0;

		/* Step2: Send File Name */
		len = strnlen_s(mbp_update->file, sizeof(mbp_update->file)-1);
		memset((char *)&req, 0, sizeof(req));

		req.data[0] = 0x02;
		data_len = 1;

		FILL_INT(req.netfn,		UPDATE_FIRMWARE_NETFN);
		FILL_INT(req.cmd,		UPDATE_FM_SEND_FILE_CMD);
		FILL_STR(req.name,		am_rmcp_username);
		FILL_STR(req.password,	am_rmcp_password);

		for (i = 0; i < len; i++) {
			req.data[i+1] = mbp_update->file[i];
			data_len++;
		}

		req.data[len+1] = 0x00;
		data_len++;

		FILL_INT(req.data_len,	data_len);

		libjipmi_rmcp_cmd(mbp_update->host, IPMI_RMCP_PORT, &req,
						 ipmi_upgrade_file_rsp_handler, mbp_update, JIPMI_NON_SYNC);

		return 0;
	} else {
		unlink(mbp_update->file);
		free(mbp_update);

		cm_set_firmware_update_status(CM_FW_UPDATE_FAIL);
		printf("ipmi_upgrade_ip_rsp_handler, rsp failed, rsp is %x.\n", rsp[0]);
		return -1;
	}

	return 0;
}

/**
 * @brief: upgrade the CM firmware via IPMI.
 *         Steps:
 *         1. send the tftp server ip to the CM
 *         2. send the file name for upgrading to the CM
 *         3. send the command to start to do the upgradation
 *         4. check the serial port if disappeared for a while,
 *            if yes, the cm reboot itself and the update finished.
 *            or need to send the command "reboot" via serial console
 */
int cm_update_mbp_firmware(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	struct jipmi_msg req = {};
	struct _fm_update_info *fm_update = NULL;
	int status;
	json_t *attr = NULL;
	jrpc_data_integer host = 0;
	jrpc_data_string dev = NULL;
	jrpc_data_string file = NULL;
/*
	am_node_t **nptrs = NULL;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;
*/
	get_array_elem(req_pkg, JRPC_ACTION_MBP_UPDATE_HOST, JSON_INTEGER, &host);
	get_array_elem(req_pkg, JRPC_ACTION_MBP_UPDATE_DEV, JSON_STRING, &dev);
	get_array_elem(req_pkg, JRPC_ACTION_MBP_UPDATE_FILE, JSON_STRING, &file);

	#if 0
	memdb_integer cm_node_id = 0;

	

	/* Step1: Send IP */
	memset((char *)&req, 0, sizeof(req));

	if (-1 == libutils_get_ip(ip)) {
		return RESULT_OTHER_ERR;
	}

	cm_node_id = get_subnode_id_by_idx(idx, MC_NODE_ROOT, MC_TYPE_CM);
	libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	#endif

	if (NULL == file)
		return -1;

	if (dev != NULL)
		printf("\nMPB UPDATE, cm host %llx, dev %s, file %s.\n", host, dev, file);

	fm_update = (struct _fm_update_info *)malloc(sizeof(struct _fm_update_info));
	if (NULL == fm_update)
		return 1;

	memset(fm_update, 0, sizeof(struct _fm_update_info));

	fm_update->host = host;
	memset(fm_update->dev, 0, sizeof(fm_update->dev));
	memset(fm_update->file, 0, sizeof(fm_update->file));
	if (dev != NULL)
		memcpy_s(fm_update->dev, sizeof(fm_update->dev), dev, strnlen_s(dev, sizeof(fm_update->dev)-1));
	memcpy_s(fm_update->file, sizeof(fm_update->file), file, strnlen_s(file, sizeof(fm_update->file)-1));

	set_handler_fn(AMACTION_MBP_FW_UPDATE, 2, host, fm_update);

	rmm_log(DBG, "post MBP, host %d, fm_update 0x%x.\n", host, fm_update);

	/* waiting for update finish */
	while(1) {
		status = cm_get_firmware_update_status();

		if (status == CM_FW_UPDATE_SUCCESS)
			return 0;
		else if (status == CM_FW_UPDATE_FAIL)
			return -1;

		sleep(1);
	}

	free(fm_update);
	return 0;
}


int do_cm_update_mbp_firmware()
{
	struct jipmi_msg req = {};
	uint8 param_num;
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
	struct _fm_update_info *fm_update = NULL;
	unsigned int host = 0;
	unsigned int tftp_host = 0;
	int8 tftp_srv[IPV4_ADDR_LEN] = {};

	if (get_handler_fn(AMACTION_MBP_FW_UPDATE, params) != 2) {
		rmm_log(ERROR, "do actions params error.\n");
		return -1;
	}

	host = (typeof(host))params[0];
	libutils_net_get_ip("eth0.4092", tftp_srv, IPV4_ADDR_LEN);
	tftp_host = inet_addr(tftp_srv);

	fm_update = (typeof(fm_update))((unsigned long)params[1]);
	
	rmm_log(DBG, "Do post MBP, host %d, fm_update 0x%x.\n", host, fm_update);

	req.data[0] = 0x01;

	FILL_INT(req.netfn, 	UPDATE_FIRMWARE_NETFN);
	FILL_INT(req.cmd,		UPDATE_FM_SEND_IP_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	1 + sizeof(int));

	memcpy_s(req.data + 1, sizeof(req.data)-1, (char *)&tftp_host, sizeof(int));

	cm_set_firmware_update_status(CM_FW_UPDATE_ONGOING);
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, ipmi_upgrade_ip_rsp_handler, fm_update, JIPMI_NON_SYNC);

	return 0;
}




