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


#ifndef __IPMI_IPMI_H__
#define __IPMI_IPMI_H__

#include <stdlib.h>
#include <stdio.h>

#include "libjipmi/common.h"

struct app_recv_msg {
	struct app_recv_msg *next;	/* free buffer link */
	struct sockaddr_in addr;
	union app_msg_union appmsg;
};

struct app_msg_head {
	struct app_recv_msg  *head;
	struct app_recv_msg **tail;
};

extern bool ipmi_module_exit;

/* util.c */
extern unsigned char ipmi_csum(unsigned char *data, int size);
extern void *mem_freelist_create(int num, size_t size);


/* subscribe.c */
extern void ipmi_subscribe_init(void);
extern void handle_cmd_subscribe(unsigned short user_port,
			unsigned char netfn, unsigned char cmd);
extern unsigned short find_user_port(unsigned char netfn, unsigned char cmd);


/* app_intf.c */
extern void app_interface_init(void);
extern void deliver_app_msg_to_user(struct app_msg_hdr *msg, unsigned short port, ipmi_json_ipc_header_t header);



/* rmcp_intf.c */
extern void rmcp_interface_init(void);
extern void rmcp_send_outbound_msg(unsigned char *msg, int len, unsigned int host, unsigned int port);


/* rmcp_handler.c */
extern void init_rmcp_msg_handler(void);
extern void handle_rmcp_msg(unsigned char *msg, int len, unsigned int host, unsigned int port);
extern void deliver_ipmi_msg_by_rmcp(struct appmsg_ipmi_msg *msg);

/* serial_intf.c */
extern void start_serial_intf(char *dev, unsigned char netfn, unsigned char cmd,
							  int msgid, unsigned short user_port, ipmi_json_ipc_header_t header);
extern void stop_serial_intf(char *dev, unsigned char netfn, unsigned char cmd,
							 int msgid, unsigned short user_port, ipmi_json_ipc_header_t header);
extern void serial_send_outbound_msg(int fd, unsigned char *msg, int len);

/* serial_handler.c */
extern int get_unused_serial_port(void);
extern int get_spec_serial_port_by_fd(int fd);
extern int get_spec_serial_port_by_dev(char *dev);
extern void set_serial_port_used(int cur, int fd, char *dev, pthread_t tid, long inode);
extern int serial_port_is_updated(int cur, long inode);
extern void serial_port_update(int cur, int fd, long inode);
extern void set_serial_port_unused(int cur);
extern void init_serial_msg_handler(int cur);
extern int check_serial_port_by_dev(char *dev, int *fd);
extern int set_serial_port_timeout_by_dev(char *dev, unsigned int timeout);
extern int send_back_result_to_app(int fd, unsigned char netfn, unsigned char cmd,
								   int msgid, unsigned short user_port, ipmi_json_ipc_header_t header);
extern void handle_serial_msg(int cur, unsigned char *msg, int len);
extern void process_serial_cmd(struct appmsg_ipmi_msg *msg);
extern void deliver_ipmi_msg_by_serial(struct appmsg_ipmi_msg *msg);
extern int format_app_jrpc_to_user(unsigned char *buffer, ipmi_json_ipc_header_t header, const struct app_msg_hdr *msg/*, unsigned short len*/);
extern int app_json_parse(struct app_recv_msg *list, char *dest_msg);
#endif

