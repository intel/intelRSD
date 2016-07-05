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


#include <stdio.h>
#include "libjson/json.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/utils.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"

static void fill_pkg_hdr(jrpc_req_pkg_t *req_pkg, int64 *type_value, char *uuid, char *parent_uuid)
{
	req_pkg->num_of_params = 0;
	fill_param(req_pkg, JRPC_EVT_TYPE, type_value, JSON_INTEGER);
	fill_param(req_pkg, JRPC_MODULE_NAME, module_name[*type_value], JSON_STRING);
	fill_param(req_pkg, JRPC_UUID, uuid, JSON_STRING);
	if (parent_uuid)
		fill_param(req_pkg, JRPC_PARENT_UUID, parent_uuid, JSON_STRING);
}


int assetd_add(jrpc_req_pkg_t *req)
{
	return send_msg_to_assetd(req, ADD_EVT);
}

int assetd_remove(jrpc_req_pkg_t *req)
{
	return send_msg_to_assetd(req, REMOVE_EVT);
}

int assetd_update(jrpc_req_pkg_t *req)
{
	return send_msg_to_assetd(req, CHANGE_EVT);
}


int assetd_rmm_factory_rest(int32 * result)
{
	jrpc_req_pkg_t req_pkg = {};

	*result = send_msg_to_assetd(&req_pkg, RMM_FACTORY_RESET);

	return *result;
}

/*
int assetd_post_psu_by_node_id(int64 pzone_idx, int64 psu_idx, int64 request_enabled_state)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;
	
	fill_param(&req_pkg, JRPC_PZ_INDEX, &pzone_idx, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_INDEX, &psu_idx, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_ENABLE_STATE, &request_enabled_state, JSON_INTEGER);

	rc = send_msg_to_assetd(&req_pkg, POST_PSU_BY_NODE_ID);
	
	return rc;
}

int assetd_set_id_field(memdb_integer *node_id, int64 field_type, int64 field_instance, int64 byte_num, int32 *data)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;
	
	fill_param(&req_pkg, JRPC_NODE_ID, node_id, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FIELD_TYPE, &field_type, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FIELD_INSTANCE, &field_instance, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_BYTE_NUM, &byte_num, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_DATA, integer_array_to_json(data, byte_num), JSON_ARRAY);

	rc = send_msg_to_assetd(&req_pkg, SET_ID_FIELD);
	
	return rc;
}

int assetd_get_id_field(memdb_integer *node_id, int64 field_instance)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;
	
	fill_param(&req_pkg, JRPC_NODE_ID, node_id, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FIELD_INSTANCE, &field_instance, JSON_INTEGER);

	rc = send_msg_to_assetd(&req_pkg, GET_ID_FIELD);
	
	return rc;
}


int assetd_get_aggregate_thermal_sensor(memdb_integer *node_id)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;
	
	fill_param(&req_pkg, JRPC_NODE_ID, node_id, JSON_INTEGER);

	rc = send_msg_to_assetd(&req_pkg, GET_THERMAL_SENSOR);
	
	return rc;
}

int assetd_get_aggregate_health_sensor(memdb_integer *node_id)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;
	
	fill_param(&req_pkg, JRPC_NODE_ID, node_id, JSON_INTEGER);

	rc = send_msg_to_assetd(&req_pkg, GET_HEALTH_SENSOR);
	
	return rc;
}

int assetd_get_tray_power(memdb_integer *node_id)
{
	jrpc_req_pkg_t req_pkg = {};
	int rc;

	fill_param(&req_pkg, JRPC_NODE_ID, node_id, JSON_INTEGER);

	rc = send_msg_to_assetd(&req_pkg, GET_TRAY_POWER);

	return rc;
}*/

int assetd_tzone_add(int64 *tz_lid, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_TZ_LID, tz_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

int assetd_tzone_remove(char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}



static int64 get_tzone_u_location(int cm_lid)
{
	int64 u_loc = 0;

	if (1 == cm_lid)
		u_loc = 5;
	else if (2 == cm_lid)
		u_loc = 16;

	return u_loc;
}


int assetd_tzone_attr_init(char *uuid, int cm_lid, int64 *tz_lid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 tz_uloc = get_tzone_u_location(cm_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_TZ_LID, tz_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_TZ_ULOC, &tz_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}


int assetd_tzone_rpm_change(char *uuid, int32 rpm)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 rpm_send = rpm;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_TZ_RPM, &rpm_send, JSON_INTEGER);

	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_tzone_airflow_change(char *uuid, int32 q)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 q_send = q;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_TZ_AIRFLOW, &q_send, JSON_INTEGER);

	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_tzone_outlet_temp_change(char *uuid, int32 temp)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 temp_send = temp;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_TZ_OUTLET_TEMP, &temp_send, JSON_INTEGER);

	rc = assetd_update(&req_pkg);

	return rc;
}



int assetd_tzone_temp_change(char *uuid, int32 temp)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 intel_temp = temp;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_TZ_TEMP, &intel_temp, JSON_INTEGER);

	rc = assetd_update(&req_pkg);

	return rc;
}


int assetd_dzone_add(int64 *dz_lid, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DZ_TYPE;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_DZ_LID, dz_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

int assetd_dzone_attr_init(char *uuid, int64 *dz_lid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DZ_TYPE;
	int64 dz_uloc = *dz_lid;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_DZ_LID, dz_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_DZ_ULOC, &dz_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_cm_add(int64* cm_lid, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = CM_TYPE;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_CM_LID, cm_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

static int64 get_cm_u_location(int cm_lid)
{
	int64 u_loc = 5;

	if (cm_lid == 1)
		u_loc = 5;
	else if (cm_lid == 2)
		u_loc = 16;

	return u_loc;
}

int assetd_cm_attr_init(char *uuid, int64 *cm_lid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = CM_TYPE;
	int64 cm_uloc = get_cm_u_location(*cm_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_CM_LID, cm_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_CM_ULOC, &cm_uloc, JSON_INTEGER);

	rc = assetd_update(&req_pkg);
	return rc;
}

int assetd_cm_remove(char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = CM_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	return assetd_remove(&req_pkg);
}

int assetd_cm_ip_change(int64 *ip_addr, int64 *ver_high, int64 *ver_low, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = CM_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_IPADDRESS, ip_addr, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_VER_HIGH, ver_high, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_VER_LOW, ver_low, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_cm_ver_change(int64 *ip_addr, int64 *ver_high, int64 *ver_low, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = CM_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_IPADDRESS, ip_addr, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_VER_HIGH, ver_high, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_VER_LOW, ver_low, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_pzone_remove(char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PZ_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}

int assetd_pzone_add(int64 *pz_lid, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PZ_TYPE;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_PZ_LID, pz_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

static int64 get_pzone_u_location(int cm_lid)
{
	int64 u_loc = 0;

	if (1 == cm_lid)
		u_loc = 13;
	else if (2 == cm_lid)
		u_loc = 24;

	return u_loc;
}


int assetd_pzone_attr_init(char *uuid, int cm_lid, int64 *pz_lid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PZ_TYPE;
	int64 pz_uloc = get_pzone_u_location(cm_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PZ_LID, pz_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PZ_ULOC, &pz_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_psu_add(int pos, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;
	int64 psu_lid = pos + 1;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_PSU_LID, &psu_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

static int64 get_psu_u_location(int cm_lid)
{
	int64 u_loc = 13;

	if (cm_lid == 1)
		u_loc = 13;
	else if (cm_lid == 2)
		u_loc = 24;

	return u_loc;
}

int assetd_psu_attr_init(char *uuid, int cm_lid, int pos)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;
	int64 psu_lid = pos + 1;
	int64 psu_xloc = psu_lid;
	int64 psu_uLoc = get_psu_u_location(cm_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_LID, &psu_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_XLOC, &psu_xloc, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_ULOC, &psu_uLoc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_psu_remove(char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}

int assetd_psu_present(int *present, char *uuid)
{
	return 0;
}

int assetd_psu_power_in_change(int64 *pin, int64 *total, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_POWER_IN, pin, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_TOTAL_POWER_IN, total, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_psu_current_out_change(int64 *iout, int64 *total, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_CURRENT_OUT, iout, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_TOTAL_CURRENT_OUT, total, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_psu_power_out_change(int64 *pout, int64 *total, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_POWER_OUT, pout, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_PSU_TOTAL_POWER_OUT, total, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_psu_health_status_change(int64 *status, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_HEALTH_STATUS, status, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_psu_power_cap_change(int64 *cap, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_POWER_CAP, cap, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_psu_total_power_cap_change(int64 *cap, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PZ_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_TOTAL_POWER_CAP, cap, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_psu_serial_change(char *serial, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_SERIAL, serial, JSON_STRING);

	return assetd_update(&req_pkg);
}

int assetd_psu_manufacture_change(char *manufacture, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_MANUFACTURE, manufacture, JSON_STRING);

	return assetd_update(&req_pkg);
}

int assetd_psu_model_change(char *model, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_MODEL, model, JSON_STRING);

	return assetd_update(&req_pkg);
}

int assetd_psu_fw_ver_change(char *fw_ver, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = PSU_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PSU_FW_VER, fw_ver, JSON_STRING);

	return assetd_update(&req_pkg);
}

int assetd_fan_add(int pos, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = FAN_TYPE;
	int64 fan_lid = pos + 1;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_FAN_LID, &fan_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

static int64 get_fan_x_location(int fan_lid)
{
	int64 x_loc = 1;

	switch (fan_lid) {
	case 1:
	case 4:
		x_loc = 1;
		break;
	case 2:
	case 5:
		x_loc = 3;
		break;
	case 3:
	case 6:
		x_loc = 5;
		break;
	default:
		break;
	}

	return x_loc;
}

static int64 get_fan_u_location(int cm_lid, int fan_lid)
{
	int64 u_loc = 5;

	switch (fan_lid) {
	case 1:
	case 2:
	case 3:
		if (cm_lid == 1)
			u_loc = 5;
		else if (cm_lid == 2)
			u_loc = 16;
		break;
	case 4:
	case 5:
	case 6:
		if (cm_lid == 1)
			u_loc = 9;
		else if (cm_lid == 2)
			u_loc = 20;
		break;
	default:
		break;
	}

	return u_loc;
}

int assetd_fan_attr_init(char *uuid, int cm_lid, int pos)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = FAN_TYPE;
	int64 fan_lid = pos + 1;
	int64 fan_xloc = get_fan_x_location(fan_lid);
	int64 fan_uloc = get_fan_u_location(cm_lid, fan_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_FAN_LID, &fan_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FAN_XLOC, &fan_xloc, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_FAN_ULOC, &fan_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}

int assetd_fan_remove(char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = FAN_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}

int assetd_fan_present(int *present, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = TZ_TYPE;
	int64 present64 = *present;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PRESENT_INFO, &present64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_fans_speed_change(unsigned int *data, char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = FAN_TYPE;
	int64 data64 = *data;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_INFO, &data64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_drawer_add(int pos, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;
	int64 drawer_lid = pos + 1;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_DRAWER_LID, &drawer_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}

int assetd_sled_add(int pos, char *uuid, char *parent_uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 sled_lid = pos + 1;
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, parent_uuid);
	fill_param(&req_pkg, JRPC_SLED_LID, &sled_lid, JSON_INTEGER);
	rc = assetd_add(&req_pkg);

	return rc;
}


static int64 get_drawer_u_location(int cm_lid, int drawer_lid)
{
	int64 u_loc = 5;

	if (cm_lid == 1)
		u_loc = 5 + (drawer_lid - 1)*2;
	else if (cm_lid == 2)
		u_loc = 16 + (drawer_lid - 1)*2;

	return u_loc;
}


int assetd_drawer_attr_init(char *uuid, int cm_lid, int pos)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;
	int64 drawer_lid = pos + 1;
	int64 drawer_uloc = get_drawer_u_location(cm_lid, drawer_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_DRAWER_LID, &drawer_lid, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_DRAWER_ULOC, &drawer_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}


int assetd_drawer_remove(char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}

int assetd_sled_attr_init(char *uuid, int cm_lid, int pos)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 sled_lid = pos + 1;
//	int64 drawer_uloc = get_drawer_u_location(cm_lid, drawer_lid);
	int rc;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_LID, &sled_lid, JSON_INTEGER);
	//fill_param(&req_pkg, JRPC_DRAWER_ULOC, &drawer_uloc, JSON_INTEGER);
	rc = assetd_update(&req_pkg);

	return rc;
}


int assetd_sled_remove(char * uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);

	return assetd_remove(&req_pkg);
}


int assetd_drawer_alert_change(int *alert, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DZ_TYPE;
	int64 alert64 = *alert;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_DRAWER_ALERT, &alert64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_drawer_present(int *present, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DZ_TYPE;
	int64 present64 = *present;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PRESENT_INFO, &present64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_sled_present(int *present, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;
	int64 present64 = *present;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_PRESENT_INFO, &present64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_sled_type_change(uint8 *sled_type, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 sled_type64 = *sled_type;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_TYPE, &sled_type64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_sled_temp_change(uint8 *temp, char *uuid, uint8 *avg)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 temp64 = *temp;
	int64 avg64 = *avg;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_TEMP, &temp64, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_SLED_TEMP_AVG, &avg64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_tz_temp_change(uint8 *avg, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 avg64 = *avg;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_TEMP_AVG, &avg64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_sled_pwm_change(uint8 *pwm, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 pwm64 = *pwm;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_PWM, &pwm64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_sled_power_change(uint16 *power, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = SLED_TYPE;
	int64 power64 = *power;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SLED_POWER, &power64, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_drawer_temp_change(int64 *temp, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_INFO, temp, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_drawer_health_status_change(int64 *health, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_HEALTH_STATUS, health, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_drawer_ip_change(int64 *ip_addr, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_IPADDRESS, ip_addr, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_drawer_power_change(int64 *power, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_DRAWER_POWER, power, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


int assetd_drawer_ip_source_change(int64 *ip_src, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_IP_SOURCE, ip_src, JSON_INTEGER);

	return assetd_update(&req_pkg);
}

int assetd_drawer_subnet_mask_change(int64 *mask, char *uuid)
{
	jrpc_req_pkg_t req_pkg = {};
	int64 type = DRAWER_TYPE;

	fill_pkg_hdr(&req_pkg, &type, uuid, NULL);
	fill_param(&req_pkg, JRPC_SUBNET_MASK, mask, JSON_INTEGER);

	return assetd_update(&req_pkg);
}


