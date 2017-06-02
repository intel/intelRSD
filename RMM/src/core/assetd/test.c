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


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <pthread.h>

#include "libjsonrpc/jsonrpc.h"
#include "libcfg/cfg.h"

#include "libutils/sock.h"
/*#include "libasset_driver/ad_def.h"*/
#include "libassetmgr/am_type.h"
#include "libassetmgr/am_jrpc_def.h"
#include "liblog/log.h"

int cmfd = 0;

static int fill_param(jrpc_req_pkg_t *req, char *name, void *value, json_type type)
{
	if (req->num_of_params + 1 > sizeof(req->params)/sizeof(req->params[0]))
		return -1;

	req->params[req->num_of_params].name = name;
	req->params[req->num_of_params].value = value;
	req->params[req->num_of_params].value_type = type;
	req->num_of_params++;

	return 0;
}


static int parse_rsp(char *string, jrpc_rsp_pkg_t *rsp)
{
	int i = 0;
	char *method = NULL;

	if (NULL == string || NULL == rsp ||
		jrpc_parse_rsp_to_struct(string, rsp)) {
		return -1;
	}
	return 0;
}

static void ad_connect_am(void)
{
	int port = rmm_cfg_get_port(ASSETD_PORT);

	if (port == 0) {
		rmm_log(ERROR, "Can't get asset monitor port...\n");
		exit(-1);
	}
	cmfd = udp_connect(INADDR_LOOPBACK, port);
	if (cmfd < 0) {
		rmm_log(ERROR, "Connect memdbd failed...\n");
		exit(-1);
	}
}

static void get_node_id_from_rsp(json_t *rsp, int64 *node_id)
{
	json_t *jnode_id = json_object_get(rsp, JRPC_NODE_ID);
	*node_id = json_integer_value(jnode_id);
}

static int send_msg(jrpc_req_pkg_t *req, int evt_id)
{
#define MAX_RETRIES		2
#define RECV_TIMEO		5000000 /* in useconds */
	jrpc_rsp_pkg_t resp;
	int fd = cmfd;
	int rc, retries = MAX_RETRIES;
	fd_set fds;
	struct timeval timeo;
	static unsigned int seqnum;
	char *req_str = NULL;
	static pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
	char rsp_string[JSONRPC_MAX_STRING_LEN] = {0};
	int64 error_code = 0;

	pthread_mutex_lock(&socket_mutex);
	if (fd < 0) {
		exit(-1);
	}

	req->id = ++seqnum;
	if (NULL == (req_str = jrpc_create_req_string(req->id, am_cmd_map[evt_id].cmd_name, req->num_of_params, req->params))) {
		error_code = -1;
		goto failed;
	}

	printf("------------>evt_id: %d    ,", evt_id);
	printf("%s\n", req_str);

	if ((rc = socket_send(fd, req_str, strnlen_s(req_str, RSIZE_MAX_STR) + 1)) < 0) {
		error_code = -1;
		goto failed;
	}

	req->json = NULL;

	for (;;) {
		if (--retries < 0)
			break;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		timeo.tv_sec = 0;/*RECV_TIMEO;*/
		timeo.tv_usec = RECV_TIMEO;

		rc = select(fd + 1, &fds, NULL, NULL, &timeo);
		if (rc <= 0)
			continue;

		rc = socket_recv(fd, rsp_string, sizeof(rsp_string));

		printf("rsp string:%s.\n", rsp_string);

		if (-1 == rc || 0 != parse_rsp(rsp_string, &resp)) {
			error_code = -1;
			goto failed;
		}

		get_node_id_from_rsp(resp.data.result.value_obj, &req->node_id);
		if (resp.id_type == JSONRPC_ID_TYPE_NORMAL &&
			resp.id == req->id) {
			if (resp.rsp_type == JSONRPC_RSP_ERROR) {
				error_code = resp.data.error.code;
			} else if (resp.rsp_type == JSONRPC_RSP_INVALID) {
				error_code = -1;
			} else {
				if (req_str != NULL) {
					jrpc_free_string(req_str);
					req_str = NULL;
				}
				pthread_mutex_unlock(&socket_mutex);
				return 0;
			}
		} else if (resp.id < req->id) {
			/*jrpc_rsp_free(&resp);*/
			continue;
		} else {
			goto failed;
		}

	}

failed:
	if (req_str != NULL) {
		jrpc_free_string(req_str);
		req_str = NULL;
	}
	jrpc_rsp_pkg_free(&resp);
	pthread_mutex_unlock(&socket_mutex);
	return (int)error_code;
}

int get_powerzone_collection(int64 pz_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_PZ_INDEX, &pz_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_PZ_COL);
}

int get_powerzone(int64 pz_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_PZ_INDEX, &pz_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_PZ);
}

int get_psu_collection(int64 pz_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_PZ_INDEX, &pz_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_PSU_COL);
}

int get_psu(int64 pz_index, int64 psu_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_PZ_INDEX, &pz_index, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_INDEX, &psu_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_PSU);
}

int get_thermalzone_collection(void)
{
	jrpc_req_pkg_t req_pkg = {};

	return send_msg(&req_pkg, GET_TZ_COL);
}

int get_tz(int64 cz_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_CZ_INDEX, &cz_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_TZ);
}


int get_fan_collection(int64 cz_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_CZ_INDEX, &cz_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_FAN_COL);

}

int get_fan(int64 cz_index, int64 fan_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_CZ_INDEX, &cz_index, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FAN_INDEX, &fan_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_FAN);
}


int get_drawer_collection(void)
{
	jrpc_req_pkg_t req_pkg = {};

	return send_msg(&req_pkg, GET_DRAWER_COL);
}

int get_drawer(int64 draw_index)
{
	jrpc_req_pkg_t req_pkg = {};

	fill_param(&req_pkg, JRPC_DRAWER_INDEX, &draw_index, JSON_INTEGER);

	return send_msg(&req_pkg, GET_DRAWER);
}

int main(int argc, char **argv)
{

	ad_connect_am();
	get_powerzone_collection(0);
	get_powerzone(0);
	get_psu_collection(0);
	get_psu(0, 0);
	get_thermalzone_collection();
	get_tz(0);
	get_fan_collection(0);
	get_fan(0, 0);
	get_drawer_collection();
	get_drawer(0);

	return 0;
}
