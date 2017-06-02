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


#include <stdlib.h>
#include <stdio.h>
#include "libassetd/assetd_type.h"
#include "liblog/log.h"
#include "libjsonrpcapi/parser.h"

int parse_rsp(char * string, jrpc_rsp_pkg_t *rsp)
{
	int i = 0;
	char * method = NULL;
	
	if (NULL == string || NULL == rsp ||
		jrpc_parse_rsp_to_struct(string, rsp)) {
		return -1;
	}
	return 0;
}

int parse_req(char *string, jrpc_req_pkg_t *req, int *cmd_id)
{
	int i = 0;
	char *method = NULL;
	jrpc_data_string db_string = NULL;
	jrpc_data_integer p_lock_id = 0;
	jrpc_data_integer p_node_id = 0;

	if (NULL == string || NULL == req)
		return -1;

	rmm_log(INFO, "req: %s\n", string);
	if (jrpc_parse_req(string, &req->json, &req->req_type) ||
		jrpc_get_method(req->json, &method) ||
		jrpc_get_id(req->json, &req->id_type, &req->id))
		return JSONRPC_INVALID_REQ;

	/* currently memdb do not support notifications and null id */
	if (req->req_type != JSONRPC_REQ_NORMAL ||
		req->id_type != JSONRPC_ID_TYPE_NORMAL)
		return -1;

	/* check if cmd support */
	for (i = 0; i < sizeof(am_cmd_map)/sizeof(am_cmd_map[0]); i++) {
		if (0 == strcmp(method, am_cmd_map[i].cmd_name)) {
			*cmd_id = i;
			return 0;
		}
		continue;
	}
	return JSONRPC_METHOD_NOTFOUND;	/* command not found */
}

