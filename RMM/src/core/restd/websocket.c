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


/* http://tools.ietf.org/rfc/rfc6455.txt */
#include <pthread.h>
#include <openssl/sha.h>

#include "libjipmi/common.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libutils/rack.h"
#include "handler/handler.h"
#include "libutils/rmm.h"
#include "libwrap/wrap.h"
#include "libutils/rmm_json.h"
#include "libjson/json.h"
#include "libutils/base64.h"
#include "websocket.h"
#include "libsecurec/safe_lib.h"


#define SHA1_DIGEST_BYTES	20
#define WS_KEEP_TIME	120		/* 2 min */
#define WS_FRAMESZ		0x4000	/* MAX Frame size we support: 16KB */
#define WS_MSGSZ		0xFFFF	/* MAX Message size we support: 64KB, -1 for '\0' */
#define WS_SECRET		"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

enum ws_state {
	WS_STATE_ERROR,
	WS_STATE_READING,
	WS_STATE_FRAME_COMPLETE,
	WS_STATE_MSG_COMPLETE	/* Like WS_STATE_FRAME_COMPLETE, but with FIN set */
};

enum ws_frame_type {
	WS_FRAME_TEXT	= 0x01,
	WS_FRAME_BINARY	= 0x02,
	WS_FRAME_CLOSE	= 0x08,
	WS_FRAME_PING	= 0x09,
	WS_FRAME_PONG	= 0x0A
};

struct ws_msg {
	int32 type;
	int32 payload_sz;
	uint8 payload[WS_MSGSZ + 1];
};


static struct ws_msg ws_client_msg;
static int32 ws_fd;
static memdb_integer sub_node_add_handle = -1;
static memdb_integer sub_node_del_handle = -1;
static memdb_integer sub_host_state_handle = -1;

static enum ws_state ws_parse_frame(uint8 *frame, int32 *psz, struct ws_msg *msg);
static int32 ws_send_frame(int32 fd, int32 type, const uint8 *payload, int32 sz);
static void ws_process_cmd(const json_t *json);
static int32 ws_compute_handshake(const int8 *key, int8 *out, int32 *out_sz);
static void ws_handshake_reply(struct http_request *req);
static void ws_node_event_callback_fn(struct event_info *evt, void *cb_data);


int32 mutex_cond_init(mutex_cond_t *mutex_cond)
{
	int32 rs = 0;

	rs = pthread_mutex_init(&(mutex_cond->mutex_cb), NULL);
	if (0 != rs) {
		HTTPD_IPMI_PRINT("%s:%d:%s error %d\n", __FILE__, __LINE__, __func__, rs);
		return rs;
	}
	rs = pthread_cond_init(&(mutex_cond->cond_cb), NULL);
	if (0 != rs) {
		HTTPD_IPMI_PRINT("%s:%d:%s error %d\n", __FILE__, __LINE__, __func__, rs);
		return rs;
	}
	return 0;
}

int32 mutex_cond_destroy(mutex_cond_t *mutex_cond)
{
	int32 rs = 0;

	rs = pthread_mutex_destroy(&(mutex_cond->mutex_cb));
	if (0 != rs)	{
		HTTPD_IPMI_PRINT("%s:%d:%s error %d\n", __FILE__, __LINE__, __func__, rs);
		return rs;
	}
	rs = pthread_cond_destroy(&(mutex_cond->cond_cb));
	if (0 != rs) {
		HTTPD_IPMI_PRINT("%s:%d:%s error %d\n", __FILE__, __LINE__, __func__, rs);
		return rs;
	}
	return 0;
}

int32 mutex_cond_signal(mutex_cond_t *mutex_cond)
{
	pthread_mutex_lock(&(mutex_cond->mutex_cb));
	pthread_cond_signal(&(mutex_cond->cond_cb));
	pthread_mutex_unlock(&(mutex_cond->mutex_cb));
	return 0;
}

int32 mutex_cond_wait(mutex_cond_t *mutex_cond)
{
	pthread_mutex_lock(&(mutex_cond->mutex_cb));
	pthread_cond_wait(&(mutex_cond->cond_cb), &(mutex_cond->mutex_cb));
	pthread_mutex_unlock(&(mutex_cond->mutex_cb));
	return 0;
}

void ws_process(struct http_request *req)
{
	int32 fd;
	int32 maxfd;
	int32 alive;
	int32 rc, sz;
	fd_set rfds;
	uint32 count;
	struct ws_msg *msg;
	struct timeval timo;
	enum ws_state state;
	uint8 frame[WS_FRAMESZ];

	ws_handshake_reply(req);

	if (libdb_init_subscription(NOTIFY_BY_SELECT, ws_node_event_callback_fn, NULL) < 0) {
		printf("Failed to init select subscirbe mode!\n");
		exit(-1);
	}

	sz = 0;
	alive = 1;
	count = 0;
	fd = req->fd;
	msg = &ws_client_msg;
	msg->type = 0;
	msg->payload_sz = 0;

	ws_fd = fd;

	for (;;) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		maxfd = fd;
		libdb_event_selectfds(&rfds, &maxfd);

		timo.tv_sec  = WS_KEEP_TIME;
		timo.tv_usec = 0;
		rc = select(maxfd + 1, &rfds, NULL, NULL, &timo);
		if (rc < 0)
			break;

		libdb_event_processfds(&rfds);

		if (rc == 0) {
			if (alive == 0)
				break;

			alive = 0;
			count++;
			ws_send_frame(fd, WS_FRAME_PING, (void *)&count, sizeof(count));
			continue;
		}

		if (!FD_ISSET(fd, &rfds))
			continue;

		rc = read(fd, frame + sz, WS_FRAMESZ - sz);
		if (rc <= 0)
			break;
		sz += rc;
		alive = 1;

again:
		state = ws_parse_frame(frame, &sz, msg);
		if (state == WS_STATE_ERROR)
			break;
		if (state == WS_STATE_READING)
			continue;
		if (state == WS_STATE_MSG_COMPLETE) {
			if (msg->type == WS_FRAME_CLOSE) {
				ws_send_frame(fd, WS_FRAME_CLOSE, NULL, 0);
				break;
			}
			if (msg->type == WS_FRAME_PING) {
				if (!ws_send_frame(fd, WS_FRAME_PONG, msg->payload, msg->payload_sz))
					break;
			} else if (msg->type == WS_FRAME_TEXT) {
				json_t *json;

				msg->payload[msg->payload_sz] = '\0';
				json = json_parse((int8 *)msg->payload);
				if (json != NULL) {
					ws_process_cmd(json);
					json_free(json);
				}
			}

			msg->type = 0;
			msg->payload_sz = 0;
		}

		if (sz != 0)
			goto again;
	}

	if (sub_node_add_handle != -1)
		libdb_unsubscribe_event(DB_RMM, sub_node_add_handle, LOCK_ID_NULL);
	if (sub_node_del_handle != -1)
		libdb_unsubscribe_event(DB_RMM, sub_node_del_handle, LOCK_ID_NULL);
	if (sub_host_state_handle != -1)
		libdb_unsubscribe_event(DB_RMM, sub_host_state_handle, LOCK_ID_NULL);

	libdb_exit_memdb();
}


static int32 ws_compute_handshake(const int8 *key, int8 *out, int32 *out_sz)
{
	int32 buf_sz;
	int32 key_sz = strnlen_s(key, RSIZE_MAX_STR);
	uint8 dig[SHA1_DIGEST_BYTES];
	uint8 buf[MAX_HEADER_LEN];

	buf_sz = key_sz + sizeof(WS_SECRET) - 1;
	if (buf_sz > sizeof(buf))
		return 0;

	memcpy_s(buf, sizeof(buf), key, key_sz);
	memcpy_s(buf + key_sz, sizeof(buf)-key_sz, WS_SECRET, sizeof(WS_SECRET) - 1);

	SHA1(buf, buf_sz, dig);
	*out_sz = base64_encode(out, dig, SHA1_DIGEST_BYTES);

	return 1;
}

static void ws_handshake_reply(struct http_request *req)
{
	int32  fd = req->fd;
	int32  handshake_sz, reply_sz;
	int8 handshake[MAX_HEADER_LEN], reply[BUFFSIZ];

	if (req->method != M_GET ||
		req->host == NULL || req->origin == NULL ||
		req->is_connection_upgrade == 0 ||
		req->sec_websocket_key == NULL ||
		req->sec_websocket_version != WS_RFC6455_VERSION) {
		send_error(fd, 400, "Bad Request", NULL, "Not a valid WebSocket request!");
		return;
	}

	if (!ws_compute_handshake(req->sec_websocket_key, handshake, &handshake_sz)) {
		send_error(fd, 400, "Bad Request", NULL, "Sec-WebSocket-key is too long!");
		return;
	}

	reply_sz = snprintf_s_s(reply, sizeof(reply),
					"HTTP/1.1 101 Switching Protocols\r\n"
					"Upgrade: websocket\r\n"
					"Connection: Upgrade\r\n"
					"Sec-WebSocket-Accept: %s\r\n"
					"\r\n", handshake);
	if (http_write(fd, reply, reply_sz) != reply_sz)
		exit(-1);
}

static inline void ws_msg_add(struct ws_msg *m, const uint8 *p, int32 len,
			uint8 *mask)
{
	int32 i;
	uint8 *cp = &m->payload[m->payload_sz];

	for (i = 0; i < len; i++)
		cp[i] = p[i] ^ mask[i%4];

	m->payload_sz += len;
}

static enum ws_state ws_parse_frame(uint8 *frame, int32 *psz, struct ws_msg *msg)
{
	int32 last;
	int32 len, sz;
	uint8 *data;
	uint8 *mask;

	sz = *psz;
	if (sz < 2)
		return WS_STATE_READING;

	if ((frame[1] & 0x80) == 0)
		return WS_STATE_ERROR;

	len = frame[1] & 0x7F;
	if (len <= 125) {
		if (sz < 6)
			return WS_STATE_READING;
		mask = frame + 2;
		data = frame + 6;
	} else if (len == 126) {
		if (sz < 8)
			return WS_STATE_READING;
		len = (frame[2] << 8) | (frame[3]);
		mask = frame + 4;
		data = frame + 8;
	} else {
		return WS_STATE_ERROR;	/* NOT Support 64 bits Now */
	}

	if (len + msg->payload_sz > WS_MSGSZ)
		return WS_STATE_ERROR;

	sz -= (data - frame) + len;
	if (sz < 0)
		return WS_STATE_READING;	/* Not Enough Data */

	last = frame[0] & 0x80;
	msg->type = frame[0] & 0x0F;
	ws_msg_add(msg, data, len, mask);

	*psz = sz;
	if (sz != 0)
		memcpy_s(frame, sz, data + len, sz);	/* More Frame, next loop to process. */

	if (last)
		return WS_STATE_MSG_COMPLETE;
	else
		return WS_STATE_FRAME_COMPLETE;
}

static int32 ws_send_frame(int32 fd, int32 type, const uint8 *payload, int32 sz)
{
	int32 hdrsz;
	uint8 hdr[4];

	hdr[0] = 0x80 | (type & 0x0F);
	if (sz <= 125) {
		hdrsz  = 2;
		hdr[1] = (uint8) sz;
	} else if (sz <= 0xFFFF) {
		hdrsz = 4;
		hdr[1] = 126;
		hdr[2] = (uint8)((sz >> 8) & 0xFF);
		hdr[3] = (uint8)((sz) & 0xFF);
	} else {
		return 0;
	}

	if (http_write(fd, (int8 *)hdr, hdrsz) != hdrsz ||
	    http_write(fd, (int8 *)payload, sz) != sz)
		return 0;

	return 1;
}

/*************************************************************/


static void pub_node_event(memdb_integer node_id, int32 add, uint8 usize)
{
	uint32 zoneId, trayId, nodeId;
	uint8 height, width;
	int32  msglen;
	int8 msg[256];

	zoneId = ZONE_ID(node_id);
	trayId = TRAY_ID(node_id);
	nodeId = NODE_ID(node_id);

	height = MC_NODE_HEIGHT(usize);
	width  = MC_NODE_WIDTH(usize);

	/*** simple JSON format ***
	 *
	 *  1. { "nodeAdded"   : { "zone" : 1, "tray" : 1, "node" : 1, "height": 1, "width": 1} }
	 *  2. { "nodeRemoved" : { "zone" : 1, "tray" : 1, "node" : 1 } }
	 */
	if (add) {
		msglen = snprintf(msg, sizeof(msg),
			"{\"%s\":{"
				"\"%s\":%u,\"%s\":%u,\"%s\":%u,"
				"\"%s\":%u,\"%s\":%u}}",
			RMM_JSON_NODE_ADDED, RMM_JSON_ZONE, zoneId, RMM_JSON_TRAY, trayId, RMM_JSON_NODE, nodeId, RMM_JSON_HEIGHT, height, RMM_JSON_WIDTH, width);
	} else {
		msglen = snprintf(msg, sizeof(msg),
			"{\"%s\":{"
				"\"%s\":%u,\"%s\":%u,\"%s\":%u}}",
			RMM_JSON_NODE_REMOVED, RMM_JSON_ZONE, zoneId, RMM_JSON_TRAY, trayId, RMM_JSON_NODE, nodeId);
	}

	ws_send_frame(ws_fd, WS_FRAME_TEXT, (const uint8 *)msg, msglen);
}

static void pub_host_state(uint32 addr, int32 on)
{
	uint32 zoneId, trayId, nodeId;
	int32  msglen;
	int8 msg[256];

	zoneId = ZONE_ID(addr);
	trayId = TRAY_ID(addr);
	nodeId = NODE_ID(addr);

	/*** simple JSON format ***
	 *
	 *  1. { "hostStateOn"  : { "zone" : 1, "tray" : 1, "node" : 1} }
	 *  2. { "hostStateOff" : { "zone" : 1, "tray" : 1, "node" : 1} }
	 */
	msglen = snprintf(msg, sizeof(msg),
			 "{\"%s\":{\"%s\":%u, \"%s\":%u, \"%s\":%u}}",
			 on ? RMM_JSON_HOST_STATE_ON : RMM_JSON_HOST_STATE_OFF,
			 RMM_JSON_ZONE, zoneId, RMM_JSON_TRAY, trayId, RMM_JSON_NODE, nodeId);

	ws_send_frame(ws_fd, WS_FRAME_TEXT, (uint8 *)msg, msglen);
}

static void ws_node_event_callback_fn(struct event_info *evt, void *cb_data)
{
	int64 error_code = 0;

	if (evt->event == EVENT_NODE_CREATE || evt->event == EVENT_NODE_DELETE) {
		int8 usize = 0;

		if (evt->event == EVENT_NODE_CREATE) {
			error_code = libdb_attr_get_char(DB_RMM, evt->nnodeid, MC_U_SIZE, &usize, LOCK_ID_NULL);
			if (error_code != 0) {
				HTTPD_IPMI_PRINT("%s:%d: error code:%d\n", __FUNCTION__, __LINE__, (int32)error_code);
			}
		}
		pub_node_event(evt->nnodeid , evt->event == EVENT_NODE_CREATE, (uint8)usize);
	} else if (evt->event == EVENT_NODE_ATTR) {
		int32 on;
		uint32 addr;
		int8 *name, *data;

		name = event_attr_name(evt);
		addr = (uint32) atoi(&name[sizeof(PSU_HOSTSTATE_PREFIX) - 1]);
		data = event_attr_data(evt);
		if (evt->aaction != EVENT_ATTR_ACTION_DEL &&
		    strncmp(data, PSU_HOSTSTATE_ON, sizeof(PSU_HOSTSTATE_ON) - 1) == 0)
			on = 1;
		else
			on = 0;
		pub_host_state(addr, on);
	}
}

static void show_host_state(void)
{
	int32 on;
	uint32 addr;

	int32 offset, size;
	memdb_integer psu;
	int32 psu_num;
	void *attr;
	int8 *name, *data;
	struct attr_info *info;
	struct node_info *node;

	node = libdb_list_node_by_type(DB_RMM, MC_TYPE_PSU,
								   MC_TYPE_PSU, &psu_num, NULL, LOCK_ID_NULL);

	if (node == NULL) {
		libdb_free_node(node);
		return;
	}
	/* hc add for debug */
	psu = node[0].node_id;
	libdb_free_node(node);

	attr = libdb_list_attrs_by_node(DB_RMM, psu, &size, LOCK_ID_NULL);
	if (attr == NULL)
		return;

	foreach_attr_info(info, offset, attr, size) {
		name = attr_name(info);
		if (strncmp(name, PSU_HOSTSTATE_PREFIX, sizeof(PSU_HOSTSTATE_PREFIX) - 1) != 0)
			continue;

		addr = (uint32) atoi(&name[sizeof(PSU_HOSTSTATE_PREFIX) - 1]);
		data = attr_data(info);
		if (strncmp(data, PSU_HOSTSTATE_ON, sizeof(PSU_HOSTSTATE_ON) - 1) == 0)
			on = 1;
		else
			on = 0;
		pub_host_state(addr, on);
	}
}

static int32 ws_dev_id_resp_handler(int32 result, uint8 *rsp, int32 rsp_len,
			void *cb_data)
{
	get_dev_id_cb_data_t *p_get_dev_id_cb_data = (get_dev_id_cb_data_t *)cb_data;
	mutex_cond_t *p_mutex_cond = (mutex_cond_t *)cb_data;
	uint8 *dev_id = (uint8 *)&(p_get_dev_id_cb_data->rsp);
	int32 data_len = 0;
	int32 i;

	HTTPD_IPMI_PRINT("%s:%d:%s enter\n", __FILE__, __LINE__, __func__);
	if (NULL == cb_data)
		return -1;

	printf("DEVID RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	if (rsp[0] != IPMI_CC_OK)
		HTTPD_IPMI_PRINT("rsp error, rsp cc is %02x\n", rsp[0]);
	else
		HTTPD_IPMI_PRINT("rsp OK, len[%d]\n", rsp_len);

	data_len = ((LEN_GET_DEV_ID_RSP > rsp_len)?rsp_len:LEN_GET_DEV_ID_RSP);
	memcpy_s(dev_id, data_len, rsp, data_len);

	mutex_cond_signal(p_mutex_cond);

	ws_send_frame(ws_fd, WS_FRAME_TEXT, (uint8 *)"test ipmi respond OK", strnlen_s("test ipmi respond OK", RSIZE_MAX_STR));
	HTTPD_IPMI_PRINT("%s:%d:%s exit\n", __FILE__, __LINE__, __func__);
	return 0;
}

static int32 handle_ipmi_cmd(int32 host)
{
	struct jipmi_msg req = {};
	get_dev_id_cb_data_t *p_cb_data = NULL;
	mutex_cond_t *p_mutex_cond = NULL;

	HTTPD_IPMI_PRINT("%s:%d:%s enter\n", __FILE__, __LINE__, __func__);
	p_cb_data = malloc(sizeof(get_dev_id_cb_data_t));
	if (NULL == p_cb_data) {
		HTTPD_IPMI_PRINT("%s:%d:%s error\n", __FILE__, __LINE__, __func__);
		return RESULT_MALLOC_ERR;
	}
	memset(p_cb_data, 0, sizeof(get_dev_id_cb_data_t));

	p_mutex_cond = (mutex_cond_t *)p_cb_data;
	mutex_cond_init(p_mutex_cond);


	FILL_INT(req.netfn,		HTTPD_APP_IPMI_NETFN);
	FILL_INT(req.cmd,		GET_DEVICE_ID_CMD);
	FILL_INT(req.data_len,	0);

	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, ws_dev_id_resp_handler, (void *)p_cb_data, JIPMI_NON_SYNC);

	mutex_cond_wait(p_mutex_cond);
	mutex_cond_destroy(p_mutex_cond);

	free(p_cb_data);
	HTTPD_IPMI_PRINT("%s:%d:%s exit\n", __FILE__, __LINE__, __func__);
	return 0;

}

static int32 handle_memdb_cmd(void)
{
	memdb_integer node;
	int8 output[128] = {0};
	int8 name[64] = {0}, string[64] = {0};
	int64 error_code = 0;
	int32 rc = 0;

	node = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM,
							 SNAPSHOT_NEED, LOCK_ID_NULL);
	snprintf_s_ll(name, sizeof(name), "restd-test%lld", node);
	snprintf_s_ll(string, sizeof(string), "memdb test %lld", node);
	rc = libdb_attr_set_string(DB_RMM, node, name, 0x0, string, 0, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set %s fail\n", name);
		return -1;
	}

	error_code = libdb_attr_get_string(DB_RMM, node, name, output, 128, LOCK_ID_NULL);
	if (error_code != 0) {
		HTTPD_IPMI_PRINT("%s:%d: error code:%d\n", __FUNCTION__, __LINE__, (int32)error_code);
		return (int32)error_code;
	} else
		ws_send_frame(ws_fd, WS_FRAME_TEXT, (uint8 *)(output), strnlen_s(output, sizeof(output)-1));

	return 0;
}


/*
 * 1. { "nodeAdded" : "subscribe" }
 * 2. { "nodeAdded" : "unsubscribe" }
 * 3. { "nodeRemoved" : "subscribe" }
 * 4. { "nodeRemoved" : "unsubscribe" }
 * 5. { "hostState" : "subscribe" }
 * 6. { "hostState" : "unsubscribe" }
 */
static void ws_process_cmd(const json_t *json)
{
	json_t *target;
	int8   *command;
	int8    usize;
	int32 psu_num;
	struct node_info *node;
	int64 error_code = 0;

	target = json_object_get(json, RMM_JSON_NODE_ADDED);
	if (target != NULL) {
		if ((command = json_string_value(target)) == NULL)
			return;

		if (strcmp(command, RMM_JSON_SUBSCRIPTION) == 0) {
			int32 i;
			int32 nodenum;
			struct node_info *nodeinfo;

			if (sub_node_add_handle != -1)
				return;

			sub_node_add_handle = libdb_subscribe_type_of_node_create(DB_RMM, MC_TYPE_DRAWER,
										MC_TYPE_BMC, LOCK_ID_NULL);

			/* Node has been added */
			nodeinfo = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_BMC, &nodenum, NULL,LOCK_ID_NULL);
			for (i = 0; i < nodenum; i++) {
				error_code = libdb_attr_get_char(DB_RMM, nodeinfo[i].node_id, MC_U_SIZE, &usize, LOCK_ID_NULL);
				if (error_code != 0) {
					HTTPD_IPMI_PRINT("%s:%d: error code:%d\n", __FUNCTION__, __LINE__, (int32)error_code);
				}
				pub_node_event(nodeinfo[i].node_id, 1, (uint8)usize);
			}
			libdb_free_node(nodeinfo);
		} else if (strcmp(command, RMM_JSON_UNSUBSCRIPTION) == 0) {
			if (sub_node_add_handle != -1) {
				libdb_unsubscribe_event(DB_RMM, sub_node_add_handle, LOCK_ID_NULL);
				sub_node_add_handle = -1;
			}
		}

		return;
	}

	target = json_object_get(json, RMM_JSON_NODE_REMOVED);
	if (target != NULL) {
		if ((command = json_string_value(target)) == NULL)
			return;

		if (strcmp(command, RMM_JSON_SUBSCRIPTION) == 0) {
			if (sub_node_del_handle != -1)
				return;

			sub_node_del_handle = libdb_subscribe_type_of_node_delete(DB_RMM, MC_TYPE_DRAWER,
										MC_TYPE_BMC, LOCK_ID_NULL);
		} else if (strcmp(command, RMM_JSON_UNSUBSCRIPTION) == 0) {
			if (sub_node_del_handle != -1) {
				libdb_unsubscribe_event(DB_RMM, sub_node_del_handle, LOCK_ID_NULL);
				sub_node_del_handle = -1;
			}
		}

		return;
	}

	target = json_object_get(json, RMM_JSON_HOST_STATE);
	if (target != NULL) {
		if ((command = json_string_value(target)) == NULL)
			return;

		if (strcmp(command, RMM_JSON_SUBSCRIPTION) == 0) {
			if (sub_host_state_handle != -1)
				return;
			/* hc add for debug */
			node = libdb_list_node_by_type(DB_RMM, MC_TYPE_PSU, MC_TYPE_PSU, &psu_num, NULL, LOCK_ID_NULL);
			if (node == NULL) {
				HTTPD_IPMI_PRINT("failed to get PSU node");
				return;
			}
			sub_host_state_handle = libdb_subscribe_attr_by_prefix(DB_RMM, node[0].node_id, PSU_HOSTSTATE_PREFIX, LOCK_ID_NULL);
			libdb_free_node(node);
			show_host_state();	/* hostState has been added */
		} else if (strcmp(command, RMM_JSON_UNSUBSCRIPTION) == 0) {
			if (sub_host_state_handle != -1) {
				libdb_unsubscribe_event(DB_RMM, sub_host_state_handle, LOCK_ID_NULL);
				sub_host_state_handle = -1;
			}
		}
		return;
	}

	target = json_object_get(json, "ipmi");
	if (target != NULL) {
		if ((command = json_string_value(target)) == NULL)
			return;

		if (strcmp(command, "test") == 0)
			handle_ipmi_cmd(inet_addr("192.168.10.4"));
	}

	target = json_object_get(json, "memdb");
	if (target != NULL) {
		if ((command = json_string_value(target)) == NULL)
			return;

		if (strcmp(command, "test") == 0)
			handle_memdb_cmd();
	}
}
/*************************************************************/

