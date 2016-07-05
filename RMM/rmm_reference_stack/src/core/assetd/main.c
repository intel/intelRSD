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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/asset_module_api.h"
#include "libjsonrpcapi/registerd_socket.h"
#include "libutils/sock.h"
#include "libutils/rmm.h"
#include "libutils/dump.h"
#include "libmemdb/memdb.h"
#include "libwrap/wrap.h"
#include "liblog/log.h"
#include "libcfg/cfg.h"
#include "libjsonrpc/jsonrpc.h"
#include "libredfish/rf_event.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libinit/libinit.h"
#include "handler.h"
#include "attribute.h"
#include "libjsonrpcapi/parser.h"
#include "libsecurec/safe_lib.h"

static void *ipmi_cb_thread(void *unused)
{
	int		rc;
	int		max_fd;
	fd_set	rfds;

	prctl(PR_SET_NAME, "ipmi_cb_thread");
	while (1) {
		max_fd = -1;
		FD_ZERO(&rfds);
		libjipmi_callback_selectfds(&rfds, &max_fd);
		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0) {
			rmm_log(INFO, "select timeout\n");
			continue;
		}

		libjipmi_callback_processfds(&rfds);
	}
	printf("ipmi_cb_thread task exit, errno:%d %s\n", errno, strerror(errno));
	return NULL;
}


static void create_listen_socket(int *fd)
{
	int port; 
	
	port = rmm_cfg_get_port(ASSETD_PORT);
	if (port == 0) {
		rmm_log(ERROR, "Fail to call port.\n");
		exit(-1);
	}
	*fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
	if (*fd < 0) {
		rmm_log(ERROR, "create udp on port %d error: %s\n", port, strerror(errno));
		exit(-1);
	}

	return;
}

static void main_loop(int fd)
{
	int ret_code;
	char cmd_string[JSONRPC_MAX_STRING_LEN] = {0};
	fd_set fds;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	jrpc_req_pkg_t req;
	json_t *rsp = NULL;
	int func_id = 0;
	int reg_connection = -1;

	char *rsp_str = malloc(JSONRPC_MAX_STRING_LEN);

	if (rsp_str == NULL)
		goto err;

	for (;;) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		ret_code = select(fd + 1, &fds, NULL, NULL, NULL);
		if (ret_code < 0)
			continue;

		memset(cmd_string, 0, sizeof(cmd_string));
		ret_code = recvfrom(fd, cmd_string, sizeof(cmd_string), 0, (struct sockaddr *)&addr, &addrlen);
		if (ret_code < 0)
			continue;

		rsp = json_object();
		if (rsp == NULL)
			goto err;

		rmm_log(INFO, "receive: %s.\n", cmd_string);
		parse_req(cmd_string, &req, &func_id);
		ret_code = process_req(func_id, &req, rsp);
		jrpc_req_pkg_free(&req);
		if (ret_code < 0) {
			continue;
		}

		memset(rsp_str, 0, JSONRPC_MAX_STRING_LEN);
		ret_code = jrpc_create_result_rsp_string(req.id, JSONRPC_ID_TYPE_NORMAL, rsp, JSON_OBJECT, rsp_str);
		if (ret_code < 0)
			continue;

		rmm_log(INFO, "send: %s.\n", rsp_str);
		sendto(fd, rsp_str, strnlen_s(rsp_str, JSONRPC_MAX_STRING_LEN)+1, 0, (struct sockaddr *)&addr, addrlen);
	}

	free(rsp_str);

err:

	close(fd);
	return;
}

static void *asset_module_set_gami_attr_thread()
{
	int attr_set = -1;
	long expire = 0;
	memdb_integer reg_node_id = 0;
	int self_ip = 0;
	int port = 0;
	int self_port = 0;
	int i = 0;
	char cap_name[GAMI_MAX_NAME_LEN] = {0};
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int node_num = 0;
	prctl(PR_SET_NAME, "attr_thread");
	while(attr_set) {
		subnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_REG, &node_num, NULL, LOCK_ID_NULL);
		if (node_num != 1) {
			rmm_log(ERROR, "invalid registry node num in memdb\n");
			continue;
		}
		sub_node = (struct node_info *)malloc(CMDBUFSIZ);
		if (sub_node == NULL) {
			return 0;
		}
		memcpy_s(sub_node, sizeof(struct node_info) * node_num, subnode, sizeof(struct node_info) * node_num);

		reg_node_id = sub_node[0].node_id;
		subnode = libdb_list_subnode_by_type(DB_RMM, reg_node_id, MC_TYPE_REG_MODULE, &node_num, NULL, LOCK_ID_NULL);
		if (node_num == 0) {
			rmm_log(ERROR, "no asset module registry info found\n");
			continue;
		}
		memset(sub_node, 0, CMDBUFSIZ);
		memcpy_s(sub_node, sizeof(struct node_info) * node_num, subnode, sizeof(struct node_info) * node_num);

		for (i = 0; i < node_num; i++) {
			libdb_attr_get_string(DB_RMM, sub_node[i].node_id, JRPC_GAMI_REG_NAME, cap_name, GAMI_MAX_NAME_LEN, LOCK_ID_NULL);
			if (strcmp(cap_name, PROC_ASSET_MODULE) == 0) {
				libdb_attr_get_int(DB_RMM, sub_node[i].node_id, JRPC_GAMI_REG_PORT, &port, LOCK_ID_NULL);
				connect_asset_module(port);
				self_port = rmm_cfg_get_port(ASSETD_PORT);
				if(asset_module_set_gami_attr(&self_ip, &self_port) != -1) {
					rmm_log(INFO, "asset module set gami attr success\n");
					attr_set = 0;
					break;
				}
			}
		}
		libdb_free_node(sub_node);
		sleep(1);
	}
	return NULL;
}

static void sigterm_handler(int signum)
{
	/* do cleanup jobs here */
	exit(0);
}

int main(int argc, char **argv)
{
	int fd;
	int port;
	pthread_t	tid_ipmi_cb;
	pthread_t	tid_asset_module_set_attr;
	gami_reg_t reg_info = {{0}};
	char value[WRAP_DB_MAX_VALUE_LEN] = {0};
	int64 error_code;
	memdb_integer root_service_nid = 0;
	memdb_integer rmc_nid = 0;
	struct node_info *pnode = NULL;
	int32 node_num = 0;

	reg_sigterm_handler(sigterm_handler);

	if (rmm_modules_init(MODULEINIT_LOG | MODULEINIT_COREDUMP | MODULEINIT_REDFISHD,
				ASSETMGR_JSONRPC_PORT,
				JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI) == -1) {
		exit(-1);
	}

	rmm_log(INFO, "Assetd daemon is Running ...\n");

	create_listen_socket(&fd);

	libdb_is_ready(DB_RMM, LOCK_ID_NULL, -1);
	error_code = libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_FW_VER_STR, value, WRAP_DB_MAX_VALUE_LEN, LOCK_ID_NULL);
	if (error_code == 0) {
		/*TODO: compare the fw version to decide if need to do db-migration. */
	}

	init_rack_attr();

	pnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_V1, MC_TYPE_V1, &node_num, NULL, LOCK_ID_NULL);
	if ((pnode == NULL) || (node_num == 0)) {
		root_service_nid = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_V1, SNAPSHOT_NEED, LOCK_ID_NULL);
		if (root_service_nid == 0) {
			rmm_log(ERROR, "Failed to create root service node!\n");
			return -1;
		}
		init_root_service_attr(&root_service_nid, PERSISTENT_ALL);

		rmc_nid = libdb_create_node(DB_RMM, root_service_nid, MC_TYPE_RMC, SNAPSHOT_NEED, LOCK_ID_NULL);
		if (rmc_nid == 0) {
			rmm_log(ERROR, "Failed to create root service node!\n");
			return -1;
		}
		init_rmc_attr(&rmc_nid, PERSISTENT_ALL);
	} else {
		root_service_nid = pnode[0].node_id;
		init_root_service_attr(&root_service_nid, PERSISTENT_N);

		pnode = libdb_list_subnode_by_type(DB_RMM, root_service_nid, MC_TYPE_RMC, &node_num, NULL, LOCK_ID_NULL);
		if (pnode != NULL)
			rmc_nid = pnode[0].node_id;
		init_rmc_attr(&rmc_nid, PERSISTENT_N);
	}

	if (pthread_create(&tid_ipmi_cb, NULL, ipmi_cb_thread, NULL) != 0) {
		rmm_log(ERROR, "Failed to create ipmi callback thread!\n");
		return -1;
	}

	if (pthread_create(&tid_asset_module_set_attr, NULL, asset_module_set_gami_attr_thread, NULL) != 0) {
		rmm_log(ERROR, "Failed to create asset module notify thread!\n");
		return -1;
	}

	main_loop(fd);
	return 0;
}
