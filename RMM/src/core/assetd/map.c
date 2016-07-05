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
#include "map.h"
#include "libutils/list.h"
#include "libutils/string.h"
#include "libwrap/wrap.h"

typedef struct un_map_item {
	struct list_head list;
	char uuid[UUID_MAX_LEN];
	memdb_integer node_id;
} un_map_item_t;


struct list_head node_list = LIST_HEAD_INIT(node_list);

void nmap_add(char* uuid, memdb_integer node_id)
{
	un_map_item_t *item = NULL;

	item = (un_map_item_t *)malloc(sizeof(un_map_item_t));
	if (item == NULL)
		return;

	memset(item, 0, sizeof(un_map_item_t));

	strncpy_safe(item->uuid, uuid, UUID_MAX_LEN, UUID_MAX_LEN - 1);
	item->node_id = node_id;

	list_add_tail(&item->list, &node_list);
}

void nmap_remove_by_node_id(memdb_integer node_id)
{
	un_map_item_t *item;

	list_for_each_entry(item, &node_list, list) {
		if(item->node_id == node_id) {
			list_del(&item->list);
			free(item);
			break;
		}
	}
	
}

int nmap_get_node_id_by_uuid(memdb_integer *node_id, char *uuid)
{
	un_map_item_t *item;

	list_for_each_entry(item, &node_list, list) {
		if (strcmp((char *)uuid, item->uuid) == 0) {
			*node_id = item->node_id;
			return 0;
		}
	}
	return -1;
}
