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


#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "libutils/rack.h"
#include "memdb.h"
#include "snap.h"
#include "memdb_log.h"
#include "libutils/types.h"
#include "libutils/utils.h"
#include "libmemdb/event.h"
#include "libmemdb/node.h"
#include "libsecurec/safe_lib.h"

static struct node *new_node;
static struct node *freeing_node;
static struct node_attr *curr_attr;
static unsigned int      curr_attr_action;

static struct node *new_pod_node;
static struct node *freeing_pod_node;
static struct node_attr *curr_pod_attr;
static unsigned int      curr_pod_attr_action;

struct list_head node_list = LIST_HEAD_INIT(node_list);
struct list_head attr_list = LIST_HEAD_INIT(attr_list);

struct list_head pod_node_list = LIST_HEAD_INIT(pod_node_list);
struct list_head pod_attr_list = LIST_HEAD_INIT(pod_attr_list);

static unsigned long g_node_number = 10000000;

struct node rmm_root = {
	.node_id	= 0,
	.type		= 0,
	.parent		= NULL,
	.list		= LIST_HEAD_INIT(rmm_root.list),
	.children	= LIST_HEAD_INIT(rmm_root.children),
	.sibling	= LIST_HEAD_INIT(rmm_root.sibling),
	.attrs		= LIST_HEAD_INIT(rmm_root.attrs),
};

struct node pod_root = {
	.node_id	= 0,
	.type		= 0,
	.parent		= NULL,
	.list		= LIST_HEAD_INIT(pod_root.list),
	.children	= LIST_HEAD_INIT(pod_root.children),
	.sibling	= LIST_HEAD_INIT(pod_root.sibling),
	.attrs		= LIST_HEAD_INIT(pod_root.attrs),
};

static void type_int2str(memdb_integer type, char *type_str, uint32 len)
{
	if (type > MC_TYPE_END || type < MC_TYPE_RMM)
		snprintf_s_s(type_str, len, "%s", "UNKNOWN");
	else
		snprintf_s_s(type_str, len, "%s", mc_type_str[type]);
}

static void print_node(memdb_integer db_name, struct node *root)
{
	int i;
	struct node_attr *pa;
	struct node *n, *parent;
	char *indent, *aindent, buf[4096], out_buff[256], type_str[32], tmp[32], present[32];
	static int depth;
	FILE *file_out;
	char *file_path = MEM_DUMP_PATH;
	int len = 0;

	if (DB_POD == db_name) {
		file_path = MEM_POD_DUMP_PATH;
	}

	file_out = fopen(file_path, "a+w");
	if (NULL == file_out) {
		MEMDB_ERR("Open %s failed", file_path);
		return;
	}

	/************** Printing indent for node ***************/
	n = root;
	parent = root->parent;
	indent = buf + sizeof(buf);
	*--indent = '\0';
	for (i = 0; i < depth; i++) {
		if (i == 0) {
			*--indent = '-';
			*--indent = '-';
			if (list_is_last(&n->sibling, &parent->children))
				*--indent = '`';
			else
				*--indent = '|';
		} else {
			*--indent = ' ';
			*--indent = ' ';
			if (list_is_last(&n->sibling, &parent->children))
				*--indent = ' ';
			else
				*--indent = '|';
		}

		n = parent;
		parent = n->parent;
	}

	/*******************************************************/
	type_int2str(root->type, type_str, 32);

	if (root->snapshot_flag == SNAPSHOT_NEED)
		snprintf_s_s(tmp, sizeof(tmp), "%s", "snap_y");
	else
		snprintf_s_s(tmp, sizeof(tmp), "%s", "snap_n");

	//snprintf(out_buff, sizeof(out_buff), "%sN(%s):%lld:(%s)\n", indent, tmp,
							//root->node_id, type_str);

	len = snprintf_s_ss(out_buff, sizeof(out_buff), "%sN(%s)", indent, tmp);
	if (len < 0)
		return;

	len += snprintf_s_ll(out_buff+len, sizeof(out_buff)-len, ":%lld", root->node_id);
	if (len < 0)
		return;

	len += snprintf_s_s(out_buff+len, sizeof(out_buff)-len, ":(%s)\n", type_str);
	if (len < 0)
		return;

	printf("%s", out_buff);
	fprintf(file_out, "%s", out_buff);

	/************ Printing indent for attribute ************/
	n = root;
	parent = root->parent;
	indent = buf + sizeof(buf);
	*--indent = '\0';
	*--indent = '-';
	*--indent = '-';
	aindent = --indent;
	for (i = 0; i < depth; i++) {
		*--indent = ' ';
		*--indent = ' ';
		if (list_is_last(&n->sibling, &parent->children))
			*--indent = ' ';
		else
			*--indent = '|';

		n = parent;
		parent = n->parent;
	}
	/*******************************************************/

	list_for_each_entry(pa, &root->attrs, group) {
		if (!list_is_last(&pa->group, &root->attrs) ||
		    !list_empty(&root->children))
			*aindent = '|';
		else
			*aindent = '`';

		if (pa->snapshot_flag == SNAPSHOT_NEED)
			snprintf_s_s(tmp, sizeof(tmp), "%s", "snap_y");
		else
			snprintf_s_s(tmp, sizeof(tmp), "%s", "snap_n");

		snprintf_s_sss(out_buff, sizeof(out_buff), "%s-A(%s)::%s:", indent, tmp, pa->name);
		printf("%s", out_buff);
		fprintf(file_out, "%s", out_buff);

		if (pa->type == TYPE_STRING) {
			if (pa->cookie == 0x01) {
				snprintf_s_sss(out_buff, sizeof(out_buff), "%s%s%s", COLOR_RED, (char *)(pa->data), COLOR_NONE);
				printf("%s", out_buff);
				fprintf(file_out, "%s", out_buff);
			} else if (strstr(pa->name, "ip_addr") || strstr(pa->name, "subnet_mask")) {
				int ip_addr = atoi((char *)pa->data);

				printf("%d.%d.%d.%d", ip_addr&0xff,
									(ip_addr>>8)&0xff,
									(ip_addr>>16)&0xff,
									(ip_addr>>24)&0xff);
				fprintf(file_out, "%d.%d.%d.%d", ip_addr&0xff,
									(ip_addr>>8)&0xff,
									(ip_addr>>16)&0xff,
									(ip_addr>>24)&0xff);
			} else if (strstr(pa->name, "indexes")) {
				int indexes = atoi((char *)pa->data);

				printf("%08X", indexes);
				fprintf(file_out, "%08X", indexes);
			}
			/* dump present for 8 bit*/
			else if (strstr(pa->name, "present")) {
				int ipresent = atoi((char *)pa->data);

				memset(present, 0, sizeof(present));
				for (i = 0; i < 8; i++) {
					if (((ipresent>>i)&1) == 1)
						snprintf_s_i(tmp, sizeof(tmp), "%d", 1);
					else
						snprintf_s_i(tmp, sizeof(tmp), "%d", 0);
					strncat_s(tmp, sizeof(tmp), present, strnlen_s(present, sizeof(present)-1));
					snprintf_s_s(present, sizeof(present), "%s", tmp);
				}
				printf("%s", present);
				fprintf(file_out, "%s", present);
			} else {
				printf("%s", pa->data);
				fprintf(file_out, "%s", pa->data);
			}
		} else {
			for (i = 0; i < pa->datalen; i++) {
				if (pa->type == TYPE_RAW || (strstr(pa->name, "mac_addr"))) {
					printf("%02X ", pa->data[i]);
					fprintf(file_out, "%02X ", pa->data[i]);
				} else {
					printf("%X", pa->data[i]);
					fprintf(file_out, "%X", pa->data[i]);
				}
			}
		}
		printf("\n");
		fprintf(file_out, "\n");
	}
	fclose(file_out);

	list_for_each_entry(n, &root->children, sibling) {
		depth++;
		print_node(db_name, n);
		depth--;
	}
}

void delete_node(memdb_integer db_name, struct node *root)
{
	struct node *p, *n;
	struct node_attr *pa, *na;

	list_for_each_entry_safe(p, n, &root->children, sibling) {
			delete_node(db_name, p);
		}

	if (DB_RMM == db_name) {
		if (root == &rmm_root)
			return;

	} else if (DB_POD == db_name) {
		if (root == &pod_root)
			return;
	} else {
		MEMDB_ERR("No matched DB for deleting node\n");
		return;
	}

	list_for_each_entry_safe(pa, na, &root->attrs, group) {
		if (pa->datalen > ATTR_DATA_BUFFSIZE)
			free(pa->data);

		list_del(&pa->group);
		list_del(&pa->list);
		free(pa);
	}

	list_del(&root->list);
	list_del(&root->sibling);

	node_delete_notify(db_name, root);

	free(root);
}

/**
 * @brief: When node or attribute is need to do snapshot,
 *         update its parent nodes to do snapshot.
 */
static void update_node_snapshot_mask(struct node *node)
{
	struct node *n = NULL;

	for (n = node; n != NULL; n = n->parent) {
		n->snapshot_flag = SNAPSHOT_NEED;
	}
}

struct node *insert_node(memdb_integer db_name, struct node *parent, memdb_integer type,
						 memdb_integer snapshot_flag)
{
	struct node *n;

	n = malloc(sizeof(struct node));
	if (n == NULL) {
		rmm_log(ERROR, "malloc failed\n");
		return NULL;
	}

	clock_gettime(CLOCK_MONOTONIC, &(n->create_time));

	n->node_id = g_node_number++;
	n->type = type;
	n->parent = parent;
	n->snapshot_flag = snapshot_flag;

	if (SNAPSHOT_NEED == snapshot_flag)
		update_node_snapshot_mask(n->parent);

	INIT_LIST_HEAD(&n->attrs);
	INIT_LIST_HEAD(&n->children);

	if (DB_RMM == db_name) {
		list_add_tail(&n->list, &node_list);
		list_add_tail(&n->sibling, &parent->children);
		if (SNAPSHOT_NEED == n->snapshot_flag) {
			memdb_log(DB_RMM, MEMDB_LOG_RMM_FILE, n,
				MEMDB_LOG_CREATE_NODE);
		}
		node_create_notify(DB_RMM, n);
	} else if (DB_POD == db_name) {
		list_add_tail(&n->list, &pod_node_list);
		list_add_tail(&n->sibling, &parent->children);
		if (SNAPSHOT_NEED == n->snapshot_flag) {
			memdb_log(DB_POD, MEMDB_LOG_POD_FILE, n,
				MEMDB_LOG_CREATE_NODE);
		}
		node_create_notify(DB_POD, n);
	} else {
		MEMDB_ERR("No matched DB for inserting node\n");
		free(n);
		n = NULL;
	}

	return n;
}

/**
 * @brief: When we load the snapshot file to the memdb, we need to
 *         set the node id which loaded from the snapshot file.
 */
struct node *insert_node_with_node_id(memdb_integer db_name, struct node *parent,
								   memdb_integer node_id, memdb_integer type,
								   memdb_integer snapshot_flag, bool log_load)
{
	struct node *n;

	n = find_node_by_node_id(db_name, node_id);
	if (NULL != n) {
		n->snapshot_flag = snapshot_flag;
		if (SNAPSHOT_NEED == snapshot_flag)
			update_node_snapshot_mask(n->parent);
		return n;
	}

	n = (struct node *)malloc(sizeof(struct node));
	if (n == NULL) {
		rmm_log(ERROR, "malloc failed\n");
		return NULL;
	}

	clock_gettime(CLOCK_MONOTONIC, &(n->create_time));

	n->node_id = node_id;
	g_node_number = (g_node_number > node_id ? g_node_number : (node_id + 1));
	n->type = type;
	n->parent = parent;
	n->snapshot_flag = snapshot_flag;

	if (SNAPSHOT_NEED == snapshot_flag)
		update_node_snapshot_mask(n->parent);

	INIT_LIST_HEAD(&n->attrs);
	INIT_LIST_HEAD(&n->children);

	if (DB_RMM == db_name) {
		list_add_tail(&n->list, &node_list);
		list_add_tail(&n->sibling, &parent->children);
		if (n->snapshot_flag == SNAPSHOT_NEED && log_load == FALSE)
			memdb_log(DB_RMM, MEMDB_LOG_RMM_FILE, n,
				  MEMDB_LOG_CREATE_NODE);
		node_create_notify(DB_RMM, n);
	} else if (DB_POD == db_name) {
		list_add_tail(&n->list, &pod_node_list);
		list_add_tail(&n->sibling, &parent->children);
		if (n->snapshot_flag == SNAPSHOT_NEED && log_load == FALSE)
			memdb_log(DB_POD, MEMDB_LOG_POD_FILE, n,
				  MEMDB_LOG_CREATE_NODE);
		node_create_notify(DB_POD, n);
	} else {
		MEMDB_ERR("No matched DB for inserting node\n");
		free(n);
		n = NULL;
	}

	return n;
}

void destroy_node(memdb_integer db_name, struct node *root, bool log_load)
{
	if (DB_RMM == db_name) {
		if (SNAPSHOT_NEED == root->snapshot_flag && log_load == FALSE) {
			memdb_log(DB_RMM, MEMDB_LOG_RMM_FILE, root,
				  MEMDB_LOG_DESTROY_NODE);
		}
		delete_node(DB_POD, root);
	}
	else if (DB_POD == db_name) {
		if (SNAPSHOT_NEED == root->snapshot_flag && log_load == FALSE) {
			memdb_log(DB_POD, MEMDB_LOG_POD_FILE, root,
				  MEMDB_LOG_DESTROY_NODE);
		}
		delete_node(DB_POD, root);
	} else
		MEMDB_ERR("No matched DB to destroy the node\n");
}

void dump_all_subscribes(memdb_integer db_name)
{
	struct subscription *sub;
	char buff[512] = {0};
	char *file_path = MEM_DUMP_PATH;
	struct list_head *which_sub = &sublist;

	if (DB_POD == db_name) {
		file_path = MEM_POD_DUMP_PATH;
		which_sub = &pod_sublist;
	}

	FILE *file_out = fopen(file_path, "a+w");

	if (file_out == NULL)
		return;

	snprintf(buff, sizeof(buff), "\n\n"
		"*************************************\n"
		"*          Subscribe  Dump          *\n"
		"*************************************\n"
		"\n");

	printf("%s", buff);
	fprintf(file_out, "%s", buff);

	list_for_each_entry(sub, which_sub, list) {
		if (sub->event == EVENT_NODE_CREATE) {
			snprintf_s_ll(buff, sizeof(buff), "subscribe: NODE CREATE, type is %lld\n", sub->data.node.type_min);
			printf("%s", buff);
			fprintf(file_out, "%s", buff);
		} else if (sub->event == EVENT_NODE_DELETE) {
			snprintf_s_ll(buff, sizeof(buff), "subscribe: NODE DELETE, type is %lld\n", sub->data.node.type_min);
			printf("%s", buff);
			fprintf(file_out, "%s", buff);
		} else if (sub->event == EVENT_NODE_ATTR) {
			snprintf_s_ll(buff, sizeof(buff), "subscribe: NODE ATTR, node_id is %lld\n", sub->data.attr.node_id);
			printf("%s", buff);
			fprintf(file_out, "%s", buff);
			if (sub->data.attr.prefix_len != 0) {
				snprintf_s_s(buff, sizeof(buff), "|-----------prefix is %s\n", sub->data.attr.name_prefix);
				printf("%s", buff);
				fprintf(file_out, "%s", buff);
			}
		} else {
			snprintf(buff, sizeof(buff), "Unknown subscribe event type\n");
			printf("%s", buff);
			fprintf(file_out, "%s", buff);
		}
	}

	snprintf(buff, sizeof(buff),
		"\n"
		"*************************************\n"
		"*************************************\n"
		"\n");

	printf("%s", buff);
	fprintf(file_out, "%s", buff);

	fclose(file_out);

}

void dump_all_nodes(memdb_integer db_name)
{
	char *file_path = MEM_DUMP_PATH;
	struct node *node_print = &rmm_root;
	char *memdb_name = "RMM MEMDB";

	if (DB_POD == db_name) {
		file_path = MEM_POD_DUMP_PATH;
		node_print = &pod_root;
		memdb_name = "POD MEMDB";
	}

	printf("\033[31m############# \033[32m%s\033[31m #############\033[0m\n",
		   memdb_name);

	FILE *file_out = fopen(file_path, "a+w");

	if (NULL == file_out)
		return;

	char buff[1024] = {0};

	snprintf(buff, sizeof(buff),
		"\n"
		"*************************************\n"
		"*      MemoryDB Data Tree Dump      *\n"
		"*                                   *\n"
		"*      NODE: N:ID:Addr(Type)        *\n"
		"*      ATTR: A:Cookie:Name:Data     *\n"
		"*************************************\n"
		"\n");

	printf("%s", buff);
	fprintf(file_out, "%s", buff);
	fclose(file_out);

	print_node(db_name, node_print);

	file_out = fopen(file_path, "a+w");

	if (NULL == file_out)
		return;

	snprintf(buff, sizeof(buff),
		"\n"
		"*************************************\n"
		"*************************************\n"
		"\n");
	printf("%s", buff);
	fprintf(file_out, "%s", buff);
	fclose(file_out);
}

struct node *find_node_by_node_id(memdb_integer db_name, memdb_integer node_id)
{
	struct node *n;
	struct list_head *node_list_search = NULL;

	if (DB_RMM == db_name) {
		node_list_search = &node_list;
	} else if (DB_POD == db_name) {
		node_list_search = &pod_node_list;
	} else {
		MEMDB_ERR("No matched DB to find thd node\n");
		return NULL;
	}

	list_for_each_entry(n, node_list_search, list) {
		if (n->node_id == node_id)
			return n;
	}

	return NULL;
}


static inline struct node_attr *find_attr_item(struct node *node, char *name,
		unsigned short namelen)
{
	struct node_attr *pa;

	list_for_each_entry(pa, &node->attrs, group) {
		if (pa->namelen == namelen &&
		    strcmp(pa->name, name) == 0)
			return pa;
	}

	return NULL;
}

static inline unsigned char *alloc_attr_data(struct node_attr *pa, int datalen)
{
	if (datalen > ATTR_DATA_BUFFSIZE)
		return malloc(datalen);
	else
		return pa->buf;
}

int set_node_attr(memdb_integer db_name, struct node *node, memdb_integer cookie,
				  unsigned char *name, unsigned short namelen,
				  unsigned char *data, unsigned short datalen,
				  memdb_integer snapshot_flag, memdb_integer type, bool log_load)
{
	struct node_attr *pa;

	if (namelen == 0 || datalen == 0)
		return -1;

	name[namelen - 1] = '\0';	/* paranoia ;-) */

	if (DB_RMM == db_name) {
		curr_attr_action = EVENT_ATTR_ACTION_MOD;
		if (node->node_id == 0) {
			rmm_root.snapshot_flag = snapshot_flag;
		}
	} else if (DB_POD == db_name) {
		curr_pod_attr_action = EVENT_ATTR_ACTION_MOD;
		if (node->node_id == 0) {
			pod_root.snapshot_flag = snapshot_flag;
		}
	}

	pa = find_attr_item(node, (char *)name, namelen);
	if (pa == NULL) {
		pa = malloc(sizeof(*pa) + namelen);
		if (pa == NULL)
			return -1;

		pa->data = alloc_attr_data(pa, datalen);
		if (pa->data == NULL) {
			free(pa);
			return -1;
		}

		clock_gettime(CLOCK_MONOTONIC, &(node->modify_time));

		pa->node = node;
		pa->namelen = namelen;
		pa->datalen = datalen;
		pa->snapshot_flag = snapshot_flag;
		pa->type = type;
		memcpy_s(pa->name, namelen, name, namelen);
		list_add_tail(&pa->group, &node->attrs);

		if (DB_RMM == db_name)
			list_add_tail(&pa->list, &attr_list);
		else if (DB_POD == db_name)
			list_add_tail(&pa->list, &pod_attr_list);
		else {
			free(pa);
			return -1;
		}
	}

	if (datalen != pa->datalen) {
		unsigned char *cp;

		cp = alloc_attr_data(pa, datalen);
		if (cp == NULL)
			return -1;

		if (pa->datalen > ATTR_DATA_BUFFSIZE)
			free(pa->data);

		pa->data = cp;
		pa->datalen = datalen;
	}

	pa->cookie = cookie;
	memcpy_s(pa->data, datalen, data, datalen);

	if (DB_RMM == db_name) {
		node_attr_notify(DB_RMM, pa, EVENT_ATTR_ACTION_ADD);
		if (SNAPSHOT_NEED == pa->snapshot_flag && log_load == FALSE) {
			memdb_log(DB_RMM, MEMDB_LOG_RMM_FILE, pa,
				  MEMDB_LOG_SET_ATTR);
		}
	} else if (DB_POD == db_name) {
		node_attr_notify(DB_POD, pa, EVENT_ATTR_ACTION_ADD);
		if (SNAPSHOT_NEED == pa->snapshot_flag && log_load == FALSE) {
			memdb_log(DB_POD, MEMDB_LOG_POD_FILE, pa,
				  MEMDB_LOG_SET_ATTR);
		}
	}

	if (SNAPSHOT_NEED == snapshot_flag)
		update_node_snapshot_mask(node);

	return 0;
}

int get_node_attr(struct node *node, unsigned char *name,
				unsigned short namelen, memdb_integer *cookie,
				char **data_ptr)
{
	struct node_attr *pa;

	pa = find_attr_item(node, (char *)name, namelen);
	if (pa == NULL)
		return -1;

	*cookie = pa->cookie;
	*data_ptr = (char *)pa->data;

	return 0;
}

static void del_node_attr(struct node_attr *pa)
{
	struct node *n = pa->node;

	if (pa->datalen > ATTR_DATA_BUFFSIZE)
		free(pa->data);

	clock_gettime(CLOCK_MONOTONIC, &(n->modify_time));

	list_del(&pa->group);
	list_del(&pa->list);
	free(pa);
}

int remove_node_attr(memdb_integer db_name, struct node *node, unsigned char *name,
					 unsigned short namelen, bool log_load)
{
	struct node_attr *pa;

	list_for_each_entry(pa, &node->attrs, group) {
		if (pa->namelen != namelen || strcmp(pa->name, (char *)name) != 0)
			continue;

		clock_gettime(CLOCK_MONOTONIC, &(node->create_time));

		if (DB_RMM == db_name) {
			if (SNAPSHOT_NEED == pa->snapshot_flag && log_load == FALSE) {
				memdb_log(DB_RMM, MEMDB_LOG_RMM_FILE, pa,
						  MEMDB_LOG_REMOVE_ATTR);
			}
			del_node_attr(pa);
			node_attr_notify(DB_RMM, pa, EVENT_ATTR_ACTION_DEL);
		} else if (DB_POD == db_name) {
			if (SNAPSHOT_NEED == pa->snapshot_flag && log_load == FALSE) {
				memdb_log(DB_POD, MEMDB_LOG_POD_FILE, pa,
						  MEMDB_LOG_REMOVE_ATTR);
			}
			del_node_attr(pa);
			node_attr_notify(DB_POD, pa, EVENT_ATTR_ACTION_DEL);
		} else
			return 0;

		return 1;
	}

	return 0;
}

void snapshot_check(void)
{
	if (MEMDB_LOG_MAX_NUMBER <= memdb_log_get_rmm_number()) {
		memdb_node_snap(FILE_SNAP, DB_RMM);
		unlink(MEMDB_LOG_RMM_FILE);
		memdb_log_set_rmm_number(0);
	}

	if (MEMDB_LOG_MAX_NUMBER <= memdb_log_get_pod_number()) {
		memdb_node_snap(FILE_POD_SNAP, DB_POD);
		unlink(MEMDB_LOG_POD_FILE);
		memdb_log_set_pod_number(0);
	}
}

void int_node_module(void)
{
	clock_gettime(CLOCK_MONOTONIC, &(rmm_root.create_time));
	clock_gettime(CLOCK_MONOTONIC, &(pod_root.create_time));

	list_add_tail(&rmm_root.list, &node_list);
	list_add_tail(&pod_root.list, &pod_node_list);
}

