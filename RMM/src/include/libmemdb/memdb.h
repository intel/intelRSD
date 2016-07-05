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


#ifndef __LIBMEMDB_MEMDB_H__
#define __LIBMEMDB_MEMDB_H__

#include <sys/socket.h>
#include <sys/types.h>

#include "libmemdb/node.h"
#include "libmemdb/event.h"
#include "libutils/types.h"

#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"

enum event_mode {
	NOTIFY_BY_SIGNAL,	/* if event happens, the process will be signaled */
	NOTIFY_BY_SELECT	/* if event happens, the select fd will be set */
};

typedef int (*filter_callback)(memdb_integer nodeid);

extern void libdb_init(void);

#ifndef LOCK_ID_NULL
typedef memdb_integer lock_id_t;
#define LOCK_ID_NULL (0)
#endif
extern memdb_integer libdb_dump_nodes(unsigned char db_name, lock_id_t lock_id);

extern memdb_integer libdb_create_node(unsigned char db_name,
									   memdb_integer parent, unsigned int type,
									   unsigned char snapshot_flag, lock_id_t lock_id);
extern memdb_integer libdb_create_node_with_node_id(unsigned char db_name,
												 memdb_integer parent,
												 unsigned int type,
												 memdb_integer node_id,
												 unsigned char snapshot_flag,
												 lock_id_t lock_id);
extern struct node_info *libdb_get_node_by_node_id(unsigned char db_name,
												 memdb_integer node_id, lock_id_t lock_id);

extern memdb_integer libdb_destroy_node(unsigned char db_name, memdb_integer node, lock_id_t lock_id);

extern struct node_info *libdb_list_subnode(unsigned char db_name,
											memdb_integer node,
											int *nodenum,
											filter_callback filter,
											lock_id_t lock_id);
extern struct node_info *libdb_list_subnode_by_type(unsigned char db_name,
													memdb_integer node,
													int type, int *nodenum,
													filter_callback filter,
													lock_id_t lock_id);
extern void libdb_free_node(struct node_info *node);
extern struct node_info *libdb_list_node(unsigned char db_name, int *nodenum, filter_callback filter, lock_id_t lock_id);
extern struct node_info *libdb_list_node_by_type(unsigned char db_name,
												 unsigned int type_min,
												 unsigned int type_max,
												 int *nodenum,
												 filter_callback filter,
												 lock_id_t lock_id);

extern memdb_integer libdb_attr_remove(unsigned char db_name, memdb_integer node, char *name, lock_id_t lock_id);

extern memdb_integer libdb_attr_set_char(unsigned char db_name, memdb_integer node,
								char *name, unsigned int cookie,
								char value, unsigned char snapshot_flag, lock_id_t lock_id);

extern lock_id_t libdb_lock(long timeout);
extern memdb_integer libdb_unlock(lock_id_t lock_id);

extern memdb_integer libdb_attr_set_short(unsigned char db_name, memdb_integer node,
								 char *name, unsigned int cookie,
								 short value, unsigned char snapshot_flag, lock_id_t lock_id);
extern memdb_integer libdb_attr_set_int	(unsigned char db_name, memdb_integer node,
								 char *name, unsigned int cookie,
								 int value, unsigned char snapshot_flag, lock_id_t lock_id);
extern memdb_integer libdb_attr_set_string(unsigned char db_name, memdb_integer node,
								  char *name, unsigned int cookie,
								  char *value, unsigned char snapshot_flag, lock_id_t lock_id);
extern memdb_integer libdb_attr_set_raw(unsigned char db_name, memdb_integer node,
							   char *name, unsigned int cookie,
							   int datalen, void *value, unsigned char snapshot_flag, lock_id_t lock_id);

/*
 * @@ libdb_attr_get_char/short/int return '-1' if failed;
 * @@ libdb_attr_get_string return "" if failed, and the sting
 * is saved in static memory.
 */
extern memdb_integer   libdb_attr_get_char(unsigned char db_name, memdb_integer node, char *name, char *output, lock_id_t lock_id);
extern memdb_integer   libdb_attr_get_short(unsigned char db_name, memdb_integer node, char *name, short *output, lock_id_t lock_id);
extern memdb_integer   libdb_attr_get_int(unsigned char db_name, memdb_integer node, char *name, int *output, lock_id_t lock_id);
extern memdb_integer   libdb_attr_get_string(unsigned char db_name, memdb_integer node, char *name, char *output, int64 len, lock_id_t lock_id);


/*
 * @@ libdb_list_attrs_by_cookie returns the attributes whose [cookie & cmask == cmask].
 *
 * @@ libdb_list_attrs_xx returns the static data buffer with length in '*size'.
 *
 * use @@foreach_attr_info @@attr_name @@attr_data to access 'struct attr_info'.
 */
extern void *libdb_list_attrs_by_node(unsigned char db_name, memdb_integer node, int *size, lock_id_t lock_id);
extern void *libdb_list_attrs_by_cookie(unsigned char db_name, unsigned int cmask, int *size, lock_id_t lock_id);


extern int  libdb_init_subscription(enum event_mode mode,
									void (*callback)(struct event_info *, void*),
									void *cb_data);
extern void libdb_event_selectfds(fd_set *readset, int *max_fd);
extern void libdb_event_processfds(fd_set *readset);
extern memdb_integer libdb_subscribe_node_create(unsigned char db_name, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_node_create_by_type(unsigned char db_name,
												 unsigned int node_type, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_type_of_node_create(unsigned char db_name,
												 unsigned int node_type_min,
												 unsigned int node_type_max,
												 lock_id_t lock_id);
extern memdb_integer libdb_subscribe_node_delete(unsigned char db_name, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_node_delete_by_type(unsigned char db_name,
												 unsigned int node_type,
												 lock_id_t lock_id);
extern memdb_integer libdb_subscribe_type_of_node_delete(unsigned char db_name,
												 unsigned int node_type_min,
												 unsigned int node_type_max,
												 lock_id_t lock_id);
extern memdb_integer libdb_subscribe_attr_all(unsigned char db_name, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_attr_special(unsigned char db_name, memdb_integer node_id, char *prefix, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_attr_by_prefix(unsigned char db_name, memdb_integer node_id, char *prefix, lock_id_t lock_id);
extern memdb_integer libdb_subscribe_attr_by_node(unsigned  char db_name, memdb_integer node_id, lock_id_t lock_id);
extern memdb_integer libdb_unsubscribe_event(unsigned char db_name, memdb_integer handle, lock_id_t lock_id);
extern memdb_integer libdb_is_ready(unsigned char db_name, lock_id_t lock_id, memdb_integer timeout_s);
extern void libdb_exit_memdb(void);

#endif

