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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "libutils/list.h"
#include "libutils/rmm.h"
#include "libjipmi/libjipmi.h"
#include "libjipmi/jrpc_ipmi.h"
#include "libutils/sock.h"
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libutils/base64.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libsecurec/safe_lib.h"


#define MAX_RPC_LEN     20

struct jipmi_rsp_hndl {
	struct list_head list;

	unsigned int timeout;
	int          broadcast;

	int64        jrpc_id;

	jipmi_rsp_callback_fn cb_fn;
	void                *cb_arg;
};

static int async_jipmi_fd = -1;
static int sync_jipmi_fd = -1;
static unsigned short user_port;

static struct list_head rsp_hndl_head = LIST_HEAD_INIT(rsp_hndl_head);
static struct list_head req_hndl_head = LIST_HEAD_INIT(req_hndl_head);

static pthread_mutex_t libjipmi_lock = PTHREAD_MUTEX_INITIALIZER;


static int64 gen_req_jrpc_id(void)
{
	static unsigned int jrpc_id;
	struct jipmi_rsp_hndl *hndl;

again:
	if (++jrpc_id == 0)
		jrpc_id = 1;

	list_for_each_entry(hndl, &rsp_hndl_head, list) {
		if (hndl->jrpc_id == jrpc_id)
			goto again;
	}

	return jrpc_id;
}

static struct jipmi_rsp_hndl *find_resp_hndl(unsigned int jrpc_id)
{
	struct jipmi_rsp_hndl *hndl;

	list_for_each_entry(hndl, &rsp_hndl_head, list) {
		if (hndl->jrpc_id == jrpc_id)
			return hndl;
	}

	return NULL;
}

static int handle_jipmi_msg(char *msg)
{
	json_t *rsp = NULL;
	json_t *result = NULL;
	int ret = 0;
	int jrpc_id = 0;
	int netfn, cmd = 0;
	int64 err_code = 0;
	unsigned char *rsp_data = NULL;
	char* origin_data = NULL;
	int  data_len = 0;
	struct jipmi_rsp_hndl *hndl = NULL;
	jipmi_rsp_callback_fn rsp_cb_fn;
	void                *rsp_cb_arg = NULL;
	
	rsp = json_parse(msg);

	result = json_object_get(rsp, STR_RESULT);

	if (result == NULL) {
		result = json_object_get(rsp, STR_ERROR);
		if (result == NULL)
			goto end;

		err_code = json_integer_value(json_object_get(result, STR_CODE));

		rsp_data = malloc(sizeof(int64));
		if (rsp_data == NULL)
			goto end;

		memcpy_s(rsp_data, sizeof(int64), &err_code, sizeof(int64));
		ret = -1;
		data_len = sizeof(int64);
	} else {
		origin_data = json_string_value(json_object_get(result, STR_DATA));
		if(origin_data == NULL) {
			IPMI_DEBUG("jipmi: fail to get data field!\n");
			goto end;
		}
		/*rsp_data = (unsigned char *)base64_decode(origin_data, strlen(origin_data), &data_len);*/
		rsp_data = malloc(strnlen_s(origin_data, JSONRPC_MAX_STRING_LEN));
		if (rsp_data == NULL) {
			IPMI_DEBUG("jipmi: fail to alloc buffer\n");
			goto end;
		}
		data_len = hexstr2buf(origin_data, strnlen_s(origin_data, JSONRPC_MAX_STRING_LEN), (char *)rsp_data);
		/*if (rsp_data == NULL) {
			IPMI_DEBUG("jipmi: fail to get base64 code!\n");
			goto end;
		}*/
		ret = 0;
	}

	jrpc_id = json_integer_value(json_object_get(rsp, STR_JRPC_ID));
	
	pthread_mutex_lock(&libjipmi_lock);
	hndl = find_resp_hndl(jrpc_id);
	if (hndl == NULL) {
		pthread_mutex_unlock(&libjipmi_lock);
		IPMI_DEBUG("jipmi: No handle for this IPMI response!\n");
		jrpc_id = -1;
		goto end;
	}
	rsp_cb_fn  = hndl->cb_fn;
	rsp_cb_arg = hndl->cb_arg;

	if (!hndl->broadcast) {
		list_del(&hndl->list);
	}
	else
		hndl = NULL; /* Free until timeout for broadcast */

	pthread_mutex_unlock(&libjipmi_lock);
	rsp_cb_fn(ret, rsp_data, data_len, rsp_cb_arg);

end:
	if (hndl != NULL)
		free(hndl);

	if (rsp_data)
		free(rsp_data);

	if (rsp)
		json_free(rsp);

	return jrpc_id;

}



static int send_jmsg_to_ipmid(char *msg, int msg_id, unsigned int timeout, sync_mode_t mode, bool bridge_level)
{
	struct sockaddr_in dest;
	int port;
	struct timeval timeo;
	int rc, rc_id, retries = 2;	/* only allow 2 retries */
	fd_set fds;
	int bridge_cnt = 1;

	port = rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT);
	if (port == 0) {
		printf("Failed to call rmm_cfg_get_ipmi_json_rpc_server_port!\n");
		exit(-1);
	}

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	rmm_log(DBG, "-----------msg sent out is:%s.\n", msg);

	if(mode == JIPMI_NON_SYNC) {
		if (sendto(async_jipmi_fd, msg, IPMI_JSONRPC_MAX_MSG_LEN, 0, (struct sockaddr *)&dest, sizeof(dest)) == -1)
			return -1;

		return 0;
	} else {
		char rsp_string[IPMI_JSONRPC_MAX_MSG_LEN] = {0};
		printf("mode is %d, msg is %s\n", mode, msg);

		if (sendto(sync_jipmi_fd, msg, IPMI_JSONRPC_MAX_MSG_LEN, 0, (struct sockaddr *)&dest, sizeof(dest))== -1) {
			printf("%s: send to sync fd fail\n", __func__);
			return -1;
		}

		for (;;) {
			if (--retries < 0)
				break;

			FD_ZERO(&fds);
			FD_SET(sync_jipmi_fd, &fds);

			if(timeout > IPMI_MAX_SYNC_TIMEOUT_MS) {
				timeo.tv_sec = IPMI_MAX_SYNC_TIMEOUT_MS/1000;
				timeo.tv_usec = (IPMI_MAX_SYNC_TIMEOUT_MS%1000)*1000;
			} else {
				timeo.tv_sec = timeout/1000;
				timeo.tv_usec = (timeout%1000)*1000;
			}

			rc = select(sync_jipmi_fd + 1, &fds, NULL, NULL, &timeo);
			if (rc < 0) {
				printf("%s: select sync fd fail\n", __func__);
				break;
			}

			rc = socket_recv(sync_jipmi_fd, rsp_string, sizeof(rsp_string));
			if (rc <= 0) {
				printf("%s: recv sync fd fail, errno is %s\n", __func__, strerror(errno));
				break;
			}

			rsp_string[rc] = '\0';

			rc_id = handle_jipmi_msg(rsp_string);
			if (rc_id != msg_id) {
				printf("received rc_id %d, expect %d.\n", rc_id, msg_id);
				continue;
			} else {
				if (bridge_level && bridge_cnt) {
					bridge_cnt --;
					continue;
				}
				return 0;
			}
		}

		printf("sync mode command process fail!\n");
		return -1;
	}

	return 0;
}


static void *libjipmi_time_thread(void *unused)
{
	unsigned char fake_rsp[1];
	struct timespec expire;
	struct list_head timeout;
	struct jipmi_rsp_hndl *hndl, *nxt;
	pthread_cond_t nul_cond = PTHREAD_COND_INITIALIZER;

	fake_rsp[0] = IPMI_CC_TIMEOUT;

	for (;;) {
		INIT_LIST_HEAD(&timeout);
		clock_gettime(CLOCK_REALTIME, &expire);
		expire.tv_sec += (IPMI_PERIOD_TIME_MS/1000);
		expire.tv_nsec += (IPMI_PERIOD_TIME_MS % 1000) * (1000*1000);
		if (expire.tv_nsec >= BILLION) {
		    expire.tv_sec += expire.tv_nsec / BILLION;
		    expire.tv_nsec %= BILLION;
		}

		pthread_mutex_lock(&libjipmi_lock);
		while (pthread_cond_timedwait(&nul_cond, &libjipmi_lock,
				&expire) != ETIMEDOUT) /* Nothing! */;

		list_for_each_entry_safe(hndl, nxt, &rsp_hndl_head, list) {
			hndl->timeout -= IPMI_PERIOD_TIME_MS;
			if (hndl->timeout > 0)
				continue;

			list_del(&hndl->list);
			list_add_tail(&hndl->list, &timeout);
		}
		pthread_mutex_unlock(&libjipmi_lock);

		list_for_each_entry_safe(hndl, nxt, &timeout, list) {
			list_del(&hndl->list);
			if (!hndl->broadcast)
				hndl->cb_fn(-1, fake_rsp, 1, hndl->cb_arg);
			free(hndl);
		}
    }

	return NULL;
}

int libjipmi_init(unsigned short async_listen_port)
{
	int fd;
	int rc, val;
	int jipmi_port;
	pthread_t tid;

	/* create async mode fd */
	fd = create_udp_listen(INADDR_LOOPBACK, async_listen_port, 0, 1);
	if(fd == -1)
		return -1;

	if (pthread_create(&tid, NULL, libjipmi_time_thread, NULL) != 0) {
		printf("Failed to create libjipmi time thread!\n");
		return -1;
	}
	
	async_jipmi_fd = fd;
	user_port = htons(async_listen_port);

	/* create sync mode fd */
	jipmi_port = rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT);
	if(jipmi_port == 0) {
		printf("Get json rpc ipmi port from rmm config fail....\n");
		return -1;
	}
	sync_jipmi_fd= udp_connect(INADDR_LOOPBACK, jipmi_port);
	if (sync_jipmi_fd < 0)
	{
		printf("Connect json rpc ipmi port failed...\n");
		return -1;
	}

	fprintf(stderr, "LibIPMI init is OK, listen on port:%u. nonsync mode enabled.\n", async_listen_port);
	
	return 1;
}

void libjipmi_callback_selectfds(fd_set *rfds, int *max_fd)
{
	if (async_jipmi_fd == -1) {
		printf("Error: need call libjipmi_init function firstly!\n");
		return;
	}

	FD_SET(async_jipmi_fd, rfds);
	if (async_jipmi_fd > *max_fd)
		*max_fd = async_jipmi_fd;
}

void libjipmi_callback_processfds(fd_set *rfds)
{
	int rc;
	int fd = async_jipmi_fd;
	char msg[IPMI_JSONRPC_MAX_MSG_LEN] = {0};

	if (fd == -1 || !FD_ISSET(fd, rfds))
		return;

	for (;;) {
		rc = recv(fd, &msg, sizeof(msg), 0);
		if (rc <= 0)
			break;

		msg[rc] = '\0';

		handle_jipmi_msg(msg);

	}
}


static void send_msg_payload_callback_handler(jipmi_br_msg_t* req, unsigned int timeo, void *cb_arg)
{
	struct jipmi_rsp_hndl   *hndl_br;

	if (req == NULL) {
		printf("Send IPMI msg failed for no handle!\n");
		return;
	}

	if (timeo > IPMI_MAX_TIMEOUT_MS) {
		timeo = IPMI_MAX_TIMEOUT_MS;
	}

	if (timeo == 0) {
		timeo = IPMI_DFLT_TIMEOUT_MS;
	}

	hndl_br = malloc(sizeof(*hndl_br));
	if (hndl_br == NULL) {
		printf("Send IPMI msg failed for no memory!\n");
		return;
	}

	hndl_br->timeout = timeo;
	hndl_br->broadcast = 0;
	hndl_br->cb_fn  = req->br_cb_fn;
	hndl_br->cb_arg = cb_arg;


	pthread_mutex_lock(&libjipmi_lock);

	hndl_br->jrpc_id  = req->br_rpcid;
	list_add_tail(&hndl_br->list, &rsp_hndl_head);

	pthread_mutex_unlock(&libjipmi_lock);

}


static int send_msg_resp_handler(int result, unsigned char *rsp, int rsp_len, void *cb_arg)
{
	jipmi_br_msg_t *req;

	if(0 != result) {
		printf("%s : return error: %02x\n", __FUNCTION__, rsp[0]);
		//return IPMI_FUNCTION_RETURN_ERROR;
	}
	req = (jipmi_br_msg_t *)cb_arg;

#ifdef IPMI20_SUPPORT
	req->br_cb_fn(result, rsp, rsp_len, cb_arg);
	goto end;
#endif

	send_msg_payload_callback_handler(req, IPMI_DFLT_TIMEOUT_MS, req->cb_arg);

#ifdef IPMI20_SUPPORT
end:
#endif

	free(req);
	req = NULL;

	return IPMI_FUNCTION_RETURN_SUCCESS;
}

static int64 join_handle_list(jipmi_rsp_callback_fn cb_fn, void *cb_arg,
							unsigned int timeo, int broadcast)
{

	struct jipmi_rsp_hndl   *hndl;
	int64 jrpc_id= 0;

	if (cb_fn == NULL) {
		printf("Send IPMI msg failed for no handle!\n");
		return -1;
	}

	hndl = malloc(sizeof(*hndl));
	if (hndl == NULL) {
		printf("Send IPMI msg failed for no memory!\n");
		return -1;
	}

	if (timeo > IPMI_MAX_TIMEOUT_MS)
		timeo = IPMI_MAX_TIMEOUT_MS;
	if (timeo == 0)
		timeo = IPMI_DFLT_TIMEOUT_MS;

	hndl->timeout = timeo;
	hndl->broadcast = broadcast;
	hndl->cb_fn  = cb_fn;
	hndl->cb_arg = cb_arg;

	pthread_mutex_lock(&libjipmi_lock);

	jrpc_id = gen_req_jrpc_id();
	hndl->jrpc_id  = jrpc_id;
	list_add_tail(&hndl->list, &rsp_hndl_head);

	pthread_mutex_unlock(&libjipmi_lock);

	return jrpc_id;
}

static void jipmi_set_base_info(jrpc_param_t *param, int *i, jipmi_msg_t *req)
{
	int j = 0;
	static char str_rsp_addr[20] = {0};

	param[*i].name = STR_RSP_TYPE;
	if (req->rsp_type[0] != '\0')
		param[*i].value = (void *)&(req->rsp_type);
	else
		param[*i].value = STR_DEFAULT_RSP_TYPE;
	param[*i].value_type = JSON_STRING;

	*i += 1;

	param[*i].name = STR_NETFN;
	param[*i].value = (void *)&(req->netfn);
	param[*i].value_type = JSON_STRING;
	*i += 1;

	param[*i].name = STR_CMD;
	param[*i].value = (void *)&(req->cmd);
	param[*i].value_type = JSON_STRING;
	*i += 1;

	param[*i].name = STR_NAME;
	param[*i].value = (void *)&(req->name);
	param[*i].value_type = JSON_STRING;
	*i += 1;

	param[*i].name = STR_PASSWD;
	param[*i].value = (void *)&(req->password);
	param[*i].value_type = JSON_STRING;
	*i += 1;

	param[*i].name = STR_DATA_LEN;
	param[*i].value = (void *)&(req->data_len);
	param[*i].value_type = JSON_STRING;
	*i += 1;

	if (atoi(req->data_len)) {
		/*base64_encode(req->base64data, req->data, atoi(req->data_len));*/
		buf2hexstr((const char *)req->data, atoi(req->data_len), req->base64data);
		param[*i].value = (void *)&(req->base64data);
		param[*i].name = STR_DATA;
		param[*i].value_type = JSON_STRING;
		*i += 1;
	}

}


static int jipmi_br_cmd_timeout(unsigned int host, unsigned int port, jipmi_br_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned int timeo, sync_mode_t mode)
{
	struct in_addr net_addr;
	char *ip_addr = NULL;
	json_t *jreq = NULL;
	int i = 0;
	int64 jrpc_id;
	jrpc_param_t param[MAX_RPC_LEN] = {{0}};
	char buffer[IPMI_JSONRPC_MAX_MSG_LEN] = {0};
	char str_transit_addr[20] = {0};
	char str_transit_ch[20] = {0};
	char str_target_addr[20] = {0};
	char str_target_ch[20] = {0};
	char str_level[20] = {0};
	char str_target_port[20] = {0};

	jrpc_id = join_handle_list(cb_fn, cb_arg, timeo, 0);
	req->br_rpcid = jrpc_id;

	net_addr.s_addr = host;
	ip_addr = inet_ntoa(net_addr);
	param[i].name = STR_TARGET_IP;
	param[i].value = (void *)ip_addr;
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TARGET_PORT;
	FILL_INT(str_target_port, port);
	param[i].value = (void *)&(str_target_port);
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TARGET_ADDR;
	FILL_INT(str_target_addr, req->target_addr);
	param[i].value = (void *)&(str_target_addr);
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TARGET_CH;
	FILL_INT(str_target_ch, req->target_channel);
	param[i].value = (void *)&(str_target_ch);
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TRANSIT_ADDR;
	FILL_INT(str_transit_addr, req->transit_addr);
	param[i].value = (void *)&(str_transit_addr);
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TRANSIT_CH;
	FILL_INT(str_transit_ch, req->transit_channel);
	param[i].value = (void *)&(str_transit_ch);
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_BRIDGE_LEVEL;
	FILL_INT(str_level, req->bridge_level);
	param[i].value = (void *)&(str_level);
	param[i].value_type = JSON_STRING;
	i++;

	jipmi_set_base_info(param, &i, &(req->msg_base));

	jreq = jrpc_create_req(jrpc_id, JSON_RPC_RMCP_BR_REQ, i, param);
	jrpc_format_string(jreq, buffer, sizeof(buffer));

	if (jreq)
		json_free(jreq);
	return send_jmsg_to_ipmid(buffer, jrpc_id, timeo, mode, true);

}

static int jipmi_rmcp_cmd_timeout(unsigned int host, unsigned int port, jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned int timeo, sync_mode_t mode)
{
	int i = 0;
	int64 jrpc_id = 0;
	jrpc_param_t param[MAX_RPC_LEN] = {{0}};
	struct in_addr net_addr;
	char *ip_addr = NULL;
	char str_target_port[20] = {0};
	json_t *jreq = NULL;
	char buffer[IPMI_JSONRPC_MAX_MSG_LEN] = {0};

	jrpc_id = join_handle_list(cb_fn, cb_arg, timeo, 0);

	net_addr.s_addr = host;
	ip_addr = inet_ntoa(net_addr);
	param[i].name = STR_TARGET_IP;
	param[i].value = (void *)ip_addr;
	param[i].value_type = JSON_STRING;
	i++;

	param[i].name = STR_TARGET_PORT;
	FILL_INT(str_target_port, port);
	param[i].value = (void *)&(str_target_port);
	param[i].value_type = JSON_STRING;
	i++;

	jipmi_set_base_info(param, &i, req);

	jreq = jrpc_create_req(jrpc_id, JSON_RPC_RMCP_REQ, i, param);

	jrpc_format_string(jreq, buffer, sizeof(buffer));
	if (jreq)
		json_free(jreq);

	return send_jmsg_to_ipmid(buffer, jrpc_id, timeo, mode, false);
}

static int jipmi_serial_cmd_timeout(jipmi_serial_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned int timeo, sync_mode_t mode)
{
	json_t *jreq = NULL;
	jrpc_param_t param[MAX_RPC_LEN] = {{0}};
	int i = 0;
	int64 jrpc_id = 0;
	char buffer[IPMI_JSONRPC_MAX_MSG_LEN] = {0};
	char str_serial_flag[20] = {0};
	char str_serial_fd[20] = {0};

	jrpc_id = join_handle_list(cb_fn, cb_arg, timeo, 0);

	jipmi_set_base_info(param, &i, &(req->msg_base));


	
	if ((IPMI_SERIAL_OPEN_DEV == req->serial_flag)
		|| (IPMI_SERIAL_CLOSE_DEV == req->serial_flag)) {
		param[i].name = STR_SERIAL_FLAG;
		FILL_INT(str_serial_flag, req->serial_flag);
		param[i].value = (void *)&(str_serial_flag);
		param[i].value_type = JSON_STRING;
		i++;

		jreq = jrpc_create_req(jrpc_id, JSON_RPC_SERIAL_OP_REQ, i, param);
	} else if (IPMI_SERIAL_OPERATION == req->serial_flag) {
		param[i].name = STR_SERIAL_FLAG;
		FILL_INT(str_serial_flag, req->serial_flag);
		param[i].value = (void *)&(str_serial_flag);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_SERIAL_FD;
		FILL_INT(str_serial_fd, req->serial_fd);
		param[i].value = (void *)&(str_serial_fd);
		param[i].value_type = JSON_STRING;
		i++;

		jreq = jrpc_create_req(jrpc_id, JSON_RPC_SERIAL_CMD_REQ, i, param);
	}

	if(jreq == NULL)
		printf("jreq is NULL\n");
	jrpc_format_string(jreq, buffer, sizeof(buffer));

	json_free(jreq);
	return send_jmsg_to_ipmid(buffer, jrpc_id, timeo, mode, false);

}


static int jipmi_br_serial_cmd_timeout(jipmi_serial_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned long target_addr, unsigned short target_channel,
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level, unsigned int timeo, sync_mode_t mode)
{
	json_t *jreq = NULL;
	jrpc_param_t param[MAX_RPC_LEN] = {{0}};
	int i = 0;
	int64 jrpc_id = 0;
	char buffer[IPMI_JSONRPC_MAX_MSG_LEN] = {0};
	char str_serial_flag[20] = {0};
	char str_serial_fd[20] = {0};

	char str_transit_addr[20] = {0};
	char str_transit_ch[20] = {0};
	char str_target_addr[20] = {0};
	char str_target_ch[20] = {0};
	char str_level[20] = {0};

	jrpc_id = join_handle_list(cb_fn, cb_arg, timeo, 0);

	jipmi_set_base_info(param, &i, &(req->msg_base));
	
	if ((IPMI_SERIAL_OPEN_DEV == req->serial_flag)
		|| (IPMI_SERIAL_CLOSE_DEV == req->serial_flag)) {
		param[i].name = STR_SERIAL_FLAG;
		FILL_INT(str_serial_flag, req->serial_flag);
		param[i].value = (void *)&(str_serial_flag);
		param[i].value_type = JSON_STRING;
		i++;

		jreq = jrpc_create_req(jrpc_id, JSON_RPC_SERIAL_OP_REQ, i, param);
	} else if (IPMI_SERIAL_OPERATION == req->serial_flag) {
		param[i].name = STR_SERIAL_FLAG;
		FILL_INT(str_serial_flag, req->serial_flag);
		param[i].value = (void *)&(str_serial_flag);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_SERIAL_FD;
		FILL_INT(str_serial_fd, req->serial_fd);
		param[i].value = (void *)&(str_serial_fd);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_TARGET_ADDR;
		FILL_INT(str_target_addr, target_addr);
		param[i].value = (void *)&(str_target_addr);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_TARGET_CH;
		FILL_INT(str_target_ch, target_channel);
		param[i].value = (void *)&(str_target_ch);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_TRANSIT_ADDR;
		FILL_INT(str_transit_addr, transit_addr);
		param[i].value = (void *)&(str_transit_addr);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_TRANSIT_CH;
		FILL_INT(str_transit_ch, transit_channel);
		param[i].value = (void *)&(str_transit_ch);
		param[i].value_type = JSON_STRING;
		i++;

		param[i].name = STR_BRIDGE_LEVEL;
		FILL_INT(str_level, bridge_level);
		param[i].value = (void *)&(str_level);
		param[i].value_type = JSON_STRING;
		i++;

		jreq = jrpc_create_req(jrpc_id, JSON_RPC_SERIAL_BR_REQ, i, param);
	}

	if (jreq == NULL)
		rmm_log(ERROR, "jreq is NULL\n");

	if (jrpc_format_string(jreq, buffer, sizeof(buffer)))
		rmm_log(ERROR, "Format Json RPC String Failed.\n");

	json_free(jreq);
	return send_jmsg_to_ipmid(buffer, jrpc_id, timeo, mode, false);

}
int libjipmi_rmcp_cmd(unsigned int host, unsigned int port, jipmi_msg_t *req,
			jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode)
{
	return jipmi_rmcp_cmd_timeout(host, port, req, cb_fn, cb_arg, IPMI_DFLT_TIMEOUT_MS, mode);
}

int libjipmi_rmcp_close_session(unsigned int host, unsigned int port, jipmi_msg_t *req, jipmi_rsp_callback_fn cb_fn,
								void *cb_arg, sync_mode_t mode)
{
	return jipmi_rmcp_cmd_timeout(host, port, req, cb_fn, cb_arg, IPMI_DFLT_TIMEOUT_MS, mode);
}

int libjipmi_serial_cmd_timeout(jipmi_serial_msg_t *req,
								jipmi_rsp_callback_fn cb_fn, void *cb_arg,
								unsigned int timeo, sync_mode_t mode)
{
	return jipmi_serial_cmd_timeout(req, cb_fn, cb_arg, timeo, mode);
}

int libjipmi_serial_cmd(jipmi_serial_msg_t *req,
						jipmi_rsp_callback_fn cb_fn, void *cb_arg, sync_mode_t mode)
{
	return jipmi_serial_cmd_timeout(req, cb_fn, cb_arg, IPMI_DFLT_TIMEOUT_MS, mode);
}


int libjipmi_serial_br_cmd(jipmi_serial_msg_t *req,
						jipmi_rsp_callback_fn cb_fn, void *cb_arg, unsigned long target_addr, unsigned short target_channel,
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level,sync_mode_t mode)
{
	return jipmi_br_serial_cmd_timeout(req, cb_fn, cb_arg, target_addr, target_channel, transit_addr, transit_channel, bridge_level, IPMI_DFLT_TIMEOUT_MS, mode);
}



int libjipmi_rmcp_cmd_send_msg(unsigned int host, unsigned int port, jipmi_msg_t *request,	jipmi_rsp_callback_fn cb_fn,
			void *cb_arg, unsigned long target_addr, unsigned short target_channel,
			unsigned long transit_addr, unsigned short transit_channel, int bridge_level, sync_mode_t mode)
{
	struct jipmi_br_msg *req;

	req = malloc(sizeof(*req));
	if (req == NULL) {
		printf("%s failed for no memory!\n", __func__);
		return -1;
	}

	memset(req, 0, sizeof(*req));

	req->bridge_level = bridge_level;
	req->target_addr = target_addr;
	req->target_channel = target_channel;
	req->transit_addr = transit_addr;
	req->transit_channel = transit_channel;
	req->br_cb_fn = cb_fn;
	req->cb_arg = cb_arg;

	memcpy_s(&(req->msg_base), sizeof(jipmi_msg_t), request, sizeof(jipmi_msg_t));

	return jipmi_br_cmd_timeout(host, port, req, send_msg_resp_handler, req, IPMI_DFLT_TIMEOUT_MS, mode);
}


