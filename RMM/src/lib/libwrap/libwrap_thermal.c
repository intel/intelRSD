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


result_t get_tzone_node_id(int tzone_idx, memdb_integer *tzone_node_id)
{
	memdb_integer cm_node_id;
	int cm_lid = 0;
	int tzone_lid = 0;

	cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (cm_node_id == 0) {
		return RESULT_NO_NODE;
	}

	*tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
	if (*tzone_node_id == 0) {
		return RESULT_NO_NODE;
	}

	return RESULT_OK;
}

result_t get_fan_node_id_by_idx(int tzone_idx, int fan_idx, memdb_integer *node_id)
{
	memdb_integer cm_node_id;
	memdb_integer tzone_node_id;
	memdb_integer fan_node_id;
	int cm_lid = 0;
	int tzone_lid = 0;
	int fan_lid = fan_idx;

	cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (cm_node_id == 0) {
		return RESULT_NO_NODE;
	}

	tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
	if (tzone_node_id == 0) {
		return RESULT_NO_NODE;
	}

	fan_node_id = get_subnode_id_by_lid(fan_lid, tzone_node_id, MC_TYPE_FAN);
	if (fan_node_id == 0) {
		return RESULT_NO_NODE;
	}

	*node_id = fan_node_id;
	return RESULT_OK;
}


result_t libwrap_pack_tzone_coll_to_json(json_t *output, struct rest_uri_param *param)
{
	collections_t *tzone_collections = NULL;
	int8 *members = NULL;
	uint32 tzone_num = 0;
	result_t rs = RESULT_OK;
	int32 i;
	int32 zone_num = 0;

	if ((NULL == output) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	zone_num = libwrap_get_asset_num(MC_TYPE_TZONE);
	if (zone_num == 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		goto end;
	}

	tzone_collections = (collections_t *)malloc(zone_num * sizeof(collections_t));
	if (tzone_collections == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	memset(tzone_collections, 0, zone_num * sizeof(collections_t));

	members = (int8 *)malloc(zone_num * HREF_URL_LEN);
	if (members == NULL)
		return RESULT_NO_NODE;
	
	memset(members, 0, zone_num * HREF_URL_LEN);

	rs = libwrap_get_tzone_coll(tzone_collections, &tzone_num, param->host);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_GET_COLL_ERR;
	}

	for (i = 0; i < tzone_num; i++) {
		snprintf_s_si((int8 *)tzone_collections[i].href, HREF_URL_LEN, "%s/%d", (char *)(param->url), tzone_collections[i].id);
		memcpy_s(members + i * HREF_URL_LEN, HREF_URL_LEN, tzone_collections[i].href, HREF_URL_LEN);
	}
end:
	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_TZONE_COLL, FALSE);
	add_json_string(output, RMM_JSON_NAME, "ThermalZone Collection");
	json_add_multi_member(output, tzone_num, members);

	if (tzone_collections)
		free(tzone_collections);

	if (members)
		free(members);

	return RESULT_OK;
}

result_t libwrap_pack_tzone_to_json(json_t *output, tzone_member_t *tzone_member, struct rest_uri_param *param)
{
	json_t *loc = NULL;
	json_t *related_item = NULL;
	json_t *related_item_arry = NULL;
	json_t *pwm_arry = NULL;
	int32 presence_len = 0;
	json_t *av_action = NULL;
	json_t *speed = NULL;
	int8 speed_target[REST_MAX_ODATA_LEN] = {0};
	int8 speed_action[REST_MAX_ACTION_LEN] = {0};
	int8 id_str[8] = {0};
	json_t *status = NULL;
	json_t *fans = NULL;
	json_t *fan = NULL;
	json_t *temperature_array = NULL;
	json_t *inlet_temperature = NULL;
	json_t *outlet_temperature = NULL;
	int32 fan_tach = 0;

	if ((output == NULL) || (tzone_member == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_TZONE, FALSE);

	/* "@odata.type" */
	add_json_string(output, RMM_JSON_ODATA_TYPE, "http://rackscale.intel.com/schema#Intel.Oem.Thermal");

	/* "Id": "1" */
	snprintf_s_i(id_str, sizeof(id_str), "%d", tzone_member->be.id);
	add_json_string(output, RMM_JSON_ID, id_str);

	/* "Name": "TZone 1 on cm 1" */
	add_json_string(output, RMM_JSON_NAME, tzone_member->be.name);
	
	/* "Description": "thermalzone 1 on cm 1" */
	add_json_string(output, RMM_JSON_DESC, tzone_member->be.desc);


	/*
	 * "UUID": "9eec205b-31ba-41af-b523-9de420a6309f",
	 * "CreatedDate": "2016-01-06T05:55:04-05:00",
	 * "UpdatedDate": "2016-01-06T05:55:04-05:00",
	 * "RackLocation": {
	 *     "XLocation": 1,
	 *     "RackUnits": "OU",
	 *     "ULocation": 13,
	 *     "UHeight": 3
	 * } 
	 */
	pack_oem_element_info(&(tzone_member->be), output);
	pack_oem_loc_info(&(tzone_member->tzone_loc), output);

	add_json_string(output, RMM_JSON_PRESENCE, tzone_member->presence);

	add_json_integer(output, RMM_JSON_DESIRED_SPEED_PWM, tzone_member->desired_speed_pwm);
	add_json_integer(output, RMM_JSON_DESIRED_SPEED_RPM, tzone_member->desired_speed_rpm);

	add_json_integer(output, RMM_JSON_MAX_FANS_SUPPORTED, MAX_PWM_NUM);

	presence_len = strnlen_s(tzone_member->presence, sizeof(tzone_member->presence));
	if ((presence_len > -1) && (presence_len < 9)) {
		int32 i = 0;
		int32 number_fan = 0;
		for (i = 0; i < presence_len; i++) {
			if (tzone_member->presence[i] == 49)/*"1"*/
				number_fan++;
		}
		add_json_integer(output, RMM_JSON_NUM_OF_FANS_PRESENT, number_fan);
	}

	add_json_integer(output, RMM_JSON_AIRFLOW, tzone_member->volumetric_airflow);

	temperature_array = json_array();
	if (temperature_array != NULL) {
		int8 odata[REST_MAX_ODATA_LEN] = {0};
		int8 m_id = 0;
		int32 tzone_idx = tzone_member->be.id;
		int32 tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
		int8 attr_str[REST_RACK_STRING_LEN];

		/* inlet_temperature */
		inlet_temperature = json_object();
		if (inlet_temperature) {
			/* "@odata.id": "/redfish/v1/Chassis/Rack/ThermalZones/1/ThermalZone#/Temperatures/1", */
			snprintf_s_ii(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/ThermalZones/%d/ThermalZone#/Temperatures/%d", tzone_idx, m_id+1);
			add_json_string(inlet_temperature, RMM_JSON_ODATA_ID, odata);		
			m_id++;

			/* "Name": "Inlet Temperature" */
			add_json_string(inlet_temperature, RMM_JSON_NAME, "Inlet Temperature");

			/*
			 * "Status": {
			 *     "State": "Enabled",
			 *     "Health": "OK"
			 * }
			 */
			status = json_object();
			if (status != NULL) {
				add_json_string(status, RMM_JSON_FAN_ENABLE_STATE, RMM_JSON_STATE_ENABLED);
				add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
				json_object_add(inlet_temperature, RMM_JSON_HEALTH_STATUS, status);
			}

			add_json_integer(inlet_temperature, RMM_JSON_READING_CELSIUS, tzone_member->pres_temp);
			add_json_string(inlet_temperature, RMM_JSON_PHYSICAL_CONTEXT, "Intake");

			related_item_arry = json_array();
			if (related_item_arry != NULL) {
				related_item = json_object();
				if (related_item != NULL) {
					/* "@odata.id": "/redfish/v1/Chassis/Rack/ThermalZones/1", */
					snprintf_s_i(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/ThermalZones/%d", tzone_idx);
					add_json_string(related_item, RMM_JSON_ODATA_ID, odata);
				}
				json_array_add(related_item_arry, related_item);
			}
			json_object_add(inlet_temperature, RMM_JSON_RELATED_ITEM, related_item_arry);

			json_array_add(temperature_array, inlet_temperature);
		}

		/* outlet_temperature */
		outlet_temperature = json_object();
		if (outlet_temperature) {
			/* "@odata.id": "/redfish/v1/Chassis/Rack/ThermalZones/1/ThermalZone#/Temperatures/1", */
			snprintf_s_ii(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/ThermalZones/%d/ThermalZone#/Temperatures/%d", tzone_idx, m_id+1);
			add_json_string(outlet_temperature, RMM_JSON_ODATA_ID, odata);		
			m_id++;

			/* "Name": "Inlet Temperature" */
			add_json_string(outlet_temperature, RMM_JSON_NAME, "Outlet Temperature");

			/*
			 * "Status": {
			 *     "State": "Enabled",
			 *     "Health": "OK"
			 * }
			 */
			status = json_object();
			if (status != NULL) {
				add_json_string(status, RMM_JSON_FAN_ENABLE_STATE, RMM_JSON_STATE_DISABLED);
				add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
				json_object_add(outlet_temperature, RMM_JSON_HEALTH_STATUS, status);
			}

			add_json_integer(outlet_temperature, RMM_JSON_READING_CELSIUS, tzone_member->outlet_temp);
			add_json_string(outlet_temperature, RMM_JSON_PHYSICAL_CONTEXT, "Exhaust");

			related_item = json_object();
			if (related_item != NULL) {
				/* "@odata.id": "/redfish/v1/Chassis/Rack/ThermalZones/1", */
				snprintf_s_i(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/ThermalZones/%d", tzone_idx);
				add_json_string(related_item, RMM_JSON_ODATA_ID, odata);
			
				json_object_add(outlet_temperature, RMM_JSON_RELATED_ITEM, related_item);
			}

			json_array_add(temperature_array, outlet_temperature);
		}
	}
	json_object_add(output, RMM_JSON_TEMPERATURES, temperature_array);
	

	/*SetDesiredSpeedPWM Start*/
	av_action = json_object();
	if (av_action == NULL)
		return RESULT_MALLOC_ERR;

	speed = json_object();
	if (speed == NULL)
		return RESULT_MALLOC_ERR;

	snprintf_s_is(speed_target, REST_MAX_ODATA_LEN, 
		"/redfish/v1/Chassis/Rack/ThermalZones/%d/Actions/ThermalZone.%s", tzone_member->be.id, RMM_JSON_SET_DESIRED_SPEED_PWM);
	add_json_string(speed, RMM_JSON_TARGET, speed_target);
	pwm_arry = json_array();
	if (pwm_arry == NULL)
		return RESULT_MALLOC_ERR;
	json_object_add(speed, RMM_JSON_DESIRED_SPEED_PWM_VALUES, pwm_arry);

	snprintf_s_s(speed_action, REST_MAX_ACTION_LEN, "#ThermalZone.%s", RMM_JSON_SET_DESIRED_SPEED_PWM);
	json_object_add(av_action, speed_action, speed);

	json_object_add(output, RMM_JSON_POST_ACTIONS, av_action);
	/*SetDesiredSpeedPWM End*/


	/*
	 * "Status": {
	 *     "State": "Enabled",
	 *     "Health": "OK"
	 * }
	 */
	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_FAN_ENABLE_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
		json_object_add(output, RMM_JSON_HEALTH_STATUS, status);
	}

	fans = json_array();
	if (fans != NULL) {
		int32 i = 0;
		int8 odata[REST_MAX_ODATA_LEN] = {0};
		int8 name[REST_MAX_NAME_LEN] = {0};
		memdb_integer cm_node_id, tzone_node_id, fan_node_id;
		int32 tzone_idx = tzone_member->be.id;
		int32 cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
		int32 tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
		int8 attr_str[REST_RACK_STRING_LEN];
		json_t *loc = NULL;

		cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
		tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
		for (i = 0; i < MAX_PWM_NUM; i++) {
			fan_node_id = get_subnode_id_by_lid(i + 1, tzone_node_id, MC_TYPE_FAN);
			if (fan_node_id != INVALID_NODE_ID) {
				fan = json_object();
				if (fan) {
					/* "@odata.id": "/redfish/v1/Chassis/Rack/ThermalZones/1/Fans/0", */
					snprintf_s_ii(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/ThermalZones/%d/ThermalZone#/Fans/%d", tzone_idx, i+1);
					add_json_string(fan, RMM_JSON_ODATA_ID, odata);

					/* "Name": "Fan 1" */
					snprintf_s_i(name, sizeof(name), "Fan%d", i+1);
					add_json_string(fan, RMM_JSON_NAME, name);

					/*
					 * "Status": {
					 *     "State": "Enabled",
					 *     "Health": "OK"
					 * }
					 */
					status = json_object();
					if (status != NULL) {
						add_json_string(status, RMM_JSON_FAN_ENABLE_STATE, RMM_JSON_STATE_ENABLED);
						add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
						json_object_add(fan, RMM_JSON_HEALTH_STATUS, status);
					}

					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, fan_node_id, FAN_UUID_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(fan, RMM_JSON_UUID, attr_str);

					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, fan_node_id, FAN_CREATE_DATE_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(fan, RMM_JSON_CREATED_DATE, attr_str);

					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, fan_node_id, FAN_UPDATE_DATE_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(fan, RMM_JSON_UPDATED_DATE, attr_str);

					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, fan_node_id, FAN_TACH_READ_STR, attr_str, REST_RACK_STRING_LEN);
					fan_tach = str2int(attr_str);
					add_json_integer(fan, RMM_JSON_TACHOMETER_READING, fan_tach);

					loc = json_object();
					if (loc != NULL) {
						add_json_string(loc, RMM_JSON_RACK_UNITS, RMM_JSON_RACK_UNITS_OU);
						
						bzero(attr_str, sizeof(attr_str));
						get_db_info_string(DB_RMM, fan_node_id, U_LOC, attr_str, REST_RACK_STRING_LEN);
						add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_ULOC);

						bzero(attr_str, sizeof(attr_str));
						get_db_info_string(DB_RMM, fan_node_id, U_HEIGHT, attr_str, REST_RACK_STRING_LEN);
						add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_UHEIGHT);

						bzero(attr_str, sizeof(attr_str));
						get_db_info_string(DB_RMM, fan_node_id, X_LOC, attr_str, REST_RACK_STRING_LEN);
						add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_XLOC);

						json_object_add(fan, RMM_JSON_RACK_LOC, loc);
					}

					json_array_add(fans, fan);
				}
			}
		}
		json_object_add(output, RMM_JSON_FANS, fans);
	}

	return RESULT_OK;
}

static void get_fan_present_by_node_id(memdb_integer tzone_node_id,int fan_idx, struct fan_member *fan_member)
{
	unsigned int present = 0;
	int64 error_code = 0;
	int rc = -1;

	rc = get_present(tzone_node_id, &present);
	
	if ((error_code != 0)||(rc == -1)) {
		snprintf(fan_member->presence, REST_PRESENCE_LEN, "Not Present");
		return;
	}

	if ((present>>(fan_idx - 1))&1) {
		snprintf(fan_member->presence, REST_PRESENCE_LEN, "Present");
	} else {
		snprintf(fan_member->presence, REST_PRESENCE_LEN, "Not Present");
	}

	return;
}

result_t libwrap_pre_put_tzone_by_idx(int32 tzone_idx, put_tzone_t *put_tzone_info)
{
	memdb_integer cm_node_id = 0;
	memdb_integer tzone_node_id = 0;
	int64 error_code = 0;
	char output[128] = {0};
	int cm_lid = 0;
	int tzone_lid = 0;

	cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (cm_node_id == 0)
		return RESULT_NO_NODE;

	tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
	if (tzone_node_id == 0)
		return RESULT_NO_NODE;

	error_code = libdb_attr_get_string(DB_RMM, tzone_node_id, TZONE_DESCRIPT_STR, output, 128, LOCK_ID_NULL);
	if (error_code != 0) {
		return RESULT_ATTR_ERR;
	}
	strncpy_safe((char*)put_tzone_info->descr, output, DESCRIPTION_LEN, DESCRIPTION_LEN - 1);
	return RESULT_OK;
}

result_t libwrap_put_tzone_by_idx(int tzone_idx, const put_tzone_t put_tzone_info)
{
	memdb_integer cm_node_id = 0;
	memdb_integer tzone_node_id = 0;
	int cm_lid = 0;
	int tzone_lid = 0;
	int rc = 0;

	cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (cm_node_id == 0)
		return RESULT_NO_NODE;

	tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
	if (tzone_node_id == 0)
		return RESULT_NO_NODE;

	rc = libdb_attr_set_string(DB_RMM, tzone_node_id, TZONE_DESCRIPT_STR,
						  0x0, (char*)put_tzone_info.descr, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}
	return RESULT_OK;
}

static result_t get_fan_by_idx(uint32 tzone_idx, uint32 fan_idx, struct fan_member *fan_member)
{
	int cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	int cm_tzone_idx = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	int fan_lid = fan_idx;
	char buff[RMM_NAME_LEN] = {0};
	result_t rc;
	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer tzone_node_id = get_subnode_id_by_lid(cm_tzone_idx, cm_node_id, MC_TYPE_TZONE);
	memdb_integer fan_node_id = get_subnode_id_by_lid(fan_idx, tzone_node_id, MC_TYPE_FAN);

	if (0 == fan_node_id)
		return RESULT_NO_NODE;

	fan_member->be.id = get_db_info_num(DB_RMM, fan_node_id, FAN_LOC_ID_STR);

	get_db_info_string(DB_RMM, fan_node_id, FAN_UUID_STR,fan_member->be.uuid,UUID_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_NAME_STR,fan_member->be.name,RMM_NAME_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_DESCRIPT_STR,fan_member->be.desc,DESCRIPTION_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_CREATE_DATE_STR,fan_member->be.create_date,DATE_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_UPDATE_DATE_STR,fan_member->be.update_date,DATE_LEN);

	fan_member->enable_state  = get_db_info_num(DB_RMM, fan_node_id, FAN_ENABLE_STATE_STR);
	fan_member->health_state  = get_db_info_num(DB_RMM, fan_node_id, FAN_HEALTH_STATE_STR);

	fan_member->tachometer.reading = (uint32)get_db_info_num(DB_RMM, fan_node_id, FAN_TACH_READ_STR);
	fan_member->tachometer.threshold.lower_non_critical = get_db_info_num(DB_RMM, fan_node_id, FAN_TACH_THRESH_LNC_STR);
	fan_member->tachometer.threshold.lower_critical     = get_db_info_num(DB_RMM, fan_node_id, FAN_TACH_THRESH_LC_STR);  
	fan_member->tachometer.threshold.upper_non_critical = get_db_info_num(DB_RMM, fan_node_id, FAN_TACH_THRESH_UNC_STR);
	fan_member->tachometer.threshold.upper_critical     = get_db_info_num(DB_RMM, fan_node_id, FAN_TACH_THRESH_UC_STR);

	get_db_info_string(DB_RMM, fan_node_id, FAN_SER_NUM_STR, fan_member->asset.fru.serial_num, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_MANUFACT_STR, fan_member->asset.fru.manufacture, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_MODEL_STR, fan_member->asset.fru.model, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_PART_NUM_STR, fan_member->asset.fru.part_num, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_FW_VER_STR, fan_member->asset.fru.fw_ver, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, fan_node_id, FAN_ASSET_TAG_STR,fan_member->asset.asset_tag, REST_ASSET_TAG_LEN);

	get_fan_present_by_node_id(tzone_node_id, fan_idx, fan_member);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, fan_node_id, UNITS, buff, RMM_NAME_LEN);
	fan_member->fan_loc.units = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, fan_node_id, X_LOC, buff, RMM_NAME_LEN);
	fan_member->fan_loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, fan_node_id, U_LOC, buff, RMM_NAME_LEN);
	fan_member->fan_loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, fan_node_id, U_HEIGHT, buff, RMM_NAME_LEN);
	fan_member->fan_loc.u_height = (uint32)atoi((const char *)buff);

	return RESULT_OK;
}

result_t libwrap_get_tzone_fan_by_idx(uint32 tzone_idx, uint32 fan_idx, struct fan_member *fan_member)
{
	return get_fan_by_idx(tzone_idx, fan_idx, fan_member);
}


int32 libwrap_set_tzone_pwm(memdb_integer tz_node_id, int64 pwm)
{
	return am_set_tzone_pwm(tz_node_id, pwm);
}


result_t libwrap_post_fan_set_speed(uint32 tzone_idx, uint32 type, int64 speed)
{
	memdb_integer fan_node_id = 0;
	char buff[64] = {0};
	int rc;
	memdb_integer tz_node_id = 0;
	char speed_str[64] = {0};

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_FAN, RMM_JSON_SET_DESIRED_SPEED)) != RESULT_OK)
		return rc;

	if (RESULT_OK != get_tzone_node_id(tzone_idx, &tz_node_id))
		return RESULT_NO_NODE;
	else {
		int i;
		struct node_info *subnode = NULL;
		struct node_info *sub_node = NULL;
		int subnode_num = 0;

		subnode = libdb_list_subnode_by_type(DB_RMM, tz_node_id, MC_TYPE_FAN, &subnode_num, NULL, LOCK_ID_NULL);
		if (subnode == NULL)
			return RESULT_NO_NODE;
		sub_node = (struct node_info *)malloc(CMDBUFSIZ);
		if (sub_node == NULL) {
			return RESULT_MALLOC_ERR;
		}
		memcpy_s(sub_node, sizeof(struct node_info) * subnode_num, subnode, sizeof(struct node_info) * subnode_num);

		/*set desired speed for thermalzone*/
		if (type == 0) {/* unit is rpm */
			snprintf_s_ll(buff, sizeof(buff), "%lld", speed);
			rc = libdb_attr_set_string(DB_RMM, tz_node_id, FAN_DESIRED_SPD_RPM_STR,
						0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
			if (rc == -1)
				return RESULT_ATTR_ERR;
		} else {/* unit is pwm */
			snprintf_s_ll(buff, sizeof(buff), "%lld", speed);
			rc = libdb_attr_set_string(DB_RMM, tz_node_id, FAN_DESIRED_SPD_PWM_STR,
						0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
			if (rc == -1)
				return RESULT_ATTR_ERR;
			am_set_tzone_pwm(tz_node_id, speed);
		}

		for (i = 0; i < subnode_num; i++) {
			/* unit is rpm */
			if (type == 0) {
				snprintf_s_ll(buff, sizeof(buff), "%lld", speed);
				rc = libdb_attr_set_string(DB_RMM, sub_node[i].node_id, FAN_DESIRED_SPD_RPM_STR,
										  0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
				if (rc == -1) {
					return RESULT_ATTR_ERR;
				}
			} else {
				/* unit is pwm */
				snprintf_s_ll(buff, sizeof(buff), "%lld", speed);
				rc = libdb_attr_set_string(DB_RMM, sub_node[i].node_id, FAN_DESIRED_SPD_PWM_STR,
										  0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
				if (rc == -1) {
					return RESULT_ATTR_ERR;
				}

				if (speed == 0) {
					rc = libdb_attr_set_int(DB_RMM, sub_node[i].node_id, FAN_ENABLE_STATE_STR,
							  0x0, FAN_STATE_DISABLED, SNAPSHOT_NEED, LOCK_ID_NULL);
					if (rc == -1) {
						return RESULT_ATTR_ERR;
					}
				} else {
					rc = libdb_attr_set_int(DB_RMM, sub_node[i].node_id, FAN_ENABLE_STATE_STR,
							  0x0, FAN_STATE_ENABLED, SNAPSHOT_NEED, LOCK_ID_NULL);
					if (rc == -1) {
						return RESULT_ATTR_ERR;
					}

					snprintf_s_ll(speed_str, sizeof(speed_str), "%lld", speed);
					rc = libdb_attr_set_string(DB_RMM, sub_node[i].node_id, FAN_SPEED_PWM_RESTORE_STR,
										  0x0, speed_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
					if (rc == -1) {
						return RESULT_ATTR_ERR;
					}
				}
			}
		}
		libdb_free_node(sub_node);
	}
	return RESULT_OK;
}

static result_t fan_change_state(uint32 tzone_idx, int fan_idx, 
								memdb_integer fan_node_id,
								int state)
{
	int pwm = 0;
	char ppwm[128] = {0};
	int rc = 0;

	rc = libdb_attr_set_int(DB_RMM, fan_node_id, FAN_ENABLE_STATE_STR,
						  0x0, state, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	if (state == FAN_STATE_ENABLED) {
		libdb_attr_get_string(DB_RMM, fan_node_id, FAN_SPEED_PWM_RESTORE_STR, ppwm, 128, LOCK_ID_NULL);
		pwm = atoi(ppwm);
	}
	else if (state == FAN_STATE_DISABLED){
		pwm = 0;
	}

	return am_set_tzone_pwm(fan_node_id, pwm);//Fix me: not tzone pwm
}

result_t libwrap_post_fan_change_state(uint32 tzone_idx, uint32 fan_idx, int64 state)
{
	memdb_integer fan_node_id = 0;
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_FAN, RMM_JSON_REQ_STATE_CHANGE)) != RESULT_OK)
		return rc;

	get_fan_node_id_by_idx(tzone_idx, fan_idx, &fan_node_id);
	if (0 == fan_node_id)
		return RESULT_NO_NODE;

	return fan_change_state(tzone_idx, fan_idx, fan_node_id, state);
}


static result_t get_tzone_by_id(uint32 tzone_idx, struct tzone_member *tz_member)
{
	int cm_lid = get_cm_lid(MC_TYPE_TZONE, tzone_idx);
	int tzone_lid = get_zone_lid(MC_TYPE_TZONE, tzone_idx);
	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer tzone_node_id = get_subnode_id_by_lid(tzone_lid, cm_node_id, MC_TYPE_TZONE);
	int8 buff[RMM_NAME_LEN] = {0};

	if (0 == tzone_node_id)
		return RESULT_NO_NODE;

	tz_member->be.id = tzone_idx;
	get_db_info_string(DB_RMM, tzone_node_id, 
		TZONE_UUID_STR, tz_member->be.uuid, UUID_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, 
		TZONE_NAME_STR, tz_member->be.name, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, 
		TZONE_DESCRIPT_STR, tz_member->be.desc, DESCRIPTION_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, 
		TZONE_CREATE_DATE_STR, tz_member->be.create_date, DATE_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, 
		TZONE_UPDATE_DATE_STR, tz_member->be.update_date, DATE_LEN);
	get_db_info_string(DB_RMM, tzone_node_id,
		TZONE_POLICY_STR, tz_member->policy, DATE_LEN);

    tz_member->pres_temp = 
		get_db_info_num(DB_RMM, tzone_node_id, TZ_PRESENT_TEMP);//"cz_present_temperature"
    tz_member->outlet_temp = 
		get_db_info_num(DB_RMM, tzone_node_id, TZ_OUTLET_TEMP);//"cz_outlet_temperature"
    tz_member->volumetric_airflow = 
		get_db_info_num(DB_RMM, tzone_node_id, TZ_VOLUM_AIRFLOW);//"cz_volumetric_airflow"
    tz_member->max_fan_sup = 
		get_db_info_num(DB_RMM, tzone_node_id, TZ_MAX_FAN_SUP);//"cz_max_fan_support"
		
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, UNITS, buff, RMM_NAME_LEN);
	tz_member->tzone_loc.units = (uint32)atoi((const char *)buff);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, X_LOC, buff, RMM_NAME_LEN);
	tz_member->tzone_loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, U_LOC, buff, RMM_NAME_LEN);
	tz_member->tzone_loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, tzone_node_id, U_HEIGHT, buff, RMM_NAME_LEN);
	tz_member->tzone_loc.u_height = (uint32)atoi((const char *)buff);

	memset(tz_member->presence, 0, REST_PRESENCE_LEN);
	get_present_string(tzone_node_id, tz_member->presence, MAX_PWM_NUM, REST_PRESENCE_LEN);

	//tz_member->pres_temp = get_db_info_num(DB_RMM, tzone_node_id, JRPC_TZ_TEMP);
	tz_member->desired_speed_pwm = get_db_info_num(DB_RMM, tzone_node_id, FAN_DESIRED_SPD_PWM_STR);
	tz_member->desired_speed_rpm = get_db_info_num(DB_RMM, tzone_node_id, FAN_DESIRED_SPD_RPM_STR);
	return RESULT_OK;
}

result_t libwrap_get_tzone_by_idx(uint32 tzone_idx, struct tzone_member *tzone_member)
{
	return get_tzone_by_id(tzone_idx, tzone_member);
}

static result_t get_thermalzone_collection(collections_t* tzone, uint32 *number, int8 *host)
{
    struct node_info *subnode = NULL;
	int subnode_num = 0;
	int i;
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};
	int cm_lid = 0;
	int max_tz_num = 0;
	int tz_lid = 0;
	int32 tz_count = 0;

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_TZONE, MC_TYPE_TZONE, &subnode_num, memdb_filter, LOCK_ID_NULL);
	if (subnode_num == 0) {
		libdb_free_node(subnode);
		return RESULT_NO_NODE;
	}

	for (i = 0; i < subnode_num; i++) {
		cm_lid = get_db_info_num(DB_RMM, subnode[i].parent, MBP_LOC_ID_STR);
		max_tz_num = MAX_TZONE_NUM;
		tz_lid = get_db_info_num(DB_RMM, subnode[i].node_id, TZONE_LOC_ID_STR);
		tzone->id = tz_lid + (cm_lid - 1) * max_tz_num;
		if (tzone->id < 1) {
			tzone->id = 0;
			continue;
		}

		get_db_info_string(DB_RMM, subnode[i].node_id, TZONE_UUID_STR, tzone->uuid, UUID_LEN);
		if (strnlen_s(tzone->uuid, sizeof(tzone->uuid)) == 0)
			continue;

		get_db_info_string(DB_RMM, subnode[i].node_id, TZONE_NAME_STR, tzone->name, RMM_NAME_LEN);
		memset(prefix, 0, PREFIX_LEN);
		memset(new_link, 0, MAX_URL + 48);
		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		insert_str(new_link, "http://%s:%d/Rack/ThermalZoneCollection/%d/", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/
		snprintf_s_sii((char *)(tzone->href), DESCRIPTION_LEN, new_link,
					host, rmm_cfg_get_port(RESTD_PORT), tzone->id);
		tzone++;
		tz_count += 1;
	}

	*number = tz_count;
	libdb_free_node(subnode);

	return RESULT_OK;
}

result_t libwrap_get_tzone_coll(collections_t* tzone, uint32 *number, int8 *host)
{
	return get_thermalzone_collection(tzone, number, host);
}

static input_attr_t patch_tz_attrs[] = {
	{"Description",         NULL}
};

result_t libwrap_update_put_tzone_info(json_t *req, put_tzone_t *put_info)
{
	result_t rs = RESULT_OK;
	json_t *obj = NULL;
	int32  i = 0;
	uint32 ary_size = sizeof(patch_tz_attrs)/sizeof(input_attr_t);

	if ((req == NULL) || (put_info == NULL))
		return RESULT_NONE_POINTER;

	for (i = 0; i < ary_size; i++)
		patch_tz_attrs[i].value = NULL;

	if (libwrap_check_input_attrs(patch_tz_attrs, ary_size, req, NULL) != RESULT_OK)
		return RESULT_JSON_ARR_ERR;

	obj = libwrap_get_attr_json(patch_tz_attrs, ary_size, "Description");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_DESC_LEN)) {
			strncpy_safe(put_info->descr, input, REST_DESC_LEN, REST_DESC_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	return RESULT_OK;
}
