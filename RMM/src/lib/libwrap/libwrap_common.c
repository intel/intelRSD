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
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <termios.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include "libmemdb/memdb.h"
#include "libredfish/msg_reg.h"
#include "libredfish/rf_event.h"
#include "libredfish/mr_def.h"

#include "libwrap/wrap.h"
#include "libcfg/cfg.h"
#include "libjsonrpcapi/assetd_api.h"

#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libutils/ip.h"
#include "libutils/rmm_json.h"
#include "libutils/types.h"
#include "libutils/string.h"
#include "libassetmodule/am_action.h"
#include "libutils/utils.h"
#include "libutils/time.h"
#include "libsecurec/safe_lib.h"
#include "am_api.h"

wrap_msg_t wrap_ipmi_msg;
static pthread_mutex_t wrap_mutex[MAX_REQ_PROCESS];
static pthread_cond_t  wrap_cond[MAX_REQ_PROCESS];

int get_db_info_num(uint8 db_name, memdb_integer node_id, int8 *name)
{
	int8 result[128] = {0};
	int8 *offset;
	int64 error_code = 0;

	error_code = libdb_attr_get_string(db_name, node_id, name, result, 128, LOCK_ID_NULL);
	if (error_code != 0) {
		printf("getting %s from memdb error, code is %llu\n", name, error_code);
		return 0;
	}

	return atoi(result);
}

int memdb_filter(memdb_integer node_id)
{
	int8 result[128] = {0};
	int64 rc = 0;

	rc = libdb_attr_get_string(DB_RMM, node_id, WRAP_UUID_STR, result, 128, LOCK_ID_NULL);
	if (rc != 0)
		return 1;

	if (strnlen_s(result, sizeof(result)-1) == 0)
		return 1;
	else
		return 0;
}

static int32 get_state_str(int32 state, int8 *output, int32 len)
{
	if (state == PSU_STATE_ENABLED || state == FAN_STATE_ENABLED) {
		snprintf_s_s(output, len, "%s", "Enabled");
		return 0;
	} else if (state == PSU_STATE_DISABLED || state == FAN_STATE_DISABLED) {
		snprintf_s_s(output, len, "%s", "Disabled");
		return 0;
	} else if (state == PSU_STATE_EN_OFFLINE) {
		snprintf_s_s(output, len, "%s", "Enabled but offline");
		return 0;
	} else if (state == PSU_STATE_NOT_APP) {
		snprintf_s_s(output, len, "%s", "Not applicable");
		return 0;
	}

	return -1;
}

result_t libwrap_get_state_str(int32 state, int8 *output, int32 len)
{
	return get_state_str(state, output, len);
}

void update_response_info(rest_uri_param_t *param, int32 status)
{
	param->status = status;
}

int32 libwrap_get_asset_num(int32 zone_type)
{
	int32 subnode_num = 0;
	struct node_info *node = NULL;

	node = libdb_list_node_by_type(DB_RMM, zone_type, zone_type, &subnode_num, memdb_filter, LOCK_ID_NULL);
	libdb_free_node(node);

	return subnode_num;
}

static int32 get_zone_num(int32 zone_type)
{
	int32 zone_num = 0;

	switch (zone_type) {
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
		return 0;
	}

	return zone_num;
}

int32 get_cm_lid(int32 zone_type, int32 zone_idx)
{
	int32 lid = 0;
	int32 zone_num = get_zone_num(zone_type);

	if (zone_num == 1)
		lid = (zone_idx / zone_num);
	else if (zone_num)
		lid = (zone_idx / zone_num) + 1;

	return lid;
}

int32 get_zone_lid(int32 zone_type, int32 zone_idx)
{
	int32 lid = 0;
	int32 zone_num = get_zone_num(zone_type);

	if (zone_num == 1)
		lid = 1;
	else if (zone_num)
		lid = zone_idx % zone_num;

	return lid;
}


int32 get_present(memdb_integer zone_node_id, uint32 *present)
{
	int32 i;
	int64 error_code = 0;
	struct node_info *sub_node_infos = 0;

	int32 sub_nodenum = 0;
	int32 index = 0;

	sub_node_infos = libdb_list_subnode(DB_RMM, zone_node_id, &sub_nodenum, memdb_filter, LOCK_ID_NULL);
	if (sub_node_infos == NULL) {
		return -1;
	}

	for (i = 0; i < sub_nodenum; i++) {
		index = 0;
		libdb_attr_get_int(DB_RMM, sub_node_infos[i].node_id, WRAP_LOC_ID_STR, &index, LOCK_ID_NULL);
		if (index > 0)
			*present = *present | 1 << (index -1);
	}
	return 0;
}

int32 get_present_string(memdb_integer node_id, int8 *output, int32 max_num, int32 max_len)
{
	uint32 present = 0;
	int32 i = 0;
	int8 tmp[8] = {0};
	int8 buff[REST_PRESENCE_LEN] = {0};
	int8 tmp_buff[REST_PRESENCE_LEN] = {0};

	if (-1 ==  get_present(node_id, &present))
		return -1;
	printf("%s node_id is %lld, present is %d\n", __func__, node_id, present);

	for (i = 0; i < max_num; i++) {
		memset(tmp, 0, sizeof(tmp));
		memset(tmp_buff, 0, sizeof(tmp_buff));
		if (((present>>i)&1) == 1) {
			snprintf_s_i(tmp, sizeof(tmp), "%d", 1);
		} else {
			snprintf_s_i(tmp, sizeof(tmp), "%d", 0);
		}
		snprintf_s_ss(tmp_buff, sizeof(tmp_buff), "%s%s", tmp, buff);
		memset(buff, 0, sizeof(buff));
		strncpy_s(buff, sizeof(buff), tmp_buff, strnlen_s(tmp_buff, sizeof(tmp_buff)-1));
	}

	strncpy_safe(output, buff, sizeof(buff), sizeof(buff) - 1);

	return 0;
}

int32 get_db_info_string(uint8 db_name, memdb_integer node_id, int8 *name, int8 *output, uint32 len)
{
	int8 result[1024] = {0};
	int64 error_code = 0;

	error_code = libdb_attr_get_string(db_name, node_id, name, result, sizeof(result), LOCK_ID_NULL);

	if (error_code != 0) {
		return (int32)error_code;
	}

	memcpy_s(output, (strnlen_s(result, sizeof(result))>len)?len:strnlen_s(result, sizeof(result)), result, (strnlen_s(result, sizeof(result)-1)>len)?len:strnlen_s(result, sizeof(result)-1));

	return 0;
}

int32 get_base_element(base_element_t* be, uint8 db_name, memdb_integer node_id)
{
	be->id = get_db_info_num(db_name, node_id, RACK_LOC_ID_STR);

	get_db_info_string(db_name, node_id, RACK_UUID_STR, be->uuid, UUID_LEN);
	get_db_info_string(db_name, node_id, RACK_NAME_STR, be->name, RMM_NAME_LEN);
	get_db_info_string(db_name, node_id, RACK_DESCRIPT_STR, be->desc, DESCRIPTION_LEN);
	get_db_info_string(db_name, node_id, RACK_CREATE_DATE_STR, be->create_date, DATE_LEN);
	get_db_info_string(db_name, node_id, RACK_UPDATE_DATE_STR, be->update_date, DATE_LEN);

	return 0;
}

int32 get_asset_info(uint8 db_name, asset_info_t* asset, memdb_integer node_id)
{
	get_db_info_string(db_name, node_id, RACK_SER_NUM_STR, asset->fru.serial_num, REST_RACK_STRING_LEN);
	get_db_info_string(db_name, node_id, RACK_MANUFACT_STR, asset->fru.manufacture, REST_RACK_STRING_LEN);
	get_db_info_string(db_name, node_id, RACK_MODEL_STR, asset->fru.model, REST_RACK_STRING_LEN);
	get_db_info_string(db_name, node_id, RACK_PART_NUM_STR, asset->fru.part_num, REST_RACK_STRING_LEN);
	get_db_info_string(db_name, node_id, RACK_FW_VER_STR, asset->fru.fw_ver, REST_RACK_STRING_LEN);
	get_db_info_string(db_name, node_id, RACK_ASSET_TAG_STR, asset->asset_tag, REST_ASSET_TAG_LEN);

	return  0;
}



static result_t gen_odata_context(const rest_uri_param_t *param, int8 *output, int32 len)
{
	int32 i = 0;
	int8 nodes[REST_MAX_NODE_LEN] = {0};

	if ((NULL == param) || (NULL == output) || (0 == len))
		return RESULT_NONE_POINTER;

	memset(output, 0, len);

	for (i = 0; i < param->num_nodes; i++) {
		memset(nodes, 0, REST_MAX_NODE_LEN);
		snprintf_s_s(nodes, REST_MAX_NODE_LEN, "/%s", param->nodes[i]);
		if ((strnlen_s(output, RSIZE_MAX_STR) + strnlen_s(nodes, sizeof(nodes)-1)) >= len)
			return RESULT_LEN_ERR;

		if ((i + 3 == param->num_nodes) && (0 != (unsigned char)atoi(param->nodes[i + 1]))) {
			strncat_s(output, RSIZE_MAX_STR, nodes, sizeof(nodes)-1);
			strncat_s(output, RSIZE_MAX_STR, "/Members/$entity", strnlen_s( "/Members/$entity", RSIZE_MAX_STR));
			return RESULT_OK;
		} else if ((2 == i) /*&& (0 == strncmp(param->nodes[i], "rack", strlen("rack")))*/) {
			strncat_s(output, RSIZE_MAX_STR, "/$metadata#", strnlen_s("/$metadata#", RSIZE_MAX_STR));
			strncat_s(output, RSIZE_MAX_STR, nodes + 1, sizeof(nodes)-2);
		} else if (TRUE == is_str_num(param->nodes[i])) {
			strncat_s(output, RSIZE_MAX_STR, "/Members/$entity", strnlen_s( "/Members/$entity", RSIZE_MAX_STR));
		} else
			strncat_s(output, RSIZE_MAX_STR, nodes, sizeof(nodes)-1);
	}

	if (param->num_nodes == 2) {
		strncat_s(output, RSIZE_MAX_STR, "/$metadata#ServiceRoot", RSIZE_MAX_STR);
	}

	return RESULT_OK;
} 

result_t gen_odata_id(const rest_uri_param_t *param, int8 *output, int32 len)
{
	int32 i = 0;
	int8 nodes[REST_MAX_NODE_LEN] = {0};

	if ((NULL == param) || (NULL == output) || (0 == len))
		return RESULT_NONE_POINTER;

	memset(output, 0, len);

	for (i = 0; i < param->num_nodes; i++) {
		memset(nodes, 0, REST_MAX_NODE_LEN);
		snprintf_s_s(nodes, REST_MAX_NODE_LEN, "/%s", param->nodes[i]);
		if ((strnlen_s(output, RSIZE_MAX_STR) + strnlen_s(nodes, sizeof(nodes)-1)) >= len)
			return RESULT_LEN_ERR;
		strncat_s(output, RSIZE_MAX_STR, nodes, sizeof(nodes)-1);
	}
	return RESULT_OK;
} 

result_t gen_odata_type(const rest_uri_param_t *param, int8 *output, int32 len, const int8 *name, bool redfish)
{
	if ((NULL == param) || (NULL == output) || (0 == len))
		return RESULT_NONE_POINTER;

	memset(output, 0, len);

	if (redfish == FALSE)
		snprintf_s_s(output, len, "#Intel.Oem.%s", (int8 *)name);
	else
		snprintf_s_ss(output, len, "#%s.1.0.0.%s", (int8 *)name, (int8 *)name);

	return RESULT_OK;
} 

void add_odata_context(json_t *result, const rest_uri_param_t *param)
{
	int8 context[REST_MAX_ODATA_LEN] = {0};

	if ((NULL == param) || (NULL == result))
		return;

	if (RESULT_OK == gen_odata_context(param, context, REST_MAX_ODATA_LEN))
		add_json_string(result, RMM_JSON_ODATA_CONTEXT, context);	
	return;	
}

void add_odata_id(json_t *result, const rest_uri_param_t *param)
{
	int8 id[REST_MAX_ODATA_LEN] = {0};

	if ((NULL == param) || (NULL == result))
		return;

	if (RESULT_OK == gen_odata_id(param, id, REST_MAX_ODATA_LEN))
		add_json_string(result, RMM_JSON_ODATA_ID, id);
	return;
}

void add_odata_type(json_t *result, const int8 *type)
{
	if ((NULL == type) || (NULL == result))
		return;

	add_json_string(result, RMM_JSON_ODATA_TYPE, type);
	return;
}

result_t pack_odata_head(json_t *result, const rest_uri_param_t *param, const int8 *name, bool redfish)
{
	int8 odata[REST_MAX_ODATA_LEN] = {0};

	if ((NULL == param) || (NULL == result))
		return RESULT_NONE_POINTER;

	if (RESULT_OK == gen_odata_context(param, odata, REST_MAX_ODATA_LEN))
		add_json_string(result, RMM_JSON_ODATA_CONTEXT, odata);
	if (RESULT_OK == gen_odata_id(param, odata, REST_MAX_ODATA_LEN))
		add_json_string(result, RMM_JSON_ODATA_ID, odata);
	if (RESULT_OK == gen_odata_type(param, odata, REST_MAX_ODATA_LEN, name, redfish))
		add_json_string(result, RMM_JSON_ODATA_TYPE, odata);

	return RESULT_OK;
}

result_t libwrap_pack_service_root_to_json(json_t *output, rest_uri_param_t *param)
{
	json_t *odata_id_chassis = NULL;
	json_t *odata_id_mgr = NULL;
	json_t *odata_id_evt = NULL;
	json_t *odata_id_mr = NULL;
	struct node_info *pnode = NULL;
	int32 nodenum = 0;
	int8 uuid[UUID_LEN] = {};

	if ((output == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}
	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_SVR_ROOT, TRUE);

	add_json_string(output, RMM_JSON_ID, "RootService");

	pnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_V1, &nodenum, NULL, LOCK_ID_NULL);
	if (pnode == NULL) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	if (get_db_info_string(DB_RMM, pnode->node_id, WRAP_UUID_STR, uuid, UUID_LEN) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	add_json_string(output, RMM_JSON_UUID, uuid);
	add_json_string(output, RMM_JSON_NAME, "Root Service");
	add_json_string(output, RMM_JSON_REDFISH_VERSION, "1.0.2");

	odata_id_chassis = json_object();
	if (odata_id_chassis != NULL) {
		add_json_string(odata_id_chassis, RMM_JSON_ODATA_ID, "/redfish/v1/Chassis");
	}

	json_object_add(output, "Chassis", odata_id_chassis);

	odata_id_mgr = json_object();
	if (odata_id_mgr != NULL) {
		add_json_string(odata_id_mgr, RMM_JSON_ODATA_ID, "/redfish/v1/Managers");
	}

	json_object_add(output, "Managers", odata_id_mgr);

	odata_id_evt = json_object();
	if (odata_id_evt != NULL) {
		add_json_string(odata_id_evt, RMM_JSON_ODATA_ID, "/redfish/v1/EventService");
	}

	json_object_add(output, "EventService", odata_id_evt);

	odata_id_mr = json_object();
	if (odata_id_mr != NULL) {
		add_json_string(odata_id_mr, RMM_JSON_ODATA_ID, "/redfish/v1/MessageRegistry");
	}

	json_object_add(output, "MessageRegistry", odata_id_mr);

	update_response_info(param, HTTP_OK);
	return RESULT_OK;
}

result_t libwrap_pack_chassis_to_json(json_t *output, rest_uri_param_t *param)
{
	json_t *members_arr = NULL;
	json_t *member = NULL;
	int32 nodenum = 0;
	int32 cm_lid = 0;
	int32 drawer_lid = 0;
	int32 i = 0;
	struct node_info *drawer_node = NULL;
	struct node_info *dz_node = NULL;
	int8 buf[128] = {0};
	int8 uuid[UUID_LEN] = {};
	unsigned int members_count = 0;
	memdb_integer rc = -1;

	if ((output == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_CHASSIS_COLL, TRUE);

	add_json_string(output, RMM_JSON_NAME, "Chassis Collection");

	drawer_node = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_DRAWER, &nodenum, NULL, LOCK_ID_NULL);

	members_arr = json_array();
	if (members_arr == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
        libdb_free_node(drawer_node);
		return RESULT_MALLOC_ERR;
	}

	member = json_object();
	if (member != NULL) {
		add_json_string(member, RMM_JSON_ODATA_ID, "/redfish/v1/Chassis/Rack");
		json_array_add(members_arr, member);
		members_count++;
	}

	if (drawer_node != NULL) {
		for (i = 0; i < nodenum; i++) {
			rc = libdb_attr_get_string(DB_RMM, drawer_node[i].node_id, DRAWER_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL);
			if (rc != 0 || strnlen_s(uuid, sizeof(uuid)) == 0) {
				continue;
			}

			libdb_attr_get_int(DB_RMM, drawer_node[i].node_id, DRAWER_LOC_ID_STR, &drawer_lid, LOCK_ID_NULL);
			dz_node = libdb_get_node_by_node_id(DB_RMM, drawer_node[i].parent, LOCK_ID_NULL);
			if (dz_node == NULL) {
				update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
				libdb_free_node(drawer_node);
				return RESULT_NO_NODE;
			}
			libdb_attr_get_int(DB_RMM, dz_node->parent, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);

			snprintf_s_i(buf, sizeof(buf), "/redfish/v1/Chassis/Drawer%d", drawer_lid + (cm_lid - 1) * MAX_DZONE_NUM * MAX_DRAWER_NUM);
			member = json_object();
			if (member != NULL) {
				add_json_string(member, RMM_JSON_ODATA_ID, buf);
				json_array_add(members_arr, member);
				members_count++;
			}
		}
	}
	json_object_add(output, RMM_JSON_MEMBERS, members_arr);
	add_json_integer(output, RMM_JSON_MEMBERS_COUNT, members_count);
	update_response_info(param, HTTP_OK);
	libdb_free_node(drawer_node);

	return RESULT_OK;
}

int32 prepare_get_action(const avail_action_t *av_action, json_t *action, int32 index)
{
	if (action == NULL) {
		return -1;
	}
	if (0 != strcmp((int8 *)(av_action[index].action), ""))
		add_json_string(action, RMM_JSON_ACTION, av_action[index].action);

	return 0;
}


int32 pack_fru_info(const fru_info_t *fru_info, json_t *fru)
{
	add_json_string(fru, RMM_JSON_SERIAL_NUM, fru_info->serial_num);
	add_json_string(fru, RMM_JSON_MANUFACTURE, fru_info->manufacture);
	add_json_string(fru, RMM_JSON_MODEL_NUM, fru_info->model);
	add_json_string(fru, RMM_JSON_PART_NUM, fru_info->part_num);

	return 0;
}

int32 pack_basic_element_info(const base_element_t *be, json_t *result)
{
	int8 id_str[32] = {};

	snprintf_s_i(id_str, sizeof(id_str), "%d", be->id);
	add_json_string(result, RMM_JSON_ID, id_str);
	add_json_string(result, RMM_JSON_NAME, be->name);
	add_json_string(result, RMM_JSON_DESC, be->desc);

	return 0;
}

int32 pack_oem_element_info(const base_element_t *be, json_t *result)
{
	json_t *loc;

	add_json_string(result, RMM_JSON_UUID, be->uuid);
	add_json_string(result, RMM_JSON_CREATED_DATE, be->create_date);
	add_json_string(result, RMM_JSON_UPDATED_DATE, be->update_date);

	return 0;
}

int32 pack_oem_loc_info(const location_t *loc_t, json_t *result)
{
	json_t *loc;

	loc = json_object();
	if (loc != NULL) {
		add_json_string(loc, RMM_JSON_RACK_UNITS, RMM_JSON_RACK_UNITS_OU);
		add_loc_info(loc, loc_t->x_location, RMM_JSON_XLOC);
		add_loc_info(loc, loc_t->u_location, RMM_JSON_ULOC);
		add_loc_info(loc, loc_t->u_height, RMM_JSON_UHEIGHT);
	}
	json_object_add(result, RMM_JSON_RACK_LOC, loc);

	return 0;
}

int32 json_add_link(json_t *result, const int8 *name, const int8 *url)
{
	json_t *obj_link = NULL;

	if ((NULL == result) || (NULL == name) || (NULL == url))
		return RESULT_NONE_POINTER;

	obj_link = json_object();
	if (obj_link == NULL)
		return RESULT_MALLOC_ERR;

	add_json_string(obj_link, RMM_JSON_ODATA_ID, url);
	json_object_add(result, name, obj_link);

	return RESULT_OK;
}

int32 json_add_single_member(json_t *result, const int8 *url, const int8 *element)
{
	json_t *member = NULL;
	int8 new_url[REST_MAX_ODATA_LEN] = {0};

	if ((NULL == result) || (NULL == url) || (NULL == element))
		return RESULT_NONE_POINTER;

	member = json_object();
	if (member != NULL) {
		snprintf_s_ss(new_url, REST_MAX_ODATA_LEN, "%s/%s", (int8*)url, (int8*)element);
		add_json_string(member, RMM_JSON_ODATA_ID, new_url);
	}

	json_object_add(result, element, member);

	return RESULT_OK;
}

int32 json_add_multi_member(json_t *result, int32 count, const int8 *members)
{
	json_t *members_arr = NULL;
	json_t *member = NULL;
	int32 i = 0;

	if (NULL == result)
		return RESULT_NONE_POINTER;

	add_json_integer(result, RMM_JSON_MEMBERS_ODATA_COUNT, count);

	members_arr = json_array();
	if (members_arr == NULL)
		return RESULT_MALLOC_ERR;

	if (NULL != members) {
		for (i = 0; i < count; i++) {
			member = NULL;
			member = json_object();
			if (member != NULL) {
				add_json_string(member, RMM_JSON_ODATA_ID, members + i * HREF_URL_LEN);
				json_array_add(members_arr, member);
			}
		}
	}

	json_object_add(result, RMM_JSON_MEMBERS, members_arr);

	return RESULT_OK;
}

result_t libwrap_get_node_id_by_uuid(int8 *uuid, memdb_integer *node_id)
{
	struct node_info *n = NULL;
	int32 node_num = 0;
	int32 i = 0;
	void *attr = NULL;
	struct attr_info *attr_info = NULL;
	int32 attr_info_offset = 0;
	int32 size = 0;
	int8 attr_name[256] = {0};
	int8 attr_data[256] = {0};

	n = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_DRAWER, &node_num, NULL, LOCK_ID_NULL);
	for (i = 0; i < node_num; i++) {
		attr = libdb_list_attrs_by_node(DB_RMM, n[i].node_id, &size, LOCK_ID_NULL);

		foreach_attr_info(attr_info, attr_info_offset, attr, size) {
			memset(attr_name, 0, 256);
			memset(attr_data, 0, 256);

			memcpy_s(attr_name, sizeof(attr_name), attr_info->elems, attr_info->data_offset);
			memcpy_s(attr_data, sizeof(attr_data), attr_info->elems + attr_info->data_offset,
				   attr_info->data_len);

			if (strcasecmp(uuid, attr_data) == 0) {
				*node_id = n[i].node_id;
				libdb_free_node(n);
				return 0;
			}
		}
	}

	libdb_free_node(n);
	return -1;
}

result_t check_module_capability(int8 *module, int8 *cap_name)
{
	int32 i = 0;
	int8 *cap= NULL;
	int32 cap_len = 0;
	int32 nodenum = 0;
	int8 query[MAX_NAME_LEN] = {0};
	int8 buff[256] = {0};
	int8 ip_str[64] = {0};
	memdb_integer module_node_id = 0;
	memdb_integer reg_node_id = 0;
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;

	if ((NULL == module) || (NULL == cap_name))
		return RESULT_LEN_ERR;

	cap_len = strnlen_s(cap_name, RSIZE_MAX_STR);

	subnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_REG, &nodenum, NULL, LOCK_ID_NULL);
	if (nodenum != 1) {
		return RESULT_NO_NODE;
	}
	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return RESULT_MALLOC_ERR;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * nodenum, subnode, sizeof(struct node_info) * nodenum);

	reg_node_id = sub_node[0].node_id;

	subnode = libdb_list_subnode_by_type(DB_RMM, reg_node_id, MC_TYPE_REG_MODULE, &nodenum, NULL, LOCK_ID_NULL);
	memset(sub_node, 0, CMDBUFSIZ);
	memcpy_s(sub_node, sizeof(struct node_info) * nodenum, subnode, sizeof(struct node_info) * nodenum);
	for (i = 0; i < nodenum; i++) {
		libdb_attr_get_string(DB_RMM, sub_node[i].node_id, JRPC_GAMI_REG_NAME, query, MAX_NAME_LEN, LOCK_ID_NULL);
		if (strcmp(query, JRPC_GAM_REG_ASSETMODULE) == 0)
			break;
	}

	if (nodenum == i) {
		free(sub_node);
		return RESULT_NO_NODE;
	} else
		module_node_id = sub_node[i].node_id;

	libdb_attr_get_string(DB_RMM, module_node_id, module, buff, 256, LOCK_ID_NULL);

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (i == 0)
			cap = buff;
		else
			cap = strstr(cap, GAMI_CAP_SPLIT_STR) + 1;

		if (cap == NULL) {
			free(sub_node);
			return RESULT_ATTR_ERR;
		}

		if (*cap == '\0') {
			free(sub_node);
			return RESULT_ATTR_ERR;
		}

		if (strncmp(cap, cap_name, cap_len) == 0 && strncmp(cap+cap_len, GAMI_CAP_SPLIT_STR, 1) == 0) {
			free(sub_node);
			return RESULT_OK;
		}
	}
	free(sub_node);
	return RESULT_OTHER_ERR;
}

result_t check_asset_module_capability(int8 *cap_name)
{
	int32 i = 0;
	int8 *cap= NULL;
	int32 cap_len = strnlen_s(cap_name, RSIZE_MAX_STR);
	int32 nodenum = 0;
	int8 query[MAX_NAME_LEN] = {0};
	int8 buff[256] = {0};
	int8 ip_str[64] = {0};
	memdb_integer module_node_id = 0;
	memdb_integer reg_node_id = 0;
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;

	subnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_REG, &nodenum, NULL, LOCK_ID_NULL);
	if (nodenum != 1) {
		return RESULT_NO_NODE;
	}
	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return RESULT_MALLOC_ERR;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * nodenum, subnode, sizeof(struct node_info) * nodenum);

	reg_node_id = sub_node[0].node_id;

	subnode = libdb_list_subnode_by_type(DB_RMM, reg_node_id, MC_TYPE_REG_MODULE, &nodenum, NULL, LOCK_ID_NULL);
	if (subnode == NULL) {
		free(sub_node);
		return RESULT_NO_NODE;
	}
	memset(sub_node, 0, CMDBUFSIZ);
	memcpy_s(sub_node, sizeof(struct node_info) * nodenum, subnode, sizeof(struct node_info) * nodenum);

	for (i = 0; i < nodenum; i++) {
		libdb_attr_get_string(DB_RMM, sub_node[i].node_id, JRPC_GAMI_REG_NAME, query, MAX_NAME_LEN, LOCK_ID_NULL);
		if (strcmp(query, JRPC_GAM_REG_ASSETMODULE) == 0)
			break;
	}

	if (nodenum == i) {
		free(sub_node);
		return RESULT_NO_NODE;
	} else
		module_node_id = sub_node[i].node_id;

	libdb_attr_get_string(DB_RMM, module_node_id, JRPC_GAMI_REG_CAP_MBP, buff, 256, LOCK_ID_NULL);

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (i == 0)
			cap = buff;
		else
			cap = strstr(cap, GAMI_CAP_SPLIT_STR) + 1;

		if (cap == NULL) {
			free(sub_node);
			return RESULT_ATTR_ERR;
		}

		if (*cap == '\0') {
			free(sub_node);
			return RESULT_ATTR_ERR;
		}

		if (strncmp(cap, cap_name, cap_len) == 0 &&
		    strncmp(cap+cap_len, GAMI_CAP_SPLIT_STR, 1) == 0) {
			free(sub_node);
			return RESULT_OK;
		}
	}

	free(sub_node);

	return RESULT_OTHER_ERR;
}

void add_loc_info(json_t *loc, int32 input, const int8 *name)
{
	json_t *rs_json = NULL;

	rs_json = json_integer((int64)input);
	if (rs_json)
		json_object_add(loc, name, rs_json);
}

int get_drawer_pres_from_cm(int8 *pres)
{
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int subnode_num = 0;
	int32 i,j;
	int32 cm_lid = 0;
	int32 drawer_pres = 0;;
	int32 tmp = 0;
	int32 dr_number = 0;
	struct node_info *dr_subnode = NULL;
	struct node_info *dr_sub_node = NULL;
	int dr_lid = 0;
	memdb_integer dz_node_id = 0;
	memdb_integer cm_node_id = 0;

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_DZONE, MC_TYPE_DZONE, &subnode_num, memdb_filter, LOCK_ID_NULL);

	if (subnode_num == 0) {
		libdb_free_node(subnode);
		return RESULT_NO_NODE;
	}

	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		libdb_free_node(subnode);
		return 0;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	for (i = 0; i < subnode_num; i++) {
		dz_node_id = sub_node[i].node_id;
		cm_node_id = sub_node[i].parent;
		dr_subnode = libdb_list_subnode_by_type(DB_RMM, dz_node_id, MC_TYPE_DRAWER, &dr_number, &memdb_filter, LOCK_ID_NULL);
		dr_sub_node = (struct node_info *)malloc(CMDBUFSIZ);
		if (dr_sub_node == NULL) {
			libdb_free_node(subnode);
			free(sub_node);
			return 0;
		}
		memcpy_s(dr_sub_node, sizeof(struct node_info) * dr_number, dr_subnode, sizeof(struct node_info) * dr_number);

		libdb_attr_get_int(DB_RMM, cm_node_id, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
		for (j = 0; j < dr_number; j++) {
			dr_lid = get_db_info_num(DB_RMM, dr_sub_node[j].node_id, WRAP_LOC_ID_STR);
			drawer_pres = (1 << (dr_lid - 1 + ((cm_lid - 1) * 4)))|drawer_pres;
			printf("cm[%d] dr_number[%d] dr_idx[%d] drawer_pres[%d], node_id[%lld]\n", cm_lid, dr_number, dr_lid, drawer_pres, dr_sub_node[j].node_id);
		}
		free(dr_sub_node);
	}

	for (i = 0; i < 8; i++) {
		strncat_s(pres, RSIZE_MAX_STR, ((0x80 == (0x80 & (drawer_pres << i)))? "1" : "0"), 1);
	}

	libdb_free_node(subnode);
	free(sub_node);

	return subnode_num;
}

memdb_integer get_subnode_id_by_lid(uint32 lid, memdb_integer parent, int32 type)
{
	int32 i, loc_id = 0;
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int32 subnode_num = 0;
	memdb_integer node_id = 0;

	subnode = libdb_list_subnode_by_type(DB_RMM, parent, type, &subnode_num, NULL, LOCK_ID_NULL);

	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return 0;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

	for (i = 0; i < subnode_num; i++) {
		loc_id = get_db_info_num(DB_RMM, sub_node[i].node_id, WRAP_LOC_ID_STR);
		if (loc_id == lid) {
			node_id = sub_node[i].node_id;
			break;
		}
	}

	libdb_free_node(sub_node);

	return node_id;
}

/**
 * @brief: check if the tftp service has been started, assume the port
 *         for the tftp is "69" which is for tftp by default.
 */
result_t libwrap_check_tftp_service(void)
{
	int32 status = 0;

	status = system("netstat  -nla | grep \":69\\>\" >/dev/null 2>/dev/null");
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return -1;
		}
	} else {
		return -1;
	}
	return RESULT_OK;
}

int32 get_asset_module_port(int32 *port)
{
	memdb_integer reg_node_id = 0;
	int32 self_ip = 0;
	int32 self_port = 0;
	int32 i = 0;
	int8 cap_name[GAMI_MAX_NAME_LEN] = {0};
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int32 node_num = 0;

	subnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_REG, &node_num, NULL, LOCK_ID_NULL);
	if (node_num != 1) {
		printf("invalid registry node num in memdb\n");
		return -1;
	}

	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return 0;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * node_num, subnode, sizeof(struct node_info) * node_num);

	reg_node_id = sub_node[0].node_id;
	subnode = libdb_list_subnode_by_type(DB_RMM, reg_node_id, MC_TYPE_REG_MODULE, &node_num, NULL, LOCK_ID_NULL);
	if (node_num == 0) {
		free(sub_node);
		printf("no asset module registry info found\n");
		return -1;
	}
	memset(sub_node, 0, CMDBUFSIZ);
	memcpy_s(sub_node, sizeof(struct node_info) * node_num, subnode, sizeof(struct node_info) * node_num);

	printf("reg_node is is %llu, sub node num is %d\n", reg_node_id, node_num);

	for (i = 0; i < node_num; i++) {
		libdb_attr_get_string(DB_RMM, sub_node[i].node_id, JRPC_GAMI_REG_NAME, cap_name, GAMI_MAX_NAME_LEN, LOCK_ID_NULL);
		printf("subnode cap name is %s\n", cap_name);
		if (strcmp(cap_name, PROC_ASSET_MODULE) == 0) {
			memdb_integer node_id = sub_node[i].node_id;
			libdb_free_node(sub_node);
			return libdb_attr_get_int(DB_RMM, node_id, JRPC_GAMI_REG_PORT, port, LOCK_ID_NULL);
		}
	}

	libdb_free_node(sub_node);
	return -1;
}

result_t libwrap_connect_asset_module(void)
{
	int32 port = 0;

	if (0 != get_asset_module_port(&port) || 0 == port || 0 != connect_asset_module(port))
		return RESULT_OTHER_ERR;

	return RESULT_OK;
}

int32 get_rf_evt_type(int8 *input)
{
	if (0 == strcmp(input, RF_EVENT_TYPE_ADD_STR))
		return MC_REDFISH_RESOURCE_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_DELETE_STR))
		return MC_REDFISH_RESOURCE_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_UPDATE_STR))
		return MC_REDFISH_RESOURCE_UPDATE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_CHANGE_STR))
		return MC_REDFISH_RESOURCE_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_ALERT_STR))
		return MC_REDFISH_RESOURCE_ALERT;
	else if (0 == strcmp(input, RF_EVENT_TYPE_RACK_CHANGE_STR))
		return MC_REDFISH_RACK_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_RACK_UPDATE_STR))
		return MC_REDFISH_RACK_UPDATE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_PZONE_ADD_STR))
		return MC_REDFISH_PZONE_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_PZONE_REMOVE_STR))
		return MC_REDFISH_PZONE_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_PSU_CHANGE_STR))
		return MC_REDFISH_PSU_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_PSU_ADD_STR))
		return MC_REDFISH_PSU_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_PSU_REMOVE_STR))
		return MC_REDFISH_PSU_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_TZONE_ADD_STR))
		return MC_REDFISH_TZONE_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_TZONE_REMOVE_STR))
		return MC_REDFISH_TZONE_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_TZONE_UPDATE_STR))
		return MC_REDFISH_TZONE_UPDATE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_FAN_ADD_STR))
		return MC_REDFISH_FAN_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_FAN_REMOVE_STR))
		return MC_REDFISH_FAN_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_FAN_CHANGE_STR))
		return MC_REDFISH_FAN_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_DRAWER_CHANGE_STR))
		return MC_REDFISH_DRAWER_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_DRAWER_ADD_STR))
		return MC_REDFISH_DRAWER_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_DRAWER_REMOVE_STR))
		return MC_REDFISH_DRAWER_REMOVE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_DRAWER_ALERT_STR))
		return MC_REDFISH_DRAWER_ALERT;
	else if (0 == strcmp(input, RF_EVENT_TYPE_MBP_CHANGE_STR))
		return MC_REDFISH_MBP_CHANGE;
	else if (0 == strcmp(input, RF_EVENT_TYPE_MBP_ADD_STR))
		return MC_REDFISH_MBP_ADD;
	else if (0 == strcmp(input, RF_EVENT_TYPE_MBP_REMOVE_STR))
		return MC_REDFISH_MBP_REMOVE;

	return -1;
}

memdb_integer libwrap_get_uniq_node_by_type(int32 type)
{
	int32 output = 0;
	struct node_info *subnode = NULL;
	int32 subnode_num = 0;
	memdb_integer node_id = 0;

	subnode = libdb_list_node_by_type(DB_RMM, type, type, &subnode_num, NULL, LOCK_ID_NULL);

	if (subnode_num != 1) {
		libdb_free_node(subnode);
		return 0;
	}

	node_id = subnode[0].node_id;
	libdb_free_node(subnode);

	return node_id;
}

memdb_integer libwrap_get_subnode_id_by_type(memdb_integer parent, int32 type)
{
	int32 output = 0;
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int32 subnode_num = 0;
	memdb_integer node_id = 0;

	subnode = libdb_list_subnode_by_type(DB_RMM, parent, type, &subnode_num, NULL, LOCK_ID_NULL);

	if (subnode_num != 1)
		return 0;

	sub_node = (struct node_info *)malloc(sizeof(struct node_info));
	if (sub_node == NULL) {
		return 0;
	}
	memcpy_s(sub_node, sizeof(struct node_info), subnode, sizeof(struct node_info));

	node_id = sub_node->node_id;
	libdb_free_node(sub_node);

	return node_id;
}

result_t libwrap_evt_subscribe(listener_dest_t *listener, memdb_integer listener_nid, memdb_integer evt_nid)
{
	int32 mask = 0;
	int32 rc = 0;

	if (listener_nid == 0) {
		listener_nid = libdb_create_node(DB_RMM, evt_nid, MC_REDFISH_LISTENER, SNAPSHOT_NEED, LOCK_ID_NULL);
		rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_DEST_STR, 0x0, listener->dest, SNAPSHOT_NEED, LOCK_ID_NULL);
		if (rc == -1) {
			return RESULT_ATTR_ERR;
		}
	}

	libdb_attr_get_int(DB_RMM, listener_nid, RF_EVENT_LISTENER_INDEXES_STR, &mask, LOCK_ID_NULL);
	mask |= listener->idx_mask;
	rc = libdb_attr_set_int(DB_RMM, listener_nid, RF_EVENT_LISTENER_INDEXES_STR, 0x0, mask, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_CONTEXT_STR, 0x0, listener->context, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_PROTOCOL_STR, 0x0, listener->protocol, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_NAME_STR, 0x0, listener->name, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_DESC_STR, 0x0, listener->description, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}

result_t libwrap_evt_unsubscribe(listener_dest_t *listener, memdb_integer listener_nid)
{
	int32 mask = 0;
	int32 rc = 0;

	libdb_attr_get_int(DB_RMM, listener_nid, RF_EVENT_LISTENER_INDEXES_STR, &mask, LOCK_ID_NULL);
	mask &= ~(listener->idx_mask);
	if (mask == 0) {
		libdb_destroy_node(DB_RMM, listener_nid, LOCK_ID_NULL);
	}

	rc = libdb_attr_set_int(DB_RMM, listener_nid, RF_EVENT_LISTENER_INDEXES_STR, 0x0, mask, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_CONTEXT_STR, 0x0, listener->context, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_PROTOCOL_STR, 0x0, listener->protocol, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_NAME_STR, 0x0, listener->name, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, listener_nid, RF_EVENT_LISTENER_DESC_STR, 0x0, listener->description, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}

static bool is_attr_name_valid(input_attr_t *input_attrs, uint32 size, int8 *name, json_t *value)
{
	uint32 i = 0;

	for(i = 0; i < size; i++) {
		if (strcmp(name, input_attrs[i].attr_name) == 0) {
			input_attrs[i].value = value;
			return true;
		}
	}
	return false;
}

json_t *libwrap_get_attr_json(input_attr_t *input_attrs, uint32 size, int8 *name)
{
	uint32 i = 0;

	for(i = 0; i < size; i++) {
		if (strcmp(name, input_attrs[i].attr_name) == 0) {
			return input_attrs[i].value;
		}
	}
	return NULL;
}

#define ATTR_NAME_LEN_MAX     (128)
#define ATTR_TIER_MAX         (5)
int32 libwrap_check_input_attrs(input_attr_t *input_attrs, uint32 size, json_t *req, int8 *prefix)
{
	if (req->type == JSON_OBJECT) {
		json_pair_t *pair = NULL;
		json_object_t *object = NULL;

		object = json_to_object(req);
		for (pair = object->next; pair != NULL; pair = pair->next) {
			int8 tmp_name[ATTR_NAME_LEN_MAX * ATTR_TIER_MAX] = {0};

			if (prefix)
				snprintf_s_ss(tmp_name, sizeof(tmp_name),
					 "%s.%s", prefix, pair->name);
			else
				snprintf_s_s(tmp_name, sizeof(tmp_name),
					 "%s", pair->name);

			printf("%s\n", tmp_name);
			if (!is_attr_name_valid(input_attrs, size, tmp_name, pair->value)) {
				printf("%s is not valid\n", tmp_name);
				return RESULT_JSON_ARR_ERR;
			}
			if (pair->value->type == JSON_OBJECT) {
				if (libwrap_check_input_attrs(input_attrs, size, pair->value, tmp_name) != RESULT_OK)
					return RESULT_JSON_ARR_ERR;
			}
		}
	} else {
		return RESULT_JSON_ARR_ERR;
	}

	return RESULT_OK;
}
