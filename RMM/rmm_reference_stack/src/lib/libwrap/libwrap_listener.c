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

#include "libwrap.h"


static result_t add_listener(listener_t** header, int8 *tmp_url, listener_dest_t *listener, int type_id)
{
	int32 ret = 0;
	listener_t *plast = NULL;
	listener_t *new_node;

	if (*header == NULL) {
		/* No listener has been added before */
		*header = (struct listener *)malloc(sizeof(struct listener));
		if (*header == NULL)
			return RESULT_MALLOC_ERR;

		memset(*header, 0, sizeof(struct listener));
		strncpy_safe((*header)->url, tmp_url, sizeof((*header)->url), sizeof((*header)->url) - 1);
		strncpy_safe((*header)->context, listener->context, sizeof((*header)->context), sizeof((*header)->context) - 1);
		strncpy_safe((*header)->protocol, listener->protocol, sizeof((*header)->protocol), sizeof((*header)->protocol) - 1);
		strncpy_safe((*header)->name, listener->name, sizeof((*header)->name), sizeof((*header)->name) - 1);
		strncpy_safe((*header)->description, listener->description, sizeof((*header)->description), sizeof((*header)->description) - 1);
		(*header)->event_types[(*header)->evt_index] = type_id;
		(*header)->evt_index = (*header)->evt_index + 1;

		return RESULT_OK;
	} else {
		listener_t *orig_header = *header;

		while (*header) {
			ret = strcmp((*header)->url, tmp_url);
			if (ret == 0) {
				if ((*header)->evt_index < TYPES_NUM) {
					/* Add a new event type to exist listener. */
					(*header)->event_types[(*header)->evt_index] = type_id;
					(*header)->evt_index = (*header)->evt_index + 1;
					*header = orig_header;
					return RESULT_OK;
				} else
					rmm_log(ERROR, "Invalid event index:%d\n", (*header)->evt_index);

			} else {
				plast = *header;
				*header = plast->pnext;
			}
		}
		if (*header == NULL && plast != NULL) {
			/* Add a new listener */
			new_node = (struct listener *)malloc(sizeof(struct listener));
			if (new_node == NULL)
				return RESULT_MALLOC_ERR;
			memset(new_node, 0, sizeof(struct listener));
			strncpy_safe(new_node->url, tmp_url, sizeof(new_node->url), sizeof(new_node->url) - 1);
			strncpy_safe(new_node->context, listener->context, sizeof(new_node->context), sizeof(new_node->context) - 1);
			strncpy_safe(new_node->protocol, listener->protocol, sizeof(new_node->protocol), sizeof(new_node->protocol) - 1);
			strncpy_safe(new_node->name, listener->name, sizeof(new_node->name), sizeof(new_node->name) - 1);
			strncpy_safe(new_node->description, listener->description, sizeof(new_node->description), sizeof(new_node->description) - 1);
			new_node->event_types[new_node->evt_index] = type_id;
			new_node->evt_index = new_node->evt_index + 1;
			plast->pnext = new_node;
			*header = orig_header;
		}

		return RESULT_OK;
	}
}

static void free_listeners(listener_t *header)
{
	listener_t *pnext = NULL;

	while (header) {
		pnext = header->pnext;
		free(header);
		header = pnext;
	}
}

static result_t get_listeners(int32 mask, listener_t** header)
{
	struct node_info *subnode = NULL;
	int32 subnode_num = 0;
	struct node_info *listener_subnode = NULL;
	struct node_info *listener_sub_node = NULL;
	int32 listener_subnode_num = 0;
	int32 i,j = 0;
	memdb_integer error_code = 0;
	int32 subscribe_mask = 0;
	int8 tmp_url[MAX_URL] = {0};
	listener_dest_t listener = {};
	int32 listener_count = 0;
	struct node_info *evt_type_nodes = NULL;
	result_t rc = RESULT_OK;

	/* get event root node */
	subnode = libdb_list_node_by_type(DB_RMM, MC_REDFISH_EVENT, MC_REDFISH_EVENT, &subnode_num, NULL, LOCK_ID_NULL);
	if (subnode_num != 1) {
		rc = RESULT_NO_NODE;
		goto end;
	}

	/* get supported event type nodes */
	subnode = libdb_list_subnode(DB_RMM, subnode[0].node_id, &subnode_num, NULL, LOCK_ID_NULL);
	if (subnode_num == 0) {
		rc = RESULT_NO_NODE;
		goto end;
	}

	evt_type_nodes = (struct node_info *)malloc(CMDBUFSIZ);
	if (evt_type_nodes == NULL) {
		rc = RESULT_MALLOC_ERR;
		goto end;
	}
	memcpy_s(evt_type_nodes, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	listener_sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (listener_sub_node == NULL) {
		rc = RESULT_MALLOC_ERR;
		goto end;
	}

	for (i = 0; i < subnode_num; i++) {
		/* get listeners of each event type */
		listener_subnode = libdb_list_subnode_by_type(DB_RMM, evt_type_nodes[i].node_id, MC_REDFISH_LISTENER, &listener_subnode_num, NULL, LOCK_ID_NULL);
		if (listener_subnode == NULL)
			goto end;

		bzero(listener_sub_node, CMDBUFSIZ);
		memcpy_s(listener_sub_node, sizeof(struct node_info) * listener_subnode_num, listener_subnode, sizeof(struct node_info) * listener_subnode_num);

		for (j = 0; j < listener_subnode_num; j++) {
			error_code = 0;
			/* get listener's mask */
			error_code = libdb_attr_get_int(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_INDEXES_STR, &subscribe_mask, LOCK_ID_NULL);
			if (error_code != 0)
				continue;

			if ((subscribe_mask & mask) != 0) {
				error_code = 0;
				error_code = libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_DEST_STR, tmp_url, sizeof(tmp_url), LOCK_ID_NULL);
				if (error_code == 0) {
					if (libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_CONTEXT_STR, listener.context, sizeof(listener.context), LOCK_ID_NULL) != 0)
						continue;
					if (libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_PROTOCOL_STR, listener.protocol, sizeof(listener.protocol), LOCK_ID_NULL) != 0)
						continue;
					if (libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_NAME_STR, listener.name, sizeof(listener.name), LOCK_ID_NULL) != 0)
						continue;
					if (libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_DESC_STR, listener.description, sizeof(listener.description), LOCK_ID_NULL) != 0)
						continue;

					/* add matched listener into list */
					add_listener(header, tmp_url, &listener, evt_type_nodes[i].type);
				}
			}
		}
	}

end:
	libdb_free_node(listener_sub_node);
	libdb_free_node(evt_type_nodes);

	return rc;
}

result_t libwrap_get_evt_listeners_count(int32 mask, int32 *count)
{
	listener_t *header = NULL;
	listener_t *orig_header;
	result_t rc = RESULT_OK;

	*count = 0;
	rc = get_listeners(mask, &header);
	orig_header = header;
	while (header) {
		*count = *count + 1;
		header = header->pnext;
	}
	free_listeners(orig_header);

	return rc;
}

static result_t del_listener(int8 *url)
{
	struct node_info *subnode = NULL;
	struct node_info *listener_subnode = NULL;
	struct node_info *listener_sub_node = NULL;
	int32 subnode_num = 0;
	int32 listener_subnode_num = 0;
	int32 i,j = 0;
	memdb_integer error_code = 0;
	int8 tmp_url[MAX_URL] = {0};
	int32 listener_count = 0;
	struct node_info *evt_type_nodes = NULL;
	result_t rc = RESULT_OK;

	/* get event root node */
	subnode = libdb_list_node_by_type(DB_RMM, MC_REDFISH_EVENT, MC_REDFISH_EVENT, &subnode_num, NULL, LOCK_ID_NULL);
	if (subnode_num != 1) {
		rc = RESULT_NO_NODE;
		goto end;
	}

	/* get supported event type nodes */
	subnode = libdb_list_subnode(DB_RMM, subnode[0].node_id, &subnode_num, NULL, LOCK_ID_NULL);
	if (subnode_num == 0) {
		rc = RESULT_NO_NODE;
		goto end;
	}

	evt_type_nodes = (struct node_info *)malloc(CMDBUFSIZ);
	if (evt_type_nodes == NULL) {
		rc = RESULT_MALLOC_ERR;
		goto end;
	}
	memcpy_s(evt_type_nodes, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	for (i = 0; i < subnode_num; i++) {
		/* get listeners of each event type */
		listener_subnode = libdb_list_subnode_by_type(DB_RMM, evt_type_nodes[i].node_id, MC_REDFISH_LISTENER, &listener_subnode_num, NULL, LOCK_ID_NULL);
		if (listener_subnode == NULL)
			goto end;
	
		listener_sub_node = (struct node_info *)malloc(CMDBUFSIZ);
		if (listener_sub_node == NULL) {
			rc = RESULT_MALLOC_ERR;
			goto end;
		}
		memcpy_s(listener_sub_node, sizeof(struct node_info) * listener_subnode_num, listener_subnode, sizeof(struct node_info) * listener_subnode_num);

		for (j = 0; j < listener_subnode_num; j++) {
			if (libdb_attr_get_string(DB_RMM, listener_sub_node[j].node_id, RF_EVENT_LISTENER_DEST_STR, tmp_url, sizeof(tmp_url), LOCK_ID_NULL) != 0)
				goto end;
			if (strcmp(tmp_url, url) == 0)
				libdb_destroy_node(DB_RMM, listener_sub_node[j].node_id, LOCK_ID_NULL);
		}
	}

end:

	if (listener_sub_node)
		libdb_free_node(listener_sub_node);
	if (evt_type_nodes)
		libdb_free_node(evt_type_nodes);

	return rc;
}

result_t libwrap_del_evt_listener(int32 mask, int32 listener_idx)
{
	int32 count = 0;
	result_t rc = RESULT_OK;
	listener_t *header = NULL;

	rc = get_listeners(mask, &header);
	if (rc != RESULT_OK) {
		return rc;
	}

	if (header == NULL)
		return RESULT_NONE_POINTER;

	if (listener_idx < 1)
		return RESULT_NO_NODE;
	else
		listener_idx--;

	while (listener_idx) {
		header = header->pnext;
		listener_idx--;
	}

	if (header)
		rc = del_listener(header->url);

	free_listeners(header);

	return rc;

}

int32 libwrap_get_evt_listener_idx(int32 mask, int8 *url)
{
	int32 count = 0;
	result_t rc = RESULT_OK;
	listener_t *header = NULL;
	int32 listener_idx = 0;

	rc = get_listeners(mask, &header);
	if (rc != RESULT_OK) {
		return -1;
	}

	if (header == NULL)
		return -1;

	while (header) {
		header = header->pnext;
		listener_idx++;
	}

	return listener_idx;

}

result_t libwrap_get_evt_listener(int32 mask, evt_listener_t *listener, int32 listener_idx)
{
	int32 count = 0;
	result_t rc = RESULT_OK;
	listener_t *header = NULL;

	rc = get_listeners(mask, &header);
	if (rc != RESULT_OK) {
		return rc;
	}

	if (header == NULL)
		return RESULT_NONE_POINTER;

	if (listener_idx < 1)
		return RESULT_NO_NODE;
	else
		listener_idx--;

	while (listener_idx) {
		header = header->pnext;
		listener_idx--;
	}

	if (header) {
		memcpy_s(listener->url, sizeof(listener->url), header->url, sizeof(listener->url));
		memcpy_s(listener->event_types, sizeof(listener->event_types), header->event_types, sizeof(listener->event_types));
		memcpy_s(listener->context, sizeof(listener->context), header->context, sizeof(listener->context));
		memcpy_s(listener->protocol, sizeof(listener->protocol), header->protocol, sizeof(listener->protocol));
		memcpy_s(listener->name, sizeof(listener->name), header->name, sizeof(listener->name));
		memcpy_s(listener->description, sizeof(listener->description), header->description, sizeof(listener->description));
		listener->evt_index = header->evt_index;
	}

	free_listeners(header);

	return rc;
}


memdb_integer libwrap_get_listener_id_by_dest(char *dest, memdb_integer parent, int type)
{
	int i = 0;
	char output[1024] = {0};
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int subnode_num = 0;

	if (dest == NULL) {
		return 0;
	}

	subnode = libdb_list_subnode_by_type(DB_RMM, parent, type, &subnode_num, NULL, LOCK_ID_NULL);
	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return 0;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	for (i = 0; i < subnode_num; i++) {
		libdb_attr_get_string(DB_RMM, sub_node[i].node_id, RF_EVENT_LISTENER_DEST_STR, output, 1024, LOCK_ID_NULL);

		if (0 == strcmp(dest, output))
			return sub_node[i].node_id;
	}

	libdb_free_node(sub_node);

	return 0;
}



memdb_integer libwrap_get_listener(char *dest, memdb_integer *evt_nid, int type)
{
	int nodenum  = 0;
	memdb_integer listener_nid = 0;
	struct node_info *nodeinfo;

	nodeinfo = libdb_list_node_by_type(DB_RMM, type, type, &nodenum, NULL, LOCK_ID_NULL);
	if (nodenum != 1) {
		libdb_free_node(nodeinfo);
		return -1;
	}

	*evt_nid = nodeinfo->node_id;
	listener_nid = libwrap_get_listener_id_by_dest(dest, *evt_nid, MC_REDFISH_LISTENER);

	libdb_free_node(nodeinfo);
	return listener_nid;

}


result_t libwrap_get_listener_links(int reg, memdb_integer nid, rf_link_t *links)
{
	int mask = 0;
	int subnode_num, i, j, index = 0;
	memdb_integer listener_nid = 0;
	char listener[256] = {0};
	struct node_info *subnode = NULL;

	subnode = libdb_list_subnode(DB_RMM, nid, &subnode_num, NULL, LOCK_ID_NULL);

	for (i = 0; i < subnode_num; i++) {
		listener_nid = libwrap_get_subnode_id_by_type(subnode[i].node_id, MC_REDFISH_LISTENER);
		if (listener_nid == 0)
			continue;

		libdb_attr_get_int(DB_RMM, listener_nid, RF_EVENT_LISTENER_INDEXES_STR, &mask, LOCK_ID_NULL);
		if ((mask & reg) != 0) {
			libdb_attr_get_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_DEST_STR, listener, 256, LOCK_ID_NULL);
			for (j = 0; j < index; j++) {
				if (strcmp(listener, links->listeners[j]) == 0)
					break;
			}
			if (j == index) {
				snprintf_s_s(links->listeners[index], sizeof(links->listeners[index]), "%s", listener);
				index++;
			}
		}
	}

	return RESULT_OK;
}


