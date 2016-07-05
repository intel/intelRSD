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


#ifndef __LIBIPMI_COMMON_H__
#define __LIBIPMI_COMMON_H__
#include "libutils/list.h"
#include "libutils/rack.h"
#include "libjipmi/jrpc_ipmi.h"


typedef int (*ipmi_rsp_callback_fn)(unsigned char *rs, int rslen, void *cb_arg);
typedef int (*ipmi_req_callback_fn)(unsigned char *rq, int rqlen, unsigned char *rs);
typedef int (*ipmi_jrpc_callback_fn)(int fd, ipmi_json_ipc_header_t header, unsigned char *ipmi_data, unsigned short len, void *cb_arg);


/* IPMI specification Generic Completion Codes */
#define IPMI_CC_OK								0x00
#define IPMI_CC_NODE_BUSY						0xC0
#define IPMI_CC_INV_CMD							0xC1
#define IPMI_CC_INV_CMD_FOR_LUN					0xC2
#define IPMI_CC_TIMEOUT							0xC3
#define IPMI_CC_OUT_OF_SPACE					0xC4
#define IPMI_CC_RES_CANCELED					0xC5
#define IPMI_CC_REQ_DATA_TRUNC					0xC6
#define IPMI_CC_REQ_DATA_INV_LENGTH				0xC7
#define IPMI_CC_REQ_DATA_FIELD_EXCEED			0xC8
#define IPMI_CC_PARAM_OUT_OF_RANGE				0xC9
#define IPMI_CC_CANT_RET_NUM_REQ_BYTES			0xCA
#define IPMI_CC_REQ_DATA_NOT_PRESENT			0xCB
#define IPMI_CC_INV_DATA_FIELD_IN_REQ			0xCC
#define IPMI_CC_ILL_SENSOR_OR_RECORD			0xCD
#define IPMI_CC_RESP_COULD_NOT_BE_PRV			0xCE
#define IPMI_CC_CANT_RESP_DUPLI_REQ				0xCF
#define IPMI_CC_CANT_RESP_SDRR_UPDATE			0xD0
#define IPMI_CC_CANT_RESP_FIRM_UPDATE			0xD1
#define IPMI_CC_CANT_RESP_BMC_INIT				0xD2
#define IPMI_CC_DESTINATION_UNAVAILABLE			0xD3
#define IPMI_CC_INSUFFICIENT_PRIVILEGES			0xD4
#define IPMI_CC_NOT_SUPPORTED_PRESENT_STATE		0xD5
#define IPMI_CC_ILLEGAL_COMMAND_DISABLED		0xD6
#define IPMI_CC_UNSPECIFIED_ERROR				0xFF

#define DEBUG_IPMI
#if 1

#define IPMI_DEBUG(format, args...)		printf(format, ##args)
#else
#define IPMI_DEBUG(format, args...)
#endif

#define INVALID_PORT	0xFFFF

#ifdef IPMI_RMCP_PORT
#undef IPMI_RMCP_PORT
#endif
#define IPMI_RMCP_PORT		0x26F /* port 623 */


/* timeout in seconds */
#define IPMI_PERIOD_TIME	1	/* call every ~1 s to check timeout */
#define IPMI_DFLT_TIMEOUT	10
#define IPMI_MAX_TIMEOUT	30	/* Don't let a message sit in a queue forever. */
#define RMCP_IPMI_SESSION_SETUP_TIMEOUT		5
#define RMCP_IPMI_SESSION_IDLE_TIMEOUT		60

/* timeout in ms */
#define IPMI_PERIOD_TIME_MS      100 /* call every ~100 ms to check timeout */
#define IPMI_DFLT_TIMEOUT_MS     2000
#define IPMI_MAX_TIMEOUT_MS      5000 /* Don't let a message sit in a queue forever. */
#define IPMI_MAX_SYNC_TIMEOUT_MS 1000	/* for sync mode, only allow 1 second timeout */
#define RMCP_IPMI_SESSION_SETUP_TIMEOUT_MS      5000
#define RMCP_IPMI_SESSION_IDLE_TIMEOUT_MS       60000


#define IPMI_IPMB_ADDR			0x02	/* We are R/W on this I2C address */
#define IPMI_IPMB_LUN			0x00	/* We are on this LUN */
#define IPMI_REMOTE_SWID		0x81	/* */
#define IPMI_REMOTE_LUN			0x00	/* */

/* FIXME: to fix this addr, now assume it to be "0x03" */
#define IPMI_SERIAL_ADDR		0x03
#define IPMI_SERIAL_LUN			0x00

#define SERIAL_SEQ_SIZE		(64)
#define SERIAL_SEQ_MASK		(SERIAL_SEQ_SIZE - 1)
#define SERIAL_SEQ_HASH(seq)	((seq) & SERIAL_SEQ_MASK)

#define IPMB_SEQ_SIZE		(64)
#define IPMB_SEQ_MASK		(IPMB_SEQ_SIZE - 1)
#define IPMB_SEQ_HASH(seq)	((seq) & IPMB_SEQ_MASK)


#define IPMI_IPMB_BCAST_ADDR	0x00


#define IPMI_BMC_SLAVE_ADDR		0x20
#define IPMI_BMC_CMD_LUN		0x00	/* 00b - BMC commands and Event Request Messages */

#define IPMI_BRIDGE_NETFN_APP   0x06
#define IPMI_BRIDGE_IPMI_CMD    0x34
#define IPMI_BRIDGE_MSG_NONE    0
#define IPMI_BRIDGE_MSG_SINGLE  1
#define IPMI_BRIDGE_MSG_DUAL    2
#define IPMI_SESSION_SEQ_LEN    4

/* Serial Dual Bridge RSP format is:
	Encapsulated data for IPMB Resposne from MMP: [Completion Code] [Rq Addr A] [NetFn] [Chk 1 A] [Rq Addr B] [Rq Seq] [Cmd Send Msg 0x34] [Completion Code]
	Followed by Encapsulated data for IPMB Response from CPP: [Rq Addr] [Net Fn] [Chk 1B] [Rs Addr] [Rq Seq] [Cmd] [Completion Code] [Data]
	Then the offset to CPP completion code is 14, to real CPP data is 15.
*/

#define SERIAL_DUALBRIDGE_COMP_OFFSET 14
#define SERIAL_DUALBRIDGE_DATA_OFFSET 15

#define IPMI_FUNCTION_RETURN_SUCCESS    0
#define IPMI_FUNCTION_RETURN_ERROR      -1

#define IPMI_SERIAL_OPEN_DEV	0x01
#define IPMI_SERIAL_OPERATION	0x02
#define IPMI_SERIAL_CLOSE_DEV	0x03

#define MAX_SERIAL_PORT	256
#define SERIAL_PORT_IS_OPENED			1
#define SERIAL_PORT_IS_CLOSED			2
#define SERIAL_PORT_WAITING				3

#define SERIAL_CCODE_OK				0x00
#define SERIAL_CCODE_FAILED			0x01

static inline int is_ipmb_addr_bcast(unsigned char sa)
{
	return sa == IPMI_IPMB_BCAST_ADDR ? 1 : 0;
}


/* port used internally */

#define IPMI_RMCP_CLOSE_SESSION_NETFN	0x06
#define IPMI_RMCP_CLOSE_SESSION_CMD		0x3c

#define RMCP_USERNAME_LEN		16
#define RMCP_PASSWORD_LEN		16

enum {
	IPMI_ADDR_TYPE_IPMB = 1,
	IPMI_ADDR_TYPE_RMCP,
	IPMI_ADDR_TYPE_SERIAL
};

struct ipmi_addr {
	int type;

	union {
		struct {
			unsigned int host;	/* network order byte */
			unsigned int port;
			char			username[RMCP_USERNAME_LEN];
			char			password[RMCP_PASSWORD_LEN];
		} rmcp;

		struct {
			unsigned char sa;
			unsigned char lun;
			unsigned char seq;
		} ipmb;

		struct {
			int serial_fd;
			unsigned char seq;
		} serial;
	} addr;
};


struct app_serial_req {
	unsigned char   serial_flag;
	int             serial_fd;
};

struct app_rmcp_bridge_req {
	unsigned int    bridge_level;
	unsigned long   my_addr;
	unsigned long   target_addr;
	unsigned short  target_channel;
	unsigned long   transit_addr;
	unsigned short  transit_channel;
	unsigned char   br_netfn;
	unsigned char   br_cmd;
};

#define IPMI_MAX_DATA_LENGTH	200
#define IPMI_MAX_MSG_LENGTH		256
//#define APPMSGSIZ				384
#define APPMSGSIZ				1024

struct ipmi_msg {
	unsigned char   netfn;
	unsigned char   cmd;
	unsigned short  data_len;
	struct {	//change union to struct for serial_bridge.
		struct app_serial_req serial;
		struct app_rmcp_bridge_req bridge;
	}union_app_req;

	unsigned char   data[IPMI_MAX_DATA_LENGTH];
};


struct ipmi_session_hdr {
	unsigned char   authtype;
	unsigned char   seq[IPMI_SESSION_SEQ_LEN];
	unsigned int    session_id;
} __attribute__((packed));


struct ipmi_msg_hdr {
	unsigned char   rs_addr;
	unsigned char   netfun_lun;
	unsigned char   checksum;
	unsigned char   rq_addr;
	unsigned char   rq_seq;
	unsigned char   cmd;
} __attribute__((packed));


enum {
	APPMSG_SUB_MSG = 1,
	APPMSG_IPMI_MSG
};

struct appmsg_sub_msg {
	unsigned short user_port;
	unsigned char netfn;
	unsigned char cmd;
};

struct appmsg_ipmi_msg {
	unsigned int     id;
	unsigned int     timeo;
	unsigned short   user_port;
	ipmi_json_ipc_header_t    header;
	struct ipmi_addr addr;
	struct ipmi_msg  msg;
};

#define APPMSG_IPMI_MSG_LEN(datalen)	\
	(offset_of(struct appmsg_ipmi_msg, msg.data) + datalen)


/* Message exchange format header between app and ipmi-d */
#define APP_MSG_HDR_SIZE    4
struct app_msg_hdr {
	unsigned short type;
	unsigned short datalen;

	unsigned char  data[APPMSGSIZ - APP_MSG_HDR_SIZE];
};

union app_msg_union {
	struct app_msg_hdr hdr;

	unsigned long buff[APPMSGSIZ/sizeof(long)];
};


#endif

