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

result_t libwrap_pack_pzone_coll_to_json(json_t *output, struct rest_uri_param *param)
{
	collections_t *pzone_collections = NULL;
	char *members = NULL;
	uint32 pzone_num = 0;
	result_t rs = RESULT_OK;
	int32 i;
	int32 zone_num = 0;

	if ((output == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	zone_num = libwrap_get_asset_num(MC_TYPE_PZONE);
	if (zone_num == 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		goto end;
	}

	pzone_collections = (collections_t *)malloc(zone_num * sizeof(collections_t));
	if (pzone_collections == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_NO_NODE;
	}
	memset(pzone_collections, 0, zone_num * sizeof(collections_t));

	members = (char *)malloc(zone_num * HREF_URL_LEN);
	if (members == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_NO_NODE;
	}
	
	memset(members, 0, zone_num * HREF_URL_LEN);

	rs = libwrap_get_pzone_coll(pzone_collections, &pzone_num, param->host);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_GET_COLL_ERR;
	}

	for (i = 0; i < pzone_num; i++) {
		snprintf_s_si((int8 *)pzone_collections[i].href, HREF_URL_LEN, "%s/%d", (char *)(param->url), pzone_collections[i].id);
		memcpy_s(members + i * HREF_URL_LEN, HREF_URL_LEN, pzone_collections[i].href, HREF_URL_LEN);
	}
end:
	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_PZONE_COLL, FALSE);
	add_json_string(output, RMM_JSON_NAME, "PowerZone Collection");
	json_add_multi_member(output, pzone_num, members);

	if (pzone_collections)
		free(pzone_collections);

	if (members)
		free(members);

	return RESULT_OK;
}

result_t libwrap_pack_pzone_to_json(json_t *output, pzone_member_t *pzone_member, struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *actions = NULL;
	json_t *status = NULL;
	json_t *power_supplies = NULL;
	json_t *target_index = NULL;
	json_t *power_supply = NULL;
	json_t *loc = NULL;
	int8 id_str[8] = {0};
	json_t *act_obj = NULL;
	json_t *act_type = NULL;
	int8 attr_str[REST_RACK_STRING_LEN];

	if ((output == NULL) || (pzone_member == NULL) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	add_odata_context(output, param);
	add_odata_id(output, param);
	add_odata_type(output, "http://rackscale.intel.com/schema#Intel.Oem.PowerZone");

	int32 presence_len = 0;

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
	pack_oem_element_info(&(pzone_member->be), output);
	pack_oem_loc_info(&(pzone_member->power_zone_loc), output);

	/* "MaxPSUsSupported": 6 */
	add_json_integer(output, RMM_JSON_MAX_PSUS_SUPPORT, MAX_PSU_NUM);

	/* "NumberOfPSUsPresent": 4 */
	presence_len = strnlen_s(pzone_member->presence, sizeof(pzone_member->presence)-1);
	if ((presence_len > -1) && (presence_len < 9)) {
		int32 i = 0;
		int32 number_psu = 0;
		for (i = 0; i < presence_len; i++) {
			if (pzone_member->presence[i] == 49)//"1"
				number_psu++;
		}
		add_json_integer(output, RMM_JSON_PSU_NUMBER_OF_PRESENT, number_psu);
	}
	add_json_string(output, RMM_JSON_PRESENCE, pzone_member->presence);

	/* "PowerConsumedWatts": 256 */
	add_json_integer(output, RMM_JSON_POWER_CONSUMED, pzone_member->tot_power_consumption);

	/* "PowerOutputWatts": 320 */
	add_json_integer(output, RMM_JSON_POWER_OUTPUT, pzone_member->tot_power_production);

	/* "PowerCapacityWatts": 10112 */
	add_json_integer(output, RMM_JSON_POWER_CAP, pzone_member->tot_power_cap);


	/* "Id": "1" */
	snprintf_s_i(id_str, sizeof(id_str), "%d", pzone_member->be.id);
	add_json_string(output, RMM_JSON_ID, id_str);

	/* "Description": "powerzone 1 on cm 1" */
	add_json_string(output, RMM_JSON_DESC, pzone_member->be.desc);

	/* "Name": "pzone 1 on cm 1" */
	add_json_string(output, RMM_JSON_NAME, pzone_member->be.name);


	/*
	 * "Status": {
	 *     "State": "Enabled",
	 *     "Health": "OK"
	 * }
	 */
	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_PSU_ENABLE_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
		json_object_add(output, RMM_JSON_HEALTH_STATUS, status);
	}

	actions = json_object();
	if (actions != NULL) {
		act_obj = json_object();
		if (act_obj != NULL) {
			bzero(attr_str, sizeof(attr_str));
			snprintf_s_i((char *)attr_str, REST_MAX_ODATA_LEN, 
					"/redfish/v1/Chassis/Rack/PowerZones/%d/Actions/PowerZone.RequestStateChange", pzone_member->be.id);
			add_json_string(act_obj, RMM_JSON_TARGET, attr_str);

			target_index = json_array();
			if (target_index != NULL) {
				int32 i = 0;
				memdb_integer cm_node_id, pzone_node_id, psu_node_id;
				int32 pzone_idx = pzone_member->be.id;
				int32 cm_lid = get_cm_lid(MC_TYPE_PZONE, pzone_idx);
				int32 pzone_lid = get_zone_lid(MC_TYPE_PZONE, pzone_idx);

				cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
				pzone_node_id = get_subnode_id_by_lid(pzone_lid, cm_node_id, MC_TYPE_PZONE);
				for (i = 0; i < MAX_PSU_NUM; i++) {
					psu_node_id = get_subnode_id_by_lid(i + 1, pzone_node_id, MC_TYPE_PSU);
					if (psu_node_id != INVALID_NODE_ID) {
						json_array_add(target_index, json_integer(i+1));
					}
				}
				bzero(attr_str, sizeof(attr_str));
				snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_TARGET_INDEX, RMM_JSON_ALLOWABLE_VALUES);
				json_object_add(act_obj, (char *)attr_str, target_index);
			}

			act_type = json_array();
			if (act_type != NULL) {
				json_array_add(act_type, json_string(RMM_JSON_STATE_ENABLED));
				json_array_add(act_type, json_string(RMM_JSON_STATE_DISABLED));

				bzero(attr_str, sizeof(attr_str));
				snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_STATE, RMM_JSON_ALLOWABLE_VALUES);
				json_object_add(act_obj, (char *)attr_str, act_type);
			}

			bzero(attr_str, sizeof(attr_str));
			snprintf_s_s((char *)attr_str, REST_MAX_ACTION_LEN, "#PowerZone.%s", RMM_JSON_REQ_STATE_CHANGE);
			json_object_add(actions, (char *)attr_str, act_obj);
		}
		json_object_add(output, RMM_JSON_POST_ACTIONS, actions); 
	}

	power_supplies = json_array();
	if (power_supplies != NULL) {
		int32 i = 0;
		int8 odata[REST_MAX_ODATA_LEN] = {0};
		int8 name[REST_MAX_NAME_LEN] = {0};
		memdb_integer cm_node_id, pzone_node_id, psu_node_id;
		int32 pzone_idx = pzone_member->be.id;
		int32 cm_lid = get_cm_lid(MC_TYPE_PZONE, pzone_idx);
		int32 pzone_lid = get_zone_lid(MC_TYPE_PZONE, pzone_idx);
		int8 attr_str[REST_RACK_STRING_LEN];
		json_t *loc = NULL;
		json_t *act = NULL;
		int32 enable_state = 0;
		int32 health_state = 0;

		cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
		pzone_node_id = get_subnode_id_by_lid(pzone_lid, cm_node_id, MC_TYPE_PZONE);
		for (i = 0; i < MAX_PSU_NUM; i++) {
			psu_node_id = get_subnode_id_by_lid(i + 1, pzone_node_id, MC_TYPE_PSU);
			if (psu_node_id != INVALID_NODE_ID) {
				power_supply = json_object();
				if (power_supply) {
					/* "@odata.id": "/redfish/v1/Chassis/Rack/PowerZones/1/Power#/PowerSupplies/0" */
					snprintf_s_ii(odata, sizeof(odata), "/redfish/v1/Chassis/Rack/PowerZones/%d/PowerZone#/PowerSupplies/%d", pzone_idx, i+1);
					add_json_string(power_supply, RMM_JSON_ODATA_ID, odata);

					/* "Name": "Power Supply 1" */
					snprintf_s_i(name, sizeof(name), "Power Supply %d", i+1);
					add_json_string(power_supply, RMM_JSON_NAME, name);

					/*
					 * "Status": {
					 *     "State": "Enabled",
					 *     "Health": "OK"
					 * }
					 */
					status = json_object();
					if (status != NULL) {
						enable_state = (int32)get_db_info_num(DB_RMM, psu_node_id, PSU_ENABLE_ST_STR);
						health_state = (int32)get_db_info_num(DB_RMM, psu_node_id, PSU_HEALTH_ST_STR);

						if (PSU_STATE_ENABLED == enable_state)
							add_json_string(status, RMM_JSON_PSU_ENABLE_STATE, RMM_JSON_STATE_ENABLED);
						else if (PSU_STATE_DISABLED == enable_state)
							add_json_string(status, RMM_JSON_PSU_ENABLE_STATE, RMM_JSON_STATE_DISABLED);
						else if (PSU_STATE_NOT_APP == enable_state)
							add_json_string(status, RMM_JSON_PSU_ENABLE_STATE, RMM_JSON_STATE_NOT_APP);
						else if (PSU_STATE_EN_OFFLINE == enable_state)
							add_json_string(status, RMM_JSON_PSU_ENABLE_STATE, RMM_JSON_STATE_OFFLINE);

						if (RMM_JSON_HEALTH_CODE_CLEAR == (health_state & RMM_JSON_HEALTH_MASK))
							add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_HEALTH_STR);
						else
							add_json_string(status, RMM_JSON_HEALTH_CODE, RMM_JSON_HEALTH_CODE_WARNING_STR);

						json_object_add(power_supply, RMM_JSON_HEALTH_STATUS, status);
					}

						/* "@odata.type" */
						add_json_string(power_supply, RMM_JSON_ODATA_TYPE, "http://rackscale.intel.com/schema#Intel.Oem.PowerSupply");

						bzero(attr_str, sizeof(attr_str));
						get_db_info_string(DB_RMM, psu_node_id, PSU_UUID_STR, attr_str, REST_RACK_STRING_LEN);
						add_json_string(power_supply, RMM_JSON_UUID, attr_str);

						loc = json_object();
						if (loc != NULL) {
							add_json_string(loc, RMM_JSON_RACK_UNITS, RMM_JSON_RACK_UNITS_OU);
							
							bzero(attr_str, sizeof(attr_str));
							get_db_info_string(DB_RMM, psu_node_id, U_LOC, attr_str, REST_RACK_STRING_LEN);
							add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_ULOC);

							bzero(attr_str, sizeof(attr_str));
							get_db_info_string(DB_RMM, psu_node_id, U_HEIGHT, attr_str, REST_RACK_STRING_LEN);
							add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_UHEIGHT);

							bzero(attr_str, sizeof(attr_str));
							get_db_info_string(DB_RMM, psu_node_id, X_LOC, attr_str, REST_RACK_STRING_LEN);
							add_loc_info(loc, (uint32)atoi((const char *)attr_str), RMM_JSON_XLOC);

							json_object_add(power_supply, RMM_JSON_RACK_LOC, loc);
						}


					/* "SerialNumber": "HSW142900990W0E" */
					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, psu_node_id, PSU_SER_NUM_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(power_supply, RMM_JSON_SERIAL_NUM, attr_str);

					/* "Manufacturer": "DELTA" */
					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, psu_node_id, PSU_MANUFACT_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(power_supply, RMM_JSON_MANUFACTURE, attr_str);	

					/* "Model": "DPST-2400CB A" */
					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, psu_node_id, PSU_MODEL_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(power_supply, RMM_JSON_MODEL, attr_str);	

					/* "PartNumber": "123" */
					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, psu_node_id, PSU_PART_NUM_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(power_supply, RMM_JSON_PART_NUM, attr_str);	

					/* "FirmwareVersion": "S4" */
					bzero(attr_str, sizeof(attr_str));
					get_db_info_string(DB_RMM, psu_node_id, PSU_FW_VER_STR, attr_str, REST_RACK_STRING_LEN);
					add_json_string(power_supply, RMM_JSON_FIRMWARE_VER, attr_str);	
		
					/* "PowerCapacityWatts": 2528 */
					bzero(attr_str, sizeof(attr_str));
					add_json_integer(power_supply, RMM_JSON_POWER_CAP, (int32)get_db_info_num(DB_RMM, psu_node_id, PSU_TT_PWR_CAP_STR));

					add_json_integer(power_supply, RMM_JSON_LAST_POWER_OUTPUT, (int32)get_db_info_num(DB_RMM, psu_node_id, PSU_TT_PWR_OUT_STR));

					json_array_add(power_supplies, power_supply);
				}
			}
		}
		json_object_add(output, RMM_JSON_POWERSUPPLIES, power_supplies);
	}

	return RESULT_OK;
}


static input_attr_t patch_pz_attrs[] = {
};


result_t libwrap_update_put_pzone_info(json_t* req, put_pzone_t* put_info) {
	int32 i = 0;
	uint32 ary_size = sizeof(patch_pz_attrs) / sizeof(input_attr_t);

	if ((req == NULL) || (put_info == NULL)) {
		return RESULT_NONE_POINTER;
	}

	for (i = 0; i < ary_size; i++) {
		patch_pz_attrs[i].value = NULL;
	}

	if (libwrap_check_input_attrs(patch_pz_attrs, ary_size, req, NULL) != RESULT_OK) {
		return RESULT_JSON_ARR_ERR;
	}

	return RESULT_OK;
}

static result_t get_pzone_by_idx(uint32 pzone_idx, struct pzone_member *pzone_member)
{
	memdb_integer cm_node_id = 0;
	memdb_integer pzone_node_id = 0;
	char buff[RMM_NAME_LEN] = {0};
	int cm_lid = get_cm_lid(MC_TYPE_PZONE, pzone_idx);
	int pzone_lid = get_zone_lid(MC_TYPE_TZONE, pzone_idx);

	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (0 == cm_node_id)
		return RESULT_NO_NODE;

	pzone_node_id = get_subnode_id_by_lid(pzone_lid,cm_node_id,MC_TYPE_PZONE);

	pzone_member->be.id = pzone_idx;
	get_db_info_string(DB_RMM, pzone_node_id,
		PZONE_UUID_STR, pzone_member->be.uuid, UUID_LEN);
	get_db_info_string(DB_RMM, pzone_node_id,
		PZONE_NAME_STR, pzone_member->be.name, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, pzone_node_id,
		PZONE_DESCRIPT_STR, pzone_member->be.desc, DESCRIPTION_LEN);
	get_db_info_string(DB_RMM, pzone_node_id,
		PZONE_CREATE_DATE_STR, pzone_member->be.create_date, DATE_LEN);
	get_db_info_string(DB_RMM, pzone_node_id,
		PZONE_UPDATE_DATE_STR, pzone_member->be.update_date, DATE_LEN);
	pzone_member->tot_power_consumption =
		(uint32)get_db_info_num(DB_RMM, pzone_node_id, PZONE_TT_PWR_CONSUM_STR);
	pzone_member->tot_power_cap =
		(uint32)get_db_info_num(DB_RMM, pzone_node_id, PZONE_TT_PWR_CAP_STR);
	pzone_member->tot_power_production =
		(uint32)get_db_info_num(DB_RMM, pzone_node_id, PZONE_TT_PWR_PROD_STR);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, pzone_node_id, UNITS, buff, RMM_NAME_LEN);
	pzone_member->power_zone_loc.units = (uint32)atoi((const char *)buff);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, pzone_node_id, X_LOC, buff, RMM_NAME_LEN);
	pzone_member->power_zone_loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, pzone_node_id, U_LOC, buff, RMM_NAME_LEN);
	pzone_member->power_zone_loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, pzone_node_id, U_HEIGHT, buff, RMM_NAME_LEN);
	pzone_member->power_zone_loc.u_height = (uint32)atoi((const char *)buff);

	memset(pzone_member->presence, 0, REST_PRESENCE_LEN);
	get_present_string(pzone_node_id, pzone_member->presence, MAX_PSU_NUM, REST_PRESENCE_LEN);

	return RESULT_OK;
}

result_t libwrap_get_pzone_by_idx(uint32 pzone_idx, struct pzone_member *pzone_member)
{
	return get_pzone_by_idx(pzone_idx, pzone_member);
}

result_t libwrap_pre_put_pzone_by_idx(int pzone_idx, put_pzone_t *put_pzone_info)
{
	int cm_lid = get_cm_lid(MC_TYPE_PZONE, pzone_idx);
	int pzone_lid = get_zone_lid(MC_TYPE_PZONE, pzone_idx);
	int64 error_code = 0;
	char output[128] = {0};
	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer pzone_node_id = get_subnode_id_by_lid(pzone_lid, cm_node_id, MC_TYPE_PZONE);

	if (0 == pzone_node_id)
		return RESULT_NO_NODE;

	error_code = libdb_attr_get_string(DB_RMM, pzone_node_id, PZONE_DESCRIPT_STR, output, sizeof(output), LOCK_ID_NULL);
	if (error_code != 0) {
		return RESULT_ATTR_ERR;
	}
	strncpy_safe((char*)put_pzone_info->descr, output, DESCRIPTION_LEN, DESCRIPTION_LEN - 1);

	return RESULT_OK;
}

result_t libwrap_put_pzone_by_idx(int pzone_idx, const put_pzone_t put_pzone_info)
{
	int cm_lid = get_cm_lid(MC_TYPE_PZONE, pzone_idx);
	int pzone_lid = get_zone_lid(MC_TYPE_PZONE, pzone_idx);
	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer pzone_node_id = get_subnode_id_by_lid(pzone_lid, cm_node_id, MC_TYPE_PZONE);
	int rc = 0;

	rc = libdb_attr_set_string(DB_RMM, pzone_node_id, PZONE_DESCRIPT_STR,
						  0x0, (char*)put_pzone_info.descr, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}

static result_t get_pzone_collection(collections_t *pzone, uint32 *number, int8 *host)
{
    struct node_info *subnode = NULL;
	int subnode_num = 0;
	int i;
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};
	int cm_lid = 0;
	int max_pz_num = 0;
	int pz_lid = 0;
	int32 pz_count = 0;

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_PZONE, MC_TYPE_PZONE, &subnode_num, memdb_filter, LOCK_ID_NULL);
	if (subnode_num == 0) {
		libdb_free_node(subnode);
		return RESULT_NO_NODE;
	}

	for (i = 0; i < subnode_num; i++) {
		cm_lid = get_db_info_num(DB_RMM, subnode[i].parent, MBP_LOC_ID_STR);
		max_pz_num = MAX_PZONE_NUM;
		pz_lid = get_db_info_num(DB_RMM, subnode[i].node_id, PZONE_LOC_ID_STR);
		pzone->id = pz_lid + (cm_lid - 1) * max_pz_num;
		if (pzone->id < 1) {
			pzone->id = 0;
			continue;
		}

		get_db_info_string(DB_RMM, subnode[i].node_id, PZONE_UUID_STR, pzone->uuid, UUID_LEN);
		if (strnlen_s(pzone->uuid, sizeof(pzone->uuid)) == 0)
			continue;

		get_db_info_string(DB_RMM, subnode[i].node_id, PZONE_NAME_STR, pzone->name, RMM_NAME_LEN);
		memset(prefix, 0, PREFIX_LEN);
		memset(new_link, 0, MAX_URL + 48);
		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		insert_str(new_link, "http://%s:%d/Rack/PowerZoneCollection/%d/", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/

		snprintf_s_sii((char *)(pzone->href), DESCRIPTION_LEN, new_link,
					host, rmm_cfg_get_port(RESTD_PORT), pzone->id);
		pzone++;
		pz_count += 1;
	}

	*number = pz_count;
	libdb_free_node(subnode);

	return RESULT_OK;
}

result_t libwrap_get_pzone_coll(collections_t* pzone, uint32 *number, int8 *host)
{
	return get_pzone_collection(pzone, number, host);
}

result_t libwrap_post_pzone_psu_by_idx(uint32 pzone_idx, uint32 psu_idx, int state)
{
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_PSU, RMM_JSON_REQ_STATE_CHANGE)) != RESULT_OK)
		return rc;

	return am_post_psu_by_node_id(pzone_idx, psu_idx, state);
}


