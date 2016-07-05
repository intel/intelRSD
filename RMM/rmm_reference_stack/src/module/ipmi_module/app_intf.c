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
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libutils/sock.h"
#include "libcfg/cfg.h"
#include "event.h"
#include "ipmi.h"
#include "ipmi_log.h"

#define APP_MSG_QUEUE_NUM	256

/** application socket fd */
static struct fd_event      app_fd_event;

/**
  *  @brief deliver socket msg to applications
  *
  *  @param
  *  @return socket fd
  */
static inline int open_app_interface(void)
{
	int port;

	port = rmm_cfg_get_port(IPMIMODULE_PORT);
	if (port == 0) {
		IPMI_LOG_ERR("Failed to call rmm_cfg_get_ipmid_port\n");
		exit(-1);
	}
	return create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
}

/**
  *  @brief deliver socket msg to applications
  *
  *  @param[in] msg socket msg
  *  @param[in] port socket port
  *  @return
  */
void deliver_app_msg_to_user(struct app_msg_hdr *msg, unsigned short port, ipmi_json_ipc_header_t header)
{
	struct sockaddr_in dest;
	unsigned char buffer[IPMI_JSONRPC_MAX_STRING_LEN] = { 0 };
	int len = 0;
	int i = 0;

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	len = format_app_jrpc_to_user(buffer, header, msg/*, msg->datalen + sizeof(*msg)*/);

	i = sendto(app_fd_event.fd, buffer, len, 0, (struct sockaddr *)&dest, sizeof(dest));
	rmm_log(DBG, "send to user %d:%d, port[%d:%d]\n", len, i, port, htons(port));
}

/**
  *  @brief handle socket packets from applications
  *
  *  @param[in] msg socket msg received from applications
  *  @return
  */
static void handle_app_msg(struct app_recv_msg *msg)
{
	if (msg->appmsg.hdr.type == APPMSG_SUB_MSG) {
		struct appmsg_sub_msg *sub;

		sub = (struct appmsg_sub_msg *)msg->appmsg.hdr.data;
		if (msg->appmsg.hdr.datalen != sizeof(*sub)) {
			IPMI_LOG_DEBUG("appmsg-sub is truncated!\n");
			return;
		}

		handle_cmd_subscribe(sub->user_port, sub->netfn, sub->cmd);
	} else if (msg->appmsg.hdr.type == APPMSG_IPMI_MSG) {
		struct appmsg_ipmi_msg *ipmi;

		ipmi = (struct appmsg_ipmi_msg *)msg->appmsg.hdr.data;
		if (msg->appmsg.hdr.datalen != APPMSG_IPMI_MSG_LEN(ipmi->msg.data_len)) {
			IPMI_LOG_DEBUG("appmsg-ipmi is truncated!\n");
			return;
		}

		if (ipmi->addr.type == IPMI_ADDR_TYPE_RMCP)
			deliver_ipmi_msg_by_rmcp(ipmi);
		else if (ipmi->addr.type == IPMI_ADDR_TYPE_SERIAL) {
			if ((ipmi->msg.union_app_req.serial.serial_flag == IPMI_SERIAL_OPEN_DEV)
				|| (ipmi->msg.union_app_req.serial.serial_flag == IPMI_SERIAL_CLOSE_DEV))
				process_serial_cmd(ipmi);
			else
				deliver_ipmi_msg_by_serial(ipmi);
		}
	}
}

/**
  *  @brief receive socket packets from applications
  *
  *  @param[in] fd socket fd
  *  @return
  */

static void app_recv_inbound_msg(int fd)
{
	int rc;
	struct app_recv_msg msg = {0};
	socklen_t addr_len = sizeof(struct sockaddr_in);

	for (;;) {
		memset(&msg, 0, sizeof(struct app_recv_msg));

		rc = recvfrom(fd, &msg.appmsg, sizeof(msg.appmsg), 0, (struct sockaddr *)&msg.addr, (socklen_t *)&addr_len);
		if (rc <= 0)
			return;
		else {
			struct app_recv_msg dest_msg = {0};

			rmm_log(DBG, "Receive %d data from app %s:%d\nbuffer is %s.\n", rc, inet_ntoa(msg.addr.sin_addr), ntohs(msg.addr.sin_port), (char *)&msg.appmsg);
			memset(&dest_msg.appmsg, 0, sizeof(dest_msg.appmsg));

			/*parse json msg*/
			if (app_json_parse(&msg, (char *)&dest_msg.appmsg))
				return;

			handle_app_msg(&dest_msg);
		}
	}
}

/**
  *  @brief start application interface
  *
  *  open socket to applications,   create new thread to send/receive socket packets to/from applications
  *  @param
  *  @return
  */
void app_interface_init(void)
{
	app_fd_event.fd = open_app_interface();
	if (app_fd_event.fd == -1)
		FATAL("Failed to open APP interface!\n");
	app_fd_event.handle_fd = app_recv_inbound_msg;
	fd_event_add(&app_fd_event);

	IPMI_LOG_INFO("APP-Intf is started successfully!\n");
}

