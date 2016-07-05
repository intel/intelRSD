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


#include <stdlib.h>
#include <stdio.h>

#include "libmemdb/memdb_jrpc.h"
#include "libmemdb/node.h"
#include "memdb.h"
#include "memdb_log.h"

int memdb_parse_req(char *string, struct request_pkg *req)
{
	int i = 0;
	char *method = NULL;
	jrpc_data_string db_string = NULL;
	jrpc_data_integer p_lock_id = 0;
	jrpc_data_integer p_node_id = 0;

	if (NULL == string || NULL == req)
		return MEMDB_OEM_HANDLE_ERR;

	if (jrpc_parse_req(string, &req->jrpc_pkg.json, &req->jrpc_pkg.req_type) ||
		jrpc_get_method(req->jrpc_pkg.json, &method) ||
		jrpc_get_id(req->jrpc_pkg.json, &req->jrpc_pkg.id_type, &req->jrpc_pkg.id))
		return JSONRPC_INVALID_REQ;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "lock_id", JSON_INTEGER, &p_lock_id) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "db_name", JSON_STRING, &db_string) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "node_id", JSON_INTEGER, &p_node_id))
		return JSONRPC_INVALID_PARAMS;

	req->lock_id = (lock_id_t)p_lock_id;
	req->node_id = (memdb_integer)p_node_id;

	/* analyze db name */
	for (i = 0; i <= DB_MAX; i++) {
		if (i == DB_MAX)
			return -1;
		if (strcmp(db_name_str[i], db_string) == 0) {
			req->db_name = i;
			break;
		}
	}

	/* currently memdb do not support notifications and null id */
	if (req->jrpc_pkg.req_type != JSONRPC_REQ_NORMAL ||
		req->jrpc_pkg.id_type != JSONRPC_ID_TYPE_NORMAL)
		return -1;

	/* check if cmd support */
	for (i = 0; i < sizeof(memdb_cmd_t) / sizeof(memdb_cmd_t[0]); i++) {
		if (0 == strcmp(method, memdb_cmd_t[i].cmd_name)) {
			req->cmd = i;
			return 0;
		}
		continue;
	}

	/* command not found */
	return JSONRPC_METHOD_NOTFOUND;
}

