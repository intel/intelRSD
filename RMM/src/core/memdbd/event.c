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
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "libmemdb/event.h"
#include "libutils/rmm.h"
#include "libutils/sock.h"
#include "libjsonrpc/jsonrpc.h"
#include "libsecurec/safe_lib.h"


struct list_head sublist = LIST_HEAD_INIT(sublist);
struct list_head pod_sublist = LIST_HEAD_INIT(pod_sublist);

static int notify_fd = -1;

static void event_notify(struct subscription *s, struct event_info *evt, int evtlen)
{
	struct sockaddr_in addr;
	char *notify_str = NULL;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = s->cb_port;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	switch (evt->event) {
	case EVENT_NODE_CREATE:
	case EVENT_NODE_DELETE:
		{
			jrpc_param_t params[3] = {
				{"node_id", &evt->nnodeid, JSON_INTEGER},
				{"parent", &evt->nparent, JSON_INTEGER},
				{"type", mc_type_str[evt->ntype], JSON_STRING}
			};
			notify_str = jrpc_create_notify_string(event_string[evt->event], 3, params);
		}
		break;
	case EVENT_NODE_ATTR:
		{
			jrpc_param_t params[5] = {
				{"node_id", &evt->info.attr.nodeid, JSON_INTEGER},
				{"cookie", &evt->info.attr.cookie, JSON_INTEGER},
				{"action", &evt->info.attr.action, JSON_INTEGER},
				{"name", &evt->info.attr.elems[0], JSON_STRING},
				{"data", &evt->info.attr.elems[evt->info.attr.datalen], JSON_STRING}
			};
			notify_str = jrpc_create_notify_string(event_string[evt->event], 5, params);
		}
		break;
	default:
		break;
	}

	if (notify_str) {
		sendto(notify_fd, notify_str, strnlen_s(notify_str, RSIZE_MAX_STR)+1, 0, (struct sockaddr *)&addr, sizeof(addr));
		jrpc_free_string(notify_str);
	}

	if (s->cb_pid != 0)
		kill(s->cb_pid, SIGEVT);
}

void node_create_notify(memdb_integer db_name, struct node *n)
{
	struct subscription *s;
	struct event_info event;
	struct list_head *which_sub = &sublist;

	if (DB_POD == db_name)
		which_sub = &pod_sublist;

	event.event = EVENT_NODE_CREATE;
	event.nnodeid = n->node_id;
	event.nparent = n->parent != NULL ? n->parent->node_id : 0UL;
	event.ntype = n->type;

	list_for_each_entry(s, which_sub, list) {
		if (s->event == EVENT_NODE_CREATE &&
		    n->type >= s->data.node.type_min &&
		    n->type <= s->data.node.type_max)
			event_notify(s, &event, sizeof(event));
	}
}

void node_delete_notify(memdb_integer db_name, struct node *n)
{
	struct subscription *s;
	struct event_info event;
	struct list_head *which_sub = &sublist;

	if (DB_POD == db_name)
		which_sub = &pod_sublist;

	event.event = EVENT_NODE_DELETE;
	event.nnodeid = n->node_id;
	event.nparent = n->parent != NULL ? n->parent->node_id : 0UL;
	event.ntype = n->type;

	list_for_each_entry(s, which_sub, list) {
		if (s->event == EVENT_NODE_DELETE &&
		    n->type >= s->data.node.type_min &&
		    n->type <= s->data.node.type_max)
			event_notify(s, &event, sizeof(event));
	}
}

void node_attr_notify(memdb_integer db_name, struct node_attr *a,
					  memdb_integer action)
{
	int size;
	char *name;
	memdb_integer node_id = 0;
	struct subscription *s;
	struct event_info *event;
	struct list_head *which_sub = &sublist;

	if (DB_POD == db_name)
		which_sub = &pod_sublist;

	node_id = a->node->node_id;
	list_for_each_entry(s, which_sub, list) {
		if (s->event == EVENT_NODE_ATTR &&
		    s->data.attr.node_id == node_id) {
			break;
		}
	}

	size = sizeof(*event) + a->namelen + a->datalen;
	if ((event = malloc(size)) == NULL)
		return;

	event->event = EVENT_NODE_ATTR;
	event->info.attr.nodeid   = node_id;
	event->info.attr.cookie   = a->cookie;
	event->info.attr.action   = action;
	event->info.attr.namelen  = a->namelen;
	event->info.attr.datalen  = a->datalen;
	memcpy_s(&event->info.attr.elems[0], a->namelen, a->name, a->namelen);
	memcpy_s(&event->info.attr.elems[a->namelen], a->datalen, a->data, a->datalen);

	name = a->name;
	list_for_each_entry(s, which_sub, list) {
		if (s->event == EVENT_NODE_ATTR) {
			if (s->data.attr.prefix_len != 0) {
				if (strncmp(name, s->data.attr.name_prefix, s->data.attr.prefix_len) == 0 &&
					s->data.attr.node_id == node_id) {
					event_notify(s, event, size);
				}
			} else if (s->data.attr.node_id != 0) {
				if (s->data.attr.node_id == node_id)
					event_notify(s, event, size);
			} else {
				event_notify(s, event, size);
			}
		}
	}

	free(event);
}

void int_event_module(void)
{
	notify_fd = udp_create();

	if (notify_fd == -1)
		exit(-1);
}
