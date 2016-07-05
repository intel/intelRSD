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


#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ipmi.h"
#include "rmcp.h"
#include "ipmi_log.h"
#include "libsecurec/safe_lib.h"


/**
  *  @brief deliver IPMI msg via RMCP
  *
  *  @param[in] msg IPMI msg
  *  @return
  */
void deliver_ipmi_msg_by_rmcp(struct appmsg_ipmi_msg *msg)
{
	unsigned int host;
	unsigned int port;
	int rmcp_msg_len;
	struct ipmi_msg *ipmi_msg;
	unsigned char rmcp_msg[IPMI_MAX_MSG_LENGTH];

	ipmi_msg = &msg->msg;
	if (ipmi_msg->data_len > IPMI_MAX_DATA_LENGTH) {
		IPMI_LOG_ERR("Too big IPMI message length[%d], ignore!\n", ipmi_msg->data_len);
		return;
	}

	if (ipmi_msg->data_len + offset_of(struct ipmi_msg, data) > IPMI_MAX_MSG_LENGTH) {
		IPMI_LOG_DEBUG("Too big IPMI message to send, ignore!\n");
		return;
	}

	if (ipmi_msg->netfn & 0x01) {
		IPMI_LOG_DEBUG("BMC should have no request to us, ignore this response!\n");
		return;
	}

	host = msg->addr.addr.rmcp.host;
	port = msg->addr.addr.rmcp.port;
	IPMI_LOG_DEBUG("deliver_ipmi_msg_by_rmcp: send to port %d\n", ntohs(port));

	rmcp_msg_len = format_rmcp_ipmi_msg(rmcp_msg, ipmi_msg,
							msg->id, msg->timeo,
							&(msg->addr),
							msg->addr.addr.rmcp.username,
							msg->addr.addr.rmcp.password,
							msg->user_port,
							msg->header);
	if (rmcp_msg_len == 0) {
		IPMI_LOG_DEBUG("The RMCP-IPMI session is not Ready, waiting to send ...\n");
		return;
	}

	if (rmcp_msg_len > IPMI_MAX_MSG_LENGTH) {
		IPMI_LOG_ERR("rmcp msg len too long ...\n");
		return;
	}

	rmcp_send_outbound_msg(rmcp_msg, rmcp_msg_len, host, port);
}

#ifdef IPMI20_SUPPORT
/**
  *  @brief: Thid function is used to process the message for rmcp+(ipmi2.0)
  *  @param[in] rmcp_msg RMCP msg
  *  @param[in] rmcp_len RMCP msg length
  *  @param[in] host IP address
  *  @param[in] port port
  */
static void process_rmcp_plus_msg(unsigned char *rmcp_msg, int rmcp_len, unsigned int host, unsigned int port)
{
	struct rmcp_plus_ipmi_info ipmi_info = {0};

	unsigned char *msg = rmcp_msg;
	unsigned len = rmcp_len;
	int offset = IPMI20_AUTH_TYPE_OFFSET;

	ipmi_info.host = host;
	ipmi_info.port = port;
	ipmi_info.auth_type = msg[offset];

	offset += IPMI20_AUTH_TYPE_LEN;


	/** Payload Type:
	 * [7] - 0b = payload is unencrypted
	 *       1b = payload is encrypted
	 * [6] - 0b = payload is unauthenticated(no AuthCode field
	 *       1b = payload is authenticated(AuthCode field is present
	 * [5:0] = payload type
	 *     Payload Type Numbers:
	 *       Standard Payload Types:
	 *         0h - IPMI Message
	 *         1h - SOL (serial over LAN)
	 *         2h - OEM Explicit
	 *       Session Setup Payload Types:
	 *         10h - RMCP+ Open Session Request
	 *         11h - RMCP+ Open Session Response
	 *         12h - RAKP Message 1
	 *         13h - RAKP Message 2
	 *         14h - RAKP Message 3
	 *         15h - RAKP Message 4
	 *       OEM Payload Type Handles
	 *         20h-27h - Handle values for OEM payloads OEM0 through OEM7,
	 *                   respectively
	 *       all other reserverd
	 */
	ipmi_info.payload_type = msg[offset];
	offset += IPMI20_PAYLOAD_TYPE_LEN;

	/** OEM IANA and OEM Payload ID only present when Payload Type = 02h(OEM Explicit) */
	if ((ipmi_info.payload_type & IPMI20_PAYLOAD_TYPE_MASK) ==
		IPMI20_STANDARD_PAYLOAD_OEM_EXPLICIT) {
		memcpy_s(&ipmi_info.oem_iana, sizeof(ipmi_info.oem_iana), &msg[offset], IPMI20_OEM_IANA_LEN);
		offset += IPMI20_OEM_IANA_LEN;

		memcpy_s(&ipmi_info.oem_payload_id, sizeof(ipmi_info.oem_payload_id), &msg[offset], IPMI20_OEM_PAYLOAD_ID_LEN);
		offset += IPMI20_OEM_PAYLOAD_ID_LEN;
	}

	memcpy_s(&ipmi_info.session_id, sizeof(ipmi_info.session_id), msg + offset, IPMI20_SESSION_ID_LEN);
	offset += IPMI20_SESSION_ID_LEN;

	memcpy_s(&ipmi_info.session_seq, sizeof(ipmi_info.session_seq), msg + offset, IPMI20_SESSION_SEQ_NUM_LEN);
	offset += IPMI20_SESSION_SEQ_NUM_LEN;

	msg += offset;
	len -= offset;

	memcpy_s(&ipmi_info.ipmi_data_len, sizeof(ipmi_info.ipmi_data_len), msg, IPMI20_MSG_PAYLOAD_LEN_LEN);
	ipmi_info.ipmi_data = msg + IPMI20_MSG_PAYLOAD_LEN_LEN;

	/** Be careful, for IPMI v2.0, IPMI session trailer/RSP trailer include
	   integrity pad(mayge absent), next header(1 btye, reserved, always = 07h),
	   Auth Code (Integrity Data).
	   When Integrity data absent, ipmi_info.ipmi_trailer = NULL. */
	ipmi_info.ipmi_trailer_len = len - ipmi_info.ipmi_data_len;
	if (ipmi_info.ipmi_trailer_len > 1)
		ipmi_info.ipmi_trailer = msg + ipmi_info.ipmi_data_len;

	handle_rmcp_plus_ipmi_msg(&ipmi_info);
}
#endif

/**
  *  @brief This function is used to handle the rmcp message, including
  *  rmcp(ipmi1.5) and rmcp+(ipmi2.0).
  *  We can recognize if the message is for ipmi1.5 or ipmi2.0 with
  *  the flag "AUTH_TYPE", and process them separately.
  *  @param[in] msg RMCP msg
  *  @param[in] len RMCP msg length
  *  @param[in] host IP address
  *  @param[in] port port
  *  @retrun
  */
void handle_rmcp_msg(unsigned char *msg, int len, unsigned int host, unsigned int port)
{
	struct rmcp_hdr *rmcp;

#ifdef DEBUG_IPMI
	{
		int __i;
		char __buff[1536];
		char __hdr[64];
		char __body[1280] = {0};
		char __end[64];
		char __tmp[8];
		int len = 0;

		len = snprintf_s_iiii(__hdr, sizeof(__hdr), "RMCP IN("NIPQUAD_FMT")", NIPQUAD(host));
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

	if (len < sizeof(*rmcp))
		return;

	rmcp = (struct rmcp_hdr *)msg;
	msg += sizeof(*rmcp);
	len -= sizeof(*rmcp);

#ifdef IPMI20_SUPPORT
	int ipmi20_msg_len = len;
	unsigned char *ipmi20_msg = msg;
#endif

	if (rmcp->class == RMCP_CLASS_IPMI) {
		int msglen;
		struct rmcp_ipmi_info ipmi_info;

		IPMI_LOG_DEBUG("It is a RMCP IPMI message!\n");

		if (len < IPMI_AUTH_TYPE_LEN) {
			IPMI_LOG_DEBUG("This is an invalid message!\n");
			IPMI_LOG_DEBUG("This RMCP IPMI message is too short!\n");
			return;
		}

		ipmi_info.host = host;
		ipmi_info.port = port;
		ipmi_info.auth_type = msg[IPMI_AUTH_TYPE_OFFSET];
#ifdef IPMI20_SUPPORT
		/** If the message is for RMCP+(IPMI2.0), AUTH_TYPE[3:0] will only be 6h */
		if ((ipmi_info.auth_type & IPMI_AUTH_TYPE_MASK) != IPMI20_AUTH_TYPE_RMCP_PLUS) {
#endif
			if (len < (IPMI_SESSION_HEADER_LEN_MIN + IPMI_PAYLOAD_DATA_LEN_MIN + IPMI_LEGACY_PAD_LEN)) {
				IPMI_LOG_DEBUG("This RMCP IPMI message is too short!\n");
				return;
			}
			/** memcpy(&ipmi_info.session_seq, &msg[1], 4); */
			memcpy_s(&ipmi_info.session_id, sizeof(ipmi_info.session_id), &msg[IPMI_SESSION_ID_OFFSET], IPMI_SESSION_ID_LEN);
			msg += IPMI_MSG_AUTH_CODE_OFFSET;
			len -= IPMI_MSG_AUTH_CODE_OFFSET;

			ipmi_info.auth_code = NULL;
			if (ipmi_info.auth_type != IPMI_SESSION_AUTHTYPE_NONE) {
				ipmi_info.auth_code = msg;
				msg += IPMI_MSG_AUTH_CODE_LEN;
				len -= IPMI_MSG_AUTH_CODE_LEN;
			}

			msglen = *msg++;
			if (msglen < (len - 1) || msglen < (6 + 1)) {
				IPMI_LOG_DEBUG("This RMCP IPMI message is truncated!\n");
				return;
			}

			if ((msg[IPMI_RSLUN_OFFSET] & 0x03) != IPMI_REMOTE_LUN ||
				(msg[IPMI_RQLUN_OFFSET] & 0x03) != IPMI_BMC_CMD_LUN) {
				IPMI_LOG_DEBUG("This RMCP IPMI message addresses are not right!\n");
				return;
			}

			if (ipmi_csum(msg, 3) != 0 || ipmi_csum(msg + 3, msglen - 3) != 0) {
				IPMI_LOG_DEBUG("This RMCP IPMI message has a invalid checksum!\n");
				return;
			}

			ipmi_info.netfn = msg[IPMI_NETFN_OFFSET] >> 2;
			ipmi_info.seq   = msg[IPMI_RQSEQ_OFFSET] >> 2;
			ipmi_info.cmd   = msg[IPMI_CMD_OFFSET];
			ipmi_info.data_len  = msglen - (6 + 1);
			ipmi_info.ipmi_data = &msg[IPMI_DATA_OFFSET];

#ifdef IPMI20_SUPPORT
			/** The message format between get auth_cap and the others for IPMI v2.0
			   are different. */
			if ((ipmi_info.auth_type == IPMI_SESSION_AUTHTYPE_NONE)
				&& (ipmi_info.cmd == IPMI_GET_CHAN_AUTH_CAP)) {
				if (ipmi_info.data_len < 9) {
					IPMI_LOG_DEBUG("For Get Channel CMD, data is too short (len: %d)!\n", ipmi_info.data_len);
					return;
				}

				/* ipmi_data[2] is for Authentication Type
				 * [7] - 1b = IPMI v2.0+ extended capabilities available.
				 *       0b = IPMI v1.5 support only. */
				if ((ipmi_info.ipmi_data[2] & 0x80) != 0) {
					handle_rmcp_plus_get_auth_cap_msg(&ipmi_info);

					return;
				}
			} else if ((ipmi_info.auth_type == IPMI_SESSION_AUTHTYPE_NONE)
				&& (ipmi_info.cmd == IPMI20_GET_CHANNEL_CIPHER_SUITES)) {
				handle_rmcp_plus_get_channel_cipher_suite_msg(&ipmi_info);

				return;
			}
#endif

			handle_rmcp_ipmi_msg(&ipmi_info);
#ifdef IPMI20_SUPPORT
		} else {
			if (len < (IPMI20_SESSION_HEADER_LEN_MIN + IPMI_PAYLOAD_DATA_LEN_MIN)) {
				IPMI_LOG_DEBUG("This RMCP IPMI message is too short!\n");
				return;
			}

			process_rmcp_plus_msg(ipmi20_msg, ipmi20_msg_len, host, port);
		} /** End of if (rmcp->class == RMCP_CLASS_IPMI) */
#endif
	}
}

/**
  *  @brief init RMCP msg handle
  *
  *  @param
  *  @return
  */
void init_rmcp_msg_handler(void)
{
	init_rmcp_session();
}


