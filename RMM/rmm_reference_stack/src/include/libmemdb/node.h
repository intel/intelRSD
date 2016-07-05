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


#ifndef __LIBMEMDB_NODE_H__
#define __LIBMEMDB_NODE_H__

#include "libutils/list.h"
#include "libutils/types.h"


/***********************************************************************************/

#define MEM_DUMP_PATH     "/var/log/Memdbd/dump.out"
#define MEM_POD_DUMP_PATH	"/var/log/Memdbd/dump_pod.out"


enum memdb_type {
	TYPE_CHAR   = 0,
	TYPE_SHORT  = 1,
	TYPE_INT    = 2,
	TYPE_STRING = 3,
	TYPE_RAW    = 4,
};


struct node_info {
	memdb_integer parent;
	memdb_integer node_id;
	memdb_integer  type;
	memdb_integer snapshot_flag;
};

struct attr_info {
	memdb_integer node;

	memdb_integer cookie;
	memdb_integer snapshot_flag;

	memdb_integer next_offset;	/* size of attr_info + name + data + pad */
	memdb_integer data_offset;	/* equ. name length */
	memdb_integer data_len;

	unsigned char elems[0];	/* The name, then the data(datalen). */
};

/* Helper function for 'attr_info' */
#define foreach_attr_info(info, offset, attrs, size)	\
	for ((offset) = 0, info = (void *)(attrs);	\
	     (offset) < size;	\
	     (offset) += info->next_offset, info = (void *)((char *)(attrs) + offset))

static inline char *attr_name(struct attr_info *i)
{
	return (char *)i->elems;
}

static inline void *attr_data(struct attr_info *i)
{
	return i->elems + i->data_offset;
}

/***********************************************************************************/

struct node {
	memdb_integer node_id;

	memdb_integer type;

	memdb_integer snapshot_flag;
	struct timespec create_time;
	struct timespec modify_time;

	struct node *parent;

	struct list_head list;
	struct list_head attrs;		/* list with node_attr's group */
	struct list_head children;	/* list of my children */
	struct list_head sibling;	/* linkage in my parent's children list */
};

struct node_attr {
	struct list_head group;	/* linkage in node's attrs list */
	struct list_head list;  /* linkage in global search list */

	struct node *node;

	/* the content of the following items should closely match the struct of node_attr_params defined in command.h */
	memdb_integer cookie;
	memdb_integer snapshot_flag;

	memdb_string name_ptr;	/* will be used in handle.c, getting parameter value phase */
	memdb_string data_ptr;	/* will be used in handle.c, getting parameter value phase */

	memdb_integer namelen;
	memdb_integer datalen;
	memdb_integer type;
	/* struct node_attr_params end */

	/* 'data' points to 'buf' if 'datalen' <= ATTR_DATA_BUFFSIZE */
#define ATTR_DATA_BUFFSIZE		8
	unsigned char buf[ATTR_DATA_BUFFSIZE];
	unsigned char *data;

	char name[0];
};

enum {
	DB_RMM = 0,
	DB_POD,
	DB_MAX
};

static char *db_name_str[DB_MAX] = {
	"RMM",
	"POD"
};

#define SNAPSHOT_NEED	1
#define SNAPSHOT_NEED_NOT 0
#define SNAPSHOT_NEED_ALL 2


extern struct list_head node_list;
extern struct list_head attr_list;

extern struct list_head pod_node_list;
extern struct list_head pod_attr_list;

extern struct node *insert_node(memdb_integer db_name,
								struct node *parent,
								memdb_integer type,
								memdb_integer snapshot_flag);
extern struct node *insert_node_with_node_id(memdb_integer db_name,
										  struct node *parent,
										  memdb_integer node_id,
										  memdb_integer type,
										  memdb_integer snapshot_flag, bool log_load);
extern void destroy_node(memdb_integer db_name, struct node *root, bool log_load);
extern void dump_all_nodes(memdb_integer db_name);
extern void dump_all_subscribes(memdb_integer db_name);

extern struct node *find_node_by_node_id(memdb_integer db_name,
									  memdb_integer node_id);
extern void delete_node(memdb_integer db_name, struct node *root);
extern int set_node_attr(memdb_integer db_name, struct node *node,
						 memdb_integer cookie,
						 unsigned char *name, unsigned short namelen,
						 unsigned char *data, unsigned short datalen,
						 memdb_integer snapshot_flag, memdb_integer type, bool log_load);
extern int get_node_attr(struct node *node, unsigned char *name,
						 unsigned short namelen, memdb_integer *cookie,
						 char **data_ptr);
extern int remove_node_attr(memdb_integer db_name, struct node *node,
							unsigned char *name,
							unsigned short namelen, bool log_load);
extern void snapshot_check(void);

extern void int_node_module(void);

#endif

