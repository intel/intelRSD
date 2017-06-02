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


#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/md5.h>

#include "ipmi.h"
#include "rmcp.h"
#include "ipmi_log.h"
#include "ipmi20_crypto.h"
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"

#define IPMI_SESSION_QUEUE_SIZE		256
#define IPMI_REQUEST_QUEUE_SIZE		512
#define IPMI_MSGSENT_QUEUE_SIZE		256

#define IPMI_SESSION_TABLE_SIZE		(64)
#define IPMI_SESSION_TABLE_MASK		(IPMI_SESSION_TABLE_SIZE - 1)
#define IPMI_SESSION_TABLE_HASH(host)	\
		((ntohl(host)) & IPMI_SESSION_TABLE_MASK)


enum ipmi_session_state {
	IPMI_SESS_STATE_AUTH_CAP_REQ_SENT = 1,
	IPMI_SESS_STATE_SESSION_CHALLENGE_SENT,
	IPMI_SESS_STATE_SESSION_ACTIVATE_SENT,
	IPMI_SESS_STATE_SESSION_PRIV_SENT,
	IPMI_SESS_STATE_ACTIVE = 0xFF
};

struct ipmi_msg_sent {
	struct ipmi_msg_sent *next;		/* free buffer link */


	struct list_head list;

	unsigned int    msgid;
	unsigned int    timeo;
	unsigned short  user_port;
	ipmi_json_ipc_header_t header;
	struct ipmi_msg msg;
};

struct rmcp_match_info {
	unsigned char seq;
	unsigned char netfn;
	unsigned char cmd;
};

struct ipmi_req_entry {
	struct ipmi_req_entry *next;	/* free buffer link */


	struct list_head link;	/* Link to "struct ipmi_session" */
	struct list_head list;	/* ALL the IPMI Req we have sent */

	unsigned int msgid;
	unsigned short user_port;
	ipmi_json_ipc_header_t header;

	struct rmcp_match_info match;

#ifdef DEBUG_IPMI
	struct timespec start_time;
#endif
	unsigned int timeout;
};

struct ipmi_session_table {
	pthread_mutex_t  lock;

	struct ipmi_session   *ses_freelist;
	struct ipmi_msg_sent  *msg_freelist;
	struct ipmi_req_entry *req_freelist;

	/* Scaned by time thread */
	struct list_head request_list;
	struct list_head session_list;

	struct list_head session_table[IPMI_SESSION_TABLE_SIZE];

	pthread_t time_tid;
};

static struct ipmi_session_table session_table;

/**
  *  @brief convert to little-endian 32bits
  *
  *  @param
  *  @return
  */
static void cpu_to_le32(unsigned int *p)
{
	unsigned int t;

	if (p == NULL)
		return;

	unsigned char *cp = (unsigned char *)p;

	t = (unsigned int) ((unsigned int) cp[3] << 8 | cp[2]) << 16 |
						((unsigned int) cp[1] << 8 | cp[0]);

	*p = t;
}

/**
  *  @brief multi-session authcode generation for MD5
  *  H(password + session_id + msg + session_seq + password)
  */
void ipmi_auth_md5(struct ipmi_session *s, unsigned char *authcode,
			unsigned char *data, unsigned int data_len)
{
	unsigned int seq;
	MD5_CTX ctx;

	seq = s->inbound_seq;
	cpu_to_le32(&seq);

	MD5_Init(&ctx);

	MD5_Update(&ctx, (unsigned char *)s->password, 16);
	MD5_Update(&ctx, (unsigned char *)&s->session_id, 4);
	MD5_Update(&ctx, (unsigned char *)data, data_len);
	MD5_Update(&ctx, (unsigned char *)&seq, 4);
	MD5_Update(&ctx, (unsigned char *)s->password, 16);

	MD5_Final(authcode, &ctx);
}

/**
  *  @brief find session from the session list
  *
  *  @param[in] sess IPMI session
  *  @param[in] match
  *  @param[in] timeo time out
  *  @param[in] msgid IPMI msg id
  *  @param[in] user_port
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int rmcp_intf_find_seq(struct ipmi_session *sess,
			struct rmcp_match_info *match,
			unsigned int *msgid,
			unsigned short *user_port,
			ipmi_json_ipc_header_t *header)
{
	int rv = -1;
	unsigned char netfn = match->netfn & 0x3E;	/* request netfn */
	struct ipmi_req_entry *req = NULL, *pos;
	struct ipmi_session_table *table = &session_table;

	list_for_each_entry(pos, &sess->req_list, link) {
		if (pos->match.seq == match->seq &&
		    pos->match.netfn == netfn &&
		    pos->match.cmd == match->cmd) {
			req = pos;
			break;
		}
	}

	if (req == NULL) {
		IPMI_LOG_ERR("Req found is NULL...\n");
		goto ret;
	}

	*msgid = req->msgid;
	*user_port = req->user_port;

	/*copy header*/
	memset(header->method, 0, sizeof(JRPC_METHOD_MAX));
	header->ip = req->header.ip;
	header->port = req->header.port;
	header->json_ipc_id = req->header.json_ipc_id;
	if (strnlen_s((const char *)(req->header.method), sizeof(req->header.method)-1) < JRPC_METHOD_MAX)
		memcpy_s(header->method, sizeof(header->method), req->header.method, strnlen_s((const char *)(req->header.method), sizeof(req->header.method)-1));

	list_del(&req->link);
	list_del(&req->list);
	req->next = table->req_freelist;
	table->req_freelist = req;

	rv = 0;

ret:
	return rv;
}

/**
  *  @brief process next RMCP message
  *
  *  @param[in] sess IPMI session
  *  @param[in] match
  *  @param[in] timeo time out
  *  @param[in] msgid IPMI msg id
  *  @param[in] user_port
  *  @return
  *  @retval -1 failure
  *  @retval 0 successful
  */
static int rmcp_intf_next_seq(struct ipmi_session *sess,
			struct rmcp_match_info *match,
			unsigned int timeo,
			unsigned int msgid, unsigned short user_port,
			ipmi_json_ipc_header_t header)
{
	int rv = -1;
	unsigned int i;
	unsigned char netfn = match->netfn & 0x3E;	/* request netfn */
	unsigned char seq = IPMB_SEQ_SIZE;
	struct ipmi_req_entry *req, *pos;
	struct ipmi_session_table *table = &session_table;

	req = table->req_freelist;
	if (req == NULL) {
		IPMI_LOG_DEBUG("RMCP-Intf: No Req Entry Buff, adjust #IPMI_REQUEST_QUEUE_SIZE(%d)!\n",
					   IPMI_REQUEST_QUEUE_SIZE);
		goto ret;
	}

	for (i = sess->curr_seq; IPMB_SEQ_HASH(i+1) != sess->curr_seq; i = IPMB_SEQ_HASH(i+1)) {
		int conflict = 0;

		list_for_each_entry(pos, &sess->req_list, link) {
			if (pos->match.seq == i &&
			    pos->match.netfn == netfn &&
			    pos->match.cmd == match->cmd) {
				conflict = 1;
				break;
			}
		}

		if (!conflict) {
			IPMI_LOG_DEBUG("RMCP IPMI to "NIPQUAD_FMT" NetFn:%02X,Cmd:%02X uses Seq:%02X\n",
						   NIPQUAD(sess->host),
						   netfn, match->cmd, i);
			seq = i;
			break;
		}
	}

	if (seq == IPMB_SEQ_SIZE) {
		IPMI_LOG_DEBUG("No avail seq number for IPMI msg in RMCP interface!\n");
		goto ret;
	}

	table->req_freelist = req->next;
	req->next = NULL;

	list_add_tail(&req->link, &sess->req_list);
	list_add_tail(&req->list, &table->request_list);

	req->msgid = msgid;
	req->user_port = user_port;
	req->match.seq = seq;
	req->match.netfn = netfn;
	req->match.cmd = match->cmd;

	/*copy header*/
	memset(req->header.method, 0, sizeof(JRPC_METHOD_MAX));
	req->header.ip = header.ip;
	req->header.port = header.port;
	req->header.json_ipc_id = header.json_ipc_id;
	if (strnlen_s((const char *)header.method, sizeof(header.method)-1) < JRPC_METHOD_MAX)
		memcpy_s(req->header.method, sizeof(req->header.method), header.method, strnlen_s((const char *)header.method, sizeof(header.method)-1));

#ifdef DEBUG_IPMI
	clock_gettime(CLOCK_REALTIME, &req->start_time);
#endif
	req->timeout = timeo;

	match->seq = seq;
	sess->curr_seq = IPMB_SEQ_HASH(seq + 1);
	
	rv = 0;

ret:
	return rv;
}

/**
  *  @brief release IPMI session
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void relase_ipmi_session(struct ipmi_session *sess)
{
	struct ipmi_msg_sent  *msg, *msg_next;
	struct ipmi_req_entry *req, *req_next;
	struct ipmi_session_table *table = &session_table;

	list_del(&sess->list);
	list_del(&sess->hash);

	list_for_each_entry_safe(msg, msg_next, &sess->msg_list, list) {
		list_del(&msg->list);

		msg->next = table->msg_freelist;
		table->msg_freelist = msg;
	}

	list_for_each_entry_safe(req, req_next, &sess->req_list, link) {
		list_del(&req->link);
		list_del(&req->list);

		req->next = table->req_freelist;
		table->req_freelist = req;
	}

	sess->next = table->ses_freelist;
	table->ses_freelist = sess;
}

/**
  *  @brief
  *
  *  @param
  *  @return
  */
static void *rmcp_time_thread(void *unused)
{
	struct timespec expire;
	struct ipmi_session *sess, *sess_next;
	struct ipmi_req_entry *req, *req_next;
	struct ipmi_session_table *table = &session_table;
	pthread_cond_t nul_cond = PTHREAD_COND_INITIALIZER;

	prctl(PR_SET_NAME, "rmcp_time_thread");

	for (;;) {
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

		list_for_each_entry_safe(sess, sess_next, &table->session_list, list) {
			sess->timeout -= IPMI_PERIOD_TIME_MS;
			if (sess->timeout > 0)
				continue;

			IPMI_LOG_INFO("RMCP session to "NIPQUAD_FMT" expired %ld:%ld -> %ld:%ld\n",
						  NIPQUAD(sess->host),
						  sess->start_time.tv_sec, sess->start_time.tv_nsec,
						  expire.tv_sec, expire.tv_nsec);

			relase_ipmi_session(sess);
		}

		list_for_each_entry_safe(req, req_next, &table->request_list, list) {
			req->timeout -= IPMI_PERIOD_TIME_MS;
			if (req->timeout > 0)
				continue;

			IPMI_LOG_INFO("RMCP request (%02X:%02X) expired %ld:%ld -> %ld:%ld\n",
						  req->match.netfn, req->match.cmd,
						  req->start_time.tv_sec, req->start_time.tv_nsec,
						  expire.tv_sec, expire.tv_nsec);

			list_del(&req->list);
			list_del(&req->link);

			req->next = table->req_freelist;
			table->req_freelist = req;
		}

		pthread_mutex_unlock(&table->lock);
	}

	return NULL;
}


/**
  *  @brief build IPMIv2 LAN command
  *
  *  @param[in] buff output IPMI msg buffer
  *  @param[in] sess IPMI session
  *  @param[in] req
  *  @param[in] seq IPMI sequence nnumber
  *  @return
  */
static int ipmi_lan_build_cmd(unsigned char *buff, struct ipmi_session *sess,
			struct ipmi_msg *req, unsigned char seq)
{
	static struct rmcp_hdr rmcp = {
		.ver		= RMCP_VERSION_1,
		.__reserved = 0,
		.class		= RMCP_CLASS_IPMI,
		.seq		= RMCP_IPMI_SEQ,
	};
	int len;
	unsigned char *ipmi;
	unsigned char *pauth = NULL;

	if (req->data_len > IPMI_MAX_DATA_LENGTH) {
		IPMI_LOG_ERR("req->data_len[%d] exceed IPMI_MAX_DATA_LENGTH[%d]\n", req->data_len, IPMI_MAX_DATA_LENGTH);
		return 0;
	}

	memcpy_s(buff, sizeof(rmcp), &rmcp, sizeof(rmcp));
	len = sizeof(rmcp);

	ipmi = &buff[len];
	ipmi[IPMI_AUTH_TYPE_OFFSET] = sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT ? sess->auth_type : 0;
	ipmi[IPMI_SESSION_SEQ_NUM_OFFSET] = (sess->inbound_seq & 0xFF);
	ipmi[IPMI_SESSION_SEQ_NUM_OFFSET+1] = (sess->inbound_seq >>  8) & 0xFF;
	ipmi[IPMI_SESSION_SEQ_NUM_OFFSET+2] = (sess->inbound_seq >> 16) & 0xFF;
	ipmi[IPMI_SESSION_SEQ_NUM_OFFSET+3] = (sess->inbound_seq >> 24) & 0xFF;
	memcpy_s(&ipmi[IPMI_SESSION_ID_OFFSET], IPMI_SESSION_ID_LEN, &sess->session_id, IPMI_SESSION_ID_LEN);
	len += IPMI_MSG_AUTH_CODE_OFFSET;

	if (sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT && sess->auth_type) {
		pauth = &buff[len];
		len += IPMI_MSG_AUTH_CODE_LEN;
	}

	buff[len++] = req->data_len + (6 + 1);

	ipmi = &buff[len];
	ipmi[IPMI_RSADDR_OFFSET] = IPMI_BMC_SLAVE_ADDR;
	ipmi[IPMI_NETFN_OFFSET] = (req->netfn << 2) | (IPMI_BMC_CMD_LUN);
	ipmi[IPMI_CSUM_OFFSET] = ipmi_csum(ipmi, 2);
	ipmi[IPMI_RQADDR_OFFSET] = IPMI_REMOTE_SWID;
	ipmi[IPMI_RQLUN_OFFSET] = (seq << 2) | (IPMI_REMOTE_LUN);
	ipmi[IPMI_CMD_OFFSET] = (req->cmd);
	len += 6;

	if (req->data_len > 0) {
		memcpy_s(&ipmi[IPMI_DATA_OFFSET], req->data_len, req->data, req->data_len);
		len += req->data_len;
	}

	buff[len++] = ipmi_csum(&ipmi[3], req->data_len + 3);

	if (pauth != NULL) {
		switch (sess->auth_type) {
		case IPMI_SESSION_AUTHTYPE_MD5:
			ipmi_auth_md5(sess, pauth, ipmi, ipmi[-1]);
			break;

		case IPMI_SESSION_AUTHTYPE_MD2:
			/** removed for safety since MD2 algorithm has been cracked */
			/*ipmi_auth_md2(sess, pauth, ipmi, ipmi[-1]);*/
			break;

		default:
			memcpy_s(pauth, 16, sess->password, 16);
			break;
		}
	}

	if (sess->inbound_seq) {
		sess->inbound_seq++;
		if (sess->inbound_seq == 0)
			sess->inbound_seq++;
	}

	return len;
}


/**
  *  @brief build IPMIv2 LAN bridge command
  *
  *  @param[in] buff output IPMI msg buffer
  *  @param[in] sess IPMI session
  *  @param[in] req
  *  @param[in] seq IPMI sequence nnumber
  *  @return
  */
static int ipmi_lan_build_cmd_br(unsigned char *buff, struct ipmi_session *sess,
			struct ipmi_msg *req, unsigned char seq, int bridge_level)
{
		struct rmcp_hdr rmcp = {
		.ver		= RMCP_VERSION_1,
		.class		= RMCP_CLASS_IPMI,
		.seq		= 0xff,
	};
	unsigned char *msg;
	int cs, mp, tmp;
	int len = 0;
	int cs2, cs3 = 0;
	unsigned char *pauth = NULL;
	struct ipmi_session_hdr *ipmi_session_hdr;
	struct ipmi_msg_hdr *ipmi_br_msg, *ipmi_dual_br_msg, *ipmi_msg_hdr;

	if (seq >= IPMB_SEQ_SIZE) {
		seq = 0;
	}

	/* expand req data_len with lan encaption */
	len = req->data_len + 29;

	if (sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT && sess->auth_type) {
		len += 16;
	}

	msg = malloc(len);
	if (msg == NULL) {
		IPMI_LOG_DEBUG("ipmitool: malloc failure");
		return 0;
	}
	memset(msg, 0, len);

	/* rmcp header */
	memcpy_s(msg, sizeof(rmcp), &rmcp, sizeof(rmcp));
	len = sizeof(rmcp);

	/* ipmi session header */
	ipmi_session_hdr = (struct ipmi_session_hdr *)(msg+len);
	ipmi_session_hdr->authtype = sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT ? sess->auth_type : 0;
	ipmi_session_hdr->seq[0] = sess->inbound_seq & 0xff;
	ipmi_session_hdr->seq[1] = (sess->inbound_seq >> 8) & 0xff;
	ipmi_session_hdr->seq[2] = (sess->inbound_seq >> 16) & 0xff;
	ipmi_session_hdr->seq[3] = (sess->inbound_seq >> 24) & 0xff;
	ipmi_session_hdr->session_id = sess->session_id;
	len += sizeof(*ipmi_session_hdr);
	pauth = msg+len;

	/* ipmi session authcode */
	if (sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT && sess->auth_type) {
		memcpy_s(msg+len, 16, sess->password, 16);
		len += 16;
	}

	/* message length */
	if (bridge_level == IPMI_BRIDGE_MSG_DUAL)
		msg[len++] = req->data_len + (6 + 1 + 8 + 8);
	else
		msg[len++] = req->data_len + (6 + 1 + 8);

	cs = mp = len;

	/* bridged msg encap */
	ipmi_br_msg = (struct ipmi_msg_hdr *)(msg+len);
	ipmi_br_msg->rs_addr = IPMI_BMC_SLAVE_ADDR;
	ipmi_br_msg->netfun_lun = (req->netfn << 2) | (IPMI_BMC_CMD_LUN);
	ipmi_br_msg->checksum = ipmi_csum(msg+len, 2);/* from beginning of ipmi_br_msg to checksum */
	cs2 = len + 3; /* cs is at current postion */
	ipmi_br_msg->rq_addr = IPMI_REMOTE_SWID;
	ipmi_br_msg->rq_seq = seq << 2;
	ipmi_br_msg->cmd = req->cmd;
	len += sizeof(*ipmi_br_msg);
	/* dual bridge msg encap */
	if (bridge_level == IPMI_BRIDGE_MSG_DUAL) {
		msg[len++] = (0x40|req->union_app_req.bridge.transit_channel);
		ipmi_dual_br_msg = (struct ipmi_msg_hdr *)(msg+len);
		ipmi_dual_br_msg->rs_addr = req->union_app_req.bridge.transit_addr;
		ipmi_dual_br_msg->netfun_lun = req->netfn << 2;
		ipmi_dual_br_msg->checksum = ipmi_csum(msg+len, 2);
		cs3 = len + 3;
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
	tmp = len - cs2;
	msg[len++] = ipmi_csum(msg+cs2, tmp);

	if (sess->state >= IPMI_SESS_STATE_SESSION_ACTIVATE_SENT && sess->auth_type) {
		switch (sess->auth_type) {
		case IPMI_SESSION_AUTHTYPE_MD5:
			ipmi_auth_md5(sess, pauth, msg+mp, msg[mp-1]);
			break;

		case IPMI_SESSION_AUTHTYPE_MD2:
			/** removed for safety since MD2 algorithm has been cracked */
			/* ipmi_auth_md2(sess, pauth, msg+mp, msg[mp-1]); */
			break;

		default:
			memcpy_s(pauth, 16, sess->password, 16);
			break;
		}
	}

	if (sess->inbound_seq) {
		sess->inbound_seq++;
		if (sess->inbound_seq == 0) {
			sess->inbound_seq++;
		}
	}

	memcpy_s(buff, len, msg, len);

	free(msg);

	return len;
}


/**
  *  @brief flush IPMI user message
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void flush_ipmi_user_msg(struct ipmi_session *sess)
{
	int msglen;
	struct rmcp_match_info match;
	struct ipmi_msg_sent *msg, *msg_next;
	struct ipmi_session_table *table = &session_table;
	unsigned char buff[IPMI_MAX_MSG_LENGTH];

	memset(buff, 0, IPMI_MAX_MSG_LENGTH);
	sess->timeout = RMCP_IPMI_SESSION_IDLE_TIMEOUT_MS;

	list_for_each_entry_safe(msg, msg_next, &sess->msg_list, list) {
		list_del(&msg->list);

		msg->next = table->msg_freelist;
		table->msg_freelist = msg;

		match.netfn = msg->msg.netfn;
		match.cmd   = msg->msg.cmd;
		if (rmcp_intf_next_seq(sess, &match, msg->timeo, msg->msgid, msg->user_port, msg->header) != 0)
			continue;

		if (IPMI_BRIDGE_MSG_NONE != msg->msg.union_app_req.bridge.bridge_level) {
			//match.netfn = msg->msg.union_app_req.bridge.br_netfn;
			match.netfn = IPMI_BRIDGE_NETFN_APP;
			if (IPMI_BRIDGE_MSG_SINGLE == msg->msg.union_app_req.bridge.bridge_level)
				match.cmd   = msg->msg.union_app_req.bridge.br_cmd;
			else
				match.cmd   = IPMI_BRIDGE_IPMI_CMD;

			if (rmcp_intf_next_seq(sess, &match, msg->timeo, msg->msgid, msg->user_port, msg->header) != 0)
				continue;

			msglen = ipmi_lan_build_cmd_br(buff, sess, &msg->msg, match.seq, msg->msg.union_app_req.bridge.bridge_level);
		} else {
			msglen = ipmi_lan_build_cmd(buff, sess, &msg->msg, match.seq);
			if (msglen == 0)
				continue;
		}

		if (msglen > IPMI_MAX_MSG_LENGTH)
			continue;

		rmcp_send_outbound_msg(buff, msglen, sess->host, sess->port);
	}
}


/**
  *  @brief send session privlvl command
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_privlvl_cmd(struct ipmi_session *sess)
{
	int msglen;
	struct ipmi_msg ipmi;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];

	memset(msg, 0, IPMI_MAX_MSG_LENGTH);
	if (sess->privilege <= IPMI_SESSION_PRIV_USER) {
		IPMI_LOG_DEBUG("No need to set higher session privilege!\n");

		sess->state = IPMI_SESS_STATE_ACTIVE;
		flush_ipmi_user_msg(sess);

		return;
	}

	sess->state = IPMI_SESS_STATE_SESSION_PRIV_SENT;

	ipmi.netfn	  = IPMI_NETFN_APP;
	ipmi.cmd	  = IPMI_SET_SESS_PRIV_LEV;
	ipmi.data_len = 1;
	ipmi.data[0] = sess->privilege;

	msglen = ipmi_lan_build_cmd(msg, sess, &ipmi, sess->curr_seq++);
	if (msglen == 0)
		return;

	rmcp_send_outbound_msg(msg, msglen, sess->host, sess->port);
}

/**
  *  @brief handle session privlvl response
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_session_privlvl_resp(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	if (ipmi_info->auth_type != sess->auth_type ||
		ipmi_info->session_id != sess->session_id ||
		ipmi_info->data_len < 2 ||
		ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid session privlvl response, with ccode %02X, len %d!\n",
				   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	IPMI_LOG_INFO("New Priv Level  : %02X\n", ipmi_info->ipmi_data[1]);

	sess->state = IPMI_SESS_STATE_ACTIVE;

	flush_ipmi_user_msg(sess);
}

/**
  *  @brief send activate session command
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_activate_session_cmd(struct ipmi_session *sess)
{
	int msglen;
	unsigned int seq;
	struct ipmi_msg ipmi;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];

	memset(msg, 0, IPMI_MAX_MSG_LENGTH);
	seq = (random() & 0x0FFFFFFF) | 0x00000001;

	sess->state = IPMI_SESS_STATE_SESSION_ACTIVATE_SENT;

	ipmi.netfn    = IPMI_NETFN_APP;
	ipmi.cmd      = IPMI_ACTIVATE_SESSION;
	ipmi.data_len = 22;
	ipmi.data[0] = sess->auth_type;
	ipmi.data[1] = sess->privilege;
	memcpy_s(&ipmi.data[2], 16, sess->challenge, 16);
	ipmi.data[18] = (seq & 0xFF);
	ipmi.data[19] = (seq >>  8) & 0xFF;
	ipmi.data[20] = (seq >> 16) & 0xFF;
	ipmi.data[21] = (seq >> 24) & 0xFF;

	msglen = ipmi_lan_build_cmd(msg, sess, &ipmi, sess->curr_seq++);
	if (msglen == 0)
		return;

	rmcp_send_outbound_msg(msg, msglen, sess->host, sess->port);
}

/**
  *  @brief handle activate session response
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_activate_session_resp(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	if (ipmi_info->auth_type    != sess->auth_type ||
	    ipmi_info->session_id   != sess->session_id ||
	    ipmi_info->data_len      < 11 ||
	    ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid activate session response, with ccode %02X, len %d!\n",
				   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	if (sess->auth_status & IPMI_AUTHSTATUS_PER_MSG_DISABLED) {
		sess->auth_type = IPMI_SESSION_AUTHTYPE_NONE;
		IPMI_LOG_DEBUG("Auth is None for 'Per-message Authentication' disabled!\n");
	} else if (sess->auth_type != (ipmi_info->ipmi_data[1] & 0xF)) {
		IPMI_LOG_DEBUG("Invalid Session AuthType %u in response!\n",
				   ipmi_info->ipmi_data[1] & 0xF);
		return;
	}

	memcpy_s(&sess->session_id, sizeof(sess->session_id), &ipmi_info->ipmi_data[2], 4);
	sess->inbound_seq = ipmi_info->ipmi_data[6] |
						ipmi_info->ipmi_data[7] << 8 |
						ipmi_info->ipmi_data[8] << 16 |
						ipmi_info->ipmi_data[9] << 24;

	if (sess->inbound_seq == 0)
		sess->inbound_seq++;

	IPMI_LOG_INFO("*******************************************************\n");
	IPMI_LOG_INFO("RMCP-IPMI Session to "NIPQUAD_FMT" Activated\n", NIPQUAD(sess->host));
	IPMI_LOG_INFO("  Auth Type       : %02X\n", ipmi_info->ipmi_data[1]);
	IPMI_LOG_INFO("  Max Priv Level  : %02X\n", ipmi_info->ipmi_data[10]);
	IPMI_LOG_INFO("  Session ID      : %08X\n", sess->session_id);
	IPMI_LOG_INFO("  Inbound Seq     : %08X\n", sess->inbound_seq);
	IPMI_LOG_INFO("*******************************************************\n");

	send_session_privlvl_cmd(sess);
}

/**
  *  @brief send session challenge command
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_challenge_cmd(struct ipmi_session *sess)
{
	int msglen;
	struct ipmi_msg ipmi;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];

	memset(msg, 0, IPMI_MAX_MSG_LENGTH);
	sess->state = IPMI_SESS_STATE_SESSION_CHALLENGE_SENT;

	ipmi.netfn    = IPMI_NETFN_APP;
	ipmi.cmd      = IPMI_GET_SESSION_CHALL;
	ipmi.data_len = 17;
	ipmi.data[0] = sess->auth_type;
	memcpy_s(&ipmi.data[1], 16, sess->username, 16);

	msglen = ipmi_lan_build_cmd(msg, sess, &ipmi, sess->curr_seq++);
	if (msglen == 0)
		return;

	rmcp_send_outbound_msg(msg, msglen, sess->host, sess->port);
}

/**
  *  @brief handle session challenge response msg
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_session_challenge_resp(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	if (ipmi_info->auth_type    != IPMI_SESSION_AUTHTYPE_NONE ||
	    ipmi_info->session_id   != 0 ||
	    ipmi_info->data_len      < 21 ||
	    ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid session challenge response, with ccode %02X, len %d!\n",
					   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	memcpy_s(&sess->session_id, sizeof(sess->session_id), &ipmi_info->ipmi_data[1], 4);
	memcpy_s(sess->challenge, sizeof(sess->challenge), &ipmi_info->ipmi_data[5], 16);

	IPMI_LOG_INFO("Opening RMCP-IPMI Session to "NIPQUAD_FMT"\n", NIPQUAD(sess->host));
	IPMI_LOG_INFO("  Temporary Session ID : %08X\n", sess->session_id);

	send_activate_session_cmd(sess);
}

/**
  *  @brief send author capability command
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_auth_capabilities_cmd(struct ipmi_session *sess)
{
	int msglen;
	struct ipmi_msg ipmi;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];

	memset(msg, 0, IPMI_MAX_MSG_LENGTH);
	sess->state = IPMI_SESS_STATE_AUTH_CAP_REQ_SENT;

	ipmi.netfn    = IPMI_NETFN_APP;
	ipmi.cmd      = IPMI_GET_CHAN_AUTH_CAP;
	ipmi.data_len = 2;

#ifdef IPMI20_SUPPORT
	/* [7] - 1b = get IPMI v2.0+ extended data.
	       - 0b = Backward compatible with IPMI v1.5. Result data only returns
		     bytes 1:9, bit[7] of byte 3(Authentication Type support) returns
			 as 0b, byte 5 returns 00h.

			 By default, select IPMI v2.0
	*/
	ipmi.data[0] = IPMI_LAN_CHANNEL_E | 0x80;

#else
	ipmi.data[0] = IPMI_LAN_CHANNEL_E;
#endif

	ipmi.data[1] = sess->privilege;

	msglen = ipmi_lan_build_cmd(msg, sess, &ipmi, sess->curr_seq++);
	if (msglen == 0)
		return;

	rmcp_send_outbound_msg(msg, msglen, sess->host, sess->port);
}

/**
  *  @brief handle author capability response msg
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_auth_capabilities_resp(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	int password;
	unsigned char auth;

	if (ipmi_info->auth_type    != IPMI_SESSION_AUTHTYPE_NONE ||
	    ipmi_info->session_id   != 0 ||
	    ipmi_info->data_len      < 9 ||
	    ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid auth cap response, with ccode %02X, len %d!\n",
					   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	auth = ipmi_info->ipmi_data[2];
	password = sess->password[0] != 0 ? 1 : 0;

	if (password != 0 && (auth & IPMI_SESSION_AUTHTYPE_MD5_BIT) != 0) {
		IPMI_LOG_INFO("Use MD5 Authentication!\n");
		sess->auth_type = IPMI_SESSION_AUTHTYPE_MD5;
	} else if (password != 0 && (auth & IPMI_SESSION_AUTHTYPE_MD2_BIT) != 0) {
		IPMI_LOG_INFO("Use MD2 Authentication!\n");
		sess->auth_type = IPMI_SESSION_AUTHTYPE_MD2;
	} else if (password != 0 && (auth & IPMI_SESSION_AUTHTYPE_PASSWORD_BIT) != 0) {
		IPMI_LOG_INFO("Use Password Authentication!\n");
		sess->auth_type = IPMI_SESSION_AUTHTYPE_PASSWORD;
	} else if ((auth & IPMI_SESSION_AUTHTYPE_NONE_BIT) != 0) {
		IPMI_LOG_INFO("Use None Authentication!\n");
		sess->auth_type = IPMI_SESSION_AUTHTYPE_NONE;
	} else {
		IPMI_LOG_DEBUG("None of the BCM's Authentication supported!\n");
		return;
	}

	sess->auth_status = ipmi_info->ipmi_data[3];

	IPMI_LOG_INFO("BMC "NIPQUAD_FMT" 'Per-message Authentication' is %sabled!\n",
			  NIPQUAD(sess->host),
			  (sess->auth_status & IPMI_AUTHSTATUS_PER_MSG_DISABLED) != 0 ?
			  "dis" : "en");

	send_session_challenge_cmd(sess);
}

/**
 * RCMP+, IPMI2.0
 */
#ifdef IPMI20_SUPPORT
enum ipmi20_session_state {
	IPMI20_SESS_STATE_AUTH_CAP_REQ_SENT = 1,
	IPMI20_GET_CHANNEL_CIPHER_SUITES_SENT,
	IPMI20_SESS_STATE_OPEN_SESSION_SENT,
	IPMI20_SESS_STATE_RAKP_1_SENT,
	IPMI20_SESS_STATE_RAKP_3_SENT,
	IPMI20_SESS_STATE_SESSION_PRIV_SENT,
	IPMI20_SESS_STATE_ACTIVE = IPMI_SESS_STATE_ACTIVE
};

/* Now, we support 0, 1, 2, 3, 6, 7, 11 */
static unsigned int cipher_suite_id_flag = 0x08cf;
static unsigned int bmc_cipher_suite_id_flag = 0x00;

/**
  *  @brief build IPMIv2 LAN+ bridge command
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @param[in] req request data
  *  @param[in] seq sequence number
  *  @param[in] request data lenght
  *  @return output IPMIv2 command buffer
  */
static unsigned char *ipmi20_lan_plus_build_cmd_br(struct ipmi_session *sess,
												struct rmcp_plus_ipmi_info *ipmi_info,
												struct ipmi_msg *req,
												unsigned char seq,
												int *msglen, int bridge_level)
{
	unsigned char *buf = NULL;
	unsigned char data[IPMI_MAX_MSG_LENGTH];
	unsigned char *data_crypt = NULL;
	int data_crypt_len = 0;
	struct ipmi_session_hdr *ipmi_session_hdr;
	struct ipmi_msg_hdr *ipmi_br_msg, *ipmi_dual_br_msg, *ipmi_msg_hdr;
	int cs, mp, tmp;
	int len = 0;
	int cs2, cs3 = 0;

	static struct rmcp_hdr rmcp = {
		.ver		= RMCP_VERSION_1,
		.__reserved = 0,
		.seq		= RMCP_IPMI_SEQ,
		.class		= RMCP_CLASS_IPMI,
	};

	int buf_len = 0;

	memset(data, 0, IPMI_MAX_MSG_LENGTH);
	buf = (unsigned char *)malloc(IPMI_MAX_MSG_LENGTH);
	if (NULL == buf) {
		IPMI_LOG_ERR("malloc failed\n");
		return NULL;
	}

	memset(buf, 0, IPMI_MAX_MSG_LENGTH);

	/* RMCP Header */
	memcpy_s(buf, sizeof(rmcp), &rmcp, sizeof(rmcp));
	buf_len += sizeof(rmcp);

	/* IPMI Session Header */
	buf[buf_len++] = IPMI20_AUTH_TYPE_RMCP_PLUS; /* for IPMI v2.0 only */
	buf[buf_len++] = ipmi_info->payload_type;

	/* OEM IANA and OEM Payload ID only present when payload type = 02h */
	if ((ipmi_info->payload_type & 0x3f) == IPMI20_STANDARD_PAYLOAD_OEM_EXPLICIT) {
		/* TODO: Now, assume it's absent */
	}

	memcpy_s(buf + buf_len, IPMI20_SESSION_ID_LEN, &(ipmi_info->session_id), IPMI20_SESSION_ID_LEN);
	buf_len += IPMI20_SESSION_ID_LEN;

	memcpy_s(buf + buf_len, IPMI20_SESSION_SEQ_NUM_LEN, &(ipmi_info->session_seq), IPMI20_SESSION_SEQ_NUM_LEN);
	buf_len += IPMI20_SESSION_SEQ_NUM_LEN;

	if ((sess->state > IPMI20_SESS_STATE_RAKP_3_SENT)
		&& (NULL != req)) {
		int i = 0;
		int pad_len = 0;
		unsigned char *ipmi_data_len_pos = buf + buf_len;
		unsigned char auth_code[16] = {0};

		/* bridged msg encap */
		ipmi_br_msg = (struct ipmi_msg_hdr *)data;
		ipmi_br_msg->rs_addr = IPMI_BMC_SLAVE_ADDR;
		ipmi_br_msg->netfun_lun = (req->netfn << 2) | (IPMI_BMC_CMD_LUN);
		ipmi_br_msg->checksum = ipmi_csum(data, 2);/* from beginning of ipmi_br_msg to checksum */
		cs2 = 3; /* cs is at current postion */
		ipmi_br_msg->rq_addr = IPMI_REMOTE_SWID;
		ipmi_br_msg->rq_seq = seq << 2;
		ipmi_br_msg->cmd = req->cmd;
		len += sizeof(*ipmi_br_msg);
		/* dual bridge msg encap */
		if (bridge_level == IPMI_BRIDGE_MSG_DUAL) {
			data[len++] = (0x40|req->union_app_req.bridge.transit_channel);
			ipmi_dual_br_msg = (struct ipmi_msg_hdr *)(data + len);
			ipmi_dual_br_msg->rs_addr = req->union_app_req.bridge.transit_addr;
			ipmi_dual_br_msg->netfun_lun = req->netfn << 2;
			ipmi_dual_br_msg->checksum = ipmi_csum(data + len, 2);
			cs3 = len + 3;
			ipmi_dual_br_msg->rq_seq = seq << 2;
			ipmi_dual_br_msg->cmd = req->cmd;
			len += sizeof(*ipmi_dual_br_msg);
		}
		data[len++] = (0x40|req->union_app_req.bridge.target_channel); /* Track request*/
		cs = len;

		/* ipmi message header */
		ipmi_msg_hdr = (struct ipmi_msg_hdr *)(data+len);
		ipmi_msg_hdr->rs_addr = req->union_app_req.bridge.target_addr;
		ipmi_msg_hdr->netfun_lun = req->union_app_req.bridge.br_netfn << 2 | (IPMI_BMC_CMD_LUN);
		ipmi_msg_hdr->checksum = ipmi_csum(data+len, 2);/* from beginning of ipmi_msg_hdr to checksum */
		cs = len + 3; /* cs is at current postion */
		ipmi_msg_hdr->rq_addr = req->union_app_req.bridge.my_addr;
		ipmi_msg_hdr->rq_seq = seq << 2;
		ipmi_msg_hdr->cmd = req->union_app_req.bridge.br_cmd;
		len += sizeof(*ipmi_msg_hdr);

		/* message data */
		if (req->data_len) {
			memcpy_s(data + len, req->data_len, req->data, req->data_len);
			len += req->data_len;
		}

		/* second checksum */
		tmp = len - cs;
		data[len++] = ipmi_csum(data+cs, tmp);

		/* bridged request: 2nd checksum */
		if (bridge_level == IPMI_BRIDGE_MSG_DUAL) {
			tmp = len - cs3;
			data[len++] = ipmi_csum(data+cs3, tmp);
		}
		tmp = len - cs2;
		data[len++] = ipmi_csum(data+cs2, tmp);

		data_crypt = ipmi20_payload_encrypt(sess, ipmi_info, data,
											len, &data_crypt_len);
		if (NULL == data_crypt) {
			free(buf);
			return NULL;
		}

		/* IPMI Msg/Payload Length */
		memcpy_s(ipmi_data_len_pos, IPMI20_MSG_PAYLOAD_LEN_LEN, &(data_crypt_len), IPMI20_MSG_PAYLOAD_LEN_LEN);
		buf_len += IPMI20_MSG_PAYLOAD_LEN_LEN;

		/* IPMI Payload */
		if (data_crypt_len > 0) {
			memcpy_s(buf + buf_len, data_crypt_len, data_crypt, data_crypt_len);
			buf_len += data_crypt_len;
		}

		if ((sess->integrity_alg > 0)) {
			pad_len = 4 - ((buf_len + sizeof(rmcp) - 2) % 4);
			/* IPMI Session Trailer/RSP Trailer */
			for (i = 0; i < pad_len; i++)
				buf[buf_len++] = 0xff;
			buf[buf_len++] = pad_len;
			buf[buf_len++] = 0x07;

			int auth_code_len = 0;

			auth_code_len = ipmi20_generate_auth_code(sess->integrity_alg,
													  sess->key_k1, 20,
													  buf + sizeof(rmcp),
													  buf_len - sizeof(rmcp),
													  auth_code);
			if (20 != auth_code_len) {
				free(buf);
				return NULL;
			}

			memcpy_s(buf + buf_len, 12, auth_code, 12);
			buf_len += 12;
		}
	} else {
		memcpy_s(buf + buf_len, IPMI20_MSG_PAYLOAD_LEN_LEN, &(ipmi_info->ipmi_data_len), IPMI20_MSG_PAYLOAD_LEN_LEN);
		buf_len += IPMI20_MSG_PAYLOAD_LEN_LEN;

		memcpy_s(buf + buf_len, ipmi_info->ipmi_data_len, ipmi_info->ipmi_data, ipmi_info->ipmi_data_len);
		buf_len += ipmi_info->ipmi_data_len;
	}

	*msglen = buf_len;

	return buf;
}


/**
  *  @brief build IPMIv2 LAN+ command
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @param[in] req request data
  *  @param[in] seq sequence number
  *  @param[in] request data lenght
  *  @return output IPMIv2 command buffer
  */
static unsigned char *ipmi20_lan_plus_build_cmd(struct ipmi_session *sess,
												struct rmcp_plus_ipmi_info *ipmi_info,
												struct ipmi_msg *req,
												unsigned char seq,
												int *msglen)
{
	unsigned char *buf = NULL;
	unsigned char data[IPMI_MAX_MSG_LENGTH];
	int data_len = 0;
	unsigned char *data_crypt = NULL;
	int data_crypt_len = 0;

	static struct rmcp_hdr rmcp = {
		.ver		= RMCP_VERSION_1,
		.__reserved = 0,
		.seq		= RMCP_IPMI_SEQ,
		.class		= RMCP_CLASS_IPMI,
	};

	int buf_len = 0;

	memset(data, 0, IPMI_MAX_MSG_LENGTH);
	buf = (unsigned char *)malloc(IPMI_MAX_MSG_LENGTH);
	if (NULL == buf) {
		IPMI_LOG_ERR("malloc failed\n");
		return NULL;
	}

	memset(buf, 0, IPMI_MAX_MSG_LENGTH);

	/* RMCP Header */
	memcpy_s(buf, sizeof(rmcp), &rmcp, sizeof(rmcp));
	buf_len += sizeof(rmcp);

	/* IPMI Session Header */
	buf[buf_len++] = IPMI20_AUTH_TYPE_RMCP_PLUS; /* for IPMI v2.0 only */
	buf[buf_len++] = ipmi_info->payload_type;

	/* OEM IANA and OEM Payload ID only present when payload type = 02h */
	if ((ipmi_info->payload_type & 0x3f) == IPMI20_STANDARD_PAYLOAD_OEM_EXPLICIT) {
		/* TODO: Now, assume it's absent */
	}

	memcpy_s(buf + buf_len, IPMI20_SESSION_ID_LEN, &(ipmi_info->session_id), IPMI20_SESSION_ID_LEN);
	buf_len += IPMI20_SESSION_ID_LEN;

	memcpy_s(buf + buf_len, IPMI20_SESSION_SEQ_NUM_LEN, &(ipmi_info->session_seq), IPMI20_SESSION_SEQ_NUM_LEN);
	buf_len += IPMI20_SESSION_SEQ_NUM_LEN;

	if ((sess->state > IPMI20_SESS_STATE_RAKP_3_SENT)
		&& (NULL != req)) {
		int i = 0;
		int pad_len = 0;
		unsigned char *ipmi_data_len_pos = buf + buf_len;
		unsigned char auth_code[16] = {0};

		data[IPMI_RSADDR_OFFSET] = IPMI_BMC_SLAVE_ADDR;
		data[IPMI_NETFN_OFFSET] = (req->netfn << 2) | (IPMI_BMC_CMD_LUN);
		data[IPMI_CSUM_OFFSET] = ipmi_csum(data, 2);
		data[IPMI_RQADDR_OFFSET] = IPMI_REMOTE_SWID;
		data[IPMI_RQLUN_OFFSET] = (seq << 2) | (IPMI_REMOTE_LUN);
		data[IPMI_CMD_OFFSET] = (req->cmd);
		data_len = 6; /* the above data length */

		if (req->data_len > 0) {
			memcpy_s(data + IPMI_DATA_OFFSET, sizeof(data)-IPMI_DATA_OFFSET, req->data, req->data_len);
			data_len += req->data_len;
		}

		data[data_len++] = ipmi_csum(&data[3], req->data_len + 3);

		data_crypt = ipmi20_payload_encrypt(sess, ipmi_info, data,
											data_len, &data_crypt_len);
		if (NULL == data_crypt) {
			free(buf);
			return NULL;
		}

		/* IPMI Msg/Payload Length */
		memcpy_s(ipmi_data_len_pos, IPMI20_MSG_PAYLOAD_LEN_LEN, &(data_crypt_len), IPMI20_MSG_PAYLOAD_LEN_LEN);
		buf_len += IPMI20_MSG_PAYLOAD_LEN_LEN;

		/* IPMI Payload */
		if (data_crypt_len > 0) {
			memcpy_s(buf + buf_len, data_crypt_len, data_crypt, data_crypt_len);
			buf_len += data_crypt_len;
		}

		if ((sess->integrity_alg > 0)) {
			pad_len = 4 - ((buf_len + sizeof(rmcp) - 2) % 4);
			/* IPMI Session Trailer/RSP Trailer */
			for (i = 0; i < pad_len; i++)
				buf[buf_len++] = 0xff;
			buf[buf_len++] = pad_len;
			buf[buf_len++] = 0x07;

			int auth_code_len = 0;

			auth_code_len = ipmi20_generate_auth_code(sess->integrity_alg,
													  sess->key_k1, 20,
													  buf + sizeof(rmcp),
													  buf_len - sizeof(rmcp),
													  auth_code);
			if (20 != auth_code_len) {
				free(buf);
				return NULL;
			}

			memcpy_s(buf + buf_len, 12, auth_code, 12);
			buf_len += 12;
		}
	} else {
		memcpy_s(buf + buf_len, IPMI20_MSG_PAYLOAD_LEN_LEN, &(ipmi_info->ipmi_data_len), IPMI20_MSG_PAYLOAD_LEN_LEN);
		buf_len += IPMI20_MSG_PAYLOAD_LEN_LEN;

		memcpy_s(buf + buf_len, ipmi_info->ipmi_data_len, ipmi_info->ipmi_data, ipmi_info->ipmi_data_len);
		buf_len += ipmi_info->ipmi_data_len;
	}

	*msglen = buf_len;

	return buf;
}

#define IPMI20_CIPHER_SUITE_ID_MIN	0
#define IPMI20_CIPHER_SUITE_ID_MAX	14

static unsigned char list_index;

/**
  *  @brief This function is used to get the request authentication algorithm,
  *         integrity algorithm, confidentiality algorithm based on cipher
  *         suite id.
  *         >> Table 22-19.
  */
static int ipmi20_lan_plus_get_req_ciphers(unsigned char cipher_suite_id,
										   unsigned char *req_auth_alg,
										   unsigned char *req_integrity_alg,
										   unsigned char *req_crypt_alg)
{
	if ((cipher_suite_id < IPMI20_CIPHER_SUITE_ID_MIN) ||
		(cipher_suite_id > IPMI20_CIPHER_SUITE_ID_MAX))
		return -1;

	switch (cipher_suite_id) {
	case 0:
		*req_auth_alg = AUTH_ALG_RAKP_NONE;
		*req_integrity_alg = INTEGRITY_ALG_NONE;
		*req_crypt_alg = CRYPT_ALG_NONE;
		break;
	case 1:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_SHA1;
		*req_integrity_alg = INTEGRITY_ALG_NONE;
		*req_crypt_alg = CRYPT_ALG_NONE;
		break;
	case 2:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_SHA1;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_SHA1_96;
		*req_crypt_alg = CRYPT_ALG_NONE;
		break;
	case 3:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_SHA1;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_SHA1_96;
		*req_crypt_alg = CRYPT_ALG_AES_CBC_128;
		break;
	case 4:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_SHA1;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_SHA1_96;
		*req_crypt_alg = CRYPT_ALG_xRC4_128;
		break;
	case 5:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_SHA1;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_SHA1_96;
		*req_crypt_alg = CRYPT_ALG_xRC4_40;
		break;
	case 6:
		*req_integrity_alg = INTEGRITY_ALG_NONE;
		*req_crypt_alg = CRYPT_ALG_NONE;
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		break;
	case 7:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_MD5_128;
		*req_crypt_alg = CRYPT_ALG_NONE;
		break;
	case 8:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_MD5_128;
		*req_crypt_alg = CRYPT_ALG_AES_CBC_128;
		break;
	case 9:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_MD5_128;
		*req_crypt_alg = CRYPT_ALG_xRC4_128;
		break;
	case 10:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_HMAC_MD5_128;
		*req_crypt_alg = CRYPT_ALG_xRC4_40;
		break;
	case 11:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_MD5_128;
		*req_crypt_alg = CRYPT_ALG_NONE;
		break;
	case 12:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_MD5_128;
		*req_crypt_alg = CRYPT_ALG_AES_CBC_128;
		break;
	case 13:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_MD5_128;
		*req_crypt_alg = CRYPT_ALG_xRC4_128;
		break;
	case 14:
		*req_auth_alg = AUTH_ALG_RAKP_HMAC_MD5;
		*req_integrity_alg = INTEGRITY_ALG_MD5_128;
		*req_crypt_alg = CRYPT_ALG_xRC4_40;
		break;
	default:
		return -1;
	}

	return 0;
}

/**
  *  @brief This is function is used to send Get Channel Cipher Suite command to BMC,
  *         to get the supported Cipher Suite IDs by BMC.
  *
  *  @param[in] sess IPMI session
  *  @param[in] index [7] - 1b = list algorithms by Cipher Suite, 0b = list supported algorithms
  *  @return
  */
static void send_get_channel_cipher_suites_cmd(struct ipmi_session *sess, unsigned char index)
{
	int msglen;
	struct ipmi_msg ipmi;
	unsigned char msg[IPMI_MAX_MSG_LENGTH];

	memset(msg, 0, IPMI_MAX_MSG_LENGTH);
	sess->state = IPMI20_GET_CHANNEL_CIPHER_SUITES_SENT;

	ipmi.netfn    = IPMI_NETFN_APP;
	ipmi.cmd      = IPMI20_GET_CHANNEL_CIPHER_SUITES;
	ipmi.data_len = 3;

	ipmi.data[0] = IPMI_LAN_CHANNEL_E | 0x80;

	ipmi.data[0] = IPMI_LAN_CHANNEL_E;
	ipmi.data[1] = 0x00; /* IPMI */
	ipmi.data[2] = (0x80 | index); /* [7] - 1b = list algorithms by Cipher Suite,
										    0b = list supported algorithms */

	msglen = ipmi_lan_build_cmd(msg, sess, &ipmi, sess->curr_seq++);
	if (msglen == 0)
		return;

	rmcp_send_outbound_msg(msg, msglen, sess->host, sess->port);
}

/**
  *  @brief Send the open session command to Managed System, IPMI v2.0 only.
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_open_session_cmd(struct ipmi_session *sess)
{
	struct rmcp_plus_ipmi_info ipmi_info = {0};
	unsigned char *msg = NULL;
	unsigned char *msg_to_sent = NULL;
	int msglen = 0;
	int offset = IPMI20_OPEN_SESS_REQ_MSG_TAG_OFFSET;
	int rc = -1;

	msg = (unsigned char *)malloc(IPMI20_OPEN_SESS_REQ_SIZE);
	if (NULL == msg) {
		IPMI_LOG_ERR("malloc failed\n");
		return;
	}

	memset(msg, 0, IPMI20_OPEN_SESS_REQ_SIZE);

	sess->state = IPMI20_SESS_STATE_OPEN_SESSION_SENT;

	ipmi_info.payload_type = IPMI20_SESS_PAYLOAD_OPEN_SESS_REQ;
	ipmi_info.session_id = 0;
	ipmi_info.session_seq = 0;

	msg[offset++] = 0; /* byte - 0: Message tag */
	msg[offset++] = 0; /* byte - 1, must be "0" */
	msg[offset++] = 0; /* byte - 3: reserved */
	msg[offset++] = 0; /* byte - 4: reserved */

	/* byte 5 - 8, Remote Console Session ID */
	sess->session_id = 0xc1c2c3c4; /* FIXME */
	memcpy_s(msg + offset, IPMI20_OPEN_SESS_REQ_SESSION_ID_LEN, &(sess->session_id), IPMI20_OPEN_SESS_REQ_SESSION_ID_LEN);
	offset += IPMI20_OPEN_SESS_REQ_SESSION_ID_LEN;

	rc = ipmi20_lan_plus_get_req_ciphers(sess->cipher_suite_id,
										 &(sess->req_auth_alg),
										 &(sess->req_integrity_alg),
										 &(sess->req_crypt_alg));
	if (rc < 0) {
		IPMI_LOG_DEBUG("Unknown cipher suite ID: %d\n", sess->cipher_suite_id);

		free(msg);
		return;
	}

	/* byte 9 - 16, Authentication Payload */
	msg[offset++] = 0x00; /* Authentication algorithm Type */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x08; /* Payload Length */
	msg[offset++] = sess->req_auth_alg; /* Authentication Algorithm */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */

	/* byte 17 - 24, Integrity Payload */
	msg[offset++] = 0x01; /* Integrity algorithm Type */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x08; /* Payload Length */
	msg[offset++] = sess->req_integrity_alg; /* Integrity algorithm */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */

	/* byte 25 - 32, Confidentiality Payload */
	msg[offset++] = 0x02; /* Confidentiality algorithm Type */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x08; /* Payload Length */
	msg[offset++] = sess->req_crypt_alg;
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */

	ipmi_info.ipmi_data = msg;
	ipmi_info.ipmi_data_len = IPMI20_OPEN_SESS_REQ_SIZE;

	msg_to_sent = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, NULL,
											sess->curr_seq++, &msglen);
	if (NULL == msg_to_sent) {
		IPMI_LOG_DEBUG("None message to be sent\n");
		free(msg);
		return;
	}

	rmcp_send_outbound_msg(msg_to_sent, msglen, sess->host, sess->port);

	free(msg);
	free(msg_to_sent);
}

/**
  *  @brief This function is used to handle the message "Get Channel Authentication
  *         Capabilities, Rs", this message has the same format with IPMI v1.5.
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi20_auth_cap_resp(struct ipmi_session *sess,
										struct rmcp_ipmi_info *ipmi_info)
{
	unsigned char auth;

	if (ipmi_info->auth_type != IPMI_SESSION_AUTHTYPE_NONE ||
	    ipmi_info->session_id != 0 ||
	    ipmi_info->data_len < 9 ||
	    ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid auth cap response, with ccode %02X, len %d!\n",
					   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	auth = ipmi_info->ipmi_data[2];

	if ((auth & 0x80) != 0) { /* Support IPMI v2.0 */
		IPMI_LOG_DEBUG("Use IPMI v2.0 Authentication!\n");
		sess->auth_type = IPMI20_AUTH_TYPE_RMCP_PLUS;

		sess->auth_status = ipmi_info->ipmi_data[3];

		IPMI_LOG_INFO("BMC "NIPQUAD_FMT" 'Per-message Authentication' is %sabled!\n",
					  NIPQUAD(sess->host),
					  (sess->auth_status & IPMI_AUTHSTATUS_PER_MSG_DISABLED) != 0 ?
					  "dis" : "en");

		/* TODO:
		   byte 5: For IPMI v2.0+ only - Extended Capabilities
		   [7:2] = response
		   [1] = 1b = channel supports IPMI v2.0 connections.
		   [0] = 1b = channel supports IPMI v1.5 connections.
		 */

		/* TODO:
			byte 6:8, OEM ID;
			9, OEM auxiliary data;
		 */

        sess->cipher_suite_id = 3;
        send_session_open_session_cmd(sess);
		/* CM doesn't support this optional command, so directly call send_session_open_session_cmd here. */
		//send_get_channel_cipher_suites_cmd(sess, list_index++);
	} else {
		handle_auth_capabilities_resp(sess, ipmi_info);
	}
}

static unsigned char cipher_suite_data[0x10 * 0x40] = {0}; /* at least, 16 * (0x00 ... 0x3f) */
static int cipher_suite_data_len;

/**
  *  @brief handle IPMIv2 get channal cipher suite response
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi20_get_channel_cipher_suites_resp(struct ipmi_session *sess,
														 struct rmcp_ipmi_info *ipmi_info)
{
	if (ipmi_info->auth_type != IPMI_SESSION_AUTHTYPE_NONE ||
	    ipmi_info->session_id != 0 ||
		ipmi_info->data_len < 3 ||
	    ipmi_info->ipmi_data[0] != IPMI_CC_OK) {
		IPMI_LOG_DEBUG("Invalid get channel cipher suites response, with ccode %02X, len %d!\n",
					   ipmi_info->ipmi_data[0], ipmi_info->data_len);
		return;
	}

	if ((ipmi_info->data_len > 3) && (ipmi_info->data_len < 18)) {
		memcpy_s(cipher_suite_data + cipher_suite_data_len, sizeof(cipher_suite_data)-cipher_suite_data_len, ipmi_info->ipmi_data + 2, ipmi_info->data_len - 2);
		cipher_suite_data_len += ipmi_info->data_len - 2;

		int i = 0;

		for (i = 0; i < cipher_suite_data_len; i++) {
			if (cipher_suite_data[i++] == IPMI20_CIPHER_STANDARD_CIPHER_SUITE_ID_MASK) {
				if ((cipher_suite_data_len - i) < 3) {
					IPMI_LOG_DEBUG("Incomplete cipher suite format\n");
					return;
				}

				if ((cipher_suite_data[i] >= IPMI20_CIPHER_SUITE_ID_MIN)
					&& (cipher_suite_data[i] <= IPMI20_CIPHER_SUITE_ID_MAX))
					bmc_cipher_suite_id_flag |= (1 << cipher_suite_data[i]);

				i += 3;
			} else if (cipher_suite_data[i++] == IPMI20_CIPHER_OEM_CIPHER_SUITE_ID_MASK) {
				IPMI_LOG_DEBUG("OEM\n");
				if ((cipher_suite_data_len - i) < 3) {
					IPMI_LOG_DEBUG("Incomplete cipher suite format\n");
					return;
				}
				/* FIXME: */
				i += 3;
			} else {
				IPMI_LOG_DEBUG("Unknown cipher suite format\n");
				break;
			}
		}

		sess->cipher_suite_id_flag = (cipher_suite_id_flag & bmc_cipher_suite_id_flag);

		sess->cipher_suite_id = 3;

		if (!(sess->cipher_suite_id & sess->cipher_suite_id_flag)) {
			/* Try none algorithm at last. */
			for (i = IPMI20_CIPHER_SUITE_ID_MAX; i >= IPMI20_CIPHER_SUITE_ID_MIN; i--) {
				if (i & sess->cipher_suite_id_flag) {
					sess->cipher_suite_id = i;
					break;
				}
			}
		}

	} else if ((ipmi_info->data_len == 18) && (list_index < 0x3F)) {
		memcpy_s(cipher_suite_data + cipher_suite_data_len, sizeof(cipher_suite_data)-cipher_suite_data_len, ipmi_info->ipmi_data + 2, 16);
		cipher_suite_data_len += 16;

		send_get_channel_cipher_suites_cmd(sess, list_index++);

		return;
	}

	send_session_open_session_cmd(sess);
}

/**
  *  @brief Send RAKP Message 1 to Managed System.
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_rakp_msg_1_cmd(struct ipmi_session *sess)
{
	struct rmcp_plus_ipmi_info ipmi_info;
	int msglen;
	int user_name_len = 0;
	unsigned char *msg = NULL;
	unsigned char *msg_to_send = NULL;
	int offset = IPMI20_RAKP_MSG_1_MSG_TAG_OFFSET;

	msg = (unsigned char *)malloc(IPMI20_RAKP_MSG_1_SIZE);
	if (NULL == msg) {
		IPMI_LOG_ERR("Malloc failed\n");
		return;
	}

	memset(msg, 0, IPMI20_RAKP_MSG_1_SIZE);

	sess->state = IPMI20_SESS_STATE_RAKP_1_SENT;

	ipmi_info.payload_type = IPMI20_SESS_PAYLOAD_RAKP_1;
	ipmi_info.session_id = 0;
	ipmi_info.session_seq = 0;

	msg[offset++] = sess->payload.open_sess_resp.msg_tag; /* Message tag */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */

	/* BMC session ID */
	memcpy_s(msg + offset, IPMI20_RAKP_MSG_1_BMC_ID_LEN, &(sess->payload.open_sess_resp.bmc_id),
		   IPMI20_RAKP_MSG_1_BMC_ID_LEN);
	offset += IPMI20_RAKP_MSG_1_BMC_ID_LEN;

	/* 16 bytes remote console random number */
	ipmi20_random(sess->session_random, IPMI20_RAKP_MSG_1_SESSION_RANDOM_LEN);
	memcpy_s(msg + offset, IPMI20_RAKP_MSG_1_SESSION_RANDOM_LEN, sess->session_random,
		   IPMI20_RAKP_MSG_1_SESSION_RANDOM_LEN);
	offset += IPMI20_RAKP_MSG_1_SESSION_RANDOM_LEN;

	msg[offset++] = sess->privilege; /* requested maximum privilege level (role) */
	msg[offset++] = 0x00; /*requested */
	msg[offset++] = 0x00; /* reserved */

	user_name_len = strnlen_s((char *)sess->username, sizeof(sess->username)-1);
	msg[offset++] = user_name_len;
	memcpy_s(msg + offset, user_name_len, sess->username, user_name_len);

	ipmi_info.ipmi_data = msg;
	ipmi_info.ipmi_data_len = offset + user_name_len;

	msg_to_send = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, NULL,
											sess->curr_seq++, &msglen);
	if (NULL == msg_to_send) {
		free(msg);
		return;
	}

	rmcp_send_outbound_msg(msg_to_send, msglen, sess->host, sess->port);
	free(msg);
	free(msg_to_send);
}

/**
  *  @brief Handle the open session reponse from the Managed System.
  *         And if no errors, send RAKP Message 1 to Managed System.
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_open_session_resp(struct ipmi_session *sess,
									 struct rmcp_plus_ipmi_info *ipmi_info)
{
	memset(&(sess->payload.open_sess_resp), 0, sizeof(sess->payload.open_sess_resp));

	sess->payload.open_sess_resp.msg_tag =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_MSG_TAG_OFFSET];
	sess->payload.open_sess_resp.stat_code =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_STAT_CODE_OFFSET];
	sess->payload.open_sess_resp.max_priv_level =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_MAX_PRIV_LEVEL_OFFSET];

	memcpy_s(&(sess->payload.open_sess_resp.session_id), IPMI20_OPEN_SESS_RSP_SESSION_ID_LEN,
		   ipmi_info->ipmi_data + IPMI20_OPEN_SESS_RSP_SESSION_ID_OFFSET,
		   IPMI20_OPEN_SESS_RSP_SESSION_ID_LEN);

	/* FIXME: return or do something. */
	if (sess->payload.open_sess_resp.stat_code != RMCP_PLUS_STAT_NO_ERR) {
		if ((sess->payload.open_sess_resp.stat_code == RMCP_PLUS_STAT_INVALID_AUTH_ALGO)
			|| (sess->payload.open_sess_resp.stat_code == RMCP_PLUS_STAT_INVALID_INT_ALGO)
			|| (sess->payload.open_sess_resp.stat_code == RMCP_PLUS_STAT_INVALID_CONF_ALGO)) {
			sess->cipher_suite_id = 1;

			send_session_open_session_cmd(sess);
			return;
		} else {
			IPMI_LOG_DEBUG("Error occured\n");
			return;
		}
	}

	memcpy_s(&(sess->payload.open_sess_resp.bmc_id), IPMI20_OPEN_SESS_RSP_BMC_ID_LEN,
		   ipmi_info->ipmi_data + IPMI20_OPEN_SESS_RSP_BMC_ID_OFFSET,
		   IPMI20_OPEN_SESS_RSP_BMC_ID_LEN);
	sess->bmc_id = sess->payload.open_sess_resp.bmc_id;

	sess->payload.open_sess_resp.auth_alg =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_AUTH_ALG_OFFSET + 4] & 0x3f;
	sess->payload.open_sess_resp.int_alg =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_INT_ALG_OFFSET + 4] & 0x3f;
	sess->payload.open_sess_resp.crypt_alg =
		ipmi_info->ipmi_data[IPMI20_OPEN_SESS_RSP_CRYPT_ALG_OFFSET + 4] & 0x3f;

	sess->auth_alg = sess->payload.open_sess_resp.auth_alg;
	sess->integrity_alg = sess->payload.open_sess_resp.int_alg;
	sess->crypt_alg = sess->payload.open_sess_resp.crypt_alg;

	send_session_rakp_msg_1_cmd(sess);
}

/**
  *  @brief Send RAKP Message 3 to BMC.
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_rakp_msg_3_cmd(struct ipmi_session *sess)
{
	struct rmcp_plus_ipmi_info ipmi_info = {0};
	unsigned char *msg = NULL;
	unsigned char *msg_to_send = NULL;
	int msglen = 0;
	int offset = IPMI20_RAKP_MSG_3_MSG_TAG_OFFSET;

	msg = (unsigned char *)malloc(IPMI20_RAKP_MSG_3_MAX_SIZE);
	if (NULL == msg) {
		IPMI_LOG_ERR("malloc failed\n");
		return;
	}

	sess->state = IPMI20_SESS_STATE_RAKP_3_SENT;

	ipmi_info.payload_type = IPMI20_SESS_PAYLOAD_RAKP_3;
	ipmi_info.session_id = 0;
	ipmi_info.session_seq = 0;

	msg[offset++] = sess->payload.rakp_msg_2.msg_tag; /* Message tag */
	msg[offset++] = sess->payload.rakp_msg_2.stat_code; /* Status code */
	msg[offset++] = 0x00; /* reserved */
	msg[offset++] = 0x00; /* reserved */

	/* RMCP+: BMC Session ID*/
	memcpy_s(msg + offset, IPMI20_RAKP_MSG_3_BMC_ID_LEN, &(sess->bmc_id), IPMI20_RAKP_MSG_3_BMC_ID_LEN);
	offset += IPMI20_RAKP_MSG_3_BMC_ID_LEN;

	/* If RMCP+ Status Code from RAKP Message 2 is equal to 0x00 (no errors),
	   we need to generate an auth_code key, otherwise, we can skip this step. */
	if (sess->payload.rakp_msg_2.stat_code == RMCP_PLUS_STAT_NO_ERR) {
		int auth_code_len = 0;
		unsigned char data_check[RAKP_DATA_CHECK_MAX_LEN] = {0};
		int len_check = 0;
		int user_name_len = 0;

		memcpy_s(data_check, sizeof(data_check), sess->bmc_random, IPMI20_RAKP_BMC_RANDOM_LEN);
		len_check += IPMI20_RAKP_BMC_RANDOM_LEN;
		memcpy_s(data_check + len_check, sizeof(data_check)-len_check, &(sess->session_id), IPMI20_SESSION_ID_LEN);
		len_check += IPMI20_SESSION_ID_LEN;
		memcpy_s(data_check + len_check, sizeof(data_check)-len_check,&(sess->privilege), IPMI20_PRIVILEGE_LEN);
		len_check += IPMI20_PRIVILEGE_LEN;

		user_name_len = strnlen_s((char *)sess->username, sizeof(sess->username)-1);
		memcpy_s(data_check + len_check, sizeof(data_check)-len_check, &user_name_len, 1);
		len_check += 1;

		memcpy_s(data_check + len_check, sizeof(data_check)-len_check, sess->username, user_name_len);
		len_check += user_name_len;

		auth_code_len = ipmi20_generate_auth_code(sess->auth_alg, sess->password,
												  strnlen_s((char *)sess->password, sizeof(sess->password)-1),
												  data_check, len_check, msg + offset);
		offset += auth_code_len;
	}

	ipmi_info.ipmi_data_len = IPMI20_RAKP_MSG_3_MSG_TAG_OFFSET + offset;
	ipmi_info.ipmi_data = msg;

	msg_to_send = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, NULL,
											sess->curr_seq++, &msglen);
	if (NULL == msg_to_send) {
		free(msg);
		return;
	}

	rmcp_send_outbound_msg(msg_to_send, msglen, sess->host, sess->port);

	free(msg);
	free(msg_to_send);
}

/**
  *  @brief Handle RAKP Message 2 from BMC and send back to RAKP Message 3.
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_rakp_message_2(struct ipmi_session *sess,
								  struct rmcp_plus_ipmi_info *ipmi_info)
{
	memset(&sess->payload.rakp_msg_2, 0, sizeof(sess->payload.rakp_msg_2));

	sess->payload.rakp_msg_2.msg_tag =
		ipmi_info->ipmi_data[IPMI20_RAKP_MSG_2_MSG_TAG_OFFSET];
	sess->payload.rakp_msg_2.stat_code =
		ipmi_info->ipmi_data[IPMI20_RAKP_MSG_2_STAT_CODE_OFFSET];
	memcpy_s(&(sess->payload.rakp_msg_2.session_id), IPMI20_RAKP_MSG_2_SESSION_ID_LEN,
		   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_2_SESSION_ID_OFFSET,
		   IPMI20_RAKP_MSG_2_SESSION_ID_LEN);
	memcpy_s(sess->payload.rakp_msg_2.bmc_random, sizeof(sess->payload.rakp_msg_2.bmc_random), 
		   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_2_BMC_RANDOM_NUM_OFFSET,
		   IPMI20_RAKP_MSG_2_BMC_RANDOM_NUM_LEN);

	memcpy_s(sess->bmc_random, sizeof(sess->bmc_random),
			sess->payload.rakp_msg_2.bmc_random,
		   IPMI20_RAKP_MSG_2_BMC_RANDOM_NUM_LEN);

	memcpy_s(&(sess->payload.rakp_msg_2.bmc_guid), IPMI20_RAKP_MSG_2_BMC_GUID_LEN,
		   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_2_BMC_GUID_OFFSET,
		   IPMI20_RAKP_MSG_2_BMC_GUID_LEN);

	switch (sess->auth_alg) {
	case AUTH_ALG_RAKP_NONE:
		/* Nothing */
		break;
	case AUTH_ALG_RAKP_HMAC_SHA1:
		memcpy_s(sess->payload.rakp_msg_2.auth_alg, sizeof(sess->payload.rakp_msg_2.auth_alg),
			   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_2_AUTH_CODE_OFFSET,
			   RAKP_2_AND_3_HMAC_SHA1_SIZE);
		break;
	case AUTH_ALG_RAKP_HMAC_MD5:
		memcpy_s(sess->payload.rakp_msg_2.auth_alg, sizeof(sess->payload.rakp_msg_2.auth_alg),
			   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_2_AUTH_CODE_OFFSET,
			   RAKP_HMAC_MD5_SIZE);
		break;
	default:
		break;
	}

	send_session_rakp_msg_3_cmd(sess);
}

/**
  *  @brief Set Session Privilege Level Command.
  *         IPMI Request Data: Request Privilege Level
  *                            [7:4] - reserved
  *                            [3:0] - Privilege
  *
  *         If crypt masked, need crypt for data
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void send_session_v2_privlvl_cmd(struct ipmi_session *sess)
{
	int msglen;
	struct ipmi_msg ipmi = {0};
	struct rmcp_plus_ipmi_info ipmi_info = {0};
	unsigned char *msg_to_send = NULL;

	sess->state = IPMI20_SESS_STATE_SESSION_PRIV_SENT;

	if (sess->crypt_alg != CRYPT_ALG_NONE)
		ipmi_info.payload_type |= IPMI20_PAYLOAD_ENCRYPTED_MASK;

	if (sess->integrity_alg != INTEGRITY_ALG_NONE)
		ipmi_info.payload_type |= IPMI20_PAYLOAD_AUTH_MASK;

	ipmi_info.session_id = sess->bmc_id;
	ipmi_info.session_seq = (++sess->session_seq);

	ipmi.netfn	  = IPMI_NETFN_APP;
	ipmi.cmd	  = IPMI_SET_SESS_PRIV_LEV;
	ipmi.data_len = 1;
	ipmi.data[0] = sess->privilege;

	msg_to_send = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, &ipmi,
											sess->curr_seq++, &msglen);

	if (NULL == msg_to_send) {
		IPMI_LOG_DEBUG("IPMI v2.0: Failed to build pvivlvl cmd for sending");
		return;
	}

	rmcp_send_outbound_msg(msg_to_send, msglen, sess->host, sess->port);
	free(msg_to_send);
}

/**
  *  @brief Handle RAKP Message 4 from BMC.
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_rakp_message_4(struct ipmi_session *sess,
								  struct rmcp_plus_ipmi_info *ipmi_info)
{
	unsigned char data_verify[RAKP_DATA_CHECK_MAX_LEN] = {0};
	int data_verify_len = 0;
	unsigned char key_auth[21] = {0};

	memset(&sess->payload.rakp_msg_4, 0, sizeof(sess->payload.rakp_msg_4));

	sess->payload.rakp_msg_4.msg_tag =
		ipmi_info->ipmi_data[IPMI20_RAKP_MSG_4_MSG_TAG_OFFSET];
	sess->payload.rakp_msg_4.stat_code =
		ipmi_info->ipmi_data[IPMI20_RAKP_MSG_4_STAT_CODE_OFFSET];
	memcpy_s(&(sess->payload.rakp_msg_4.session_id), sizeof(sess->payload.rakp_msg_4.session_id),
		   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_4_SESSION_ID_OFFSET,
		   ipmi_info->ipmi_data[IPMI20_RAKP_MSG_4_SESSION_ID_LEN]);

	switch (sess->auth_alg) {
	case AUTH_ALG_RAKP_NONE:
		/* Nothing. */
		break;
	case AUTH_ALG_RAKP_HMAC_SHA1:
		memcpy_s(sess->payload.rakp_msg_4.int_check, sizeof(sess->payload.rakp_msg_4.int_check),
			   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_4_INT_CHECK_OFFSET,
			   RAKP_4_HMAC_SHA1_SIZE);
		break;
	case AUTH_ALG_RAKP_HMAC_MD5:
		memcpy_s(sess->payload.rakp_msg_4.int_check, sizeof(sess->payload.rakp_msg_4.int_check),
			   ipmi_info->ipmi_data + IPMI20_RAKP_MSG_4_INT_CHECK_OFFSET,
			   RAKP_HMAC_MD5_SIZE);
		break;
	default:
		break;
	}

	/* Generate sik */
	memset(data_verify, 0, RAKP_DATA_CHECK_MAX_LEN);

	memcpy_s(data_verify, sizeof(data_verify), sess->session_random, IPMI20_RAKP_SESSION_RANDOM_LEN);
	data_verify_len = IPMI20_RAKP_SESSION_RANDOM_LEN;
	memcpy_s(data_verify + data_verify_len, sizeof(data_verify)-data_verify_len,
		   sess->bmc_random, IPMI20_RAKP_BMC_RANDOM_LEN);
	data_verify_len += IPMI20_RAKP_BMC_RANDOM_LEN;
	data_verify[data_verify_len++] = sess->privilege;
	data_verify[data_verify_len++] = strnlen_s((char *)sess->username, sizeof(sess->username)-1);
	memcpy_s(data_verify + data_verify_len, sizeof(data_verify)-data_verify_len, sess->username, strnlen_s((char *)sess->username, sizeof(sess->username)-1));
	data_verify_len += strnlen_s((char *)sess->username, sizeof(sess->username)-1);
	if (sess->key_kg[0] != '\0') {
		memcpy_s(key_auth, sizeof(key_auth), sess->key_kg, 20);
	} else {
		memcpy_s(key_auth, sizeof(key_auth), sess->password, strnlen_s((char *)sess->password, sizeof(sess->password)-1));
	}

	ipmi20_generate_auth_code(sess->auth_alg, key_auth, 20, data_verify,
							  data_verify_len, sess->key_sik);

	/* Generate k1 */
	memset(data_verify, 0x01, RAKP_DATA_CHECK_MAX_LEN);

	ipmi20_generate_auth_code(sess->auth_alg, sess->key_sik,
							  20, data_verify,
							  20, sess->key_k1);

	/* Generate k2 */
	memset(data_verify, 0x02, RAKP_DATA_CHECK_MAX_LEN);
	ipmi20_generate_auth_code(sess->auth_alg, sess->key_sik, 20,
							  data_verify, 20, sess->key_k2);

	/* Generate k3 */
	memset(data_verify, 0x03, RAKP_DATA_CHECK_MAX_LEN);
	ipmi20_generate_auth_code(sess->auth_alg, sess->key_sik, 20,
							  data_verify, 20, sess->key_k3);

	send_session_v2_privlvl_cmd(sess);
}

/**
  *  @brief flush IPMIv2 user msg
  *
  *  @param[in] sess IPMI session
  *  @return
  */
static void flush_ipmi20_user_msg(struct ipmi_session *sess)
{
	int msglen;
	struct rmcp_match_info match;
	struct ipmi_msg_sent *msg, *msg_next;
	struct ipmi_session_table *table = &session_table;
	struct rmcp_plus_ipmi_info ipmi_info = {0};
	unsigned char *msg_to_send = NULL;

	sess->timeout = RMCP_IPMI_SESSION_IDLE_TIMEOUT_MS;

	if (sess->crypt_alg != CRYPT_ALG_NONE)
		ipmi_info.payload_type |= IPMI20_PAYLOAD_ENCRYPTED_MASK;

	if (sess->integrity_alg != INTEGRITY_ALG_NONE)
		ipmi_info.payload_type |= IPMI20_PAYLOAD_AUTH_MASK;

	ipmi_info.session_id = sess->bmc_id;
	ipmi_info.session_seq = (++sess->session_seq);

	list_for_each_entry_safe(msg, msg_next, &sess->msg_list, list) {
		list_del(&msg->list);

		msg->next = table->msg_freelist;
		table->msg_freelist = msg;

		match.netfn = msg->msg.netfn;
		match.cmd   = msg->msg.cmd;
		if (rmcp_intf_next_seq(sess, &match, msg->timeo, msg->msgid, msg->user_port, msg->header) != 0)
			continue;

		if (IPMI_BRIDGE_MSG_NONE != msg->msg.union_app_req.bridge.bridge_level) {
			//match.netfn = msg->msg.union_app_req.bridge.br_netfn;
			match.netfn = IPMI_BRIDGE_NETFN_APP;
			if (IPMI_BRIDGE_MSG_SINGLE == msg->msg.union_app_req.bridge.bridge_level)
				match.cmd   = msg->msg.union_app_req.bridge.br_cmd;
			else
				match.cmd   = IPMI_BRIDGE_IPMI_CMD;

			if (rmcp_intf_next_seq(sess, &match, msg->timeo, msg->msgid, msg->user_port, msg->header) != 0)
				continue;

			msg_to_send = ipmi20_lan_plus_build_cmd_br(sess, &ipmi_info, &msg->msg,
													match.seq, &msglen, msg->msg.union_app_req.bridge.bridge_level);

		} else {
			msg_to_send = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, &msg->msg,
													match.seq, &msglen);
		}

		if (NULL == msg_to_send)
			return;

		rmcp_send_outbound_msg(msg_to_send, msglen, sess->host, sess->port);
	}
}

/**
  *  @brief Handle the reponse from BMC for Setting Privilege.
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi20_session_privlvl_resp(struct ipmi_session *sess,
											   struct rmcp_plus_ipmi_info *ipmi_info)
{
	if (ipmi_info->auth_type != sess->auth_type ||
	    ipmi_info->session_id != sess->session_id) {
		IPMI_LOG_DEBUG("IPMI v2.0: Invalid session for pvivlvl!\n");
		return;
	}

	sess->state = IPMI20_SESS_STATE_ACTIVE;
	sess->session_seq = 1; /* seq start */

	flush_ipmi20_user_msg(sess);
}

/**
  *  @brief This function is used to handle the message received from Managed
  *         System (Session Setup Payload Types).
  *         > Get Channel Authentication Capabilitieds, Rs. (Ref IPMI v1.5)
  *         > Open Session Response
  *         > RAKP Message 2
  *         > RAKP Message 4
  *         > Set Privilege Level, Rs. (Ref IPMI v1.5)
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi20_sess_msg(struct ipmi_session *sess,
								   struct rmcp_plus_ipmi_info *ipmi_info)
{
	switch (sess->state) {
	case IPMI20_SESS_STATE_OPEN_SESSION_SENT:
		if ((ipmi_info->payload_type & IPMI20_PAYLOAD_TYPE_MASK) ==
			IPMI20_SESS_PAYLOAD_OPEN_SESS_RSP)
			handle_open_session_resp(sess, ipmi_info);
		break;
	case IPMI20_SESS_STATE_RAKP_1_SENT:
		if ((ipmi_info->payload_type & IPMI20_PAYLOAD_TYPE_MASK) ==
			IPMI20_SESS_PAYLOAD_RAKP_2)
			handle_rakp_message_2(sess, ipmi_info);
		break;
	case IPMI20_SESS_STATE_RAKP_3_SENT:
		if ((ipmi_info->payload_type & IPMI20_PAYLOAD_TYPE_MASK) ==
			IPMI20_SESS_PAYLOAD_RAKP_4)
			handle_rakp_message_4(sess, ipmi_info);
		break;
	case IPMI20_SESS_STATE_SESSION_PRIV_SENT:
		handle_ipmi20_session_privlvl_resp(sess, ipmi_info);
		break;
	default:
		break;
	}
}

/**
  *  @brief This function is used to handle the message received from Managed
  *         System (Standard Payload Types).
  *         > IPMI Message
  *         > SOL (serial over LAN)
  *         > OEM Explicit ( TODO )
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi20_user_msg(struct ipmi_session *sess,
								   struct rmcp_plus_ipmi_info *ipmi_info)
{
	unsigned int msgid;
	unsigned short user_port;
	struct rmcp_match_info match;
	struct appmsg_ipmi_msg *ipmi;
	union app_msg_union msg;
	int len = 0;
	int data_len = 0;
	unsigned char *payload = NULL;
	ipmi_json_ipc_header_t header;

	if (ipmi_info->session_id != sess->session_id ||
		ipmi_info->auth_type  != sess->auth_type) {
		IPMI_LOG_DEBUG("IPMI v2.0: IPMI session not MATCH, "
					   "Rcv'd sess_id = %u auth_type = %u, "
					   "Table sess_id = %u auth_type = %u!\n",
					   ipmi_info->session_id, ipmi_info->auth_type,
					   sess->session_id, sess->auth_type);
		return;
	}

	payload = ipmi20_payload_decrypt(sess, sess->key_k2, ipmi_info, &len);
	if (NULL == payload)
		return;

	match.netfn = payload[IPMI_NETFN_OFFSET] >> 2;
	match.seq = payload[IPMI_RQSEQ_OFFSET] >> 2;
	match.cmd = payload[IPMI_CMD_OFFSET];
	if (rmcp_intf_find_seq(sess, &match, &msgid, &user_port, &header) != 0) {
		IPMI_LOG_DEBUG("No match request for this RCMP IPMI response!\n");
		return;
	}

	data_len = len - (6 + 1);

	msg.hdr.type    = APPMSG_IPMI_MSG;
	msg.hdr.datalen = APPMSG_IPMI_MSG_LEN(data_len);

	ipmi = (struct appmsg_ipmi_msg *)msg.hdr.data;
	ipmi->id = msgid;

	memset(&ipmi->addr, 0, sizeof(ipmi->addr));
	ipmi->addr.type = IPMI_ADDR_TYPE_RMCP;
	ipmi->addr.addr.rmcp.host = ipmi_info->host;

	ipmi->msg.netfn = match.netfn;
	ipmi->msg.cmd   = match.cmd;
	ipmi->msg.data_len = data_len;
	if ((ipmi->msg.data_len > 0) && (ipmi->msg.data_len <= IPMI_MAX_DATA_LENGTH))
		memcpy_s(ipmi->msg.data, sizeof(ipmi->msg.data), payload + IPMI_DATA_OFFSET, data_len);

	deliver_app_msg_to_user(&msg.hdr, user_port, header);
}

/**
  *  @brief handle IPMIv2 get author capability request
  *
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
void handle_rmcp_plus_get_auth_cap_msg(struct rmcp_ipmi_info *ipmi_info)
{
	unsigned int host;
	unsigned int port;
	struct ipmi_session *sess;
	struct ipmi_session_table *table = &session_table;

	host = ipmi_info->host;
	port = ipmi_info->port;

	pthread_mutex_lock(&table->lock);

	list_for_each_entry(sess, &table->session_table[IPMI_SESSION_TABLE_HASH(host)], hash) {
		if ((sess->host == host) && (sess->port == port)) {
			handle_ipmi20_auth_cap_resp(sess, ipmi_info);

			break;
		}
	}

	pthread_mutex_unlock(&table->lock);
}

/**
  *  @brief handle IPMIv2 get channel cipher suite request
  *
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
void handle_rmcp_plus_get_channel_cipher_suite_msg(struct rmcp_ipmi_info *ipmi_info)
{
	unsigned int host;
	unsigned int port;
	struct ipmi_session *sess;
	struct ipmi_session_table *table = &session_table;

	host = ipmi_info->host;
	port = ipmi_info->port;

	pthread_mutex_lock(&table->lock);

	list_for_each_entry(sess, &table->session_table[IPMI_SESSION_TABLE_HASH(host)], hash) {
		if ((sess->host == host) && (sess->port == port)) {
			handle_ipmi20_get_channel_cipher_suites_resp(sess, ipmi_info);

			break;
		}
	}

	pthread_mutex_unlock(&table->lock);
}

/**
  *  @brief handle IPMIv2 msg
  *
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
void handle_rmcp_plus_ipmi_msg(struct rmcp_plus_ipmi_info *ipmi_info)
{
	unsigned int host;
	unsigned int port;
	struct ipmi_session *sess;
	struct ipmi_session_table *table = &session_table;

	host = ipmi_info->host;
	port = ipmi_info->port;

	pthread_mutex_lock(&table->lock);

	list_for_each_entry(sess, &table->session_table[IPMI_SESSION_TABLE_HASH(host)], hash) {
		if ((sess->host == host) && (sess->port == port)) {
			if (sess->state != IPMI20_SESS_STATE_ACTIVE)
				handle_ipmi20_sess_msg(sess, ipmi_info);
			else
				handle_ipmi20_user_msg(sess, ipmi_info);

			break;
		}
	}

	pthread_mutex_unlock(&table->lock);
}
#endif /* End of #ifdef IPMI20_SUPPORT */

/**************************************************************************************
 *                                                                                    *
 **************************************************************************************/

/**
  *  @brief setup new IPMI session
  *
  *  @param[in] sess IPMI session
  *  @param[in] addr IPMI address
  *  @param[in] username
  *  @param[in] password
  *  @return
  */
static void setup_ipmi_session(struct ipmi_session *sess, struct ipmi_addr *addr, char *username, char *password)
{
	struct ipmi_session *next = sess->next;
	struct ipmi_session_table *table = &session_table;

	bzero(sess, sizeof(struct ipmi_session));
	sess->next = next;

	list_add_tail(&sess->hash, &table->session_table[IPMI_SESSION_TABLE_HASH(addr->addr.rmcp.host)]);
	list_add_tail(&sess->list, &table->session_list);
	INIT_LIST_HEAD(&sess->msg_list);
	INIT_LIST_HEAD(&sess->req_list);

#ifdef DEBUG_IPMI
	clock_gettime(CLOCK_REALTIME, &sess->start_time);
#endif
	sess->timeout = RMCP_IPMI_SESSION_SETUP_TIMEOUT_MS;
	sess->host = addr->addr.rmcp.host;
	sess->port = addr->addr.rmcp.port;
	sess->session_id  = 0;
	sess->inbound_seq = 0;
	sess->curr_seq    = 0;

	/* Run as an Admin */
	sess->auth_type   = IPMI_SESSION_AUTHTYPE_NONE;
	sess->auth_status = 0;
	sess->privilege   = IPMI_SESSION_PRIV_ADMIN;
	if (!(addr->addr.rmcp.port))
		sess->port = htons(IPMI_RMCP_PORT);

	//memset(sess->username, 0, sizeof(sess->username));
	//memset(sess->password, 0, sizeof(sess->password));
	
	strncpy_safe((char *)sess->username, username, sizeof(sess->username), sizeof(sess->username) - 1);
	//strncpy((char *)sess->username, username, sizeof(sess->username) - 1);
	//strncpy_s_s((char *)sess->username, sizeof(sess->username), username, sizeof(sess->username) - 1,__FILE__, __LINE__, __FUNCTION__);
	
	rmm_log(ERROR, "----------------------name:%s, name:%s, size:%d, size:%d\n", sess->username, username, sizeof(sess->username), sizeof(sess->username) - 1);
	strncpy_safe((char *)sess->password, password, sizeof(sess->password), sizeof(sess->password) - 1);
	//strncpy((char *)sess->password, password, sizeof(sess->password) - 1);
	//strncpy_s_s((char *)sess->password, sizeof(sess->password), password, sizeof(sess->password) - 1,__FILE__, __LINE__, __FUNCTION__);
	rmm_log(ERROR, "----------------------passwd:%s, passwd:%s, size:%d, size:%d\n", sess->password, password, sizeof(sess->password), sizeof(sess->password) - 1);


	send_auth_capabilities_cmd(sess);
}

/**
  *  @brief format RMCP IPMI msg
  *
  *  @param[out] buff output buffer after formating
  *  @param[in] ipmi IPMI msg data
  *  @param[in] msgid msg id
  *  @param[in] timo timeout if no response
  *  @param[in] addr IPMI address
  *  @param[in] username
  *  @param[in] password
  *  @param[in] user_port socket port
  *  @return length of output data
  */
int format_rmcp_ipmi_msg(unsigned char *buff, struct ipmi_msg *ipmi,
			unsigned int msgid, unsigned int timeo,
			struct ipmi_addr *addr, char *username, char *password,
			unsigned short user_port, ipmi_json_ipc_header_t header)
{
	int len = 0;
	unsigned int host = addr->addr.rmcp.host;
	unsigned int port = addr->addr.rmcp.port;
	struct ipmi_session *sess, *pos;
	struct ipmi_session_table *table = &session_table;

	if (timeo > IPMI_MAX_TIMEOUT_MS)
		timeo = IPMI_MAX_TIMEOUT_MS;
	if (timeo == 0)
		timeo = IPMI_DFLT_TIMEOUT_MS;

	pthread_mutex_lock(&table->lock);

	sess = NULL;
	list_for_each_entry(pos, &table->session_table[IPMI_SESSION_TABLE_HASH(host)], hash) {
		if ((pos->host == host) && (pos->port == port)) {
			sess = pos;
			break;
		}
	}

	if (sess == NULL) {
		sess = table->ses_freelist;
		if (sess == NULL) {
			IPMI_LOG_DEBUG("RMCP-Intf: No Session Buff, adjust #IPMI_SESSION_QUEUE_SIZE(%d)!\n",
						   IPMI_SESSION_QUEUE_SIZE);
			goto ret;
		}
		table->ses_freelist = sess->next;
		sess->next = NULL;

		setup_ipmi_session(sess, addr, username, password);
	}

	if (sess->state != IPMI_SESS_STATE_ACTIVE) {
		struct ipmi_msg_sent *msgsent;

		msgsent = table->msg_freelist;
		if (msgsent == NULL) {
			IPMI_LOG_DEBUG("RMCP-Intf: No MSG SENT Buff, adjust #IPMI_MSGSENT_QUEUE_SIZE(%d)!\n",
						   IPMI_MSGSENT_QUEUE_SIZE);
			goto ret;
		}
		table->msg_freelist = msgsent->next;
		msgsent->next = NULL;

		msgsent->msgid = msgid;
		msgsent->timeo = timeo;
		msgsent->user_port = user_port;

		/*copy header*/
		memset(msgsent->header.method, 0, sizeof(JRPC_METHOD_MAX));
		msgsent->header.ip = header.ip;
		msgsent->header.port = header.port;
		msgsent->header.json_ipc_id = header.json_ipc_id;
		if (strnlen_s((const char *)header.method, sizeof(header.method)-1) < JRPC_METHOD_MAX)
			memcpy_s(msgsent->header.method, sizeof(msgsent->header.method), header.method, strnlen_s((const char *)header.method, sizeof(header.method)-1));

		memcpy_s(&msgsent->msg, sizeof(msgsent->msg), ipmi, offset_of(struct ipmi_msg, data) + ipmi->data_len);

		list_add_tail(&msgsent->list, &sess->msg_list);
	} else {
		struct rmcp_match_info match;

		match.netfn = ipmi->netfn;
		match.cmd   = ipmi->cmd;

		if (rmcp_intf_next_seq(sess, &match, timeo, msgid, user_port, header) != 0)
			goto ret;
#ifdef IPMI20_SUPPORT
		if (sess->auth_type == IPMI20_AUTH_TYPE_RMCP_PLUS) {
			unsigned char *msg_to_send = NULL;
			struct rmcp_plus_ipmi_info ipmi_info = {0};

			if (sess->crypt_alg != CRYPT_ALG_NONE)
				ipmi_info.payload_type |= IPMI20_PAYLOAD_ENCRYPTED_MASK;

			if (sess->integrity_alg != INTEGRITY_ALG_NONE)
				ipmi_info.payload_type |= IPMI20_PAYLOAD_AUTH_MASK;

			ipmi_info.session_id = sess->bmc_id;
			ipmi_info.session_seq = (++sess->session_seq);

			if (IPMI_BRIDGE_MSG_NONE != ipmi->union_app_req.bridge.bridge_level) {
				//match.netfn = ipmi->union_app_req.bridge.br_netfn;
				match.netfn = IPMI_BRIDGE_NETFN_APP;
				if (IPMI_BRIDGE_MSG_SINGLE == ipmi->union_app_req.bridge.bridge_level)
					match.cmd   = ipmi->union_app_req.bridge.br_cmd;
				else
					match.cmd   = IPMI_BRIDGE_IPMI_CMD;

				if (rmcp_intf_next_seq(sess, &match, timeo, msgid, user_port, header) != 0)
					goto ret;

				msg_to_send = ipmi20_lan_plus_build_cmd_br(sess, &ipmi_info, ipmi, match.seq, &len, ipmi->union_app_req.bridge.bridge_level);
			} else
				msg_to_send = ipmi20_lan_plus_build_cmd(sess, &ipmi_info, ipmi, match.seq, &len);

			if (NULL == msg_to_send)
				return 0;

			memcpy_s(buff, len, msg_to_send, len);

			free(msg_to_send);
		} else {
#endif
			if (IPMI_BRIDGE_MSG_NONE != ipmi->union_app_req.bridge.bridge_level) {
				//match.netfn = ipmi->union_app_req.bridge.br_netfn;
				match.netfn = IPMI_BRIDGE_NETFN_APP;
				if (IPMI_BRIDGE_MSG_SINGLE == ipmi->union_app_req.bridge.bridge_level)
					match.cmd   = ipmi->union_app_req.bridge.br_cmd;
				else
					match.cmd   = IPMI_BRIDGE_IPMI_CMD;

				if (rmcp_intf_next_seq(sess, &match, timeo, msgid, user_port, header) != 0)
					goto ret;

				len = ipmi_lan_build_cmd_br(buff, sess, ipmi, match.seq, ipmi->union_app_req.bridge.bridge_level);
			} else {
				/* Need to add the session id to the ipmi message when
				   closing the session */
				if ((ipmi->netfn == IPMI_RMCP_CLOSE_SESSION_NETFN)
					&& (ipmi->cmd == IPMI_RMCP_CLOSE_SESSION_CMD)
					&& (ipmi->data_len == 0)) {
					memcpy_s(ipmi->data, sizeof(ipmi->data), &sess->session_id, IPMI_SESSION_ID_LEN);
					ipmi->data_len = IPMI_SESSION_ID_LEN;
				}

				len = ipmi_lan_build_cmd(buff, sess, ipmi, match.seq);
				if (len == 0)
					goto ret;
			}
#ifdef IPMI20_SUPPORT
		}
#endif

		/*sess->timeout = RMCP_IPMI_SESSION_IDLE_TIMEOUT_MS;*/	/* update the session time */
	}

ret:
	pthread_mutex_unlock(&table->lock);

	return len;
}

/**
  *  @brief Handle the IPMI session setting up message
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi_sess_msg(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	if ((ipmi_info->netfn & 0x3E) != IPMI_NETFN_APP)
		return;

	switch (sess->state) {
	case IPMI_SESS_STATE_AUTH_CAP_REQ_SENT:
		if (ipmi_info->cmd == IPMI_GET_CHAN_AUTH_CAP) {
			handle_auth_capabilities_resp(sess, ipmi_info);
		}
		break;

	case IPMI_SESS_STATE_SESSION_CHALLENGE_SENT:
		if (ipmi_info->cmd == IPMI_GET_SESSION_CHALL) {
			handle_session_challenge_resp(sess, ipmi_info);
		}
		break;

	case IPMI_SESS_STATE_SESSION_ACTIVATE_SENT:
		if (ipmi_info->cmd == IPMI_ACTIVATE_SESSION) {
			handle_activate_session_resp(sess, ipmi_info);
		}
		break;

	case IPMI_SESS_STATE_SESSION_PRIV_SENT:
		if (ipmi_info->cmd == IPMI_SET_SESS_PRIV_LEV) {
			handle_session_privlvl_resp(sess, ipmi_info);
		}
		break;

	default:
		break;
	}
}


/**
  *  @brief Handle the IPMI user message for app user in active session
  *
  *  @param[in] sess IPMI session
  *  @param[in] ipmi_info include the IPMI msg context
  *  @return
  */
static void handle_ipmi_user_msg(struct ipmi_session *sess,
			struct rmcp_ipmi_info *ipmi_info)
{
	unsigned int msgid;
	unsigned short user_port;
	struct rmcp_match_info match;
	struct appmsg_ipmi_msg *ipmi;
	union app_msg_union msg;
	ipmi_json_ipc_header_t header;

	if (ipmi_info->session_id != sess->session_id ||
	    ipmi_info->auth_type  != sess->auth_type) {
		IPMI_LOG_DEBUG("IPMI session not MATCH, Rcv'd sess_id = %u auth_type = %u, "
					   "Table sess_id = %u auth_type = %u!\n",
					   ipmi_info->session_id, ipmi_info->auth_type,
					   sess->session_id, sess->auth_type);
		return;
	}

	match.seq   = ipmi_info->seq;
	match.netfn = ipmi_info->netfn;
	match.cmd   = ipmi_info->cmd;

	if (rmcp_intf_find_seq(sess, &match, &msgid, &user_port, &header) != 0) {
		/* the bridged msg has two response msg with the same id
		  * need to reduce to find the first match */
		match.seq = match.seq - 1;
		if (rmcp_intf_find_seq(sess, &match, &msgid, &user_port, &header) != 0) {
			IPMI_LOG_DEBUG("No match request for this RCMP IPMI response, seq is %d, netfn is %d, cmd is %d!\n",
																										match.seq,
																										match.netfn,
																										match.cmd);
			return;
		}
	}

	msg.hdr.type    = APPMSG_IPMI_MSG;
	msg.hdr.datalen = APPMSG_IPMI_MSG_LEN(ipmi_info->data_len);

	ipmi = (struct appmsg_ipmi_msg *)msg.hdr.data;
	ipmi->id = msgid;

	memset(&ipmi->addr, 0, sizeof(ipmi->addr));
	ipmi->addr.type = IPMI_ADDR_TYPE_RMCP;
	ipmi->addr.addr.rmcp.host = ipmi_info->host;

	ipmi->msg.netfn = match.netfn;
	ipmi->msg.cmd   = match.cmd;
	ipmi->msg.data_len = ipmi_info->data_len;
	if ((ipmi->msg.data_len > 0) && (ipmi->msg.data_len <= IPMI_MAX_DATA_LENGTH))
		memcpy_s(ipmi->msg.data, sizeof(ipmi->msg.data), ipmi_info->ipmi_data, ipmi_info->data_len);

	deliver_app_msg_to_user(&msg.hdr, user_port, header);

	/* Need to release the session if the session has been closed. */
	if (((0x3E & match.netfn) == IPMI_RMCP_CLOSE_SESSION_NETFN)
		&& (IPMI_RMCP_CLOSE_SESSION_CMD == match.cmd)) {
		relase_ipmi_session(sess);
	}
}

/**
  *  @brief handle RMCP message
  *
  *  @param[in] ipmi_info include the RMCP msg context
  *  @return
  */
void handle_rmcp_ipmi_msg(struct rmcp_ipmi_info *ipmi_info)
{
	unsigned int host;
	unsigned int port;
	struct ipmi_session *sess;
	struct ipmi_session_table *table = &session_table;

	if ((ipmi_info->netfn & 0x01) == 0 || ipmi_info->data_len == 0) {
		IPMI_LOG_DEBUG("This is NOT a RMCP IPMI response message, Ignore ...\n");
		return;
	}

	host = ipmi_info->host;
	port = ipmi_info->port;

	pthread_mutex_lock(&table->lock);

	list_for_each_entry(sess, &table->session_table[IPMI_SESSION_TABLE_HASH(host)], hash) {
		if ((sess->host == host) && (sess->port == port)) {
			if (sess->state != IPMI_SESS_STATE_ACTIVE)
				handle_ipmi_sess_msg(sess, ipmi_info);
			else {
				sess->timeout = RMCP_IPMI_SESSION_IDLE_TIMEOUT_MS;	/* update the session time */
				handle_ipmi_user_msg(sess, ipmi_info);
			}

			break;
		}
	}

	pthread_mutex_unlock(&table->lock);
}

/**
  *  @brief init RMCP session
  *
  *  @param
  *  @return
  */
void init_rmcp_session(void)
{
	int i;
	struct ipmi_session_table *table = &session_table;

	pthread_mutex_init(&table->lock, NULL);

	table->ses_freelist = mem_freelist_create(IPMI_SESSION_QUEUE_SIZE,
										sizeof(struct ipmi_session));
	table->msg_freelist = mem_freelist_create(IPMI_MSGSENT_QUEUE_SIZE,
										sizeof(struct ipmi_msg_sent));
	table->req_freelist = mem_freelist_create(IPMI_REQUEST_QUEUE_SIZE,
										sizeof(struct ipmi_req_entry));
	if (table->ses_freelist == NULL ||
	    table->msg_freelist == NULL ||
	    table->req_freelist == NULL) {
		IPMI_LOG_DEBUG("RMCP session freelist:\n"
					   "    table->ses_freelist == %p\n"
					   "    table->msg_freelist == %p\n"
					   "    table->req_freelist == %p\n\n",
					   table->ses_freelist, table->msg_freelist, table->req_freelist);
		FATAL("Failed to create the RMCP session freelist!\n");
	}

	INIT_LIST_HEAD(&table->request_list);
	INIT_LIST_HEAD(&table->session_list);

	for (i = 0; i < IPMI_SESSION_TABLE_SIZE; i++)
		INIT_LIST_HEAD(&table->session_table[i]);

	if (pthread_create(&table->time_tid, NULL, rmcp_time_thread, NULL) != 0)
		FATAL("Failed to create RMCP time thread!\n");
}

