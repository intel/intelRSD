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


#ifndef __JRPC_IPMI_H__
#define __JRPC_IPMI_H__
#include "libutils/types.h"

#define JSON_RPC_RMCP_REQ		"rmcp_req"
#define JSON_RPC_SERIAL_OP_REQ		"serial_op_req"
#define JSON_RPC_SERIAL_CMD_REQ		"serial_cmd_req"
#define JSON_RPC_SERIAL_BR_REQ		"serial_br_req"
#define JSON_RPC_RMCP_BR_REQ		"rmcp_br_req"


#define STR_RSP_TYPE_UDP		"UDP"
#define STR_RSP_TYPE_HTTP		"HTTP"

#define STR_RSP_TYPE		"rsp_type"
#define STR_RSP_ADDR		"rsp_addr"
#define STR_NETFN			"netfn"
#define STR_CMD				"cmd"
#define STR_DATA_LEN		"data_len"
#define STR_DATA			"data"
#define STR_IPMB_SRC_ADDR	"ipmb_src_addr"
#define STR_TARGET_IP		"target_ip"
#define STR_TARGET_PORT		"target_port"
#define STR_SERIAL_FLAG		"serial_flag"
#define STR_SERIAL_FD		"serial_fd"
#define STR_TARGET_ADDR		"target_addr"
#define STR_TARGET_CH		"target_ch"
#define STR_TRANSIT_ADDR	"transit_addr"
#define STR_TRANSIT_CH		"transit_ch"
#define STR_JRPC_ID			"id"
#define STR_NAME			"name"
#define STR_PASSWD			"password"
#define STR_BRIDGE_LEVEL	"bridge_level"
#define STR_RESULT			"result"
#define STR_ERROR			"error"
#define STR_CODE			"code"

#define STR_DEFAULT_RSP_TYPE	"UDP"
#define STR_DEFAULT_DATA	"AA=="	/* 1 */



#define IPMI_JSONRPC_MAX_STRING_LEN (0x200)

#define JRPC_METHOD_MAX		64
typedef struct ipmi_json_ipc_header {
	uint32 ip;
	uint16 port;
	int64 json_ipc_id;
	unsigned char method[JRPC_METHOD_MAX];
} ipmi_json_ipc_header_t;

#endif
