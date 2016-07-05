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


#ifndef __LIBMEMDB_EVENT_H__
#define __LIBMEMDB_EVENT_H__

#include "libmemdb/param.h"
#include "libmemdb/node.h"
#include "libutils/list.h"
#include "libutils/types.h"

enum subscription_event {
	EVENT_NODE_CREATE = 0,
	EVENT_NODE_DELETE,
	EVENT_NODE_ATTR,
	EVENT_END
};

static char *event_string[EVENT_END] = {
	"event_node_create",
	"event_node_delete",
	"event_node_attr"
};

struct subscription {
	struct list_head list;

	memdb_integer	event;
	memdb_integer	cb_port;
	memdb_integer	cb_pid;

	union {
		struct {	/* EVENT_NODE_XXXX : [type_min, type_max] */
			memdb_integer type_min;
			memdb_integer type_max;
		} node;

		struct {
			memdb_integer node_id;
			memdb_integer prefix_len;
			char         name_prefix[0];
		} attr;
	} data;
};

struct event_info {
	memdb_integer event;

	union {
		struct {	/* EVENT_NODE_XXXX */
			memdb_integer parent;
			memdb_integer node_id;
			memdb_integer  type;
		} node;

		struct {
			memdb_integer nodeid;
			memdb_integer cookie;
			memdb_integer action;
			memdb_integer namelen;
			memdb_integer datalen;
			unsigned char  elems[0]; /* name + data */
		} attr;
	} info;
};

enum {
	EVENT_ATTR_ACTION_ADD = 1,	/* Add  a new attribution */
	EVENT_ATTR_ACTION_DEL,		/* Delete the attribution */
	EVENT_ATTR_ACTION_MOD		/* Modify the attribution */
};

#define nparent	info.node.parent
#define nnodeid	info.node.node_id
#define ntype	info.node.type

#define anodeid	info.attr.nodeid
#define acookie		info.attr.cookie
#define aaction		info.attr.action
#define adatalen	info.attr.datalen

static inline char *event_attr_name(struct event_info *e)
{
	return (char *)e->info.attr.elems;
}

static inline void *event_attr_data(struct event_info *e)
{
	return e->info.attr.elems + e->info.attr.namelen;
}

#if 0
union event_union {
	struct event_info evt;
	unsigned long data[CMDBUFSIZ/sizeof(long)];
};
#endif

extern struct list_head sublist;
extern struct list_head pod_sublist;

extern void node_create_notify(memdb_integer db_name, struct node *n);
extern void node_delete_notify(memdb_integer db_name, struct node *n);
extern void node_attr_notify(memdb_integer db_name, struct node_attr *a,
							 memdb_integer action);

extern void int_event_module(void);


#endif

