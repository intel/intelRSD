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


#ifndef __IPMI_RMCP_H__
#define __IPMI_RMCP_H__

#include "ipmi.h"
#include "rmcp+.h"
/* IPMI v1.5 on LAN */

/* Display an IP address in readable format */
#define NIPQUAD(addr) \
		((unsigned char *)&addr)[0], \
		((unsigned char *)&addr)[1], \
		((unsigned char *)&addr)[2], \
		((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

/* IPMI commands used to active session */
#define IPMI_NETFN_APP			0x6
#define IPMI_GET_CHAN_AUTH_CAP	0x38
#define IPMI_GET_SESSION_CHALL	0x39
#define IPMI_ACTIVATE_SESSION	0x3A
#define IPMI_SET_SESS_PRIV_LEV	0x3B
#define IPMI_GET_DEVICE_ID		0x01
#define IPMI_SOL_ACTIVATING		0x20
#define IPMI_GET_SOL_CONF_PARAM	0x21
#define IPMI_SET_SOL_CONF_PARAM	0x22
#define IPMI_CLOSE_SESSION		0x3C

#define IPMI_AUTH_TYPE_OFFSET       0
#define IPMI_AUTH_TYPE_LEN          1
#define IPMI_AUTH_TYPE_MASK         0x0F

#define IPMI_SESSION_SEQ_NUM_OFFSET (IPMI_AUTH_TYPE_OFFSET + IPMI_AUTH_TYPE_LEN)
#define IPMI_SESSION_SEQ_NUM_LEN    4

#define IPMI_SESSION_ID_OFFSET      (IPMI_SESSION_SEQ_NUM_OFFSET + IPMI_SESSION_SEQ_NUM_LEN)
#define IPMI_SESSION_ID_LEN         4

#define IPMI_MSG_AUTH_CODE_OFFSET   (IPMI_SESSION_ID_OFFSET + IPMI_SESSION_ID_LEN)
#define IPMI_MSG_AUTH_CODE_LEN      16

#define IPMI_MSG_PAYLOAD_LEN_OFFSET (IPMI_MSG_AUTH_CODE_OFFSET + IPMI_MSG_AUTH_CODE_LEN)
#define IPMI_MSG_PAYLOAD_LEN_LEN    1

#define IPMI_SESSION_HEADER_LEN_MIN	(IPMI_AUTH_TYPE_LEN + \
									 IPMI_SESSION_SEQ_NUM_LEN + \
									 IPMI_SESSION_ID_LEN + \
									 IPMI_MSG_PAYLOAD_LEN_LEN)

#define IPMI_PAYLOAD_DATA_LEN_MIN	(1 /* RsAddr */ \
									 + 1 /* net Fn / rsLUN */ \
									 + 1 /* checksum */	\
									 + 1 /* rqAddr */ \
									 + 1 /* rqSeq / rqLUN */ \
									 + 1 /* cmd */ \
									 + 1 /* checksum */)

#define IPMI_LEGACY_PAD_LEN         1

#define IPMI_RSADDR_OFFSET          0
#define IPMI_NETFN_OFFSET           1
#define IPMI_RSLUN_OFFSET           1
#define IPMI_RSLUN_MASK             0x03
#define IPMI_CSUM_OFFSET            2
#define IPMI_RQADDR_OFFSET          3
#define IPMI_RQSEQ_OFFSET           4
#define IPMI_RQLUN_OFFSET           4
#define IPMI_CMD_OFFSET             5
#define IPMI_DATA_OFFSET            6

#define IPMB_RES_SLAVE_ADDR_OFFSET      0
#define IPMB_NETFN_OFFSET               1
#define IPMB_RES_LUN_OFFSET             1
#define IPMB_REQ_SLAVE_ADDR_OFFSET      3
#define IPMB_SEQ_OFFSET                 4
#define IPMB_REQ_LUN_OFFSET             4
#define IPMB_CMD_OFFSET                 5
#define IPMB_SENSOR_NUMBER_OFFSET       6

#define IPMB_LUN_MASK       0x03

#define IPMI_RMCP_PORT		0x26F /* port 623 */
#define RMCP_VERSION_1		0x06
#define RMCP_IPMI_SEQ		0xFF

#define RMCP_TYPE_MASK		0x80
#define RMCP_TYPE_NORM		0x00
#define RMCP_TYPE_ACK		0x80

#define RMCP_CLASS_MASK		0x1F
#define RMCP_CLASS_ASF		0x06
#define RMCP_CLASS_IPMI		0x07
#define RMCP_CLASS_OEM		0x08

struct rmcp_hdr {
	unsigned char ver;
	unsigned char __reserved;
	unsigned char seq;
	unsigned char class;
} __attribute__((packed));



#define ASF_RMCP_IANA		0x000011BE
#define ASF_TYPE_PING		0x80
#define ASF_TYPE_PONG		0x40

struct asf_hdr {
	unsigned int  iana;

	unsigned char type;
	unsigned char tag;
	unsigned char __reserved;
	unsigned char len;
} __attribute__((packed));


#define IPMI_SESSION_AUTHTYPE_NONE			0x0
#define IPMI_SESSION_AUTHTYPE_MD2			0x1
#define IPMI_SESSION_AUTHTYPE_MD5			0x2
#define IPMI_SESSION_AUTHTYPE_PASSWORD		0x4
#define IPMI_SESSION_AUTHTYPE_NONE_BIT		(1 << IPMI_SESSION_AUTHTYPE_NONE)
#define IPMI_SESSION_AUTHTYPE_MD2_BIT		(1 << IPMI_SESSION_AUTHTYPE_MD2)
#define IPMI_SESSION_AUTHTYPE_MD5_BIT		(1 << IPMI_SESSION_AUTHTYPE_MD5)
#define IPMI_SESSION_AUTHTYPE_PASSWORD_BIT	(1 << IPMI_SESSION_AUTHTYPE_PASSWORD)


#define IPMI_SESSION_PRIV_UNSPECIFIED	0x0
#define IPMI_SESSION_PRIV_CALLBACK		0x1
#define IPMI_SESSION_PRIV_USER			0x2
#define IPMI_SESSION_PRIV_OPERATOR		0x3
#define IPMI_SESSION_PRIV_ADMIN			0x4
#define IPMI_SESSION_PRIV_OEM			0x5


#define IPMI_LAN_CHANNEL_E				0x0E


#define IPMI_AUTHSTATUS_PER_MSG_DISABLED		0x10
#define IPMI_AUTHSTATUS_PER_USER_DISABLED		0x08
#define IPMI_AUTHSTATUS_NONNULL_USERS_ENABLED	0x04
#define IPMI_AUTHSTATUS_NULL_USERS_ENABLED		0x02
#define IPMI_AUTHSTATUS_ANONYMOUS_USERS_ENABLED	0x01


struct rmcp_ipmi_info {
	unsigned int host;
	unsigned int port;

	/* unsigned int session_seq; */
	unsigned int  session_id;
	unsigned char auth_type;

	unsigned char seq;
	unsigned char netfn;
	unsigned char cmd;

	int data_len;
	unsigned char *ipmi_data;

	unsigned char *auth_code;
};

struct ipmi_session {
	struct ipmi_session *next;	/* free buffer link */


	struct list_head hash;	/* hashed by IP */
	struct list_head list;	/* added in single list for time thread scanning */

	int state;

#ifdef DEBUG_IPMI
	struct timespec start_time;
#endif
	unsigned int timeout;

	struct list_head msg_list;	/* user ipmi msg waiting for sending before session up */
	struct list_head req_list;

	unsigned int  host;
	unsigned int  port;
	unsigned int  session_id;
	/* unsigned int  outbound_seq; */
	unsigned int  inbound_seq;

	unsigned char curr_seq;		/* IPMI sequence */
	unsigned char auth_type;
	unsigned char auth_status;
	unsigned char privilege;

	unsigned char username[16];
	unsigned char password[16];
	unsigned char challenge[16];

#ifdef IPMI20_SUPPORT
	unsigned char cipher_suite_id;
	unsigned int cipher_suite_id_flag;
	unsigned char req_auth_alg;
	unsigned char req_integrity_alg;
	unsigned char req_crypt_alg;

	unsigned char auth_alg;
	unsigned char integrity_alg;
	unsigned char crypt_alg;

	unsigned char session_random[16];
	unsigned int bmc_id;
	unsigned char bmc_random[16];

	unsigned char key_kg[21];
	unsigned char key_sik[20];
	unsigned char key_k1[20];
	unsigned char key_k2[20];
	unsigned char key_k3[20];

	unsigned int session_seq;

	union {
		struct {
			unsigned char msg_tag;
			unsigned char stat_code;
			unsigned char max_priv_level;
			unsigned int session_id;
			unsigned int bmc_id;
			unsigned char auth_alg;
			unsigned char int_alg;
			unsigned char crypt_alg;
		} open_sess_resp;
		struct {
			unsigned char msg_tag;
			unsigned char stat_code;
			unsigned int session_id;
			unsigned char bmc_random[IPMI20_RAKP_MSG_2_BMC_RANDOM_NUM_LEN+1];
			unsigned char bmc_guid[IPMI20_RAKP_MSG_2_BMC_GUID_LEN+1];
			unsigned char auth_alg[IPMI20_RAKP_MSG_2_AUTH_CODE_MAX_LEN+1];
		} rakp_msg_2;
		struct {
			unsigned char msg_tag;
			unsigned char stat_code;
			unsigned int session_id;
			unsigned char int_check[IPMI20_RAKP_MSG_4_INT_CHECK_MAX_LEN+1];
		} rakp_msg_4;
	} payload;
#endif
};

extern void init_rmcp_session(void);
extern int format_rmcp_ipmi_msg(unsigned char *buff, struct ipmi_msg *ipmi,
			unsigned int msgid, unsigned int timeo,
			struct ipmi_addr *addr, char *username, char *password,
			unsigned short user_port, ipmi_json_ipc_header_t header);
extern void handle_rmcp_ipmi_msg(struct rmcp_ipmi_info *ipmi_info);


#ifdef IPMI20_SUPPORT
extern void handle_rmcp_plus_get_auth_cap_msg(struct rmcp_ipmi_info *ipmi_info);
extern void handle_rmcp_plus_get_channel_cipher_suite_msg(struct rmcp_ipmi_info *ipmi_info);
#endif


#endif

