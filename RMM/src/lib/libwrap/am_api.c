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


#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "libwrap/wrap.h"
#include "libjsonrpc/jsonrpc.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libassetd/assetd_type.h"
#include "libassetmodule/am_action.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"


int fill_array_elem(json_t *array, char *attr_name, void *value_ptr, json_type type)
{
	json_t *elem = json_object();
	json_t *obj = json_object();

	switch(type) {
		case JSON_STRING:
			add_json_string(obj, JRPC_ACTION_VALUE_STR, value_ptr);
			break;
		case JSON_INTEGER:
			add_json_integer(obj, JRPC_ACTION_VALUE_NUM, *(int *)value_ptr);
			break;
		case JSON_TRUE:
			add_json_bool(obj, JRPC_ACTION_VALUE_BOOL, *(int *)value_ptr);
		default:
			return -1;
	}

	json_object_add(elem, JRPC_ACTION_ATTR_NAME, json_string(attr_name));
	json_object_add(elem, JRPC_ACTION_VALUE, obj);

	json_array_add(array, elem);
	return 0;
}

void fill_jrpc_action_pkg(jrpc_req_pkg_t *req_pkg, int action_id, json_t *attr)
{
	if (action_id < sizeof(am_action_map)/sizeof(am_action_map[0]))
		fill_param(req_pkg, JRPC_ACTION_NAME, am_action_map[action_id].action_name, JSON_STRING);
	fill_param(req_pkg, JRPC_ACTION_ATTR, attr, JSON_OBJECT);

	return;
}



int32 am_set_tzone_pwm(memdb_integer tzone_node_id, int64 pwm)
{
	int32 rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	int cm_lid = 0;
	int8 uuid[UUID_LEN] = {0};

	if (0 != get_db_info_string(DB_RMM, tzone_node_id, TZONE_UUID_STR, uuid, UUID_LEN))
		return -1;

	json_t *attr_array = json_array();
	fill_array_elem(attr_array, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_PWM, &pwm, JSON_INTEGER);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_SET_FAN_PWM, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}


result_t am_peripheral_hard_reset(int64 dz_idx, int64 peripheral_id, int32 * result)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	memdb_integer cm_node_id = 0;
	int8	uuid[UUID_LEN] = {0};
	json_t *attr_array = json_array();
	memdb_integer dzone_node_id = 0;
	memdb_integer drawer_node_id = 0;
	int32 dz_lid = 0;
	uint32 drawer_lid = 0;

	cm_node_id = get_subnode_id_by_lid((unsigned int)dz_idx, 0, MC_TYPE_CM);
	if (0 != get_db_info_string(DB_RMM, cm_node_id, WRAP_UUID_STR, uuid, UUID_LEN))
		return RESULT_ATTR_ERR;

	if (peripheral_id != 0) {
		dz_lid = get_zone_lid(MC_TYPE_DZONE, dz_idx);
		dzone_node_id = get_subnode_id_by_lid((unsigned int)dz_lid, cm_node_id, MC_TYPE_DZONE);
		drawer_lid = (peripheral_id + 1)/2;
		drawer_node_id = get_subnode_id_by_lid(drawer_lid, dzone_node_id, MC_TYPE_DRAWER);
		if (0 == drawer_node_id)
			return RESULT_NO_NODE;
	}

	fill_array_elem(attr_array, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_PERI_ID, &peripheral_id, JSON_INTEGER);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_RESET_HARD_RESET, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}

result_t am_psme_soft_reset(int64 dz_idx, int64 drawer_lid, int32 * result)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	memdb_integer cm_node_id = 0;
	int8	uuid[UUID_LEN] = {0};
	json_t *attr_array = json_array();
	memdb_integer dzone_node_id = 0;
	memdb_integer drawer_node_id = 0;
	int32 dz_lid = 0;

	cm_node_id = get_subnode_id_by_lid((unsigned int)dz_idx, 0, MC_TYPE_CM);
	if (0 != get_db_info_string(DB_RMM, cm_node_id, WRAP_UUID_STR, uuid, UUID_LEN))
		return RESULT_ATTR_ERR;

	dz_lid = get_zone_lid(MC_TYPE_DZONE, dz_idx);
	dzone_node_id = get_subnode_id_by_lid((unsigned int)dz_lid, cm_node_id, MC_TYPE_DZONE);
	drawer_node_id = get_subnode_id_by_lid((unsigned int)drawer_lid, dzone_node_id, MC_TYPE_DRAWER);
	if (0 == drawer_node_id)
		return RESULT_NO_NODE;

	//fill_param(&req_pkg, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_DRAWER_LID, &drawer_lid, JSON_INTEGER);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_PSME_SOFT_RESET, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}



result_t am_post_psu_by_node_id(int64 rack_pzone_idx, int64 psu_idx, int64 request_enabled_state)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	memdb_integer cm_node_id = 0;
	memdb_integer pzone_node_id = 0;
	memdb_integer psu_node_id = 0;
	int cm_lid = 0;
	int pzone_lid = 0;
	int8	uuid[UUID_LEN] = {0};
	json_t *attr_array = json_array();

	cm_lid = get_cm_lid(MC_TYPE_PZONE, rack_pzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, MC_NODE_ROOT, MC_TYPE_CM);
	pzone_lid = get_zone_lid(MC_TYPE_PZONE, rack_pzone_idx);
	pzone_node_id = get_subnode_id_by_lid(pzone_lid, cm_node_id, MC_TYPE_PZONE);
	psu_node_id = get_subnode_id_by_lid(psu_idx, pzone_node_id, MC_TYPE_PSU);

	if (0 == psu_node_id)
		return RESULT_NO_NODE;

	if (0 != get_db_info_string(DB_RMM, psu_node_id, WRAP_UUID_STR, uuid, UUID_LEN))
		return RESULT_ATTR_ERR;

	//fill_param(&req_pkg, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_PSU_ENABLE_STATE, &request_enabled_state, JSON_INTEGER);
	fill_array_elem(attr_array, JRPC_PZ_LID, &rack_pzone_idx, JSON_INTEGER);
	//fill_param(&req_pkg, JRPC_ACTION_ATTR, (void *)attr_array, JSON_OBJECT);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_POST_PSU_BY_NODE_ID, attr_array);

	rc = libdb_attr_set_int(DB_RMM, psu_node_id, PSU_ENABLE_ST_STR,
						  0x0, request_enabled_state, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
			rmm_log(ERROR, "memdb set psu enable state fail\n");
			return -1;
	}

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}

result_t am_set_rack_puid(int64 puid)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};

	json_t *attr_array = json_array();

	fill_array_elem(attr_array, JRPC_RACK_PUID, &puid, JSON_INTEGER);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_SET_RACK_PUID, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}

result_t am_set_rack_location_id(char* rack_location_id)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};

	json_t *attr_array = json_array();

	fill_array_elem(attr_array, JRPC_RACK_LOCATION_ID, rack_location_id, JSON_STRING);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_SET_RACK_LOCATION_ID, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}

int am_uart_switch(int64 cm_lid, int64 tray_lid, int64 tgt_lid)
{
	int rc = -1;
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	memdb_integer cm_node_id = 0;
	int8	uuid[UUID_LEN] = {0};
	json_t *attr_array = json_array();
	memdb_integer dzone_node_id = 0;
	memdb_integer drawer_node_id = 0;
	int32 dz_lid = MAX_DZONE_NUM;

	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);

	if (0 != get_db_info_string(DB_RMM, cm_node_id, WRAP_UUID_STR, uuid, UUID_LEN))
		return -1;

	dzone_node_id = get_subnode_id_by_lid((unsigned int)dz_lid, cm_node_id, MC_TYPE_DZONE);
	drawer_node_id = get_subnode_id_by_lid((unsigned int)tray_lid, dzone_node_id, MC_TYPE_DRAWER);
	if (0 == drawer_node_id)
		return RESULT_NO_NODE;

	fill_array_elem(attr_array, JRPC_UUID, (void *)uuid, JSON_STRING);
	fill_array_elem(attr_array, JRPC_TARGET_IDX, &tgt_lid, JSON_INTEGER);

	if (is_platform_bdcr())
		fill_array_elem(attr_array, JRPC_TRAY_IDX, &tray_lid, JSON_INTEGER);

	fill_jrpc_action_pkg(&req_pkg, AMACTION_MBP_UART_SWITCH, attr_array);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	return rc;
}
