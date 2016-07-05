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


#ifndef __LIBMEMDB_COMMAND_H__
#define __LIBMEMDB_COMMAND_H__

#include "libmemdb/param.h"
#include "libutils/types.h"
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"


#ifndef LOCK_ID_NULL
typedef memdb_integer lock_id_t;
#define LOCK_ID_NULL (0)
#endif

#define LOCK_ID_STARTER 555


enum {
	CMD_DUMP_NODES,

	CMD_NODE_CREATE,
	CMD_NODE_DESTROY,
	CMD_LIST_SUBNODE,
	CMD_LIST_NODE,

	CMD_ATTRBUTE_SET,
	CMD_ATTRBUTE_GET,
	CMD_ATTRBUTE_REMOVE,
	CMD_LIST_ATTRS_BY_NODE,
	CMD_LIST_ATTRS_BY_COOKIE,

	CMD_ADD_SUBSCRIPTION,
	CMD_REMOVE_SUBSCRIPTION,

	CMD_NODE_GET_BY_NODE_ID,

	CMD_NODE_CREATE_WITH_NODE_ID,

	CMD_LOCK,
	CMD_UNLOCK,

	CMD_MAX
};

static struct _cmd_st {
	memdb_integer cmd_num;
	memdb_string cmd_name;
} memdb_cmd_t[] = {{CMD_DUMP_NODES, "dump_nodes"},
			{CMD_NODE_CREATE, "create_node"},
			{CMD_NODE_DESTROY, "node_destroy"},
			{CMD_LIST_SUBNODE, "list_subnode"},
			{CMD_LIST_NODE, "list_node"},
			{CMD_ATTRBUTE_SET, "attrbute_set"},
			{CMD_ATTRBUTE_GET, "attrbute_get"},
			{CMD_ATTRBUTE_REMOVE, "attrbute_remove"},
			{CMD_LIST_ATTRS_BY_NODE, "list_attrs_by_node"},
			{CMD_LIST_ATTRS_BY_COOKIE, "list_attrs_by_cookie"},
			{CMD_ADD_SUBSCRIPTION, "add_subscription"},
			{CMD_REMOVE_SUBSCRIPTION, "remove_subscription"},
			{CMD_NODE_GET_BY_NODE_ID, "node_get_by_node_id"},
			{CMD_NODE_CREATE_WITH_NODE_ID, "node_create_with_node_id"},
			{CMD_LOCK, "lock"},
			{CMD_UNLOCK, "unlock"} };


enum {
	/*MEMDB ERROR CODE*/
	MEMDB_HANDLE_SUCCESS = 0,
	MEMDB_INVALID_REQ,
	MEMDB_METHOD_NOTFOUND,
	MEMDB_INVALID_PARAMS,
	MEMDB_INTERNAL_ERR,
	MEMDB_OEM_NODE_NOTFOUND,
	MEMDB_OEM_ATTRI_NOTFOUND,
	MEMDB_OEM_HANDLE_ERR,
	MEMDB_OEM_MALLOC_ERR,
	MEMDB_OEM_STRING_LEN_EXCEED,
	MEMDB_OEM_VALUE_EMPTY,
	MEMDB_OEM_PARSE_ERROR,

	/* lib memdb process error code */
	LIBDB_PARSE_ERROR,
	LIBDB_SEND_ERROR,
	LIBDB_RECV_ERROR,
	LIBDB_RETRY_FAIL,
};


#define JSON_RPC_MAX_ERR    11
static struct _err_st {
	memdb_integer err_code;
	memdb_string err_msg;
} err_st_t[JSON_RPC_MAX_ERR]  = {
		{JSONRPC_OEM_HANDLE_SUCCESS, "Handle memdb request success."},
		{JSONRPC_INVALID_REQ, "The JSON sent is not a valid Request object."},
		{JSONRPC_METHOD_NOTFOUND, "The method does not exist / is not available."},
		{JSONRPC_INVALID_PARAMS, "Invalid method parameter"},
		{JSONRPC_INTERNAL_ERR, "Internal JSON-RPC error"},
		{JSONRPC_OEM_NODE_NOTFOUND, "Node requried not found."},
		{JSONRPC_OEM_ATTR_NOTFOUND, "Attribute requried not found."},
		{JSONRPC_OEM_HANDLING_ERROR, "Memdb handling err."},
		{JSONRPC_OEM_MALLOC_ERROR, "Memdb malloc err."}
};


struct node_create_param {
	memdb_integer parent;

	memdb_integer type;

	memdb_integer node_id;

	memdb_integer snapshot_flag;
};

/* @type_match == 0, all children nodes;
 * else list matched with @type_value nodes.
 */
struct list_subnode_param {
	memdb_integer type_match;
	memdb_integer type_value;
};

struct list_node_param {
	memdb_integer type_min;
	memdb_integer type_max;
};

/* the content of this structure should closely match the struct of node_attr defined in node.h */
struct node_attr_param {
	memdb_integer cookie;
	memdb_integer snapshot_flag;

	memdb_string name;
	memdb_string data;

	memdb_integer namelen;
	memdb_integer datalen;
	memdb_integer type;
};

/*******************************/

struct subscibe_param {
	memdb_integer	event;

	memdb_integer	cb_port;
	memdb_integer	cb_pid;

};

struct subscibe_node_param {
	memdb_integer type_min;
	memdb_integer type_max;
};

struct subscibe_attr_param {
	memdb_integer node_id;
	memdb_integer prefix_len;
	memdb_string name_prefix;
};

/*******************************/
struct lock_param {
	memdb_integer to_usec;	/* timeout value, unit is micro second */
};

struct request_pkg {
	memdb_integer	cmd;
	memdb_integer	db_name;
	memdb_integer	node_id;
	lock_id_t		lock_id;

	jrpc_req_pkg_t jrpc_pkg;
};

struct response_pkg {
	memdb_integer	cmd;
	memdb_integer	rcode;
	memdb_integer	node_id;

	jrpc_rsp_pkg_t jrpc_pkg;
};


#define CMDMAXDATALEN	(CMDBUFSIZ)

extern void dispose_command(int fd, struct request_pkg *req, struct sockaddr *addr, socklen_t addrlen);
extern void process_pending_cmds(void);
extern void db_timeout(void);

#endif

