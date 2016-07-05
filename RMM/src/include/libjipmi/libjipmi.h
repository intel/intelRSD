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


#ifndef __LIBIPMI_LIBIPMI_H__
#define __LIBIPMI_LIBIPMI_H__

#include <sys/types.h>
#include "libjipmi/common.h"

#define IPMI_JSONRPC_MAX_MSG_LEN       1024
#define MAX_RSP_TYPE_LEN               8
#define MAX_NUM_LEN                    16
#define MAX_NAME_LEN                   64
#define MAX_PASSWD_LEN                 64

/*
 * For non-broadcast IPMI request, libipmi will at least return
 * a timeout response: fake_rsp[1] = { IPMI_CC_TIMEOUT }.
 */



typedef int (*jipmi_rsp_callback_fn)(int result, unsigned char *rs, int rslen, void *cb_arg);

typedef struct jipmi_msg {
	char   rsp_type[MAX_RSP_TYPE_LEN];
	char   netfn[MAX_NUM_LEN];
	char   cmd[MAX_NUM_LEN];
	char   name[MAX_NAME_LEN];
	char   password[MAX_PASSWD_LEN];
	char   data_len[MAX_NUM_LEN];
	char   data[IPMI_MAX_DATA_LENGTH];
	char   base64data[IPMI_MAX_DATA_LENGTH];
} jipmi_msg_t;

typedef struct jipmi_retry_msg {
	jipmi_msg_t  msg;
	unsigned int host;
	unsigned int retry;
} jipmi_retry_msg_t;

typedef struct jipmi_br_msg {
	jipmi_msg_t     msg_base;
	unsigned long   transit_addr;
	unsigned short  transit_channel;
	unsigned long   target_addr;
	unsigned short  target_channel;
	unsigned int    br_rpcid;
	int             bridge_level;
	jipmi_rsp_callback_fn  br_cb_fn;
	void*           cb_arg;
} jipmi_br_msg_t;

typedef struct jipmi_serial_msg {
	jipmi_msg_t     msg_base;
	unsigned char   serial_flag;
	int             serial_fd;
} jipmi_serial_msg_t;

typedef struct jipmi_br_serial_msg {
	jipmi_msg_t     msg_base;
	unsigned char   serial_flag;
	int             serial_fd;
	unsigned long   transit_addr;
	unsigned short  transit_channel;
	unsigned long   target_addr;
	unsigned short  target_channel;
	unsigned int    br_rpcid;
	int             bridge_level;
} jipmi_br_serial_msg_t;


typedef enum{
	JIPMI_NON_SYNC   = 0,
	JIPMI_SYNC
} sync_mode_t;



extern int  libjipmi_init(unsigned short async_listen_port);
extern void libjipmi_callback_selectfds(fd_set *rfds, int *max_fd);
extern void libjipmi_callback_processfds(fd_set *rfds);


extern int libjipmi_ipmb_cmd(unsigned char sa, jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode);

extern int libjipmi_ipmb_cmd_broadcast(jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode);


/* @host MUST BE in network order byte */
extern int libjipmi_rmcp_cmd(unsigned int host, unsigned int port, jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode);
extern int libjipmi_rmcp_cmd_timeout(unsigned int host, unsigned int port, jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned int timeo, sync_mode_t mode);
extern int libjipmi_rmcp_close_session(unsigned int host, unsigned int port, struct jipmi_msg *req,
									   jipmi_rsp_callback_fn cb_fn,
									   void *cb_arg, sync_mode_t mode);


extern int libjipmi_rmcp_cmd_send_msg(unsigned int host, unsigned int port, jipmi_msg_t *request,	jipmi_rsp_callback_fn cb_fn, 
			void *cb_arg, unsigned long target_addr, unsigned short target_channel, 
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level, sync_mode_t mode);

extern int libjipmi_serial_cmd(jipmi_serial_msg_t *req,
							   jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode);
extern int libjipmi_serial_cmd_timeout(jipmi_serial_msg_t *req,
									   jipmi_rsp_callback_fn cb_fn,
									   void *cb_arg, unsigned int timeo, sync_mode_t mode);
extern int libjipmi_serial_br_cmd(jipmi_serial_msg_t *req,
						jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned long target_addr, unsigned short target_channel,
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level,sync_mode_t mode);

#endif

