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


#ifndef __LIBIPMI_IPMI_H__
#define __LIBIPMI_IPMI_H__

#include <sys/types.h>

#include "libipmi/common.h"

/*
 * For non-broadcast IPMI request, libipmi will at least return
 * a timeout response: fake_rsp[1] = { IPMI_CC_TIMEOUT }.
 */


extern int  libipmi_init(unsigned short port);
extern void libipmi_callback_selectfds(fd_set *rfds, int *max_fd);
extern void libipmi_callback_processfds(fd_set *rfds);



/* @host MUST BE in network order byte */
extern void libipmi_rmcp_cmd(unsigned int host, struct ipmi_msg *req,
			ipmi_rsp_callback_fn cb_fn, void *cb_arg);
extern void libipmi_rmcp_cmd_timeout(unsigned int host, struct ipmi_msg *req,
			ipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned int timeo);
extern void libipmi_rmcp_close_session(unsigned int host,
									   ipmi_rsp_callback_fn cb_fn,
									   void *cb_arg);

extern void libipmi_cmd_subscribe(unsigned char netfn, unsigned char cmd,
			ipmi_req_callback_fn cb_fn);

extern void libipmi_rmcp_cmd_send_msg(unsigned int host, struct ipmi_msg *request,	ipmi_rsp_callback_fn cb_fn,
			void *cb_arg, unsigned long target_addr, unsigned short target_channel,
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level);

extern void libipmi_serial_cmd(struct ipmi_msg *req,
							   ipmi_rsp_callback_fn cb_fn, void *cb_arg);
extern void libipmi_serial_cmd_timeout(struct ipmi_msg *req,
									   ipmi_rsp_callback_fn cb_fn,
									   void *cb_arg, unsigned int timeo);

extern void jrpc_libipmi_rmcp_cmd(unsigned int host, unsigned int port, struct ipmi_msg *req,
			ipmi_json_ipc_header_t header, ipmi_jrpc_callback_fn jrpc_cb_fn, char *uname, char *passwd);
extern void jrpc_libipmi_serial_cmd(struct ipmi_msg *req,
						ipmi_json_ipc_header_t header, ipmi_jrpc_callback_fn jrpc_cb_fn);
extern void jrpc_libipmi_rmcp_cmd_send_msg(unsigned int host, unsigned int port, struct ipmi_msg *request,
					ipmi_json_ipc_header_t header, ipmi_jrpc_callback_fn jrpc_cb_fn,
					unsigned long target_addr, unsigned short target_channel, char *uname, char *passwd,
					unsigned long transit_addr, unsigned short transit_channel, int bridge_level);
extern void jrpc_handle_ipmi_msg(struct app_msg_hdr *msg);
extern void jrpc_libipmi_set_fd_port(int client_fd, int server_fd, unsigned short port);
extern void *libipmi_time_thread(void *unused);
extern void *jrpc_libipmi_time_thread(void *unused);



#endif

