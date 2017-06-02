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
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <errno.h>
#include <libutils/dump.h>

#include "libutils/rmm_json.h"
#include "libutils/sock.h"
#include "libutils/ip.h"
#include "libutils/string.h"
#include "liblog/log.h"
#include "libcfg/cfg.h"
#include "libjsonrpcapi/asset_module_api.h"
#include "libjsonrpcapi/registerd_api.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/parser.h"
#include "libinit/libinit.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libassetmodule/am_action.h"
#include "libsecurec/safe_lib.h"
#include "libassetd/assetd_type.h"
#include "cm.h"
#include "../asset_module_common/gami.h"

bool cm_runnable = false;
typedef int (*action_handle_fn) (jrpc_req_pkg_t *req, json_t *resp, void *args);

static action_handle_fn action_handles[MAX_ACTION] = {
//	[AMACTION_SET_FAN_PWM]			= action_set_tzone_pwm,
};

static int process_action(jrpc_req_pkg_t *req, json_t *resp, void *args)
{
	int rc = 0;
	int i = 0;
	int action_id = -1;
	jrpc_data_string action = NULL;
	int j = 0;

	return 0;

	if (j >= MAX_ACTION)
		return -1;

	jrpc_get_named_param_value(req->json, JRPC_ACTION_NAME, JSON_STRING, &action);
	if (strnlen_s(action, RSIZE_MAX_STR) == 0) {
		printf("get action failed\n");
		return -1;
	}

	/* check if cmd support */
	for (i = 0; i < sizeof(am_action_map)/sizeof(am_action_map[0]); i++) {
		if (0 == strcmp(action, am_action_map[i].action_name)) {
			action_id = am_action_map[i].action_id;
			rc = action_handles[action_id](req, resp, args);
			return rc;
		}
		continue;
	}

	return -1;
}

static int am_process_req(jrpc_req_pkg_t *req, json_t *resp, int func_id, void *args)
{
	int rc = 0;
	jrpc_data_string action = NULL;

	switch (func_id) {
        case REG_APP:
			rc = process_register(req, resp);
			break;
        case DO_ACTION:
			rc = process_action(req, resp, args);
			break;
        default:
			rmm_log(ERROR, "Error function id: %d", func_id);
			break;
        }
	
	return rc;
}

static void *jprc_process_thread(void *args)
{
	int rc;
	int fd;
	int ret_code;
	int port = 0;
	fd_set fds;
	jrpc_req_pkg_t req = {};
	json_t *resp = NULL;
	int func_id = 0;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	char cmd_string[JSONRPC_MAX_STRING_LEN] = {0};

	char *rsp_str = malloc(JSONRPC_MAX_STRING_LEN);
	if (rsp_str == NULL)
		return NULL;

	port = rmm_cfg_get_port(ASSET_MODULE_PORT);
	fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
	if (fd < 0) {
		printf("create udp on port %d error: %s\n", port, strerror(errno));
		return NULL;
	}

	prctl(PR_SET_NAME, "jprc_process_thread");
	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		resp = json_object();		/* for json rpc result, resp is freed in jrpc_create_result_rsp_string */

		if (req.json)
			jrpc_req_pkg_free(&req);
		memset(&req, 0, sizeof(req));

		ret_code = select(fd + 1, &fds, NULL, NULL, NULL);
		if (ret_code < 0)
			continue;

		ret_code = recvfrom(fd, cmd_string, sizeof(cmd_string), 0, (struct sockaddr *)&addr, &addrlen);
		if (ret_code < 0)
			continue;

		rmm_log(INFO, "receive: %s.\n", cmd_string);
		printf("receive: %s.\n", cmd_string);
		parse_req(cmd_string, &req, &func_id);
		ret_code = am_process_req(&req, resp, func_id, args);
		memset(rsp_str, 0, JSONRPC_MAX_STRING_LEN);
		if (ret_code < 0) {
			jrpc_create_error_rsp_string(req.id, JSONRPC_ID_TYPE_NORMAL, ret_code, "asset module handle error", rsp_str);
			sendto(fd, rsp_str, strnlen_s(rsp_str, JSONRPC_MAX_STRING_LEN)+1, 0, (struct sockaddr *)&addr, addrlen);
			json_free(resp);
			resp = NULL;
			continue;
		}

		/*json_object_add(resp, "ret_code", json_integer(ret_code));*/
		ret_code = jrpc_create_result_rsp_string(req.id, JSONRPC_ID_TYPE_NORMAL, resp, JSON_OBJECT, rsp_str);
		resp = NULL;
		if (ret_code < 0)
			continue;

		rmm_log(INFO, "send: %s.\n", rsp_str);
		sendto(fd, rsp_str, strnlen_s(rsp_str, JSONRPC_MAX_STRING_LEN)+1, 0, (struct sockaddr *)&addr, addrlen);
	}
	printf("jprc_process_thread task exit, errno:%d %s\n", errno, strerror(errno));
	return NULL;
}


static void main_loop(void)
{
	int index = 0;
	int max_fd;
	fd_set rfds;
	int rc;
	struct timeval time_out;
	int timeout_cfg = SELECT_TIME_OUT_IN_MSECONDS;
	int cm_cnt = 0;
	struct cm_attr *attr = NULL;
	int gami_state = -1;
	int jrpc_state = -1;
	pthread_t tid_jrpc_process;

	gami_register();

	if (pthread_create(&tid_jrpc_process, NULL, jprc_process_thread, NULL) != 0) {
		rmm_log(ERROR, "Failed to create set attribute thread!\n");
		exit(-1);
	}

	rmm_log(INFO, "Asset Module daemon is Running ...\n");
	for (;;) {
		if (cm_runnable)
			am_run();
	}
	return;
}

static void sigterm_handler(int signum)
{
	/* do cleanup jobs here */
	exit(0);
}

int main(int argc, char **argv)
{
	reg_sigterm_handler(sigterm_handler);

	if (rmm_modules_init(MODULEINIT_LOG | MODULEINIT_COREDUMP | MODULEINIT_GAMI_REG,
		IPMIASSETMODULE_PORT,
		JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI) == -1) {
		exit(-1);
	}

	main_loop();

	return 0;
}
