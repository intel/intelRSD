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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipmi.h"
#include "rmcp.h"
#include "event.h"
#include "ipmi_log.h"
#include "libutils/sock.h"
#include "libcfg/cfg.h"
#include "libsecurec/safe_lib.h"


#define RMCP_MSG_QUEUE_NUM		512

struct rmcp_recv_msg {
	struct rmcp_recv_msg *next;		/* free buffer link */

	unsigned int   host;
	unsigned int   port;

	int           len;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];
};

struct rmcp_msg_head {
	struct rmcp_recv_msg  *head;
	struct rmcp_recv_msg **tail;
};

static struct fd_event       rmcp_fd_event;

/**
  *  @brief open RMCP interface, create the socket
  *
  *  @param
  *  @return RMCP socket
  */
static inline int open_rmcp_interface(void)
{
	int port;

	port = rmm_cfg_get_port(IPMIRMCPCLIENT_PORT);
	if (port == 0) {
		IPMI_LOG_ERR("Fail to call rmm_cfg_get_ipmi_rmcp_client_port");
		exit(-1);
	}

	return create_udp_listen(INADDR_ANY, port, 0, 0);
}

/**
  *  @brief send RMCP outband msg
  *
  *  @param[in] msg
  *  @param[in] len msg length
  *  @param[in] host IP address
  *  @param[in] port
  *  @return
  */
void rmcp_send_outbound_msg(unsigned char *msg, int len, unsigned int host, unsigned int port)
{
	struct sockaddr_in dest;

#ifdef DEBUG_IPMI
	{
		int __i;
		char __buff[1536];
		char __hdr[64];
		char __body[1280] = {0};
		char __end[64];
		char __tmp[8];
		int len = 0;

		len = snprintf_s_iiii(__hdr, sizeof(__hdr), "RMCP OUT("NIPQUAD_FMT")", NIPQUAD(host));
		if (len < 0)
			return;

		len += snprintf_s_i(__hdr+len, sizeof(__hdr)-len, ", port %d: ", ntohs(port));
		if (len < 0)
			return;

		for (__i = 0; __i < len; __i++) {
			snprintf_s_i(__tmp, sizeof(__tmp), "%02X ", msg[__i]);
			strncat_s(__body, sizeof(__body), __tmp, 8);
		}
		snprintf(__end, sizeof(__end), "\n");
		snprintf_s_sss(__buff, sizeof(__buff), "%s%s%s", __hdr, __body, __end);

		IPMI_LOG_DEBUG("%s", __buff);
	}
#endif


	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	if (port)
		dest.sin_port = port;
	else
		dest.sin_port = htons(IPMI_RMCP_PORT);
	dest.sin_addr.s_addr = host;

	sendto(rmcp_fd_event.fd, msg, len, 0, (struct sockaddr *)&dest, sizeof(dest));
}

/**
  *  @brief receive RMCP inband msg
  *
  *  @param[in] fd socket fd
  *  @return
  */
static void rmcp_recv_inbound_msg(int fd)
{
	int rc;
	socklen_t addrlen;
	struct sockaddr_in addr;
	struct rmcp_recv_msg msg = {0};

	addrlen	= sizeof(addr);

	for (;;) {
		memset(&msg, 0, sizeof(struct rmcp_recv_msg));

		rc = recvfrom(fd, msg.msg, sizeof(msg.msg), 0, (struct sockaddr *)&addr, &addrlen);
		if (rc <= 0)
			return;

		IPMI_LOG_DEBUG("RMCP Message from "NIPQUAD_FMT":%u\n",
					   NIPQUAD(addr.sin_addr.s_addr),
					   ntohs(addr.sin_port));

		msg.host = addr.sin_addr.s_addr;
		msg.port = addr.sin_port;
		msg.len = rc;

		handle_rmcp_msg(msg.msg, msg.len, msg.host, msg.port);
	}
}

/**
  *  @brief start RMCP interface, create the new thread
  *
  *  @param
  *  @return
  */
void rmcp_interface_init(void)
{
	init_rmcp_msg_handler();
	rmcp_fd_event.fd = open_rmcp_interface();
	if (rmcp_fd_event.fd == -1)
		FATAL("Failed to open RMCP interface!\n");
	rmcp_fd_event.handle_fd = rmcp_recv_inbound_msg;
	fd_event_add(&rmcp_fd_event);

	IPMI_LOG_INFO("RMCP-Intf (Client IPMI-v1.5) is started successfully!\n");
}
