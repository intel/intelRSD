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

#include "libwrap.h"


static int get_rack_availible_action(unsigned char db_name, avail_action_t* _action, memdb_integer node_id)
{
	char buff[RMM_NAME_LEN] = {0};
	avail_action_t *action = _action;

	strncpy_safe((char*)action->action, RMM_JSON_RESET, RMM_NAME_LEN, RMM_NAME_LEN - 1);
	strncpy_safe((char*)action->cap[0].property, RMM_JSON_RESET_TYPE, RMM_NAME_LEN, RMM_NAME_LEN - 1);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(db_name,node_id, RACK_AV_RST_1_STR, buff, RMM_NAME_LEN);
	strncpy_safe((char*)action->cap[0].av, buff, RMM_NAME_LEN, RMM_NAME_LEN - 1);

	strncpy_safe((char*)action->cap[1].property, RMM_JSON_RESET_TYPE, RMM_NAME_LEN, RMM_NAME_LEN - 1);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(db_name,node_id, RACK_AV_RST_2_STR, buff, RMM_NAME_LEN);
	strncpy_safe((char*)action->cap[1].av, buff, REST_RACK_STRING_LEN, RMM_NAME_LEN - 1);

	action++;
	strncpy_safe((char*)action->action, RMM_JSON_UPDATE, RMM_NAME_LEN, RMM_NAME_LEN - 1);
	strncpy_safe((char*)action->cap[0].property, RMM_JSON_COMPONENT_TYPE, RMM_NAME_LEN, RMM_NAME_LEN - 1);
	strncpy_safe((char*)action->cap[1].property, RMM_JSON_IMAGE, RMM_NAME_LEN, RMM_NAME_LEN - 1);

	action++;
	strncpy_safe((char*)action->action, RMM_JSON_LOAD_FACTORY_DEFAULT, RMM_NAME_LEN, RMM_NAME_LEN - 1);

	return 0;
}

static int get_rack_href(unsigned char db_name, href_t* href, memdb_integer node_id)
{
	get_db_info_string(db_name,node_id, RACK_HREF_DRAWER_STR, href->drawer, REST_RACK_STRING_LEN);
	get_db_info_string(db_name,node_id, RACK_HREF_PZONE_STR, href->powerzones, REST_RACK_STRING_LEN);
	get_db_info_string(db_name,node_id, RACK_HREF_TZONE_STR, href->thermalzones, REST_RACK_STRING_LEN);
	get_db_info_string(db_name,node_id, RACK_HREF_EVENT_STR, href->rf_event, REST_RACK_STRING_LEN);
	get_db_info_string(db_name,node_id, RACK_HREF_MBP_STR, href->mbps, REST_RACK_STRING_LEN);
	return 0;
}


static void get_mbp_pres(int8 *pres)
{
	memdb_integer cm_node_id;
	int i = 0;

	for (i = 1; i <= MAX_MBP_PRESENCE; i++) {
		cm_node_id = get_subnode_id_by_lid(i, MC_NODE_ROOT, MC_TYPE_CM);
		if (cm_node_id != 0) {
			pres[MAX_MBP_PRESENCE-i] = '1';
		}
		else {
			pres[MAX_MBP_PRESENCE-i] = '0';
		}
	}
	pres[MAX_MBP_PRESENCE] = '\0';
}


result_t libwrap_get_rack(rack_info_t *rack_info)
{
	get_base_element(&rack_info->be, DB_RMM, MC_NODE_ROOT);

	rack_info->rack_puid = get_db_info_num(DB_RMM, MC_NODE_ROOT, RACK_PUID_STR);
        get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_LOC_ID_STR,
                                           rack_info->rack_location_id, REST_RACK_ID_LEN);

	get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_GEOTAG_STR,
					   rack_info->geo_tag, RACK_TAG_LEN);
	get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_DCUID_STR,
					   rack_info->rack_dcuid, RACK_DCUID_LEN);
	get_db_info_string(DB_RMM, MC_NODE_ROOT, POD_DCUID_STR,
					   rack_info->pod_dcuid, DCUID_LEN);
	get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_API_VER_STR,
					   rack_info->api_ver, VERSION_LEN);
	get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_PODM_ADDR_STR,
					   rack_info->podm_addr, REST_RACK_PODM_LEN);

	get_asset_info(DB_RMM, &rack_info->asset, MC_NODE_ROOT);

	memset((char *)rack_info->drawer_pres, 0, MAX_DRAWER_PRESENCE + 1);
	get_drawer_pres_from_cm((char *)rack_info->drawer_pres);
	memset((char *)rack_info->mbp_pres, 0, MAX_MBP_PRESENCE + 1);
	get_mbp_pres(rack_info->mbp_pres);

	get_rack_availible_action(DB_RMM, rack_info->av_action, MC_NODE_ROOT);
	get_rack_href(DB_RMM, &(rack_info->href), MC_NODE_ROOT);

	rack_info->rack_disaggr = 0;

	return RESULT_OK;
}


static void get_rack_link_url(int8 *href, int32 len)
{
	int32 ip[4] = {};
	int32 port = 0;
	char url[96] = {};

	sscanf(href, "http://x:%d%95s", &port, url);
	snprintf_s_s(href, len, "%s", url);
}


static int32 pack_rack_link(json_t *output, rack_info_t *rack_info)
{
	get_rack_link_url(rack_info->href.powerzones, HREF_URL_LEN);
	get_rack_link_url(rack_info->href.thermalzones, HREF_URL_LEN);
	get_rack_link_url(rack_info->href.mbps, HREF_URL_LEN);

	json_add_link(output, RMM_JSON_POWER_ZONES, rack_info->href.powerzones);
	json_add_link(output, RMM_JSON_THERMAL_ZONES, rack_info->href.thermalzones);//thermalzones--->thermalzones
	json_add_link(output, RMM_JSON_MBPS, rack_info->href.mbps);

	return 0;
}


static int32 pack_rack_oem_info(rack_info_t *rack_info, json_t *output)
{

	json_t *rsa = NULL;

	rsa = json_object();
	if (rsa != NULL) {
		add_json_string(rsa, RMM_JSON_ODATA_TYPE, RACK_OEM_ODATA_TYPE);
		add_json_string(rsa, RMM_JSON_UUID, rack_info->be.uuid);
		add_json_string(rsa, RMM_JSON_CREATED_DATE, rack_info->be.create_date);
		add_json_string(rsa, RMM_JSON_UPDATED_DATE, rack_info->be.update_date);
		add_json_string(rsa, RMM_JSON_API_VER, rack_info->api_ver);
		add_json_integer(rsa, RMM_JSON_RACK_PUID, rack_info->rack_puid);

                json_t *location = json_object();
                if (NULL != location) {
                    add_json_string(location, RMM_JSON_ID, rack_info->rack_location_id);
                    json_object_add(rsa, RMM_JSON_LOC, location);
                }

		add_json_string(rsa, RMM_JSON_POD_DCUID, rack_info->pod_dcuid);
		add_json_string(rsa, RMM_JSON_PODM_ADDR, rack_info->podm_addr);
		add_json_string(rsa, RMM_JSON_GEO_TAG, rack_info->geo_tag);
		add_json_string(rsa, RMM_JSON_RACK_DCUID, rack_info->rack_dcuid);

		add_json_bool(rsa, RMM_JSON_DISAGGR, (int32)rack_info->rack_disaggr);//False or True

		add_json_string(rsa, RMM_JSON_DRAWER_PRESENCE, rack_info->drawer_pres);
		add_json_string(rsa, RMM_JSON_MBP_PRESENCE, rack_info->mbp_pres);

		pack_rack_link(rsa, rack_info);
	}

	json_object_add(output, RMM_JSON_OEM_INTEL_RSA, rsa);

	return 0;
}


int32 pack_rack_base_info(rack_info_t *rack_info, json_t *result)
{
	json_t *oem = NULL;

	pack_basic_element_info(&(rack_info->be), result);

	add_json_string(result, RMM_JSON_CHASSISTYPE, "Rack");

	oem = json_object();
	if (oem != NULL) {
		pack_rack_oem_info(rack_info, oem);
		json_object_add(result, RMM_JSON_OEM, oem);
	}

	add_json_string(result, RMM_JSON_ASSET_TAG, rack_info->asset.asset_tag);

	return 0;
}


result_t libwrap_pack_rack_info_to_json(json_t *output, rack_info_t *rack_info, struct rest_uri_param *param)
{
	json_t *fru = NULL;
	json_t *rmc_obj = NULL;
	json_t *managedby = NULL;
	json_t *drawer_obj = NULL;
	json_t *contains = NULL;
	json_t *links = NULL;
	json_t *oem = NULL;
	json_t *rsa = NULL;
	int8 odata_id_str[ODATA_ID_LEN] = {};
	json_t *av_action = NULL;
	json_t *act_obj = NULL;
	char act_target[REST_MAX_ODATA_LEN] = {0};
	json_t *act_type = NULL;
	char act_str[REST_MAX_ACTION_LEN] = {0};
	char act_comp_str[REST_MAX_ACTION_LEN] = {0};
	json_t *target_index = NULL;
	int8 attr_str[REST_RACK_STRING_LEN];
	struct node_info *subnode = NULL;
	struct node_info *parent = NULL;
	int subnode_num  = 0;
	int i;
	int cm_lid = 0;
	int drawer_idx = 0;
	int drawer_idx_array[MAX_DRAWER_PRESENCE] = {};

	if ((output == NULL) || (rack_info == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_CHASSIS, TRUE);
	pack_rack_base_info(rack_info, output);

	pack_fru_info(&(rack_info->asset.fru), output);

	links = json_object();
	if (links == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	managedby = json_array();
	if (managedby == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	rmc_obj = json_object();
	if (rmc_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	add_json_integer(links, RMM_JSON_MANAGEDBY_ODATA_COUNT, 1);
	snprintf_s_s(odata_id_str, ODATA_ID_LEN, "%s", "/redfish/v1/Managers/RMC");
	add_json_string(rmc_obj, RMM_JSON_ODATA_ID, odata_id_str);
	json_array_add(managedby, rmc_obj);
	json_object_add(links, RMM_JSON_MAGAGEDBY, managedby);

	contains = json_array();
	if (managedby == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_DRAWER, &subnode_num, memdb_filter, LOCK_ID_NULL);
	if (subnode_num > MAX_DRAWER_PRESENCE) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_OTHER_ERR;
	}

	add_json_integer(links, RMM_JSON_CONTAINS_ODATA_COUNT, subnode_num);
	for (i = 0; i < subnode_num; i++) {
		parent = libdb_get_node_by_node_id(DB_RMM, subnode[i].parent, LOCK_ID_NULL);
		if (parent == NULL) {
			libdb_free_node(subnode);
			update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
			return RESULT_NO_NODE;
		}

		cm_lid = (uint32)get_db_info_num(DB_RMM, parent[0].parent, MBP_LOC_ID_STR);
		drawer_idx = (cm_lid - 1) * MAX_DRAWER_NUM +
			(uint32)get_db_info_num(DB_RMM, subnode[i].node_id, DRAWER_LOC_ID_STR);
		if (drawer_idx < 1) {
			drawer_idx = 0;
			continue;
		}

		drawer_idx_array[i] = drawer_idx;

		drawer_obj = json_object();
		if (rmc_obj == NULL) {
			libdb_free_node(subnode);
			update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
			return RESULT_MALLOC_ERR;
		}

		snprintf_s_i(odata_id_str, ODATA_ID_LEN, "/redfish/v1/Chassis/Drawer%d", drawer_idx);
		add_json_string(drawer_obj, RMM_JSON_ODATA_ID, odata_id_str);
		json_array_add(contains, drawer_obj);
	}

	json_object_add(links, RMM_JSON_CONTAINS, contains);
	json_object_add(output, RMM_JSON_RF_LINKS, links);

	/* Actions */
	av_action = json_object();
	oem = json_object();
	rsa = json_object();
	act_obj = json_object();
	if (av_action == NULL || oem == NULL || rsa == NULL || act_obj == NULL) {
		libdb_free_node(subnode);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss((char *)act_target, REST_MAX_ODATA_LEN, "%s/%s/Oem/%s.%s", (char *)(param->url), RMM_JSON_POST_ACTIONS, RMM_JSON_ODATA_TYPE_RACK, RMM_JSON_DRAWER_RESET);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);

	target_index = json_array();
	if (target_index != NULL) {
		for (i = 0; i < MAX_DRAWER_PRESENCE; i++) {
			drawer_idx = drawer_idx_array[i];
			if (drawer_idx == 0)
				continue;
			json_array_add(target_index, json_integer(drawer_idx));
		}

		libdb_free_node(subnode);
		subnode = NULL;
		bzero(attr_str, sizeof(attr_str));
		snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_TARGET_INDEX, RMM_JSON_ALLOWABLE_VALUES);
		json_object_add(act_obj, (char *)attr_str, target_index);
	}

	act_type = json_array();
	if (act_type == NULL) {
		if (subnode) {
			libdb_free_node(subnode);
		}
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	json_array_add(act_type, json_string(RMM_JSON_RESET_SOFT_STR));
	json_array_add(act_type, json_string(RMM_JSON_RESET_HARD_STR));
	snprintf_s_ss((char *)act_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_RESET_TYPE, RMM_JSON_ALLOWABLE_VALUES);
	json_object_add(act_obj, act_str, act_type);

	snprintf_s_ss((char *)act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_ODATA_TYPE_RACK, RMM_JSON_DRAWER_RESET);

	json_object_add(rsa, act_comp_str, act_obj);
	json_object_add(oem, RMM_JSON_OEM_INTEL_RSA, rsa);
	json_object_add(av_action, RMM_JSON_OEM, oem);
	/*Reset End*/

	json_object_add(output, RMM_JSON_POST_ACTIONS, av_action);

	if (subnode) {
		libdb_free_node(subnode);
	}

	return RESULT_OK;
}

int32 put_prepare_str(const json_t *req, int8 *output, int32 len, const int8 *name)
{
	json_t *rs_json = NULL;
	int8 *input = NULL;

	rs_json = json_object_get(req, name);
	if (rs_json != NULL) {
		input = json_string_value(rs_json);
		if (input != NULL) {
			if (TRUE == check_str_len(input, len)) {
				strncpy_safe(output, input, len, len - 1);
				return 0;
			}
		}
	}

	return -1;
}

static input_attr_t patch_rack_attrs[] = {
	{"AssetTag",                  NULL},
	{"Oem",                       NULL},
	{"Oem.Intel_RackScale",             NULL},
	{"Oem.Intel_RackScale.RackPUID",    NULL},
        {"Oem.Intel_RackScale.Location",    NULL},
        {"Oem.Intel_RackScale.Location.Id", NULL},
	{"Oem.Intel_RackScale.PODDCUID",    NULL},
	{"Oem.Intel_RackScale.GeoTag",      NULL},
	{"Oem.Intel_RackScale.PODMAddress", NULL}
};

result_t libwrap_update_put_rack_info(json_t *req, put_rack_info_t *put_info)
{
	int64	rack_puid = 0;
	json_t *obj = NULL;
	int32  i = 0;
	uint32 ary_size = sizeof(patch_rack_attrs)/sizeof(input_attr_t);
        bool puid_updated = false;

	if ((req == NULL) || (put_info == NULL))
		return RESULT_NONE_POINTER;

	for (i = 0; i < ary_size; i++)
		patch_rack_attrs[i].value = NULL;

	if (libwrap_check_input_attrs(patch_rack_attrs, ary_size, req, NULL) != RESULT_OK)
		return RESULT_JSON_ARR_ERR;

	obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "AssetTag");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_ASSET_TAG_LEN)) {
			strncpy_safe(put_info->asset_tag, input, REST_ASSET_TAG_LEN, REST_ASSET_TAG_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "Oem.Intel_RackScale.PODDCUID");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_DCUID_LEN)) {
			strncpy_safe(put_info->pod_dcuid, input, REST_DCUID_LEN, REST_DCUID_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "Oem.Intel_RackScale.GeoTag");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_GEO_TAG_LEN)) {
			strncpy_safe(put_info->geo_tag, input, REST_GEO_TAG_LEN, REST_GEO_TAG_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "Oem.Intel_RackScale.PODMAddress");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_PODM_ADDR_LEN)) {
			strncpy_safe(put_info->podm_addr, input, REST_PODM_ADDR_LEN, REST_PODM_ADDR_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "Oem.Intel_RackScale.RackPUID");
	if (obj) {
		int64 input;
		input = json_integer_value(obj);
		if (input > 0x7fffffff)
			return RESULT_JSON_ARR_ERR;

		if (input && check_int_range((int32)input, 0, 0x7fffffff)) {
			if (put_info->rack_puid != input) {
			    put_info->rack_puid = input;
                            puid_updated = true;
                            snprintf_s_i(put_info->rack_location_id, sizeof(put_info->rack_location_id),
                                         "%d", (int32)put_info->rack_puid);
                        }
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

        obj = libwrap_get_attr_json(patch_rack_attrs, ary_size, "Oem.Intel_RackScale.Location.Id");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
                rsize_t len = strnlen_s(input, RSIZE_MAX_STR);
		if (input && len > 0 && len < REST_RACK_ID_LEN) {
			strncpy_safe(put_info->rack_location_id, input, REST_RACK_ID_LEN, REST_RACK_ID_LEN - 1);
                        int64 puid = str2int64((const char*)put_info->rack_location_id);
                        if (!check_int_range((int32)puid, 0, 0x7fffffff)) {
                            puid = 0;
                        }
                        if (put_info->rack_puid != puid) {
                            put_info->rack_puid = puid;
                            puid_updated = true;
                        }
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

        if (puid_updated) {
            rf_snmp_evt(INFO, MSGRackResourceUpdated, "RackPUID Updated");
        }

	return RESULT_OK;
}

result_t libwrap_update_rack_mtime()
{
	int8 value[WRAP_DB_MAX_VALUE_LEN] = {0};
	int32 rc = 0;

	libutils_get_time(value);
	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT,
					RACK_UPDATE_DATE_STR, 0x0,
					value, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc != -1)
		return RESULT_OK;
	else
		return RESULT_MEMDB_SET_ERR;
}

result_t libwrap_pre_put_rack(put_rack_info_t *put_rack_info)
{
	int8 output[128] = {0};

	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_DESCRIPT_STR, output, sizeof(output), LOCK_ID_NULL);
	strncpy_safe((char*)put_rack_info->descr, output, DESCRIPTION_LEN, DESCRIPTION_LEN - 1);
	memset(output, 0, sizeof(output));
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_GEOTAG_STR, output, sizeof(output), LOCK_ID_NULL);
	strncpy_safe((char*)put_rack_info->geo_tag, output, RACK_TAG_LEN, RACK_TAG_LEN - 1);
	memset(output, 0, sizeof(output));
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_PODM_ADDR_STR, output, sizeof(output), LOCK_ID_NULL);
	strncpy_safe((char*)put_rack_info->podm_addr, output, REST_RACK_PODM_LEN, REST_RACK_PODM_LEN - 1);
	memset(output, 0, sizeof(output));
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, POD_DCUID_STR, output, sizeof(output), LOCK_ID_NULL);
	strncpy_safe((char*)put_rack_info->pod_dcuid, output, DCUID_LEN, DCUID_LEN - 1);
	memset(output, 0, sizeof(output));
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_ASSET_TAG_STR, output, sizeof(output), LOCK_ID_NULL);
	strncpy_safe((char*)put_rack_info->asset_tag, output, REST_ASSET_TAG_LEN, REST_ASSET_TAG_LEN - 1);
	memset(output, 0, sizeof(output));
        libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_LOC_ID_STR, output, sizeof(output), LOCK_ID_NULL);
        strncpy_safe((char*)put_rack_info->rack_location_id, output, REST_RACK_ID_LEN, REST_RACK_ID_LEN - 1);
        memset(output, 0, sizeof(output));
        libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_PUID_STR, output, sizeof(output), LOCK_ID_NULL);
	put_rack_info->rack_puid = str2int64(output);

	return RESULT_OK;
}

result_t libwrap_put_rack(const put_rack_info_t put_rack_info)
{
	int8 buff[256] = {0};
	int rc = 0;

	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_DESCRIPT_STR,
						  0x0, (int8 *)put_rack_info.descr, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_GEOTAG_STR,
						  0x0, (int8 *)put_rack_info.geo_tag, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_PODM_ADDR_STR,
						  0x0, (int8 *)put_rack_info.podm_addr, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, POD_DCUID_STR,
						  0x0, (int8 *)put_rack_info.pod_dcuid, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}
	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_ASSET_TAG_STR,
						  0x0, (int8 *)put_rack_info.asset_tag, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_ASSET_TAG_STR,
						  0x0, (int8 *)put_rack_info.asset_tag, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	snprintf_s_ll(buff, sizeof(buff), "%lld", put_rack_info.rack_puid);
	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_PUID_STR,
						  0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}
        am_set_rack_puid(put_rack_info.rack_puid);

        rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_LOC_ID_STR,
					  0x0, (int8 *)put_rack_info.rack_location_id, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}
        am_set_rack_location_id((char*)put_rack_info.rack_location_id);

	snprintf_s_ss(buff, sizeof(buff), "%s-%s", (char *)(put_rack_info.pod_dcuid), (char*)put_rack_info.rack_location_id);
	rc = libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, RACK_DCUID_STR,
						  0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}



