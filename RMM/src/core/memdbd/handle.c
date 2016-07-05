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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "liblog/log.h"
#include "libmemdb/node.h"
#include "libmemdb/event.h"
#include "libmemdb/command.h"
#include "libmemdb/memdb_jrpc.h"
#include "libutils/rack.h"
#include "libutils/utils.h"
#include "memdb.h"
#include "libsecurec/safe_lib.h"


struct list_head pending_cmd_list = LIST_HEAD_INIT(pending_cmd_list);

#define DB_ALIGN(s)		(((s) + 7) & ~7)

#define MAX_LOCK_TIME	5000000		/* max lock time is 5 seconds */
static lock_id_t cur_lock_id = LOCK_ID_STARTER;
static lock_id_t candi_lock_id = LOCK_ID_STARTER;

struct pending_cmd {
	int fd;
	struct sockaddr addr;
	socklen_t addrlen;

	struct request_pkg pkg;

	struct list_head list;
};

static int type_str2int(memdb_integer *type, char *type_str)
{
	int i = 0;

	if (NULL == type || NULL == type_str)
		return -1;

	for ( ; i <= MC_TYPE_END; i++) {
		if (strcmp(type_str, mc_type_str[i]) == 0) {
			*type = i;
			return 0;
		}
	}

	return -1;
}

typedef int (*command_handle_fn) (struct request_pkg *req, json_t *resp);

static int handle_dump_nodes(struct request_pkg *req, json_t *resp)
{
	char *file_path = MEM_DUMP_PATH;

	if (DB_POD == req->db_name)
		file_path = MEM_POD_DUMP_PATH;

	remove(file_path);
	dump_all_nodes(req->db_name);
	dump_all_subscribes(req->db_name);

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_node_create(struct request_pkg *req, json_t *resp)
{
	struct node *parent, *n;
	struct node_create_param param;
	jrpc_data_string type_str = NULL;
	jrpc_data_integer p_parent = 0;
	jrpc_data_integer p_snap = 0;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_parent", JSON_INTEGER, &p_parent) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type", JSON_STRING, &type_str) ||
		-1 == type_str2int(&param.type, type_str) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_snapshot_flag", JSON_INTEGER, &p_snap))
		return MEMDB_INVALID_PARAMS;

	param.parent = (memdb_integer)p_parent;
	param.snapshot_flag = (memdb_integer)p_snap;

	parent = find_node_by_node_id(req->db_name, param.parent);
	if (parent) {
		n = insert_node(req->db_name, parent, param.type, param.snapshot_flag);
		if (n) {
			if (JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id))) {
				MEMDB_ERR("handle_node_create return fail.\n");
				return MEMDB_INTERNAL_ERR;
			}
			MEMDB_DEBUG("node created, node id is %llu\n", n->node_id);
		} else {
			return MEMDB_OEM_HANDLE_ERR;
		}
	} else {
		MEMDB_ERR("parent not found!\n");
		return MEMDB_OEM_NODE_NOTFOUND;
	}

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_node_create_with_node_id(struct request_pkg *req,
										 json_t *resp)
{
	struct node *parent, *n;
	struct node_create_param param;
	jrpc_data_string type_str = NULL;
	jrpc_data_integer p_parent = 0;
	jrpc_data_integer p_snap = 0;
	jrpc_data_integer p_node_id = 0;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_parent", JSON_INTEGER, &p_parent) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type", JSON_STRING, &type_str) ||
		-1 == type_str2int(&param.type, type_str) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_node_id", JSON_INTEGER, &p_node_id) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_snapshot_flag", JSON_INTEGER, &p_snap))
		return MEMDB_INVALID_PARAMS;

	param.parent = (memdb_integer)p_parent;
	param.snapshot_flag = (memdb_integer)p_snap;
	param.node_id = (memdb_integer)p_node_id;

	parent = find_node_by_node_id(req->db_name, param.parent);

	if (parent) {
		n = insert_node_with_node_id(req->db_name, parent, param.node_id,
								   param.type, param.snapshot_flag, FALSE);
		if (n) {
			if (JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id)))
				return MEMDB_INTERNAL_ERR;
		} else {
			return MEMDB_OEM_HANDLE_ERR;
		}
	} else {
		return MEMDB_OEM_NODE_NOTFOUND;
	}

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_node_destroy(struct request_pkg *req, json_t *resp)
{
	struct node *n;


	n = find_node_by_node_id(req->db_name, req->node_id);
	if (n) {
		destroy_node(req->db_name, n, FALSE);
		if (JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id)))
			return MEMDB_INTERNAL_ERR;
	}

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_list_subnode(struct request_pkg *req, json_t *resp)
{
	struct node_info *info;
	struct node *n, *subnode;
	struct list_subnode_param param;
	jrpc_data_string type_str = NULL;
	jrpc_data_integer p_type_match = 0;

	n = find_node_by_node_id(req->db_name, req->node_id);
	if (!n)
		return MEMDB_OEM_NODE_NOTFOUND;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_match", JSON_INTEGER, &p_type_match) ||
		(p_type_match && jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_value", JSON_STRING, &type_str)) ||
		(p_type_match && -1 == type_str2int(&param.type_value, type_str)))
		return MEMDB_INVALID_PARAMS;

	param.type_match = (memdb_integer)p_type_match;

	json_t *array = json_array();

	list_for_each_entry(subnode, &n->children, sibling) {
		if (param.type_match != 0 && param.type_value != subnode->type)
			continue;

		json_t *node = json_object();

		if (NULL != array && NULL != node &&
			JSON_SUCCESS == json_object_add(node, "parent", json_integer(req->node_id)) &&
			JSON_SUCCESS == json_object_add(node, "node_id", json_integer(subnode->node_id)) &&
			JSON_SUCCESS == json_object_add(node, "type", json_string(mc_type_str[subnode->type])) &&
			JSON_SUCCESS == json_array_add(array, node))
			continue;
		else
			return MEMDB_INTERNAL_ERR;
	}

	if (JSON_SUCCESS != json_object_add(resp, "r_nodes", array) ||
		JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id)))
		return MEMDB_INTERNAL_ERR;

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_list_node(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_info info;
	struct list_node_param param;
	struct list_head *which_node = &node_list;
	jrpc_data_string type_max_str = NULL;
	jrpc_data_string type_min_str = NULL;

	if (DB_POD == req->db_name)
		which_node = &pod_node_list;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_max", JSON_STRING, &type_max_str) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_min", JSON_STRING, &type_min_str) ||
		-1 == type_str2int(&param.type_max, type_max_str) ||
		-1 == type_str2int(&param.type_min, type_min_str)) {
		printf("handle_list_node return fail!\n");
		return MEMDB_INVALID_PARAMS;
	}

	json_t *array = json_array();

	list_for_each_entry(n, which_node, list) {
		if (n->type < param.type_min ||
		    n->type > param.type_max)
			continue;

		info.parent = n->parent != NULL ? n->parent->node_id : 0UL;
		info.node_id	= n->node_id;
		info.type	 = n->type;

		json_t *node = json_object();

		if (NULL != array && NULL != node &&
			JSON_SUCCESS == json_object_add(node, "parent", json_integer(info.parent)) &&
			JSON_SUCCESS == json_object_add(node, "node_id", json_integer(info.node_id)) &&
			JSON_SUCCESS == json_object_add(node, "type", json_string(mc_type_str[info.type])) &&
			JSON_SUCCESS == json_array_add(array, node))
			continue;
		else
			return MEMDB_INTERNAL_ERR;
	}

	if (JSON_SUCCESS != json_object_add(resp, "r_nodes", array))
		return MEMDB_INTERNAL_ERR;

	return MEMDB_HANDLE_SUCCESS;
}


static int handle_node_get_by_node_id(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_info info;
	struct list_head *which_node = &node_list;

	if (DB_POD == req->db_name)
		which_node = &pod_node_list;

	list_for_each_entry(n, which_node, list) {
		if (n->node_id == req->node_id) {
			info.parent = n->parent != NULL ? n->parent->node_id : 0UL;
			info.node_id   = n->node_id;
			info.type   = n->type;

			json_t *node = json_object();

			if (NULL == node ||
				JSON_SUCCESS != json_object_add(node, "parent", json_integer(info.parent)) ||
				JSON_SUCCESS != json_object_add(node, "node_id", json_integer(info.node_id)) ||
				JSON_SUCCESS != json_object_add(node, "type", json_string(mc_type_str[info.type])) ||
				JSON_SUCCESS != json_object_add(resp, "r_node", node)) {
				return MEMDB_INTERNAL_ERR;
			} else {
				return MEMDB_HANDLE_SUCCESS;
			}
		}
	}
	return MEMDB_INTERNAL_ERR;
}

static int handle_attr_set(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_attr_param param;
	jrpc_data_integer p_cookie = 0;
	jrpc_data_integer p_snap = 0;
	jrpc_data_string p_name = NULL;
	jrpc_data_string p_data = NULL;

	n = find_node_by_node_id(req->db_name, req->node_id);
	if (!n)
		return MEMDB_OEM_NODE_NOTFOUND;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_cookie", JSON_INTEGER, &p_cookie) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_snapshot_flag", JSON_INTEGER, &p_snap) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_name", JSON_STRING, &p_name) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_data", JSON_STRING, &p_data))
		return MEMDB_INVALID_PARAMS;

	param.cookie = (memdb_integer)p_cookie;
	param.snapshot_flag = (memdb_integer)p_snap;
	param.name = (memdb_string)p_name;
	param.data = (memdb_string)p_data;

	if (set_node_attr(req->db_name, n, param.cookie,
					  (unsigned char *)param.name, strnlen_s(param.name, JSONRPC_MAX_STRING_LEN-1)+1,
					  (unsigned char *)param.data, strnlen_s(param.data, JSONRPC_MAX_STRING_LEN-1)+1,
					  param.snapshot_flag, TYPE_STRING, FALSE) == 0) {
		if (JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id))) {
			return MEMDB_INTERNAL_ERR;
		}
	} else {
		return MEMDB_OEM_HANDLE_ERR;
	}

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_attr_get(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_attr_param param;
	jrpc_data_string p_name = NULL;

	n = find_node_by_node_id(req->db_name, req->node_id);
	if (!n)
		return MEMDB_OEM_NODE_NOTFOUND;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_name", JSON_STRING, &p_name))
		return MEMDB_INVALID_PARAMS;

	param.name = (memdb_string)p_name;

	json_t *attr = json_object();

	if (NULL == attr)
		return JSONRPC_INTERNAL_ERR;
	if (0 != get_node_attr(n, (unsigned char *)param.name, strnlen_s(param.name, JSONRPC_MAX_STRING_LEN-1)+1, &param.cookie, &param.data)) {
		return MEMDB_OEM_ATTRI_NOTFOUND;
	}

	if (JSON_SUCCESS != json_object_add(attr, "cookie", json_integer(param.cookie)) ||
		JSON_SUCCESS != json_object_add(attr, "data", json_string(param.data)) ||
		JSON_SUCCESS != json_object_add(resp, "r_attr", attr) ||
		JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id))) {
		return MEMDB_INTERNAL_ERR;
	}

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_attr_remove(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_attr_param param;
	jrpc_data_string p_name = NULL;

	n = find_node_by_node_id(req->db_name, req->node_id);
	if (!n)
		return MEMDB_OEM_NODE_NOTFOUND;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_name", JSON_STRING, &p_name))
		return JSONRPC_INVALID_PARAMS;

	param.name = (memdb_string)p_name;

	if (remove_node_attr(req->db_name, n, (unsigned char *)param.name, strnlen_s(param.name, JSONRPC_MAX_STRING_LEN-1)+1, FALSE))
		return MEMDB_HANDLE_SUCCESS;

	return MEMDB_OEM_HANDLE_ERR;
}

static int handle_list_attrs_by_node(struct request_pkg *req, json_t *resp)
{
	struct node *n;
	struct node_attr *pa;

	n = find_node_by_node_id(req->db_name, req->node_id);
	if (!n)
		return MEMDB_OEM_NODE_NOTFOUND;

	json_t *array = json_array();
	json_t *element = NULL;

	list_for_each_entry(pa, &n->attrs, group) {
		element = json_object();
		if (NULL == array || NULL == element ||
			JSON_SUCCESS != json_object_add(element, "node", json_integer(pa->node->node_id)) ||
			JSON_SUCCESS != json_object_add(element, "cookie", json_integer(pa->cookie)) ||
			JSON_SUCCESS != json_object_add(element, "name", json_string(pa->name)) ||
			JSON_SUCCESS != json_object_add(element, "data", json_string((char *)pa->data)) ||
			JSON_SUCCESS != json_array_add(array, element))
			return MEMDB_INTERNAL_ERR;
	}

	if (JSON_SUCCESS != json_object_add(resp, "r_attrs", array) ||
		JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(n->node_id)))
		return MEMDB_INTERNAL_ERR;

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_list_attrs_by_cookie(struct request_pkg *req, json_t *resp)
{
	jrpc_data_integer cmask;
	struct node_attr *pa;
	struct list_head *which_attr = &attr_list;

	if (DB_POD == req->db_name)
		which_attr = &pod_attr_list;

	json_t *array = json_array();
	json_t *element = NULL;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_cmask", JSON_INTEGER, &cmask))
		return MEMDB_INVALID_PARAMS;

	list_for_each_entry(pa, which_attr, list) {
		if ((pa->cookie & (memdb_integer)cmask) != (memdb_integer)cmask)
			continue;

		element = json_object();
		if (NULL == array || NULL == element ||
			JSON_SUCCESS != json_object_add(element, "node", json_integer(pa->node->node_id)) ||
			JSON_SUCCESS != json_object_add(element, "cookie", json_integer(pa->cookie)) ||
			JSON_SUCCESS != json_object_add(element, "name", json_string(pa->name)) ||
			JSON_SUCCESS != json_object_add(element, "data", json_string((char *)pa->data)) ||
			JSON_SUCCESS != json_array_add(array, element))
			return MEMDB_INTERNAL_ERR;
	}

	if (JSON_SUCCESS != json_object_add(resp, "r_attrs", array) ||
		JSON_SUCCESS != json_object_add(resp, "node_id", json_integer(req->node_id)))
		return MEMDB_INTERNAL_ERR;

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_add_subscription(struct request_pkg *req, json_t *resp)
{
	struct subscription *sub = NULL;
	struct subscibe_param param;
	jrpc_data_string type_max_str = NULL;
	jrpc_data_string type_min_str = NULL;
	jrpc_data_integer p_event = 0;
	jrpc_data_integer p_cb_port = 0;
	jrpc_data_integer p_cb_pid = 0;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_event", JSON_INTEGER, &p_event) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_cb_port", JSON_INTEGER, &p_cb_port) ||
		jrpc_get_named_param_value(req->jrpc_pkg.json, "p_cb_pid", JSON_INTEGER, &p_cb_pid))
		return MEMDB_INVALID_PARAMS;

	param.event = (memdb_integer)p_event;
	param.cb_port = (memdb_integer)p_cb_port;
	param.cb_pid = (memdb_integer)p_cb_pid;

	if (param.event == EVENT_NODE_CREATE || param.event == EVENT_NODE_DELETE) {
		struct subscibe_node_param data;

		if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_max", JSON_STRING, &type_max_str) ||
			jrpc_get_named_param_value(req->jrpc_pkg.json, "p_type_min", JSON_STRING, &type_min_str) ||
			-1 == type_str2int(&data.type_max, type_max_str) ||
			-1 == type_str2int(&data.type_min, type_min_str))
			return MEMDB_INVALID_PARAMS;

		sub = malloc(sizeof(*sub));
		if (sub == NULL)
			return MEMDB_OEM_MALLOC_ERR;

		sub->data.node.type_min = data.type_min;
		sub->data.node.type_max = data.type_max;
	} else if (param.event == EVENT_NODE_ATTR) {
		struct subscibe_attr_param data;

		jrpc_data_integer p_node_id = 0;
		jrpc_data_integer p_prefix_len = 0;
		jrpc_data_string p_name_prefix = NULL;

		if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_node_id", JSON_INTEGER, &p_node_id) ||
			jrpc_get_named_param_value(req->jrpc_pkg.json, "p_prefix_len", JSON_INTEGER, &p_prefix_len) ||
			(p_prefix_len != 0 && jrpc_get_named_param_value(req->jrpc_pkg.json, "p_name_prefix", JSON_STRING, &p_name_prefix)))
			return MEMDB_INVALID_PARAMS;

		data.node_id = (memdb_integer)p_node_id;
		data.prefix_len = (memdb_integer)p_prefix_len;
		if (data.prefix_len)
			data.name_prefix = (memdb_string)p_name_prefix;
		else 
			data.name_prefix = NULL;
		sub = malloc(sizeof(*sub) + data.prefix_len);
		if (sub == NULL)
			return MEMDB_OEM_MALLOC_ERR;
		sub->data.attr.node_id = data.node_id;
		sub->data.attr.prefix_len = data.prefix_len;
		if (data.prefix_len != 0) {
			memcpy_s(sub->data.attr.name_prefix, data.prefix_len, data.name_prefix, data.prefix_len);
			sub->data.attr.name_prefix[data.prefix_len] = '\0';
		}
	} else {
		rmm_log(ERROR, "No support event %u!\n", param.event);
		return MEMDB_INVALID_REQ;
	}

	sub->event   = param.event;
	sub->cb_port = htons(param.cb_port);
	sub->cb_pid  = param.cb_pid;

	if (DB_RMM == req->db_name)
		list_add_tail(&sub->list, &sublist);
	else if (DB_POD == req->db_name)
		list_add_tail(&sub->list, &pod_sublist);

	/*
	if (JSON_SUCCESS != json_object_add(resp, "r_sub", json_integer((int64)sub)))
		return JSONRPC_INTERNAL_ERR;
	*/

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_remove_subscription(struct request_pkg *req, json_t *resp)
{
	void *target;
	struct subscription *s;
	struct list_head *which_sub = &sublist;

	if (DB_POD == req->db_name)
		which_sub = &pod_sublist;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_sub", JSON_INTEGER, &target))
		return MEMDB_INVALID_PARAMS;

	list_for_each_entry(s, which_sub, list) {
		if (s == target) {
			list_del(&s->list);
			free(s);
			break;
		}
	}

	return MEMDB_HANDLE_SUCCESS;
}

static lock_id_t alloc_lock_id(void)
{
	return ++candi_lock_id;
}

static int handle_db_lock(struct request_pkg *req, json_t *resp)
{
	lock_id_t id;
	struct itimerval value, ovalue;
	struct lock_param param = { 0 };
	jrpc_data_integer p_timeout = 0;

	if (jrpc_get_named_param_value(req->jrpc_pkg.json, "p_timeout", JSON_INTEGER, &p_timeout) ||
		param.to_usec > MAX_LOCK_TIME)
		return MEMDB_INVALID_PARAMS;

	param.to_usec = (memdb_integer)p_timeout;

	value.it_value.tv_sec = param.to_usec / 1000000;
	value.it_value.tv_usec = param.to_usec % 1000000;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);

	id = alloc_lock_id();
	cur_lock_id = id;

	if (JSON_SUCCESS != json_object_add(resp, "r_lock_id", json_integer(id)))
		return MEMDB_INTERNAL_ERR;

	return MEMDB_HANDLE_SUCCESS;
}

static int handle_db_unlock(struct request_pkg *req, json_t *resp)
{
	struct itimerval value, ovalue;

	if (req->lock_id != cur_lock_id)
		return MEMDB_OEM_HANDLE_ERR;

	cur_lock_id = LOCK_ID_STARTER;

	/* unset timer */
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);

	return MEMDB_HANDLE_SUCCESS;
}

static command_handle_fn handles[CMD_MAX] = {
	[CMD_DUMP_NODES]            = handle_dump_nodes,

	[CMD_NODE_CREATE]           = handle_node_create,
	[CMD_NODE_DESTROY]          = handle_node_destroy,
	[CMD_LIST_SUBNODE]          = handle_list_subnode,
	[CMD_LIST_NODE]             = handle_list_node,

	[CMD_ATTRBUTE_SET]          = handle_attr_set,
	[CMD_ATTRBUTE_GET]          = handle_attr_get,
	[CMD_ATTRBUTE_REMOVE]       = handle_attr_remove,
	[CMD_LIST_ATTRS_BY_NODE]    = handle_list_attrs_by_node,
	[CMD_LIST_ATTRS_BY_COOKIE]  = handle_list_attrs_by_cookie,

	[CMD_ADD_SUBSCRIPTION]      = handle_add_subscription,
	[CMD_REMOVE_SUBSCRIPTION]   = handle_remove_subscription,

	[CMD_NODE_GET_BY_NODE_ID]      = handle_node_get_by_node_id,

	[CMD_NODE_CREATE_WITH_NODE_ID]	= handle_node_create_with_node_id,

	[CMD_LOCK] = handle_db_lock,
	[CMD_UNLOCK] = handle_db_unlock,
};

void pend_command(int fd, struct request_pkg *req, struct sockaddr *addr, socklen_t addrlen)
{
	struct pending_cmd *cmd;

	cmd = malloc(sizeof(struct pending_cmd));
	if (cmd == NULL) {
		rmm_log(ERROR, "malloc failed\n");
		return;
	}

	cmd->fd = fd;
	cmd->addrlen = addrlen;
	memcpy_s(&cmd->addr, sizeof(struct sockaddr), addr, sizeof(struct sockaddr));
	memcpy_s(&cmd->pkg, sizeof(struct request_pkg), req, sizeof(struct request_pkg));

	list_add_tail(&cmd->list, &pending_cmd_list);
}

#define MEMDB_REQ_TIMEOUT (-32000)	/* timeout rsp code. need to confirm */
void process_command(int fd, struct request_pkg *req, struct sockaddr *addr, socklen_t addrlen)
{
	json_t *rsp = json_object();
	json_t *rsp_rc_json = NULL;
	int rc = 0;
	char *rsp_str = malloc(JSONRPC_MAX_STRING_LEN);
	command_handle_fn fn;

	if (rsp_str == NULL)
		goto send_rsp;
	else
		memset(rsp_str, 0, JSONRPC_MAX_STRING_LEN);

	if (NULL == rsp) {
		rc = jrpc_create_error_rsp_string(req->jrpc_pkg.id, JSONRPC_ID_TYPE_NORMAL, JSONRPC_INTERNAL_ERR, "libjson internal error", rsp_str);
		goto send_rsp;
	}

	/* check if already timeout */
	if (req->lock_id && (req->lock_id < (candi_lock_id - 1) || cur_lock_id == LOCK_ID_STARTER)) {
		rmm_log(INFO, "\nreq lock id %lld timeout. return -1.\n", req->lock_id);
		rc = jrpc_create_error_rsp_string(req->jrpc_pkg.id, JSONRPC_ID_TYPE_NORMAL, MEMDB_REQ_TIMEOUT, "libjson internal error", rsp_str);
		goto send_rsp;
	} else {
		if (req->cmd < CMD_MAX) {
			fn = handles[req->cmd];
			if (fn)
				rc = fn(req, rsp);
		}
	}

	if (rc) {
		if ((rc > 0) && (rc < JSON_RPC_MAX_ERR))
			rc = jrpc_create_error_rsp_string(req->jrpc_pkg.id, JSONRPC_ID_TYPE_NORMAL, err_st_t[rc].err_code, err_st_t[rc].err_msg, rsp_str);
		json_free(rsp);
		goto send_rsp;
	} else {
		rc = jrpc_create_result_rsp_string(req->jrpc_pkg.id, JSONRPC_ID_TYPE_NORMAL, rsp, JSON_OBJECT, rsp_str);	/* rsp is freed in this call */
	}

send_rsp:
	if (rsp_str) {
			sendto(fd, rsp_str, strnlen_s(rsp_str, JSONRPC_MAX_STRING_LEN-1)+1, 0, addr, addrlen);
		free(rsp_str);
		rsp_str = NULL;
	}
	if (req->jrpc_pkg.json) {
		json_free(req->jrpc_pkg.json);
		req->jrpc_pkg.json = NULL;
	} else {
		printf("Jsonrpc Internal error\n\n");
		rmm_log(ERROR, "Jsonrpc Internal error\n");
	}
}

void dispose_command(int fd, struct request_pkg *req, struct sockaddr *addr, socklen_t addrlen)
{
	if (((cur_lock_id != LOCK_ID_STARTER) && (cur_lock_id == req->lock_id)) ||
		((cur_lock_id == LOCK_ID_STARTER) && list_empty(&pending_cmd_list))) {
		process_command(fd, req, addr, addrlen);
	} else {
		pend_command(fd, req, addr, addrlen);
	}
}

void db_timeout(void)
{
	cur_lock_id = LOCK_ID_STARTER;
}

void process_pending_cmds(void)
{
	lock_id_t pre_id = 0;
	struct pending_cmd *cmd;

again:
	if (!list_empty(&pending_cmd_list)) {
		list_for_each_entry(cmd, &pending_cmd_list, list) {
			if (cur_lock_id != LOCK_ID_STARTER)
				pre_id = cur_lock_id;

			if (cmd) {
				if ((cur_lock_id == LOCK_ID_STARTER) || (cur_lock_id == cmd->pkg.lock_id)) {
					process_command(cmd->fd, &cmd->pkg, &cmd->addr, cmd->addrlen);
					list_del(&cmd->list);
					free(cmd);
					cmd = NULL;

					/* some cmds maybe skiped in the locking process, should start the process from the list head */
					if (pre_id && (cur_lock_id == LOCK_ID_STARTER)) {
						pre_id = 0;
						goto again;
					}
				}
			}
		}
	}
}
