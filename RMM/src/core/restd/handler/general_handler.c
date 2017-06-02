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


#include "handler.h"
#include "libredfish/rf_event.h"
#include "libcfg/cfg.h"
#include "libutils/time.h"
#include "libutils/ip.h"
#include "libutils/string.h"
#include "libredfish/snmp_event.h"
#include "libsecurec/safe_lib.h"


typedef struct type_to_key {
	int32 type;
	int8 id_key[RMM_NAME_LEN];
	int8 uuid_key[RMM_NAME_LEN];
} type_to_key_t;

const id_evt_type_t nodeid_keys[] = {
	{MC_REDFISH_RACK_CHANGE, RF_EVENT_TYPE_RACK_CHANGE_STR},
	{MC_REDFISH_RACK_UPDATE, RF_EVENT_TYPE_RACK_UPDATE_STR},

	{MC_REDFISH_PZONE_ADD, RF_EVENT_TYPE_PZONE_ADD_STR},
	{MC_REDFISH_PZONE_REMOVE,    RF_EVENT_TYPE_PZONE_REMOVE_STR},
	{MC_REDFISH_PSU_CHANGE, RF_EVENT_TYPE_PSU_CHANGE_STR},
	{MC_REDFISH_PSU_ADD,  RF_EVENT_TYPE_PSU_ADD_STR},
	{MC_REDFISH_PSU_REMOVE, RF_EVENT_TYPE_PSU_REMOVE_STR},

	{MC_REDFISH_TZONE_ADD, RF_EVENT_TYPE_TZONE_ADD_STR},
	{MC_REDFISH_TZONE_REMOVE, RF_EVENT_TYPE_TZONE_REMOVE_STR},
	{MC_REDFISH_TZONE_UPDATE, RF_EVENT_TYPE_TZONE_UPDATE_STR},
	{MC_REDFISH_FAN_ADD,  RF_EVENT_TYPE_FAN_ADD_STR},
	{MC_REDFISH_FAN_REMOVE, RF_EVENT_TYPE_FAN_REMOVE_STR},
	{MC_REDFISH_FAN_CHANGE, RF_EVENT_TYPE_FAN_CHANGE_STR},

	{MC_REDFISH_DRAWER_CHANGE,    RF_EVENT_TYPE_DRAWER_CHANGE_STR},
	{MC_REDFISH_DRAWER_ADD, RF_EVENT_TYPE_DRAWER_ADD_STR},
	{MC_REDFISH_DRAWER_REMOVE,  RF_EVENT_TYPE_DRAWER_REMOVE_STR},
	{MC_REDFISH_DRAWER_ALERT, RF_EVENT_TYPE_DRAWER_ALERT_STR},

	{MC_REDFISH_MBP_CHANGE,    RF_EVENT_TYPE_MBP_CHANGE_STR},
	{MC_REDFISH_MBP_ADD, RF_EVENT_TYPE_MBP_ADD_STR},
	{MC_REDFISH_MBP_REMOVE,  RF_EVENT_TYPE_MBP_REMOVE_STR},

	{MC_REDFISH_RESOURCE_CHANGE, RF_EVENT_TYPE_CHANGE_STR},
	{MC_REDFISH_RESOURCE_UPDATE, RF_EVENT_TYPE_UPDATE_STR},
	{MC_REDFISH_RESOURCE_ADD,    RF_EVENT_TYPE_ADD_STR},
	{MC_REDFISH_RESOURCE_REMOVE, RF_EVENT_TYPE_DELETE_STR},
	{MC_REDFISH_RESOURCE_ALERT,  RF_EVENT_TYPE_ALERT_STR}
};

static int32 gen_asset_index(const int32 type, int32 loc_id,  memdb_integer node_id)
{
	int8 *name;
	struct node_info *pnode = NULL;
	int64 error_code = 0;
	int32 zone_num = 0;
	int32 cm_lid = 0;
	int32 index = 0;
	int8 rs[128] = {0};

	switch (type) {
	case MC_TYPE_PZONE:
		zone_num = MAX_PZONE_NUM;
		break;
	case MC_TYPE_TZONE:
		zone_num = MAX_TZONE_NUM;
		break;
	case MC_TYPE_DZONE:
		zone_num = MAX_DZONE_NUM;
		break;
	default:
		return loc_id;
	}

	pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
	if (pnode == NULL) {
		return -1;
	}

	memset(rs, 0, 128);
	error_code = 0;
	error_code = libdb_attr_get_string(DB_RMM, pnode->parent, MBP_LOC_ID_STR, rs, 128, LOCK_ID_NULL);
	if (error_code != 0) {
		rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, error_code);
		return -1;
	}
	cm_lid = str2int(rs);

	index = (cm_lid - 1) * zone_num + loc_id;

	return index;
}

static bool get_index_by_uuid(const int8 *uuid, const int32 type, int32 *index)
{
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int32 subnode_num = 0;
	int32 i;
	int8 rs[128] = {0};
	int64 error_code = 0;
	int32 loc_id = 0;

	subnode = libdb_list_node_by_type(DB_RMM, (uint32)type, (uint32)type, &subnode_num, NULL, LOCK_ID_NULL);

	if (subnode == NULL) {
		rmm_log(ERROR, "get subnode error\n");
		return FALSE;
	}

	sub_node = (struct node_info *)malloc(sizeof(struct node_info) * subnode_num);
	if (sub_node == NULL) {
		rmm_log(ERROR, "malloc error\n");
		libdb_free_node(subnode);
		return FALSE;
	}

	memcpy_s(sub_node, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	for (i = 0; i < subnode_num; i++) {
		memset(rs, 0, 128);
		error_code = 0;
		error_code = libdb_attr_get_string(DB_RMM, sub_node[i].node_id, WRAP_UUID_STR, rs, 128, LOCK_ID_NULL);

		if (error_code != 0) {
			rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, error_code);
			free(sub_node);
			libdb_free_node(subnode);
			return FALSE;
		} else {
			if (0 == strcasecmp(uuid, rs)) {
				memset(rs, 0, 128);
				error_code = 0;
				error_code = libdb_attr_get_string(DB_RMM, sub_node[i].node_id, WRAP_LOC_ID_STR, rs, 128, LOCK_ID_NULL);
				if (error_code != 0) {
					rmm_log(ERROR, "%s:%d: error code:%d\n", __func__, __LINE__, error_code);
					free(sub_node);
					libdb_free_node(subnode);
					return FALSE;
				} else if (index != NULL) {
					loc_id = str2int(rs);
					*index = gen_asset_index(type, loc_id, sub_node[i].node_id);
					free(sub_node);
					libdb_free_node(subnode);
					return TRUE;
				}
			}
		}
	}

	free(sub_node);
	libdb_free_node(subnode);
	return FALSE;
}

int32 get_asset_idx(const struct rest_uri_param *param, const int8 *name, int32 type)
{
	int8 *p_id = NULL;
	int32 index;

	p_id = rest_path_value(param, name);
	if (p_id == NULL) {
		HTTPD_ERR("get value fail\n");
		return -1;
	}

	if (TRUE == is_str_uuid((const int8 *)p_id)) {
		if (FALSE == get_index_by_uuid((const int8 *)p_id, type, &index)) {
			HTTPD_ERR("uuid %s error!\n", p_id);
			return -1;
		}
	} else
		index = (uint32)str2int(p_id);

	return index;
}

memdb_integer get_asset_node_id(const struct rest_uri_param *param, const int8 *name)
{
	int8 *p_uuid = NULL;
	int8 uuid[64] = {0};
	memdb_integer node_id;

	p_uuid = rest_path_value(param, name);
	if (p_uuid == NULL) {
		HTTPD_ERR("get value from path fail\n");
		return 0;
	}

	snprintf_s_s((int8 *)uuid, sizeof(uuid), "%s", p_uuid);
	libwrap_get_node_id_by_uuid(uuid, &node_id);

	return node_id;
}

void put_prepare_int(const json_t *elem, uint32 *output, const int8 *name)
{
	uint32 value;
	json_t *obj = NULL;

	obj = json_object_get(elem, name);
	if (obj != NULL) {
		value = json_integer_value(obj);
		if (TRUE == check_int_range(value, 0, FAN_THRESH_MAX))
			*output = value;
	}
}

static int32 get_fixed_info(rf_fixed_t *fixed)
{
	return rf_get_fixed_info(fixed);
}

static int32 get_evt_support(int32 *evt_types)
{
	int i = 0;

	for (i = 0; i < sizeof(nodeid_keys)/sizeof(id_evt_type_t); i++) {
		evt_types[i] = 1;
	}

	return 0;
}

static int32 get_evt_links(rf_link_t *links, const int8 *fmt, ...)
{
	va_list args;
	int32 port;
	int8 ip_addr[WRAP_DB_MAX_VALUE_LEN] = {0};
	memdb_integer nid = 0;
	int8 format[256] = {0};
	int8 prefix[PREFIX_LEN] = {0};

	rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);

	port = rmm_cfg_get_port(RESTD_PORT);
	if (port == 0) {
		HTTPD_ERR("Failed to call rmm_cfg_get_restd_port\n");
		return -1;
	}

	snprintf_s_sss(format, sizeof(format), "%s%s%s", (char *)prefix, (char *)fmt, RF_EVENT_SERVICE_STR);
	va_start(args, fmt);
	vsnprintf(links->odata_id, sizeof(links->odata_id), format, args);
	va_end(args);

	memset(format, 0, sizeof(format));
	snprintf_s_sss(format, sizeof(format), "%s%s%s", (char *)prefix, (char *)fmt, RF_EVENT_ODATA_CONTEXT_STR);
	va_start(args, fmt);
	vsnprintf(links->odata_context, CONTEXT_LEN, format, args);
	va_end(args);

	memset(format, 0, sizeof(format));
	snprintf_s_ssss(format, sizeof(format), "%s%s%s%s", (char *)prefix, (char *)fmt, RF_EVENT_SERVICE_STR, RF_EVENT_SUBSCRIBE_STR);
	va_start(args, fmt);
	vsnprintf(links->subscriptions, sizeof(links->subscriptions), format, args);
	va_end(args);

	return 0;
}


int32 rf_evt_info_init(rf_evt_svc_t *service, int32 *evt_types, const int8 *fmt, ...)
{
	va_list args;
	int32 rc = -1;

	rc = get_fixed_info(&service->fixed);
	if (rc != 0) {
		HTTPD_ERR("get redfish fixed info error\n");
		return -1;
	}

	rc = get_evt_support(evt_types);
	if (rc != 0) {
		HTTPD_ERR("get redfish event support error\n");
		return -1;
	}

	va_start(args, fmt);
	rc = get_evt_links(&service->links, fmt, args);
	va_end(args);
	if (rc != 0) {
		HTTPD_ERR("get redfish event links error\n");
		return -1;
	}

	return 0;
}

int32 rf_evt_pack_json(json_t *result, const int32 *evt_types, rf_evt_svc_t *service)
{
	int32 i = 0;
	json_t *types_array = NULL;
	json_t *listener = NULL;
	json_t *status = NULL;

	json_object_add(result, RMM_JSON_ODATA_CONTEXT, json_string(service->links.odata_context));
	json_object_add(result, RMM_JSON_ODATA_ID, json_string(service->links.odata_id));
	add_json_string(result, RMM_JSON_ODATA_TYPE, service->fixed.odata_type);
	add_json_string(result, RMM_JSON_RF_ID, service->fixed.id);
	add_json_string(result, RMM_JSON_RF_NAME, service->fixed.name);

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_RF_STATUS_STATE, service->fixed.status.state);
		add_json_string(status, RMM_JSON_RF_STATUS_HEALTH, service->fixed.status.health);
	}
	json_object_add(result, RMM_JSON_RF_STATUS, status);

	if (service->fixed.service_enabled)
		json_object_add(result, RMM_JSON_RF_SERVICE_ENABLED, json_true());
	else
		json_object_add(result, RMM_JSON_RF_SERVICE_ENABLED, json_false());

	add_json_integer(result, RMM_JSON_RF_DELIVERY_RETRY, service->fixed.retry);

	add_json_integer(result, RMM_JSON_RF_DELIVERY_RETRY_INTERVAL, service->fixed.retry_interval);


	types_array = json_array();
	if (types_array == NULL)
		return -1;

	for (i = 0;  i < MAX_EVT_ACTION_NUM; i++) {
		if (evt_types[i] == 0)
			continue;

		json_array_add(types_array, json_string((int8 *)rf_evt_msg[i].type_name));
	}
	json_object_add(result, RMM_JSON_RF_SUPPORTED_EVT_TYPES, types_array);

	listener = json_object();
	if (listener == NULL) 
		return -1;

	add_json_string(listener, RMM_JSON_ODATA_ID, service->links.subscriptions);
	json_object_add(result, RMM_JSON_RF_SUBSCRIPTION, listener);

	return 0;
}


static int32 get_root_evt_type(int32 type)
{
	switch (type) {
	case MC_REDFISH_RESOURCE_ADD:
	case MC_REDFISH_PZONE_ADD:
	case MC_REDFISH_PSU_ADD:
	case MC_REDFISH_TZONE_ADD:
	case MC_REDFISH_FAN_ADD:
	case MC_REDFISH_DRAWER_ADD:
	case MC_REDFISH_MBP_ADD:
		return MC_REDFISH_RESOURCE_ADD;

	case MC_REDFISH_RESOURCE_REMOVE:
	case MC_REDFISH_PZONE_REMOVE:
	case MC_REDFISH_PSU_REMOVE:
	case MC_REDFISH_TZONE_REMOVE:
	case MC_REDFISH_FAN_REMOVE:
	case MC_REDFISH_DRAWER_REMOVE:
	case MC_REDFISH_MBP_REMOVE:
		return MC_REDFISH_RESOURCE_REMOVE;

	case MC_REDFISH_RESOURCE_UPDATE:
	case MC_REDFISH_RACK_UPDATE:
	case MC_REDFISH_TZONE_UPDATE:
		return MC_REDFISH_RESOURCE_UPDATE;

	case MC_REDFISH_RESOURCE_CHANGE:
	case MC_REDFISH_RACK_CHANGE:
	case MC_REDFISH_PSU_CHANGE:
	case MC_REDFISH_FAN_CHANGE:
	case MC_REDFISH_DRAWER_CHANGE:
	case MC_REDFISH_MBP_CHANGE:
		return MC_REDFISH_RESOURCE_CHANGE;

	case MC_REDFISH_RESOURCE_ALERT:
	case MC_REDFISH_DRAWER_ALERT:
		return MC_REDFISH_RESOURCE_ALERT;

	default:
		break;
	}

	return type;
}


int32 subscribe_event(listener_dest_t *listener, int8 *evt_str)
{
	memdb_integer evt_nid = 0;
	memdb_integer listener_nid = 0;
	int32 mask = 0;
	int32 rc = 0;
	int32 type = get_rf_evt_type(evt_str);
	int32 root_type = get_root_evt_type(type);

	listener_nid = libwrap_get_listener(listener->dest, &evt_nid, type);
	if (listener_nid == -1) {
		HTTPD_ERR("get redfish event listener error, type is %d\n", type);
		return -1;
	}

	if (libwrap_evt_subscribe(listener, listener_nid, evt_nid) != 0)
		return rc;

	/* subscribe resource root */
	listener_nid = libwrap_get_listener(listener->dest, &evt_nid, root_type);
	if (listener_nid == -1) {
		HTTPD_ERR("get redfish event listener error, type is %d\n", type);
		return -1;
	}

	if (libwrap_evt_subscribe(listener, listener_nid, evt_nid) != 0)
		return rc;

	return 0;
}

int32 unsubscribe_event(listener_dest_t *listener, int8 *evt_str)
{
	memdb_integer evt_nid;
	memdb_integer listener_nid = 0;
	int32 mask = 0;
	int32 type = get_rf_evt_type(evt_str);

	listener_nid = libwrap_get_listener(listener->dest, &evt_nid, type);

	if (listener_nid == 0)
		return 0;

	return libwrap_evt_unsubscribe(listener, listener_nid);
}

static int32 is_event_types_valid(int32 array_size, json_t *event_types)
{
	json_t *elem = NULL;
	int32 i = 0;
	int8 *event_type = NULL;

	for (i = 0; i < array_size; i++) {
		elem = NULL;
		elem = json_array_get(event_types, i);
		if (elem == NULL) {
			HTTPD_ERR("get json array element error\n");
			return -1;
		}

		event_type = json_string_value(elem);
		if (event_type == NULL) {
			HTTPD_ERR("get json string value error\n");
			return -1;
		}

		if (get_rf_evt_type(event_type) == -1) {
			HTTPD_ERR("Invalid event type:%s\n", event_type);
			return -1;
		}
	}

	return 0;
}

int32 process_listener(json_t *req, listener_dest_t *listener, int32 mask)
{
	int8 *event_type = NULL;
	int8 *dest = NULL;
	int8 *context = NULL;
	int8 *protocol = NULL;
	int8 *name = NULL;
	int8 *description = NULL;
	json_t *elem = NULL;
	json_t *event_types = NULL;
	int32 array_size, i = 0;
	int32 rc = 0;

	context = json_string_value(json_object_get(req, RMM_JSON_CONTEXT));
	dest = json_string_value(json_object_get(req, RMM_JSON_DEST));
	protocol = json_string_value(json_object_get(req, RMM_JSON_PROTOCOL));
	event_types = json_object_get(req, RMM_JSON_RF_EVT_TYPES);
	name = json_string_value(json_object_get(req, RMM_JSON_RF_NAME));
	description = json_string_value(json_object_get(req, RMM_JSON_RF_DESC));

	if (!(context && dest && protocol && event_types)) {
		HTTPD_ERR("get json string error\n");
		return -1;
	}

	snprintf_s_s(listener->name, sizeof(listener->name), "%s", name);
	snprintf_s_s(listener->description, sizeof(listener->description), "%s", description);
	snprintf_s_s(listener->context, sizeof(listener->context), "%s", context);
	snprintf_s_s(listener->dest, sizeof(listener->dest), "%s", dest);
	snprintf_s_s(listener->protocol, sizeof(listener->protocol), "%s", protocol);
	listener->idx_mask = mask;

	if (JSON_ARRAY != event_types->type) {
		HTTPD_ERR("event_types is not an array\n");
		return -1;
	}

	array_size = json_array_size(event_types);

	if (array_size < 1) {
		HTTPD_ERR("event_types array is empty\n");
		return -1;
	}

	if (is_event_types_valid(array_size, event_types) == -1)
		return -1;

	for (i = 0; i < array_size; i++) {
		elem = NULL;
		elem = json_array_get(event_types, i);
		if (elem == NULL) {
			HTTPD_ERR("get json array element error\n");
			return -1;
		}

		event_type = json_string_value(elem);
		if(event_type == NULL) {
			HTTPD_ERR("get json string  event error\n");
			return -1;
		}

		if (subscribe_event(listener, event_type) != 0)
			return -1;
	}

	return 0;
}


json_t *process_snmp_listener(json_t *req)
{
	int8 *action = NULL;
	int8 *dest = NULL;
	int32 port = 0;

	action = json_string_value(json_object_get(req, RMM_JSON_ACTION));
	dest = json_string_value(json_object_get(req, RMM_JSON_DEST));
	port = json_integer_value(json_object_get(req, RMM_JSON_PORT));

	if (!(action && dest)) {
		HTTPD_ERR("get json string error\n");
		return NULL;
	}

	if (0 == strcmp(action, RMM_JSON_SUBSCRIPTION))
		snmp_subagentd_set(0, dest, port);
	else if (0 == strcmp(action, RMM_JSON_UNSUBSCRIPTION))
		snmp_subagentd_set(1, dest, port);

	return NULL;
}



static void get_event_type_by_node_type(int32 node_type, int8 *data, int32 data_len)
{
	int32 i  = 0;

	for (i = 0; i < sizeof(nodeid_keys) / sizeof(id_evt_type_t); i++) {
		if (nodeid_keys[i].node_type == node_type) {
			strncpy_safe(data, nodeid_keys[i].type_name, data_len, data_len - 1);
			return;
		}
	}
	HTTPD_ERR("node type input error: %d\n", node_type);
}

int32 evt_listener_init(evt_listener_t *listener, int32 mask, int32 listener_idx, const int8 *fmt, ...)
{
	listener_t *header = NULL;
	int8 format[256] = {0};
	listener_t *tmp;
	int8 prefix[PREFIX_LEN] = {0};
	json_t *evt_type_item = NULL;
	int32 rs = 0;
	int32 count = 0;
	va_list args;

	rs = libwrap_get_evt_listeners_count(mask, &count);
	if (rs != 0 || listener_idx > count) {
		HTTPD_ERR("invalid listener index \n");
		return -1;
	}

	snprintf_s_s(listener->copy_right, sizeof(listener->copy_right), "%s", RF_COPY_RIGHT);

	rs = rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
	if (rs != 0) {
		HTTPD_ERR("get rest prefix fail\n");
		return -1;
	}

	snprintf_s_ssss(format, sizeof(format), "%s%s%s%s", prefix, (char *)fmt, RF_EVENT_ODATA_CONTEXT_STR, RF_EVENT_LISTENER_MEMBER_STR);
	va_start(args, fmt);
	vsnprintf(listener->odata_context, CONTEXT_LEN, format, args);
	va_end(args);

	evt_type_item = json_array();
	if (evt_type_item == NULL) {
		HTTPD_ERR("json array alloc fail\n");
		return -1;
	}

	snprintf_s_ssssi(listener->odata_id, sizeof(listener->odata_id), "%s%s%s%s/%d", prefix
																		, (char *)fmt
																		, RF_EVENT_SERVICE_STR
																		, RF_EVENT_SUBSCRIBE_STR
																		, listener_idx);

	snprintf_s_s(listener->odata_type, sizeof(listener->odata_type), "%s", RF_EVENT_ODATA_TYPE_DEST);
	snprintf_s_i(listener->id, sizeof(listener->id), "%d", listener_idx);

	rs = libwrap_get_evt_listener(mask, listener, listener_idx);
	if (rs != 0) {
		HTTPD_ERR("get listeners fail\n");
		return -1;
	}

//	snprintf(listener->context, sizeof(listener->context), "%s", RF_EVENT_CONTEXT_STR);
//	snprintf(listener->protocol, sizeof(listener->protocol), "%s", RF_EVENT_PROTOCOL_STR);

	return 0;
}

int32 evt_listener_pack_json(json_t *result, evt_listener_t *listener)
{
	int8 evt_type[128] = {};
	int32 i = 0;
	json_t *evt_type_item = NULL;

	if (strnlen_s(listener->url, sizeof(listener->url)-1) == 0) {
		HTTPD_ERR("invalid subscribe destination\n");
		return -1;
	}

	json_object_add(result, RMM_JSON_ODATA_CONTEXT, json_string(listener->odata_context));
	json_object_add(result, RMM_JSON_ODATA_ID, json_string(listener->odata_id));
	json_object_add(result, RMM_JSON_ODATA_TYPE, json_string(listener->odata_type));
	json_object_add(result, RMM_JSON_RF_ID, json_string(listener->id));
	json_object_add(result, RMM_JSON_RF_NAME, json_string(listener->name));
	json_object_add(result, RMM_JSON_RF_DESC, json_string(listener->description));
	json_object_add(result, RMM_JSON_RF_DEST_URL, json_string(listener->url));

	evt_type_item = json_array();
	if (evt_type_item == NULL) {
		HTTPD_ERR("json array request fail\n");
		return -1;
	}

	for (i = 0; i < listener->evt_index; i++) {
		memset(evt_type, 0, sizeof(evt_type));
		get_event_type_by_node_type(listener->event_types[i], evt_type, sizeof(evt_type));
		if (strnlen_s(evt_type, sizeof(evt_type)-1) != 0)
			json_array_add(evt_type_item, json_string(evt_type));
	}
	json_object_add(result, RMM_JSON_RF_EVT_TYPES, evt_type_item);

	json_object_add(result, RMM_JSON_RF_CONTEXT, json_string(listener->context));
	json_object_add(result, RMM_JSON_RF_PROTOCOL, json_string(listener->protocol));

	return 0;
}

int32 evt_listeners_init(evt_listeners_t *listeners, int32 mask, const int8 * fmt, ...)
{
	json_t *result = NULL;
	json_t *jitem = NULL;
	json_t *evt_type_item = NULL;
	int32 i = 0;
	int8 evt_type[128];
	listener_t *tmp;
	int8 format[256] = {0};
	int8 prefix[PREFIX_LEN] = {0};
	int8 buff[256] = {0};
	int32 count = 0;
	int32 rs = 0;
	va_list args;

	snprintf_s_s(listeners->copy_right, sizeof(listeners->copy_right), "%s", RF_COPY_RIGHT);

	rs = rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
	if (rs != 0) {
		HTTPD_ERR("get rest prefix fail\n");
		return -1;
	}

	snprintf_s_ssss(format, sizeof(format), "%s%s%s%s", prefix, (char *)fmt, RF_EVENT_ODATA_CONTEXT_STR, RF_EVENT_EVT_MEMBER_STR);
	va_start(args, fmt);
	vsnprintf(listeners->odata_context, CONTEXT_LEN, format, args);
	va_end(args);

	snprintf_s_s(listeners->odata_type, sizeof(listeners->odata_type), "%s", RF_EVENT_ODATA_TYPE_DEST_COLL);
    snprintf_s_s(listeners->odata_id, sizeof(listeners->odata_id), "%s", RF_EVENT_ODATA_ID_DEST_COLL);
	snprintf_s_s(listeners->name, sizeof(listeners->name), "%s", RF_EVENT_LISTENERS_NAME);

	rs = libwrap_get_evt_listeners_count(mask, &count);
	if (rs != 0) {
		HTTPD_ERR("get listener count fail\n");
		return -1;
	}
	listeners->num = count;

	for (i = 0; i < count; i++) {
		snprintf_s_ssssi(listeners->url[i], sizeof(listeners->url[i]), "%s%s%s%s/%d", prefix, (char *)fmt, RF_EVENT_SERVICE_STR, RF_EVENT_SUBSCRIBE_STR, (i + 1));
	}

	return 0;
}

int32 evt_listeners_pack_json(json_t *result, evt_listeners_t *listeners)
{
	json_t *listener_array = NULL;
	json_t * listener = NULL;
	int32 i = 0;

	json_object_add(result, RMM_JSON_ODATA_CONTEXT, json_string(listeners->odata_context));
	json_object_add(result, RMM_JSON_ODATA_TYPE, json_string(listeners->odata_type));
    json_object_add(result, RMM_JSON_ODATA_ID, json_string(listeners->odata_id));
	json_object_add(result, RMM_JSON_NAME, json_string(listeners->name));
	json_object_add(result, RMM_JSON_MEMBERS_ODATA_COUNT, json_integer(listeners->num));

	listener_array = json_array();
	if (listener_array == NULL) {
		HTTPD_ERR("json array request fail\n");
		return -1;
	}

	for (i = 0; i < listeners->num; i++) {
		listener = json_object();
		if (listener == NULL) {
			HTTPD_ERR("json object request fail\n");
			return -1;
		}
		json_object_add(listener, RMM_JSON_ODATA_ID, json_string(listeners->url[i]));
		json_array_add(listener_array, listener);
	}

	json_object_add(result, RMM_JSON_MEMBERS, listener_array);

	return 0;
}


int32 format_uuid(char *url, int len)
{
	int i = 0;
	int8  *find = NULL;

	if (NULL == url)
		return -1;

	/*remove tail '/'*/
	if (url[len - 1] == '/')
		url[len - 1] = 0;

	for (i = len; i >= 0; i--) {
		find = strchr(&url[i], '/');
		if (find != NULL) {
			if (*(find + 1) > 'A' && *(find + 1) < 'Z')
				*(find + 1) += 32;
			break;
		}
	}
	return 0;
}


