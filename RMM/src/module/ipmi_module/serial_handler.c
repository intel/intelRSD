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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "ipmi.h"
#include "ipmi_log.h"
#include "libsecurec/safe_lib.h"


#define SERIAL_RSPHNDL_QUEUE_SIZE	256

#define SERIAL_RSP_MIN_LEN				3

#define SERIAL_RES_LUN_OFFSET			0
#define SERIAL_NETFN_OFFSET				0
#define SERIAL_SEQ_OFFSET				1
#define SERIAL_CMD_OFFSET				2
#define SERIAL_DATA_OFFSET				3

struct serial_match_info {
	unsigned char seq;
	unsigned char netfn;
	unsigned char cmd;
};

struct rsp_serial_hndl {
	struct rsp_serial_hndl *next;	/* free buffer link */

	struct list_head seq_list;	/* link in 'seq_table[i]' list */
	struct list_head list;		/* all serial response handle in 'all_hndls' list */

	struct serial_match_info match;
	unsigned int msgid;
	unsigned short user_port;
	ipmi_json_ipc_header_t header;
#ifdef DEBUG_IPMI
	struct timespec start_time;
#endif
	unsigned int timeout;
	int broadcast;
};

struct rsp_serial_table {
	pthread_mutex_t lock;

	struct rsp_serial_hndl *freelist;

	unsigned int curr_seq;
	struct list_head all_hndls;
	struct list_head seq_table[SERIAL_SEQ_SIZE];	/* Fast indexed handle by seq */

	pthread_t time_tid;
};

static struct rsp_serial_table serial_rsp_table[MAX_SERIAL_PORT];

struct _opened_serial_port {
	int fd;
	char dev_name[16];
	unsigned char is_opened;
	long inode;
	struct timespec expire;
	unsigned int timeout;
};

static struct _opened_serial_port global_serial_port[MAX_SERIAL_PORT];

static int global_cur_serial_port;

/**
  *  @brief get unused serial port
  *
  *  @param
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
int get_unused_serial_port(void)
{
	int i = 0;

	for (i = 0; i < MAX_SERIAL_PORT; i++) {
		if (!global_serial_port[i].is_opened
			&& (global_serial_port[i].dev_name[0] == '\0')) {
			global_cur_serial_port = i;
			return i;
		}
	}

	return -1;
}


/**
  *  @brief set the serial port has been used.
  *
  *  @param[in] cur serial port
  *  @param[in] fd serial device file fd
  *  @param[in] dev serial device name
  *  @param[in] tid
  *  @param[in] inode serial port device file inode
  *  @return
  */
void set_serial_port_used(int cur, int fd, char *dev, pthread_t tid, long inode)
{
	global_serial_port[cur].is_opened = 1;
	global_serial_port[cur].fd = fd;
	memcpy_s(global_serial_port[cur].dev_name, sizeof(global_serial_port[cur].dev_name), dev, strnlen_s(dev, RSIZE_MAX_STR));
	global_serial_port[cur].inode = inode;
}

/**
  *  @brief
  *
  *  @param[in] cur serial port
  *  @param[in] inode serial port device file inode
  *  @return
  */
int serial_port_is_updated(int cur, long inode)
{
	if (global_serial_port[cur].inode == inode)
		return 0;

	return 1;
}


/**
  *  @brief Sometimes, we re-plug the UART, need to re-open the serial port, and will get the new fd/inode.
  *
  *  @param[in] cur serial port
  *  @param[in] inode serial port device file inode
  *  @return
  */
void serial_port_update(int cur, int fd, long inode)
{
	global_serial_port[cur].fd = fd;
	global_serial_port[cur].inode = inode;
}


/**
  *  @brief After close the serial, need to set the serial as unused.
  *
  *  @param[in] cur serial port
  *  @return
  */
void set_serial_port_unused(int cur)
{
	global_serial_port[cur].is_opened = 0;
	global_serial_port[cur].fd = -1;
}

/**
  *  @brief search the list to find which fd is matched we need.
  *
  *  @param[in] fd serial device file fd
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
int get_spec_serial_port_by_fd(int fd)
{
	int cur = 0;

	for (cur = 0; cur < MAX_SERIAL_PORT; cur++) {
		if (global_serial_port[cur].is_opened
			&& (global_serial_port[cur].fd == fd))
			return cur;
	}

	return -1;
}

/**
  *  @brief search the list to find which device is matched we need.
  *
  *  @param[in] dev serial device name
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
int get_spec_serial_port_by_dev(char *dev)
{
	int cur = 0;

	for (cur = 0; cur < MAX_SERIAL_PORT; cur++) {
		if (global_serial_port[cur].is_opened
			&& (strcmp(global_serial_port[cur].dev_name, dev) == 0))
			return cur;
	}

	return -1;
}


/**
  *  @brief When we re-open the serial port, need to check if the CM upgradation has been finished.
  *
  *  @param[in] dev serial device name
  *  @param[in] cur serial port
  *  @return
  *  @retval -1 failure
  *  @retval 1 SERIAL_PORT_IS_OPENED
  *  @retval 2 SERIAL_PORT_IS_CLOSED
  *  @retval 3 SERIAL_PORT_WAITING
  */
int check_serial_port_by_dev(char *dev, int *cur)
{
	int i = 0;

	for (i = 0; i < MAX_SERIAL_PORT; i++) {
		if (strcmp(global_serial_port[i].dev_name, dev) == 0) {
			*cur = i;
			if (global_serial_port[i].is_opened)
				return SERIAL_PORT_IS_OPENED;
			else {
				if (global_serial_port[i].timeout > 0) {
					struct timespec expire;

					clock_gettime(CLOCK_REALTIME, &expire);
					if ((expire.tv_sec - global_serial_port[i].expire.tv_sec)
						< global_serial_port[i].timeout) {

						return SERIAL_PORT_WAITING;
					}
				}

				global_serial_port[i].timeout = 0;

				return SERIAL_PORT_IS_CLOSED;
			}
		}
	}

	return -1;
}


/**
  *  @brief When do upgradation for CM firmware, need to set the timeout for waiting it be finished.
  *
  *  @param[in] cur serial port
  *  @param[in] timeout time out
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
int set_serial_port_timeout_by_dev(char *dev, unsigned int timeout)
{
	int i = 0;

	for (i = 0; i < MAX_SERIAL_PORT; i++) {
		if (strcmp(global_serial_port[i].dev_name, dev) == 0) {
			clock_gettime(CLOCK_REALTIME, &(global_serial_port[i].expire));

			global_serial_port[i].timeout = timeout;

			return 0;
		}
	}

	return -1;
}

/**
  *  @brief serial timeout thread
  *
  *  @param
  *  @return
  */
static void *serial_time_thread(void *argv)
{
	struct timespec expire;
	struct rsp_serial_hndl *hndl, *nxt;
	int cur_serial_port = 0;

	prctl(PR_SET_NAME, "serial_time_thread");

	memcpy_s(&cur_serial_port, sizeof(cur_serial_port), (char *)argv, sizeof(int));

	/* Don't forget free it. */
	free(argv);
	argv = NULL;

	struct rsp_serial_table *table = &(serial_rsp_table[cur_serial_port]);
	pthread_cond_t nul_cond = PTHREAD_COND_INITIALIZER;

	while (1) {
		clock_gettime(CLOCK_REALTIME, &expire);
		expire.tv_sec += (IPMI_PERIOD_TIME_MS/1000);
		expire.tv_nsec += (IPMI_PERIOD_TIME_MS % 1000) * (1000*1000);
		if (expire.tv_nsec >= BILLION) {
			expire.tv_sec += expire.tv_nsec / BILLION;
			expire.tv_nsec %= BILLION;
		}

		pthread_mutex_lock(&table->lock);
		while (pthread_cond_timedwait(&nul_cond, &table->lock, &expire) != ETIMEDOUT)
			/* Nothing! */;

#ifdef DEBUG_IPMI
		clock_gettime(CLOCK_REALTIME, &expire);
#endif

		list_for_each_entry_safe(hndl, nxt, &table->all_hndls, list) {
			hndl->timeout -= IPMI_PERIOD_TIME_MS;
			if (hndl->timeout > 0)
				continue;

			IPMI_LOG_DEBUG("SERIAL(%02X:%02X) Response handle timeout %ld:%ld -> %ld:%ld\n",
						   hndl->match.netfn, hndl->match.cmd,
						   hndl->start_time.tv_sec, hndl->start_time.tv_nsec,
						   expire.tv_sec, expire.tv_nsec);

			list_del(&hndl->list);
			list_del(&hndl->seq_list);

			hndl->next = table->freelist;
			table->freelist = hndl;
		}
		pthread_mutex_unlock(&table->lock);
	}

	return NULL;
}

/**
  *  @brief init IPMI over serial message handle
  *
  *  @param[in] cur serial port
  *  @return
  */
void init_serial_msg_handler(int cur)
{
	int i = 0;
	struct rsp_serial_table *table = &(serial_rsp_table[cur]);
	int *cur_port = NULL;

	cur_port = (int *)malloc(sizeof(int));
	if (NULL == cur_port)
		return;

	*cur_port = cur;

	pthread_mutex_init(&table->lock, NULL);

	table->freelist = mem_freelist_create(SERIAL_RSPHNDL_QUEUE_SIZE,
										  sizeof(struct rsp_serial_hndl));
	if (table->freelist == NULL)
		FATAL("Failed to alloc serial rsp handle queue!\n");

	INIT_LIST_HEAD(&table->all_hndls);
	for (i = 0; i < SERIAL_SEQ_SIZE; i++)
		INIT_LIST_HEAD(&table->seq_table[i]);

	table->curr_seq = 0;

	if (pthread_create(&table->time_tid, NULL, serial_time_thread, cur_port) != 0)
		FATAL("Failed to create SERIAL time thread!\n");
}

/**
  *  @brief destroy serial message
  *
  *  @param[in] cur serial port
  *  @return
  */
void destroy_serial_msg_handler(int cur)
{
	struct rsp_serial_table *table = &(serial_rsp_table[cur]);

	pthread_mutex_destroy(&table->lock);

	free(table->freelist);
	table->freelist = NULL;

	pthread_cancel(table->time_tid);
}

/**
  *  @brief find the existed sequence which be requested.
  *
  *  @param[in] cur_serial_port serial port
  *  @param[in] match
  *  @param[in] msgid msg id
  *  @param[in] user_port user application socket port
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int serial_intf_find_seq(int cur_serial_port,
								struct serial_match_info *match,
								unsigned int *msgid,
								unsigned short *user_port,
								ipmi_json_ipc_header_t *header)
{
	int rv = -1;
	unsigned char netfn = ((match->netfn) & 0x3E);	/* request netfn */
	struct rsp_serial_hndl *hndl = NULL;
	struct rsp_serial_hndl *pos = NULL;

	struct rsp_serial_table *table = &(serial_rsp_table[cur_serial_port]);
	struct list_head *head = &table->seq_table[SERIAL_SEQ_HASH(match->seq)];

	pthread_mutex_lock(&table->lock);

	list_for_each_entry(pos, head, seq_list) {
		if ((pos->match.netfn == netfn) && (pos->match.cmd == match->cmd)) {
			hndl = pos;
			break;
		}
	}

	if (hndl == NULL)
		goto ret;

	*msgid = hndl->msgid;
	*user_port = hndl->user_port;

	/*copy header*/
	memset(header->method, 0, sizeof(JRPC_METHOD_MAX));
	header->ip = hndl->header.ip;
	header->port = hndl->header.port;
	header->json_ipc_id = hndl->header.json_ipc_id;
	if (strnlen_s((const char *)hndl->header.method, sizeof(hndl->header.method)-1) < JRPC_METHOD_MAX)
		memcpy_s(header->method, sizeof(header->method), hndl->header.method, strnlen_s((const char *)(hndl->header.method), sizeof(hndl->header.method)-1));

	/* Add by Peifeng: remove from list after processing */
	list_del(&hndl->seq_list);
	list_del(&hndl->list);
	hndl->next = table->freelist;
	table->freelist = hndl;

	rv = 0;

ret:
	pthread_mutex_unlock(&table->lock);

	return rv;
}


/**
  *  @brief process next IPMI message over serial interface
  *
  *  @param[in] fd serial device file fd
  *  @param[in] match
  *  @param[in] timeo time out
  *  @param[in] msgid msg id
  *  @param[in] user_port user application socket port
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int serial_intf_next_seq(int fd, struct serial_match_info *match,
								unsigned int timeo, unsigned int msgid,
								unsigned short user_port, ipmi_json_ipc_header_t header)
{
	int rv = -1;
	unsigned int i = 0;
	unsigned char netfn = ((match->netfn) & 0x3E);	/* request netfn */
	unsigned char seq = SERIAL_SEQ_SIZE;
	struct rsp_serial_hndl *hndl = NULL;
	struct rsp_serial_hndl *pos = NULL;
	int cur_serial_port = 0;

	cur_serial_port = get_spec_serial_port_by_fd(fd);
	if (cur_serial_port < 0) {
		IPMI_LOG_DEBUG("Can't find the matched fd %d\n", fd);
		send_back_result_to_app(-1, netfn, match->cmd, msgid, user_port, header);
		return rv;
	}

	struct rsp_serial_table *table = &(serial_rsp_table[cur_serial_port]);

	pthread_mutex_lock(&table->lock);

	hndl = table->freelist;
	if (hndl == NULL) {
		IPMI_LOG_ERR("NXT SEQ no buff, adjust #SERIAL_RSPHNDL_QUEUE_SIZE(%d)!\n",
					 SERIAL_RSPHNDL_QUEUE_SIZE);
		goto ret;
	}

	for (i = table->curr_seq;
		 SERIAL_SEQ_HASH(i+1) != table->curr_seq;
		 i = SERIAL_SEQ_HASH(i+1)) {
		int conflict = 0;

		list_for_each_entry(pos, &table->seq_table[i], seq_list) {
			if (pos->match.netfn == netfn &&
			    pos->match.cmd == match->cmd) {
				conflict = 1;
				break;
			}
		}

		if (!conflict) {
			IPMI_LOG_DEBUG("SERIAL NetFn:%02X,Cmd:%02X uses Seq:%02X\n",
						   netfn, match->cmd, i);
			seq = i;

			break;
		}
	}

	if (seq >= SERIAL_SEQ_SIZE) {
		IPMI_LOG_ERR("No avail seq number for IPMI msg in SERIAL interface!\n");
		goto ret;
	}

	table->freelist = hndl->next;

	hndl->next = NULL;
	list_add_tail(&hndl->seq_list, &table->seq_table[seq]);
	list_add_tail(&hndl->list, &table->all_hndls);

	hndl->match.seq = seq;
	hndl->match.netfn = netfn;
	hndl->match.cmd = match->cmd;

	hndl->msgid = msgid;
	hndl->user_port = user_port;

	/*copy header*/
	memset(hndl->header.method, 0, sizeof(JRPC_METHOD_MAX));
	hndl->header.ip = header.ip;
	hndl->header.port = header.port;
	hndl->header.json_ipc_id = header.json_ipc_id;
	if (strnlen_s((const char *)header.method, sizeof(header.method)-1) < JRPC_METHOD_MAX)
		memcpy_s(hndl->header.method, sizeof(hndl->header.method), header.method, strnlen_s((const char *)header.method, sizeof(header.method)-1));


#ifdef DEBUG_IPMI
	clock_gettime(CLOCK_REALTIME, &hndl->start_time);
#endif

	hndl->timeout = timeo;
	/* SERIAL: The Requester changes the Seq field whenever it issues a new instance
	 * of a command (request).
	 */
	match->seq = seq;	/* saved the seq result back */
	table->curr_seq = SERIAL_SEQ_HASH(seq + 1);

	rv = 0;

ret:
	pthread_mutex_unlock(&table->lock);

	return rv;
}


/**
  *  @brief Just like the IPMI response, we respond the operation
  *
  *  @param[in] fd serial device file fd
  *  @param[in] netfn IPMI command net function code
  *  @param[in] cmd IPMI command
  *  @param[in] msgid msg id
  *  @param[in] user_port user application socket port
  *  @return
  *  @retval 0 successful
  */
int send_back_result_to_app(int fd, unsigned char netfn, unsigned char cmd,
							int msgid, unsigned short user_port, ipmi_json_ipc_header_t header)
{
	union app_msg_union msg;
	struct serial_match_info match = {0};
	char rsp[sizeof(int)] = {0};
	int rsp_len = 0;
	struct appmsg_ipmi_msg *ipmi = (struct appmsg_ipmi_msg *)msg.hdr.data;

	match.netfn = netfn | 0x01;
	match.cmd = cmd;
	match.seq = 0;

	msg.hdr.type    = APPMSG_IPMI_MSG;

	ipmi->id = msgid;

	memset(&ipmi->addr, 0, sizeof(ipmi->addr));
	ipmi->addr.type = IPMI_ADDR_TYPE_SERIAL;
	ipmi->addr.addr.serial.seq = match.seq;

	ipmi->msg.netfn = match.netfn;
	ipmi->msg.cmd   = match.cmd;

	if (fd > 0) {
		rsp_len = sizeof(int) + 1; /* + ccode */
		ipmi->msg.data_len = rsp_len;
		memcpy_s(rsp, sizeof(rsp), &fd, sizeof(int));
		ipmi->msg.data[0] = SERIAL_CCODE_OK;
		if ((ipmi->msg.data_len > 0) && (ipmi->msg.data_len <= IPMI_MAX_DATA_LENGTH))
			memcpy_s(ipmi->msg.data + 1, sizeof(ipmi->msg.data)-1, rsp, sizeof(int));
	} else {
		rsp_len = 1;
		ipmi->msg.data_len = rsp_len;
		ipmi->msg.data[0] = SERIAL_CCODE_FAILED;
	}

	msg.hdr.datalen = APPMSG_IPMI_MSG_LEN(rsp_len);

	deliver_app_msg_to_user(&msg.hdr, user_port, header);

	return 0;
}



/**
  *  @brief build Serial bridge command
  *
  *  @param[in] buff output IPMI msg buffer
  *  @param[in] sess IPMI session
  *  @param[in] req
  *  @param[in] seq IPMI sequence nnumber
  *  @return
  */

struct serial_br_ipmi_msg_hdr {
	//unsigned char   rs_addr;
	unsigned char   netfun_lun;
	unsigned char   checksum;
	unsigned char   rq_addr;
	unsigned char   rq_seq;
	unsigned char   cmd;
} __attribute__((packed));

static int ipmi_serial_build_cmd_br(unsigned char *buff, struct ipmi_msg *req, unsigned char seq, int bridge_level)
{
	unsigned char *msg;
	int cs, mp, tmp;
	int len = 0;
	int cs2 = 0, cs3 = 0;
	unsigned char *pauth = NULL;
	//struct ipmi_msg_hdr *ipmi_br_msg, *ipmi_dual_br_msg, *ipmi_msg_hdr;
	struct ipmi_msg_hdr *ipmi_dual_br_msg, *ipmi_msg_hdr;
	struct serial_br_ipmi_msg_hdr *ipmi_br_msg;

	if (seq >= IPMB_SEQ_SIZE) {
		seq = 0;
	}

	/* expand req data_len with lan encaption */
	len = req->data_len + 29;

	msg = malloc(len);
	if (msg == NULL) {
		IPMI_LOG_DEBUG("ipmitool: malloc failure");
		return 0;
	}
	memset(msg, 0, len);

	/* message length */
	len = 0;

	/* serial command */
	msg[SERIAL_NETFN_OFFSET] = req->netfn << 2;
	msg[SERIAL_SEQ_OFFSET] = seq;
	msg[SERIAL_CMD_OFFSET] = req->cmd;
	len += 3;
	
	/* dual bridge msg encap */
	if (bridge_level == IPMI_BRIDGE_MSG_DUAL) {
		msg[len++] = (0x40|req->union_app_req.bridge.transit_channel);
		ipmi_dual_br_msg = (struct ipmi_msg_hdr *)(msg+len);
		ipmi_dual_br_msg->rs_addr = req->union_app_req.bridge.transit_addr;
		ipmi_dual_br_msg->netfun_lun = req->netfn << 2;
		ipmi_dual_br_msg->checksum = ipmi_csum(msg+len, 2);
		cs3 = len + 3;
		ipmi_dual_br_msg->rq_addr = IPMI_BMC_SLAVE_ADDR;
		ipmi_dual_br_msg->rq_seq = seq << 2;
		ipmi_dual_br_msg->cmd = req->cmd;
		len += sizeof(*ipmi_dual_br_msg);
	}
	msg[len++] = (0x40|req->union_app_req.bridge.target_channel); /* Track request*/
	cs = len;

	/* ipmi message header */
	ipmi_msg_hdr = (struct ipmi_msg_hdr *)(msg+len);
	ipmi_msg_hdr->rs_addr = req->union_app_req.bridge.target_addr;
	ipmi_msg_hdr->netfun_lun = req->union_app_req.bridge.br_netfn << 2 | (IPMI_BMC_CMD_LUN);
	ipmi_msg_hdr->checksum = ipmi_csum(msg+len, 2);/* from beginning of ipmi_msg_hdr to checksum */
	cs = len + 3; /* cs is at current postion */
	ipmi_msg_hdr->rq_addr = req->union_app_req.bridge.my_addr;
	ipmi_msg_hdr->rq_seq = seq << 2;
	ipmi_msg_hdr->cmd = req->union_app_req.bridge.br_cmd;
	len += sizeof(*ipmi_msg_hdr);

	/* message data */
	if (req->data_len) {
		memcpy_s(msg+len, req->data_len, req->data, req->data_len);
		len += req->data_len;
	}

	/* second checksum */
	tmp = len - cs;
	msg[len++] = ipmi_csum(msg+cs, tmp);

	/* bridged request: 2nd checksum */
	if (bridge_level == IPMI_BRIDGE_MSG_DUAL) {
		tmp = len - cs3;
		msg[len++] = ipmi_csum(msg+cs3, tmp);
	}

	memcpy_s(buff, len, msg, len);

	free(msg);

	return len;
}

/**
  *  @brief format serial IPMI message
  *
  *  @param[out] buff output buffer
  *  @param[in] ipmi
  *  @param[in] seq IPMI message sequence number
  *  @return
  */
static int format_serial_msg(unsigned char *buff, struct ipmi_msg *ipmi,
							 unsigned char seq)
{
	int len = -1;
	
	if (IPMI_BRIDGE_MSG_NONE != ipmi->union_app_req.bridge.bridge_level) {
		len = ipmi_serial_build_cmd_br(buff, ipmi, seq, ipmi->union_app_req.bridge.bridge_level);
	} else {
		buff[SERIAL_NETFN_OFFSET] = ipmi->netfn << 2;
		buff[SERIAL_SEQ_OFFSET] = seq;
		buff[SERIAL_CMD_OFFSET] = ipmi->cmd;

		if (ipmi->data_len >= 0 && ipmi->data_len <= (IPMI_MAX_MSG_LENGTH - SERIAL_CMD_OFFSET)) {
			if (ipmi->data_len != 0)
				memcpy_s(&buff[SERIAL_DATA_OFFSET], ipmi->data_len, ipmi->data, ipmi->data_len);
			len = SERIAL_DATA_OFFSET + ipmi->data_len;
		} else {
			IPMI_LOG_ERR("Incorrect ipmi->data_len = %d\n", ipmi->data_len);
		}

	}

	return len;
}

/**
  *  @brief process IPMI over serial command
  *
  *  @param[in] msg IPMI message
  *  @return
  */
void process_serial_cmd(struct appmsg_ipmi_msg *msg)
{
	struct ipmi_msg *ipmi_msg = &msg->msg;
	char dev[16] = {0};

	if (ipmi_msg->union_app_req.serial.serial_flag == IPMI_SERIAL_OPEN_DEV) {
		if (ipmi_msg->data_len > 0) {
			memcpy_s(dev, sizeof(dev), ipmi_msg->data, ipmi_msg->data_len);
			start_serial_intf(dev, ipmi_msg->netfn, ipmi_msg->cmd,
							  msg->id, msg->user_port, msg->header);
		}
	} else if (ipmi_msg->union_app_req.serial.serial_flag == IPMI_SERIAL_CLOSE_DEV) {
		if (ipmi_msg->data_len > 0) {
			memcpy_s(dev, sizeof(dev), ipmi_msg->data, ipmi_msg->data_len);
			stop_serial_intf(dev, ipmi_msg->netfn, ipmi_msg->cmd,
							 msg->id, msg->user_port, msg->header);
		}
	}
}

/**
  *  @brief deliver IPMI msg by serial
  *
  *  @param[in] msg IPMI message need to be delivered via serial interface
  *  @return
  */
void deliver_ipmi_msg_by_serial(struct appmsg_ipmi_msg *msg)
{
	struct ipmi_msg *ipmi_msg = NULL;
	unsigned char seq = 0;
	int serial_msg_len = 0;
	unsigned char serial_msg[IPMI_MAX_MSG_LENGTH] = {0};

	ipmi_msg = &msg->msg;
	if (ipmi_msg->data_len > IPMI_MAX_DATA_LENGTH) {
		IPMI_LOG_ERR("Too big IPMI message length[%d], ignore!\n", ipmi_msg->data_len);
		return;
	}

	if ((ipmi_msg->data_len + offset_of(struct ipmi_msg, data)) > IPMI_MAX_MSG_LENGTH) {
		IPMI_LOG_ERR("Too big IPMI message to send, ignore!\n");
		return;
	}

	if (ipmi_msg->netfn & 0x01) {
		seq = msg->addr.addr.serial.seq;
	} else {
		struct serial_match_info match = {0};

		match.netfn = ipmi_msg->netfn;
		match.cmd   = ipmi_msg->cmd;

		if (msg->timeo > IPMI_MAX_TIMEOUT_MS)
			msg->timeo = IPMI_MAX_TIMEOUT_MS;
		if (msg->timeo == 0)
			msg->timeo = IPMI_DFLT_TIMEOUT_MS;

		if (serial_intf_next_seq(ipmi_msg->union_app_req.serial.serial_fd, &match,
								 msg->timeo, msg->id, msg->user_port, msg->header) != 0)
			return;

		seq = match.seq;
	}

	serial_msg_len = format_serial_msg(serial_msg, ipmi_msg, seq);

	if (serial_msg_len > IPMI_MAX_MSG_LENGTH)
		return;

	serial_send_outbound_msg(ipmi_msg->union_app_req.serial.serial_fd, serial_msg, serial_msg_len);
}

/**
 * @brief: Handle the reponse, and send it back to the user.
 */

/**
  *  @brief
  *
  *  @param[in] cur serail port
  *  @param[in] match
  *  @param[in] rsp response data
  *  @param[in] rsp_len response data length
  *  @return
  */
static void handle_serial_msg_rsp(int cur, struct serial_match_info *match,
			unsigned char *rsp, int rsp_len)
{
	unsigned int msgid = 0;
	union app_msg_union msg;
	unsigned short user_port = 0;
	struct appmsg_ipmi_msg *ipmi = (struct appmsg_ipmi_msg *)msg.hdr.data;
	ipmi_json_ipc_header_t header;

	msg.hdr.type    = APPMSG_IPMI_MSG;
	msg.hdr.datalen = APPMSG_IPMI_MSG_LEN(rsp_len);

	if (serial_intf_find_seq(cur, match, &msgid, &user_port, &header) != 0) {
		IPMI_LOG_DEBUG("SERIAL: No match request for this response!\n");
		return;
	}

	ipmi->id = msgid;

	memset(&ipmi->addr, 0, sizeof(ipmi->addr));
	ipmi->addr.type = IPMI_ADDR_TYPE_SERIAL;
	ipmi->addr.addr.serial.seq = match->seq;

	ipmi->msg.netfn = match->netfn;
	ipmi->msg.cmd   = match->cmd;
	ipmi->msg.data_len = rsp_len;
	if ((ipmi->msg.data_len > 0) && (ipmi->msg.data_len <= IPMI_MAX_DATA_LENGTH))
		memcpy_s(ipmi->msg.data, sizeof(ipmi->msg.data), rsp, rsp_len);

	deliver_app_msg_to_user(&msg.hdr, user_port, header);
}

/**
  *  @brief handle IPMI over serial message
  *
  *  @param[in] cur serial port
  *  @param[in] msg msg
  *  @param[in] len msg length
  *  @return
  */
void handle_serial_msg(int cur, unsigned char *msg, int len)
{
	struct serial_match_info match = {0};

#ifdef DEBUG_IPMI
	{
		int __i;
		char __buff[8 * IPMI_MAX_MSG_LENGTH + 256] = {0};
		char __hdr[64] = {0};
		char __body[8 * IPMI_MAX_MSG_LENGTH] = {0};
		char __end[64] = {0};
		char __tmp[8] = {0};

		snprintf(__hdr, sizeof(__hdr), "SERIAL IN: ");
		for (__i = 0; __i < len; __i++) {
			snprintf_s_i(__tmp, sizeof(__tmp), "%02X ", msg[__i]);
			strncat_s(__body, sizeof(__body), __tmp, 8);
		}
		snprintf(__end, sizeof(__end), "\n");
		snprintf_s_sss(__buff, sizeof(__buff), "%s%s%s", __hdr, __body, __end);

		IPMI_LOG_DEBUG("%s\n", __buff);
	}
#endif

	if (len < 3) {
		IPMI_LOG_DEBUG("Invalid SERIAL message!\n");

		return;
	}

	match.netfn = msg[SERIAL_NETFN_OFFSET] >> 2;
	match.seq   = msg[SERIAL_SEQ_OFFSET];
	match.cmd   = msg[SERIAL_CMD_OFFSET];

	/* NetFN: 0x06 -- Request
	 *        0x07 -- Response */
	if (match.netfn & 0x01) {
		if (len < SERIAL_RSP_MIN_LEN) {
			IPMI_LOG_DEBUG("The SERIAL response is too small!\n");

			return;
		}

		handle_serial_msg_rsp(cur, &match, &msg[SERIAL_DATA_OFFSET],
							  len - SERIAL_RSP_MIN_LEN); /* + ccode */
	} else {
		IPMI_LOG_DEBUG("Invalid NetFN for the SERIAL response.\n");

		return;
	}
}
