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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "libmemdb/memdb.h"
#include "libmemdb/command.h"
#include "libutils/sock.h"
#include "libcfg/cfg.h"
#include "libutils/rack.h"
#include "libutils/string.h"
#include "liblog/log.h"
#include "libsecurec/safe_lib.h"


#define DB_ALIGN(s)		(((s) + 7) & ~7)

static int cmdfd = -1;	/* fd used for send commands */
static int subfd = -1;	/* fd used for subscribe events */

static unsigned short subport;
static int            subpid;

static void (* event_cb)(struct event_info *, void *);
static void *memdb_cb_data;


static int type_str2int(memdb_integer *type, char *type_str)
{
	int i = 0;

	if (NULL == type || NULL == type_str)
		return -1;

	for (i = 0 ; i <= MC_TYPE_END; i++) {
		if (strcmp(type_str, mc_type_str[i]) == 0) {
			*type = i;
			return 0;
		}
	}

	return -1;
}

static int open_subscibe_socket(void)
{
#define ERR_RET(msg)	do { perror(msg); goto err; } while (0)
	int fd;
	int rc, val;
	socklen_t addrlen;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		perror("socket failed...\n");
		return -1;
	}

	val = fcntl(fd, F_GETFL, 0); 
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0)
		ERR_RET("O_NONBLOCK failed...\n");

	fcntl(fd, F_SETFD, FD_CLOEXEC);

	val = 1;
	setsockopt(fd, SOL_SOCKET, SO_NO_CHECK, &val, sizeof(val));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	rc = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
		ERR_RET("bind failed...\n");

	addrlen = sizeof(addr);
	rc = getsockname(fd, (struct sockaddr *)&addr, &addrlen);
	if (rc < 0)
		ERR_RET("getsockname failed...\n");

	subfd = fd;
	subport = ntohs(addr.sin_port);

	return 0;

err:
	close(fd);
	return -1;
}

void libdb_init()
{
	int port = rmm_cfg_get_port(MEMDBD_PORT);
	if(port == 0) {
		printf("Get memdb port from rmm config fail....\n");
		exit(-1);
	}
	cmdfd = udp_connect(INADDR_LOOPBACK, port);
	if (cmdfd < 0)
	{
		printf("Connect memdbd failed...\n");
		exit(-1);
	}
}

static void sigevt_handler(int unused)
{
	int rc;
	int i;
	sigset_t sigset;
	char evt_string[JSONRPC_MAX_STRING_LEN] = {0};
	char * method = NULL;
	struct event_info * evt = NULL;
	json_t * json = NULL;
	jrpc_req_type_t type;
	memdb_integer event;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGEVT);

	sigprocmask(SIG_BLOCK, &sigset, NULL);
	for (;;) {
		rc = socket_recv(subfd, &evt_string, sizeof(evt_string));
		if (rc <= 0)
			break;

		if(jrpc_parse_req(evt_string, &json, &type) ||
			type != JSONRPC_REQ_NOTFICATION ||
			jrpc_get_method(json, &method))
			return;

		for (i=0; i<=EVENT_END; i++) {
			if (i==EVENT_END)
				return;
			if (strcmp(method, event_string[i]) == 0) {
				event = i;
				break;
			}
		}

		switch (event) {
		case EVENT_NODE_CREATE:
		case EVENT_NODE_DELETE:
			{
				evt = malloc(sizeof(struct event_info));
				if (NULL == evt)
					return;
				evt->event = event;
				
				jrpc_data_integer node_id = 0;
				jrpc_data_integer parent = 0;
				jrpc_data_string type = NULL;
				if (jrpc_get_named_param_value(json, "node_id", JSON_INTEGER, &node_id) ||
					jrpc_get_named_param_value(json, "parent", JSON_INTEGER, &parent)	||
					jrpc_get_named_param_value(json, "type", JSON_STRING, &type) ||
					type_str2int(&evt->ntype, type)) {
					free(evt);
					return;
				}

				evt->nnodeid = (memdb_integer)node_id;
				evt->nparent= (memdb_integer)parent;
			}
			break;
		case EVENT_NODE_ATTR:
			{
				jrpc_data_integer node_id = 0;
				jrpc_data_integer cookie = 0;
				jrpc_data_integer action = 0;
				jrpc_data_string name = NULL;
				jrpc_data_string data = NULL;

				if (jrpc_get_named_param_value(json, "node_id", JSON_INTEGER, &node_id) ||
					jrpc_get_named_param_value(json, "cookie", JSON_INTEGER, &cookie) ||
					jrpc_get_named_param_value(json, "action", JSON_INTEGER, &action) ||
					jrpc_get_named_param_value(json, "name", JSON_STRING, &name) ||
					jrpc_get_named_param_value(json, "data", JSON_STRING, &data))
					return;
				evt = malloc(sizeof(struct event_info)+strnlen_s(name, JSONRPC_MAX_STRING_LEN)+1+strnlen_s(data, JSONRPC_MAX_STRING_LEN)+1);
				if (NULL == evt)
					return;
				evt->event = event;

				evt->info.attr.nodeid = (memdb_integer)node_id;
				evt->info.attr.cookie= (memdb_integer)cookie;
				evt->info.attr.action = (memdb_integer)action;
				evt->info.attr.namelen = (memdb_integer)(strnlen_s(name, JSONRPC_MAX_STRING_LEN)+1);
				evt->info.attr.datalen = (memdb_integer)(strnlen_s(data, JSONRPC_MAX_STRING_LEN)+1);
				memcpy_s(&evt->info.attr.elems[0], strnlen_s(name, JSONRPC_MAX_STRING_LEN)+1, name, strnlen_s(name, JSONRPC_MAX_STRING_LEN)+1);
				memcpy_s(&evt->info.attr.elems[strnlen_s(name, JSONRPC_MAX_STRING_LEN)+1], strnlen_s(data, JSONRPC_MAX_STRING_LEN)+1, data, strnlen_s(data, JSONRPC_MAX_STRING_LEN)+1);
			}
			break;
			
		default:
			return;
		}
		
		event_cb(evt, memdb_cb_data);
		free(evt);
	}
	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

void libdb_event_selectfds(fd_set *readset, int *max_fd)
{
	if (subfd == -1)
		return;

	FD_SET(subfd, readset);
	if (subfd > *max_fd)
		*max_fd = subfd;
}

void libdb_event_processfds(fd_set *readset)
{
	if (subfd != -1 && FD_ISSET(subfd, readset))
		sigevt_handler(0);
}

int libdb_init_subscription(enum event_mode mode, void (*callback)(struct event_info *, void*), void* cb_data)
{
	if (callback == NULL) {
		printf("callback for process event is empty!\n");
		return -1;
	}

	if (open_subscibe_socket() != 0)
		return -1;

	if (mode == NOTIFY_BY_SIGNAL) {
		if (signal(SIGEVT, sigevt_handler) == SIG_ERR) {
			printf("signal failed!\n");
			goto err;
		}

		subpid  = getpid();
	}

	event_cb = callback;
	memdb_cb_data = cb_data;

	return 0;

err:
	close(subfd);
	subfd = -1;
	return -1;
}

void libdb_exit_memdb(void)
{
	if (cmdfd >= 0) {
		close(cmdfd);
		cmdfd = -1;
	}

	if (subfd >= 0) {
		close(subfd);
		subfd = -1;
	}
}

static int libdb_fill_param(struct request_pkg * req, char * name, void * value, json_type type)
{
	if (req->jrpc_pkg.num_of_params+1 >= sizeof(req->jrpc_pkg.params)/sizeof(req->jrpc_pkg.params[0]))
		return -1;

	req->jrpc_pkg.params[req->jrpc_pkg.num_of_params].name = name;
	req->jrpc_pkg.params[req->jrpc_pkg.num_of_params].value = value;
	req->jrpc_pkg.params[req->jrpc_pkg.num_of_params].value_type = type;
	req->jrpc_pkg.num_of_params++;

	return 0;
}

static int libdb_fill_general_params(struct request_pkg *req)
{
	if (req->db_name >= DB_MAX || req->db_name < DB_RMM ||
		libdb_fill_param(req, "db_name", db_name_str[req->db_name], JSON_STRING) ||
		libdb_fill_param(req, "node_id", &req->node_id, JSON_INTEGER) ||
		libdb_fill_param(req, "lock_id", &req->lock_id, JSON_INTEGER))
		return -1;

	return 0;
}

static int get_memdb_errno(memdb_integer jrpc_errno)
{
	int i = 0;

	for (i = 0; i < JSON_RPC_MAX_ERR; i++) {
		if (err_st_t[i].err_code == jrpc_errno)
			return i;
	}

	return MEMDB_HANDLE_SUCCESS;
}


int libdb_parse_rsp(char *string, struct response_pkg *rsp)
{
	int i = 0;
	char * method = NULL;

	if (NULL == string || NULL == rsp) {
		return -1;
	}

	if (jrpc_parse_rsp_to_struct(string, &rsp->jrpc_pkg) != 0) {
		rsp->rcode = LIBDB_PARSE_ERROR;
		return -1;
	}

	if (rsp->jrpc_pkg.rsp_type == JSONRPC_RSP_RESULT) {
		jrpc_get_named_result_value(rsp->jrpc_pkg.json, "node_id", JSON_INTEGER, &rsp->node_id);
	} else
		rsp->rcode = get_memdb_errno(rsp->jrpc_pkg.data.error.code);

	return 0;
}

static memdb_integer libdb_process_cmd(struct request_pkg *req, struct response_pkg *rsp)
{
	static unsigned int seqnum = 0;
	static pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

	int rc = 0;
	int retry = 0;
	fd_set fds;
	struct timeval timeo;
	char *req_str = NULL;
	char rsp_string[JSONRPC_MAX_STRING_LEN] = {0};

	assert(cmdfd > 0);

	pthread_mutex_lock(&socket_mutex);
	req->jrpc_pkg.id = ++seqnum;

	if (libdb_fill_general_params(req)) {
		rmm_log(ERROR, "fail at libdb_fill_general_params.\n");
		goto failed;
	}

	req_str = jrpc_create_req_string(req->jrpc_pkg.id, memdb_cmd_t[req->cmd].cmd_name,
			req->jrpc_pkg.num_of_params, req->jrpc_pkg.params);
	if (NULL == req_str) {
		rmm_log(ERROR, "fail at jrpc_create_req_string, id is %lld, name is %s, num %d, params %d.\n", 
			req->jrpc_pkg.id, memdb_cmd_t[req->cmd].cmd_name, req->jrpc_pkg.num_of_params, req->jrpc_pkg.params);
		goto failed;

	}

	rc = socket_send(cmdfd, req_str, strnlen_s(req_str, JSONRPC_MAX_STRING_LEN) + 1);
	jrpc_free_string(req_str);
	req_str = NULL;
	if (rc < 0) {
		rmm_log(ERROR, "fail at socket_send.\n");
		goto failed;
	}

	req->jrpc_pkg.json = NULL;

	for(;;) {
		if (retry >= 2) {
			rsp->rcode = LIBDB_RETRY_FAIL;
			goto failed;
		}

		FD_ZERO(&fds);
		FD_SET(cmdfd, &fds);
		timeo.tv_sec = 0;
		timeo.tv_usec = 50 * 1000;

		rc = select(cmdfd + 1, &fds, NULL, NULL, &timeo);
		if (rc > 0 && FD_ISSET(cmdfd, &fds)) {
			rc = socket_recv(cmdfd, rsp_string, sizeof(rsp_string));
			if (rc < 0) {
				rmm_log(ERROR, "fail at socket_recv.\n");
				rsp->rcode = LIBDB_RECV_ERROR;
				goto failed;
			}

			if(libdb_parse_rsp(rsp_string, rsp) != 0) {
				rmm_log(ERROR, "fail at libdb_parse_rsp.\n");
				jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
				goto failed;
			}

			if (rsp->jrpc_pkg.id_type == JSONRPC_ID_TYPE_NORMAL &&
					rsp->jrpc_pkg.id == req->jrpc_pkg.id) {
				if (rsp->jrpc_pkg.rsp_type == JSONRPC_RSP_ERROR) {
					//rmm_log(DBG, "rsp->jrpc_pkg.rsp_type == JSONRPC_RSP_ERROR.\n");
					jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
					goto failed;
				} else if (rsp->jrpc_pkg.rsp_type == JSONRPC_RSP_INVALID) {
					rmm_log(ERROR, "rsp->jrpc_pkg.rsp_type == JSONRPC_RSP_INVALID.\n");
					jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
					goto failed;
				} else {
					pthread_mutex_unlock(&socket_mutex);
					/* process memdb command successfully. */
					return 0;
				}
			} else if (rsp->jrpc_pkg.id < req->jrpc_pkg.id) {
				rmm_log(WARNING, "rsp->jrpc_pkg.id < req->jrpc_pkg.id.\n");
				jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
				retry++;
				continue;
			} else {
				rmm_log(ERROR, "rsponse data: rsp->jrpc_pkg.id_type=%d, rsp->jrpc_pkg.id=%ld, req->jrpc_pkg.id=%ld\n",
						rsp->jrpc_pkg.id_type, rsp->jrpc_pkg.id, req->jrpc_pkg.id);
				jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
				goto failed;
			}
		}
	}

failed:
	pthread_mutex_unlock(&socket_mutex);
	return rsp->rcode;
}

memdb_integer libdb_dump_nodes(unsigned char db_name, lock_id_t lock_id)
{
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_DUMP_NODES;
	req.node_id = 0;
	req.lock_id = lock_id;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return rc;
}

lock_id_t libdb_lock(long timeout)
{
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	struct lock_param param;
	lock_id_t lock_id = LOCK_ID_NULL;
	memdb_integer rc = 0;

	param.to_usec = timeout;

	if (libdb_fill_param(&req, "p_timeout", &param.to_usec, JSON_INTEGER))
		return LOCK_ID_NULL;

	req.cmd = CMD_LOCK;
	req.node_id = 0;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc != 0)
		return LOCK_ID_NULL;

	if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_lock_id", JSON_INTEGER, &lock_id))
		lock_id = LOCK_ID_NULL;
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return lock_id;
}

memdb_integer libdb_unlock(lock_id_t lock_id)
{
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.cmd = CMD_UNLOCK;
	req.node_id = 0;
	req.lock_id = lock_id;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return rc;
}

memdb_integer libdb_create_node(unsigned char db_name, memdb_integer parent,
								unsigned int type, unsigned char snapshot_flag, lock_id_t lock_id)
{
	struct node_create_param param = {0};
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_NODE_CREATE;
	req.lock_id = lock_id;

	param.parent = parent;
	param.type   = type;
	param.snapshot_flag = snapshot_flag;

	if (param.type>=MC_TYPE_END ||
		libdb_fill_param(&req, "p_parent", &param.parent, JSON_INTEGER) || 
		libdb_fill_param(&req, "p_type", mc_type_str[param.type], JSON_STRING) ||
		libdb_fill_param(&req, "p_snapshot_flag", &param.snapshot_flag, JSON_INTEGER))
		return 0;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return rc == 0 ? rsp.node_id : 0;
}

memdb_integer libdb_create_node_with_node_id(unsigned char db_name, memdb_integer parent,
										  unsigned int type, memdb_integer node_id,
										  unsigned char snapshot_flag, lock_id_t lock_id)
{
	struct node_create_param param = {0};
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_NODE_CREATE_WITH_NODE_ID;
	req.lock_id = lock_id;

	param.node_id   = node_id;
	param.parent = parent;
	param.type   = type;
	param.snapshot_flag = snapshot_flag;

	if (libdb_fill_param(&req, "p_node_id", &param.node_id, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_parent", &param.parent, JSON_INTEGER) || 
		libdb_fill_param(&req, "p_type", mc_type_str[param.type], JSON_STRING) ||
		libdb_fill_param(&req, "p_snapshot_flag", &param.snapshot_flag, JSON_INTEGER))
		return 0;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return rc == 0 ? rsp.node_id : 0;
}

struct node_info *libdb_get_node_by_node_id(unsigned char db_name, memdb_integer node_id, lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	memdb_integer rc = 0;
	/* FIXME: not thread safe */
	static memdb_integer nodeinfo[CMDBUFSIZ/sizeof(long)];
	struct node_info *ret = NULL;

	req.db_name = db_name;
	req.cmd = CMD_NODE_GET_BY_NODE_ID;
	req.node_id = node_id;
	req.lock_id = lock_id;

	rc = libdb_process_cmd(&req, &rsp);

	if(rc == 0) {
		struct node_info *info = (struct node_info *)nodeinfo;
		json_t * node = NULL;

		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_node", JSON_OBJECT, &node) ||
			-1 == (info->parent = json_integer_value(json_object_get(node, "parent"))) ||
			-1 == (info->node_id = json_integer_value(json_object_get(node, "node_id"))) ||
			-1 == type_str2int(&info->type, json_string_value(json_object_get(node, "type")))){
			ret = NULL;
		}
		else {
			ret = (struct node_info *)nodeinfo;
		}
	}
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return ret;
}


memdb_integer libdb_destroy_node(unsigned char db_name, memdb_integer node, lock_id_t lock_id)
{
	memdb_integer rc = 0;
	struct response_pkg rsp = {};
	struct request_pkg req = {};

	req.db_name = db_name;
	req.cmd = CMD_NODE_DESTROY;
	req.node_id = node;
	req.lock_id = lock_id;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return rc;
}

static struct node_info *list_subnode(unsigned char db_name,
									  memdb_integer node, int *nodenum,
									  struct list_subnode_param *param,
									  filter_callback filter,
									  lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	/* FIXME: not thread safe */
	static memdb_integer nodeinfo[CMDBUFSIZ/sizeof(long)];
	struct node_info *pinfo = NULL;
	*nodenum = 0;
	memdb_integer rc = 0;
	req.db_name = db_name;
	req.cmd = CMD_LIST_SUBNODE;
	req.node_id = node;
	req.lock_id = lock_id;
	int node_cnt = 0;

	if (libdb_fill_param(&req, "p_type_match", &param->type_match, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_type_value", mc_type_str[param->type_value], JSON_STRING))
		goto end;

	rc = libdb_process_cmd(&req, &rsp);

	if (rc == 0) {
		int i = 0;
		json_t * node_array = NULL;
		int node_array_size = 0;
		
		struct node_info * info  = (struct node_info *)nodeinfo;
		
		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_nodes", JSON_ARRAY, &node_array))
			goto end;

		node_array_size = json_array_size(node_array);
		for (i = 0; i < node_array_size; i++) {
			json_t * node = json_array_get(node_array, i);
			if (-1 == (info->parent = json_integer_value(json_object_get(node, "parent"))) ||
				-1 == (info->node_id = json_integer_value(json_object_get(node, "node_id"))) ||
				-1 == type_str2int(&info->type, json_string_value(json_object_get(node, "type")))) {
				goto end;
			}
			if((filter != NULL) && ((*filter)(info->node_id) == 1))
				continue;
			node_cnt++;
			info++;
		}

		*nodenum = node_cnt;

		pinfo = (struct node_info *)nodeinfo;
	}

end:
	jrpc_req_pkg_free(&(req.jrpc_pkg));
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return pinfo;
}

struct node_info *libdb_list_subnode(unsigned char db_name,
									 memdb_integer node, int *nodenum,
									 filter_callback filter,
									 lock_id_t lock_id)
{
	struct list_subnode_param param;

	param.type_match = 0;
	param.type_value = 0;
	return list_subnode(db_name, node, nodenum, &param, filter, lock_id);
}

struct node_info *libdb_list_subnode_by_type(unsigned char db_name,
											 memdb_integer node,
											 int type, int *nodenum,
											 filter_callback filter,
											 lock_id_t lock_id)
{
	struct list_subnode_param param;

	param.type_match = 1;
	param.type_value = type;

	return list_subnode(db_name, node, nodenum, &param, filter, lock_id);
}

static struct node_info *list_node(unsigned char db_name, int *nodenum,
								   struct list_node_param *param, 
								   filter_callback filter,
								   lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	struct node_info *info = NULL;
	struct node_info *pinfo = NULL;
	int node_cnt = 0;
	memdb_integer rc = 0;

	*nodenum = 0;
	req.db_name = db_name;
	req.cmd = CMD_LIST_NODE;
	req.node_id = 0;
	req.lock_id = lock_id;

	if (param->type_min < MC_TYPE_RMM || param->type_max > MC_TYPE_END ||
		libdb_fill_param(&req, "p_type_max", mc_type_str[param->type_max], JSON_STRING) ||
		libdb_fill_param(&req, "p_type_min", mc_type_str[param->type_min], JSON_STRING))
		goto end;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc == 0) {
		int i = 0;
		json_t * node_array = NULL;
		int node_array_size = 0;
		
		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_nodes", JSON_ARRAY, &node_array))
			goto end;

		node_array_size = json_array_size(node_array);

		info = (struct node_info *)malloc(sizeof(struct node_info) * node_array_size);
		if (info == NULL)
			goto end;
		pinfo = info;

		for (i = 0; i < node_array_size; i++) {
			json_t * node = json_array_get(node_array, i);
			if (-1 == (info->parent = json_integer_value(json_object_get(node, "parent"))))
				goto end;

			if (-1 == (info->node_id = json_integer_value(json_object_get(node, "node_id"))))
				goto end;

			if (-1 == type_str2int(&info->type, json_string_value(json_object_get(node, "type"))))
				goto end;

			if( (filter!= NULL)&& ((*filter)(info->node_id) == 1))
				continue;

			info++;
			node_cnt++;
		}

		*nodenum = node_cnt;
	}

end:
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return pinfo;
}

void libdb_free_node(struct node_info *nodes)
{
	if (!nodes)
		return;

	free(nodes);
	nodes = NULL;

}

struct node_info *libdb_list_node(unsigned char db_name, int *nodenum, filter_callback filter, lock_id_t lock_id)
{
	struct list_node_param param;

	param.type_min = MC_TYPE_RMM;
	param.type_max = MC_TYPE_END;

	return list_node(db_name, nodenum, &param, filter, lock_id);
}

struct node_info *libdb_list_node_by_type(unsigned char db_name,
										  unsigned int type_min,
										  unsigned int type_max,
										  int *nodenum,
										  filter_callback filter,
										  lock_id_t lock_id)
{
	struct list_node_param param;

	param.type_min = type_min;
	param.type_max = type_max;

	return list_node(db_name, nodenum, &param, filter, lock_id);
}


static memdb_integer set_attr_data(unsigned char db_name, memdb_integer node,
						  char *name, unsigned int cookie,
						  char *data, unsigned char snapshot_flag, lock_id_t lock_id)
{
	int namelen;
	unsigned char *cp;
	struct node_attr_param param;
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_ATTRBUTE_SET;
	req.node_id = node;
	req.lock_id = lock_id;

	param.name = name;
	param.cookie = cookie;
	param.data = data;
	param.snapshot_flag = snapshot_flag;

	if (libdb_fill_param(&req, "p_cookie", &param.cookie, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_snapshot_flag", &param.snapshot_flag, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_name", param.name, JSON_STRING) ||
		libdb_fill_param(&req, "p_data", param.data, JSON_STRING))
		return -1;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_req_pkg_free(&(req.jrpc_pkg));
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	if (rc != 0)
		return -1;


	return 0;
}


static memdb_integer get_attr_data(unsigned char db_name, memdb_integer node,
									char *name, struct response_pkg *rsp, lock_id_t lock_id, char *output, int len)
{
	struct request_pkg req = {};
	memdb_integer rc = 0;
	char *data = NULL;
	int data_len;

	req.db_name = db_name;
	req.cmd = CMD_ATTRBUTE_GET;
	req.node_id = node;
	req.lock_id = lock_id;

	jrpc_data_string p_name = name;
	if (libdb_fill_param(&req, "p_name", p_name, JSON_STRING))
		return JSONRPC_FILL_PARAM_ERR;

	rc = libdb_process_cmd(&req, rsp);
	if (rc == 0) {
		json_t * element = NULL;

		if(JSONRPC_SUCCESS == jrpc_get_named_result_value(rsp->jrpc_pkg.json, "r_attr", JSON_OBJECT, &element) &&
			NULL != (data = json_string_value(json_object_get(element, "data"))) &&
			*data != '\0') {
			data_len = strnlen_s(data, JSONRPC_MAX_STRING_LEN);
			if (data_len >= len) {
				rc = MEMDB_OEM_STRING_LEN_EXCEED;
				goto end;
			}
			strncpy_safe(output, data, len, len - 1);
			goto end;
		}
	}

end:
	jrpc_req_pkg_free(&(req.jrpc_pkg));
	jrpc_rsp_pkg_free(&(rsp->jrpc_pkg));
	return rc;
}

memdb_integer libdb_attr_remove(unsigned char db_name, memdb_integer node, char *name, lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_ATTRBUTE_REMOVE;
	req.node_id = node;
	req.lock_id = lock_id;

	jrpc_data_string p_name = name;
	if (libdb_fill_param(&req, "p_name", name, JSON_STRING))
		return -1;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return rc;
}


memdb_integer libdb_attr_set_char(unsigned char db_name, memdb_integer node,
						 char *name, unsigned int cookie, char value,
						 unsigned char snapshot_flag, lock_id_t lock_id)
{
	char data[25] = {0};

	snprintf_s_i(data, sizeof(data), "%d", value);
	return set_attr_data(db_name, node, name, cookie, data, snapshot_flag, lock_id);
}


memdb_integer libdb_attr_set_short(unsigned char db_name, memdb_integer node,
						  char *name, unsigned int cookie, short value,
						  unsigned char snapshot_flag, lock_id_t lock_id)
{
	char data[25] = {0};
	
	snprintf_s_i(data, sizeof(data), "%d", value);
	return set_attr_data(db_name, node, name, cookie, data, snapshot_flag, lock_id);
}

memdb_integer libdb_attr_set_int(unsigned char db_name, memdb_integer node,
						char *name, unsigned int cookie, int value,
						unsigned char snapshot_flag, lock_id_t lock_id)
{
	char data[25] = {0};

	snprintf_s_i(data, sizeof(data), "%d", value);

	return set_attr_data(db_name, node, name, cookie, data, snapshot_flag, lock_id);
}

memdb_integer libdb_attr_set_string(unsigned char db_name, memdb_integer node,
						   char *name, unsigned int cookie, char *value,
						   unsigned char snapshot_flag, lock_id_t lock_id)
{
	return set_attr_data(db_name, node, name, cookie, value, snapshot_flag, lock_id);
}

memdb_integer libdb_attr_set_raw(unsigned char db_name, memdb_integer node,
						char *name, unsigned int cookie,
						int datalen, void *value, unsigned char snapshot_flag, lock_id_t lock_id)
{
	return libdb_attr_set_string(db_name, node, name, cookie, (char *)value, snapshot_flag, lock_id);
}


memdb_integer libdb_attr_get_char(unsigned char db_name, memdb_integer node, char *name, char* output, lock_id_t lock_id)
{
	char cp[4] = {0};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	rc = get_attr_data(db_name, node, name, &rsp, lock_id, cp, 4);
	if (rc != 0) {
		rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, rc);
		return rc;
	}

	*output = atoi(cp);

	return 0;
}

memdb_integer libdb_attr_get_short(unsigned char db_name, memdb_integer node, char *name, short* output, lock_id_t lock_id)
{
	char cp[32] = {0};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	rc = get_attr_data(db_name, node, name, &rsp, lock_id, cp, 32);
	if (rc != 0) {
		rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, rc);
		return rc;
	}

	*output = atoi(cp);
	return 0;
}

memdb_integer libdb_attr_get_int(unsigned char db_name, memdb_integer node, char *name, int* output, lock_id_t lock_id)
{
	char cp[32] = {0};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	rc = get_attr_data(db_name, node, name, &rsp, lock_id, cp, 32);
	if (rc != 0) {
		rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, rc);
		return rc;
	}

	*output = atoi(cp);
	return 0;
}

memdb_integer libdb_attr_get_string(unsigned char db_name, memdb_integer node, char *name, char* output, int64 len, lock_id_t lock_id)
{

	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	if (output == NULL)
		return -1;

	rc = get_attr_data(db_name, node, name, &rsp, lock_id, output, len);
	if (rc != 0 &&
		rc != MEMDB_OEM_ATTRI_NOTFOUND) {
		rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, rc);
		return rc;
	}

	return 0;
}

void *libdb_list_attrs_by_node(unsigned char db_name, memdb_integer node, int *size, lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	static memdb_integer attrs[CMDBUFSIZ/sizeof(long)];
	void *ret = NULL;
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_LIST_ATTRS_BY_NODE;
	req.node_id = node;
	req.lock_id = lock_id;
	*size = 0;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc == 0) {
		unsigned short off = 0;
		int i = 0;
		struct node *n = NULL;
		struct node_attr pa = {};
		struct attr_info * info = NULL;
	
		json_t * attr_array = NULL;
		int attr_array_size = 0; 

		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_attrs", JSON_ARRAY, &attr_array))
			goto end;

		off  = 0;
		info = (struct attr_info *)attrs;
		attr_array_size = json_array_size(attr_array);

		for (i=0; i<attr_array_size; i++) {
			json_t * element = json_array_get(attr_array, i);
			unsigned short next_offset;

			if (NULL == element || 
				-1 == (info->node = json_integer_value(json_object_get(element, "node"))) ||
				-1 == (info->cookie = json_integer_value(json_object_get(element, "cookie"))) ||
				NULL == (pa.name_ptr = json_string_value(json_object_get(element, "name"))) ||
				NULL == (pa.data_ptr = json_string_value(json_object_get(element, "data"))))
				goto end;

			pa.namelen = strnlen_s(pa.name_ptr, JSONRPC_MAX_STRING_LEN)+1;
			pa.datalen = strnlen_s(pa.data_ptr, JSONRPC_MAX_STRING_LEN)+1;
			next_offset = sizeof(*info) + pa.namelen + pa.datalen;
			next_offset = DB_ALIGN(next_offset);
			if (off + next_offset > CMDMAXDATALEN) {
				printf("CMDMAXDATALEN is too small!\n");
				break;
			}

			info->next_offset = next_offset;
			info->data_offset = pa.namelen;
			info->data_len = pa.datalen;
			memcpy_s(&info->elems[0], pa.namelen, pa.name_ptr, pa.namelen);
			memcpy_s(&info->elems[pa.namelen], pa.datalen, pa.data_ptr, pa.datalen);

			off += next_offset;
			info = (void *)info + next_offset;
		}

		*size = off;
		ret = attrs;
	}

end:
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return ret;
}

/*
int libdb_is_attr_exist(unsigned char db_name, memdb_integer node, char *attr, lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	static memdb_integer attrs[CMDBUFSIZ/sizeof(long)];

	req.db_name = db_name;
	req.cmd = CMD_LIST_ATTRS_BY_NODE;
	req.node_id = node;
	req.lock_id = lock_id;

	libdb_process_cmd(&req, &rsp);

	if (rsp.rcode == 0) {
		int i = 0;
		struct node *n = NULL;
		char * p_attr = NULL;
	
		json_t * attr_array = NULL;
		int attr_array_size = 0; 

		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_attrs", JSON_ARRAY, &attr_array))
			return 0;

		attr_array_size = json_array_size(attr_array);

		for (i=0; i<attr_array_size; i++) {
			json_t * element = json_array_get(attr_array, i);
			unsigned short next_offset;

			if ((NULL != element) && 
				(NULL != (p_attr = json_string_value(json_object_get(element, "name")))))
				{
					if ((strlen(attr)<64)&&(strlen(p_attr)<64)&&(0 == strcmp(p_attr, attr)))
						return 1;
						
				}
		}

	}	
	return 0;
}
*/

void *libdb_list_attrs_by_cookie(unsigned char db_name, unsigned int cmask, int *size, lock_id_t lock_id)
{
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	static memdb_integer attrs[CMDBUFSIZ/sizeof(long)];
	void *ret = NULL;
	*size = 0;
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_LIST_ATTRS_BY_COOKIE;
	req.node_id = 0;
	req.lock_id = lock_id;

	jrpc_data_integer p_cmask = cmask;

	if (libdb_fill_param(&req, "p_cmask", &p_cmask, JSON_INTEGER))
		return NULL;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc == 0) {
		unsigned short off = 0;
		int i = 0;
		struct node *n = NULL;
		struct node_attr pa = {};
		struct attr_info * info = NULL;
	
		json_t * attr_array = NULL;
		int attr_array_size = 0; 

		if(JSONRPC_SUCCESS != jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_attrs", JSON_ARRAY, &attr_array))
			goto end;

		off  = 0;
		info = (struct attr_info *)attrs;
		attr_array_size = json_array_size(attr_array);

		for (i = 0; i < attr_array_size; i++) {
			json_t * element = json_array_get(attr_array, i);
			unsigned short next_offset;

			if (NULL == element || 
				-1 == (info->node = json_integer_value(json_object_get(element, "node"))) ||
				-1 == (info->cookie = json_integer_value(json_object_get(element, "cookie"))) ||
				NULL == (pa.name_ptr = json_string_value(json_object_get(element, "name"))) ||
				NULL == (pa.data_ptr = json_string_value(json_object_get(element, "data"))))
				goto end;

			pa.namelen = strnlen_s(pa.name_ptr, JSONRPC_MAX_STRING_LEN)+1;
			pa.datalen = strnlen_s(pa.data_ptr, JSONRPC_MAX_STRING_LEN)+1;
			next_offset = sizeof(*info) + pa.namelen + pa.datalen;
			next_offset = DB_ALIGN(next_offset);
			if (off + next_offset > CMDMAXDATALEN) {
				printf("CMDMAXDATALEN is too small!\n");
				break;
			}

			info->next_offset = next_offset;
			info->data_offset = pa.namelen;
			info->data_len = pa.datalen;
			memcpy_s(&info->elems[0], pa.namelen, pa.name_ptr, pa.namelen);
			memcpy_s(&info->elems[pa.namelen], pa.datalen, pa.data_ptr, pa.datalen);

			off += next_offset;
			info = (void *)info + next_offset;
		}
	
		*size = off;
		ret = attrs;
	}

end:
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return ret;

}

memdb_integer libdb_subscribe_type_of_node_create(unsigned char db_name,
										  unsigned int node_type_min,
										  unsigned int node_type_max,
										  lock_id_t lock_id)
{
	struct subscibe_param param = {};
	struct subscibe_node_param data = {};
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_ADD_SUBSCRIPTION;
	req.node_id = 0;

	param.event   = EVENT_NODE_CREATE;
	param.cb_port = subport;
	param.cb_pid  = subpid;

	data.type_min = node_type_min;
	data.type_max = node_type_max;

	if (data.type_min > data.type_max)
		return -1;
	
	if (data.type_max > MC_TYPE_END)
		data.type_max = MC_TYPE_END;

	if (libdb_fill_param(&req, "p_event", &param.event, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_port", &param.cb_port, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_pid", &param.cb_pid, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_type_max", mc_type_str[data.type_max], JSON_STRING) ||
		libdb_fill_param(&req, "p_type_min", mc_type_str[data.type_min], JSON_STRING))
		return -1;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc == 0) {
		memdb_integer result;
		if(JSONRPC_SUCCESS == jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_sub", JSON_INTEGER, &result))
			rc = result;
	}

	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return rc;
}

memdb_integer libdb_subscribe_node_create(unsigned char db_name, lock_id_t lock_id)
{
	return libdb_subscribe_type_of_node_create(db_name, 0, 0xFFFFFFFF, lock_id);
}

memdb_integer libdb_subscribe_node_create_by_type(unsigned char db_name,
										  unsigned int node_type, lock_id_t lock_id)
{
	return libdb_subscribe_type_of_node_create(db_name, node_type, node_type, lock_id);
}

memdb_integer libdb_subscribe_type_of_node_delete(unsigned char db_name,
										  unsigned int node_type_min,
										  unsigned int node_type_max,
										  lock_id_t lock_id)
{
	struct subscibe_param param;
	struct subscibe_node_param data;
	struct response_pkg rsp = {};
	struct request_pkg req = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_ADD_SUBSCRIPTION;
	req.node_id = 0;
	req.lock_id = lock_id;

	param.event   = EVENT_NODE_DELETE;
	param.cb_port = subport;
	param.cb_pid  = subpid;

	data.type_min = node_type_min;
	data.type_max = node_type_max;

	if (data.type_min > data.type_max)
		return -1;
	
	if (data.type_max > MC_TYPE_END)
		data.type_max = MC_TYPE_END;

	if (libdb_fill_param(&req, "p_event", &param.event, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_port", &param.cb_port, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_pid", &param.cb_pid, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_type_max", mc_type_str[data.type_max], JSON_STRING) ||
		libdb_fill_param(&req, "p_type_min", mc_type_str[data.type_min], JSON_STRING))
		return -1;

	rc = libdb_process_cmd(&req, &rsp);
	if (rc == 0) {
		memdb_integer result;
		if(JSONRPC_SUCCESS == jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_sub", JSON_INTEGER, &result))
			rc = result;
	}

	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return rc;
}

memdb_integer libdb_subscribe_node_delete(unsigned char db_name, lock_id_t lock_id)
{
	return libdb_subscribe_type_of_node_delete(db_name, 0, 0xFFFFFFFF, lock_id);
}

memdb_integer libdb_subscribe_node_delete_by_type(unsigned char db_name,
										  unsigned int node_type, lock_id_t lock_id)
{
	return libdb_subscribe_type_of_node_delete(db_name, node_type, node_type, lock_id);
}


memdb_integer libdb_subscribe_attr_special(unsigned char db_name,
								   memdb_integer node_id, char *prefix, lock_id_t lock_id)
{
	unsigned int prefix_len;
	memdb_integer rc = 0;
	struct subscibe_param param;
	struct subscibe_attr_param data;

	struct request_pkg req = {};
	struct response_pkg rsp = {};

	prefix_len = prefix != NULL ? strnlen_s(prefix, JSONRPC_MAX_STRING_LEN) : 0;

	req.db_name = db_name;
	req.cmd = CMD_ADD_SUBSCRIPTION;
	req.node_id = 0;
	req.lock_id = lock_id;

	param.event   = EVENT_NODE_ATTR;
	param.cb_port = subport;
	param.cb_pid  = subpid;
	data.prefix_len = prefix_len;
	data.node_id = node_id;
	data.name_prefix = prefix;
	
	if (libdb_fill_param(&req, "p_event", &param.event, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_port", &param.cb_port, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_cb_pid", &param.cb_pid, JSON_INTEGER) ||
		libdb_fill_param(&req, "p_node_id", &data.node_id, JSON_INTEGER) ||
		(data.prefix_len != 0 && libdb_fill_param(&req, "p_name_prefix", data.name_prefix, JSON_STRING)) ||
		libdb_fill_param(&req, "p_prefix_len", &data.prefix_len, JSON_INTEGER))
		return -1;

	rc = libdb_process_cmd(&req, &rsp);

	if (rc == 0) {
		memdb_integer result;
		if(JSONRPC_SUCCESS == jrpc_get_named_result_value(rsp.jrpc_pkg.json, "r_sub", JSON_INTEGER, &result))
			rc = result;
	}
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));
	return rc;
}

memdb_integer libdb_subscribe_attr_by_prefix(unsigned char db_name,
									 memdb_integer node_id, char* prefix, lock_id_t lock_id)
{
	return libdb_subscribe_attr_special(db_name, node_id, prefix, lock_id);

}

memdb_integer libdb_subscribe_attr_by_node(unsigned char db_name, memdb_integer node_id, lock_id_t lock_id)
{
	return libdb_subscribe_attr_special(db_name, node_id, NULL, lock_id);
}

memdb_integer libdb_subscribe_attr_all(unsigned char db_name, lock_id_t lock_id)
{
	return libdb_subscribe_attr_special(db_name, 0, NULL, lock_id);
}

memdb_integer libdb_unsubscribe_event(unsigned char db_name, memdb_integer handle, lock_id_t lock_id)
{
	struct request_pkg req = {};
	struct response_pkg rsp = {};
	memdb_integer rc = 0;

	req.db_name = db_name;
	req.cmd = CMD_REMOVE_SUBSCRIPTION;
	req.node_id = 0;
	req.lock_id = lock_id;

	rc = libdb_process_cmd(&req, &rsp);
	jrpc_rsp_pkg_free(&(rsp.jrpc_pkg));

	return rc;
}
/**
* @param timeout_s  > 0: timeout(secound) for loop check.
*					= 0: only check once;
*					< 0: forever loop check;
*
*/
memdb_integer libdb_is_ready(unsigned char db_name, lock_id_t lock_id, memdb_integer timeout_s)
{
	int nodenum = 0;
	struct node_info *nodes = NULL;

	while (1) {
		nodes = libdb_list_node(db_name, &nodenum, NULL, lock_id);
		if (nodenum > 0) {
			libdb_free_node(nodes);
			return 0;
		}

		if (timeout_s == 0) {
			libdb_free_node(nodes);
			return -1;
		} else if (timeout_s > 0)
			timeout_s--;

		sleep(1);
	}

	libdb_free_node(nodes);
	return -1;
}

