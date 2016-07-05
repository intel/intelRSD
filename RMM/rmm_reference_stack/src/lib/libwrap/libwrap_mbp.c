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


result_t libwrap_pack_mbp_coll_to_json(json_t *output, struct rest_uri_param *param)
{
	collections_t *mbp_collections = NULL;
	int8 *members = NULL;
	uint32 mbp_num = 0;
	int32 i;
	int32 cm_num = 0;
	result_t rs = RESULT_OK;

	if ((output == NULL) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	cm_num = libwrap_get_asset_num(MC_TYPE_CM);
	if (cm_num == 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		goto end;
	}

	mbp_collections = (collections_t *)malloc(cm_num * sizeof(collections_t));
	if (mbp_collections == NULL)
		return RESULT_MALLOC_ERR;

	memset(mbp_collections, 0, cm_num * sizeof(collections_t));

	members = (int8 *)malloc(cm_num * HREF_URL_LEN);
	if (members == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_NO_NODE;
	}

	memset(members, 0, cm_num * HREF_URL_LEN);

	rs = libwrap_get_mbp_coll(mbp_collections, &mbp_num, param->host);
	if (rs != RESULT_OK) {
		return RESULT_GET_COLL_ERR;
	}

	for (i = 0; i < cm_num; i++) {
		snprintf_s_si((int8 *)mbp_collections[i].href, HREF_URL_LEN, "%s/%d", (int8 *)(param->url), mbp_collections[i].id);
		memcpy_s(members + i * HREF_URL_LEN, HREF_URL_LEN, mbp_collections[i].href, HREF_URL_LEN);
	}

end:
	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_MBP_COLL, FALSE);
	add_json_string(output, RMM_JSON_NAME, "Manager Backplane Collection");
	json_add_multi_member(output, cm_num, members);

	if (mbp_collections)
		free(mbp_collections);

	if (members)
		free(members);

	return RESULT_OK;
}

static int32 pack_mbp_basic_info(mbp_member_t *mbp_member, json_t *result)
{
	pack_basic_element_info(&(mbp_member->be), result);
	add_json_string(result, RMM_JSON_UUID, mbp_member->be.uuid);
	add_json_string(result, RMM_JSON_CREATED_DATE, mbp_member->be.create_date);
	add_json_string(result, RMM_JSON_UPDATED_DATE, mbp_member->be.update_date);
	add_json_string(result, RMM_JSON_HW_ADDR, mbp_member->MBPHWAddress);
	add_json_string(result, RMM_JSON_IP_ADDR, mbp_member->MBPIPAddress);
	add_json_string(result, RMM_JSON_FW_STATE, mbp_member->fw_state);

	return 0;
}

result_t libwrap_pack_mbp_to_json(json_t *output, mbp_member_t *mbp_member, struct rest_uri_param *param)
{
	json_t *fru = NULL;
	json_t *av_action = NULL;
	json_t *loc = NULL;
	json_t *act_obj = NULL;
	int8 act_target[REST_MAX_ODATA_LEN] = {0};
	json_t *act_type = NULL;
	int8 act_str[REST_MAX_ACTION_LEN] = {0};
	int8 act_comp_str[REST_MAX_ACTION_LEN] = {0};
	json_t *managedby = NULL;
	json_t *links = NULL;
	json_t *mbp_obj = NULL;
	json_t *target_component = NULL;
	json_t *target_tray = NULL;
	json_t *image_array = NULL;
	int8 odata_id_str[ODATA_ID_LEN] = {};
	int8 attr_str[REST_RACK_STRING_LEN];
	memdb_integer drawer_lid = 0;
	struct node_info *subnode = NULL;
	struct node_info *parent = NULL;
	int32 subnode_num = 0;
	int32 i = 0;
	int32 cm_lid = 0;

	if ((output == NULL) || (mbp_member == NULL) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_MBP, FALSE);
	pack_mbp_basic_info(mbp_member, output);

	loc = json_object();
	if (loc == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_NONE_POINTER;
	}

	add_json_string(loc, RMM_JSON_RACK_UNITS, RMM_JSON_RACK_UNITS_OU);
	add_loc_info(loc, mbp_member->loc.u_location, RMM_JSON_ULOC);
	add_loc_info(loc, mbp_member->loc.u_height, RMM_JSON_UHEIGHT);
	add_loc_info(loc, mbp_member->loc.x_location, RMM_JSON_XLOC);
	json_object_add(output, RMM_JSON_RACK_LOC, loc);

	add_json_string(output, RMM_JSON_ASSET_TAG, mbp_member->asset.asset_tag);

	pack_fru_info(&(mbp_member->asset.fru), output);

	av_action = json_object();
	if (av_action == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	/*SetUartTarget Start*/
	act_obj = json_object();
	if (act_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss(act_target, REST_MAX_ODATA_LEN, "%s/%s/%s.%s", (int8 *)param->url, RMM_JSON_POST_ACTIONS, RMM_JSON_ODATA_TYPE_MBP, RMM_JSON_SETUARTTARGET);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);

	target_component = json_array();
	if (target_component != NULL) {
		json_array_add(target_component, json_string("MMP"));
		json_array_add(target_component, json_string("SLED1"));
		json_array_add(target_component, json_string("SLED2"));
		json_array_add(target_component, json_string("SLED3"));
		json_array_add(target_component, json_string("SLED4"));
		json_array_add(target_component, json_string("CPP"));
	}
	snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_TARGET_COMPONET, RMM_JSON_ALLOWABLE_VALUES);
	json_object_add(act_obj, (char *)attr_str, target_component);

	target_tray = json_array();
	if (target_tray != NULL) {
		subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_DRAWER, &subnode_num, memdb_filter, LOCK_ID_NULL);
		for (i = 0; i < subnode_num; i++) {
			parent = libdb_get_node_by_node_id(DB_RMM, subnode[i].parent, LOCK_ID_NULL);
			if (parent == NULL) {
				update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
				return RESULT_NO_NODE;
			}

			cm_lid = get_db_info_num(DB_RMM, parent[0].parent, MBP_LOC_ID_STR);
			if (cm_lid == mbp_member->be.id) {
				drawer_lid = get_db_info_num(DB_RMM, subnode[i].node_id, DRAWER_LOC_ID_STR);
				json_array_add(target_tray, json_integer(drawer_lid));
			}
		}

		libdb_free_node(subnode);
	}
	snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_TARGET_TRAY, RMM_JSON_ALLOWABLE_VALUES);
	json_object_add(act_obj, (char *)attr_str, target_tray);

	snprintf_s_ss(act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_ODATA_TYPE_MBP, RMM_JSON_SETUARTTARGET);

	json_object_add(av_action, act_comp_str, act_obj);
	/*SetUartTarget End*/

	/*Update Start*/
	act_obj = json_object();
	if (act_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss(act_target, REST_MAX_ODATA_LEN, "%s/%s/%s.%s", (int8 *)param->url, RMM_JSON_POST_ACTIONS, RMM_JSON_ODATA_TYPE_MBP, RMM_JSON_UPDATE);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);

	snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_IMAGE, RMM_JSON_ALLOWABLE_VALUES);
	image_array= json_array();
	if (image_array != NULL) {
		json_object_add(act_obj, (char *)attr_str, image_array);
	}
	snprintf_s_ss(act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_ODATA_TYPE_MBP, RMM_JSON_UPDATE);
	json_object_add(av_action, act_comp_str, act_obj);
	/*Update End*/

	json_object_add(output, RMM_JSON_POST_ACTIONS, av_action);

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

	mbp_obj = json_object();
	if (mbp_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}
	add_json_integer(links, RMM_JSON_MANAGEDBY_ODATA_COUNT, 1);
	snprintf_s_ss(odata_id_str, ODATA_ID_LEN, "%s%s", "/redfish/v1/Managers/MBPC", (int8 *)mbp_member->MBPID);
	add_json_string(mbp_obj, RMM_JSON_ODATA_ID, odata_id_str);
	json_array_add(managedby, mbp_obj);
	json_object_add(links, RMM_JSON_MAGAGEDBY, managedby);
	json_object_add(output, RMM_JSON_RF_LINKS, links);

	return RESULT_OK;
}

static input_attr_t patch_mbp_attrs[] = {
	{"Description",         NULL},
	{"AssetTag",            NULL}
};

result_t libwrap_update_put_mbp_info(json_t *req, put_mbp_t *put_info)
{
	result_t rs = RESULT_OK;
	json_t *obj = NULL;
	int32  i = 0;
	uint32 ary_size = sizeof(patch_mbp_attrs)/sizeof(input_attr_t);

	if ((req == NULL) || (put_info == NULL))
		return RESULT_NONE_POINTER;

	for (i = 0; i < ary_size; i++)
		patch_mbp_attrs[i].value = NULL;

	if (libwrap_check_input_attrs(patch_mbp_attrs, ary_size, req, NULL) != RESULT_OK)
		return RESULT_JSON_ARR_ERR;

	obj = libwrap_get_attr_json(patch_mbp_attrs, ary_size, "Description");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_DESC_LEN)) {
			strncpy_safe(put_info->descr, input, REST_DESC_LEN, REST_DESC_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	obj = libwrap_get_attr_json(patch_mbp_attrs, ary_size, "AssetTag");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_ASSET_TAG_LEN)) {
			strncpy_safe(put_info->asset_tag, input, REST_ASSET_TAG_LEN, REST_ASSET_TAG_LEN - 1);
		} else {
			return RESULT_JSON_ARR_ERR;
		}
	}

	return RESULT_OK;
}



result_t libwrap_pre_put_mbp(int idx, put_mbp_t *put_mbp_info)
{
	int8 output[128] = {0};
	int32 lid = idx;
	memdb_integer node_id = get_subnode_id_by_lid(lid, MC_NODE_ROOT, MC_TYPE_CM);
	int64 error_code = 0;
	
	if (node_id == 0) {
		return RESULT_ATTR_ERR;
	}

	error_code = libdb_attr_get_string(DB_RMM, node_id, MBP_DESCRIPT_STR, output, sizeof(output), LOCK_ID_NULL);
	if (error_code != 0) {
		return (int32)error_code;
	}

	strncpy_safe((int8 *)put_mbp_info->descr, output, DESCRIPTION_LEN, DESCRIPTION_LEN - 1);
	error_code = 0;
	error_code = libdb_attr_get_string(DB_RMM, node_id, MBP_ASSET_TAG_STR, output, sizeof(output), LOCK_ID_NULL);
	if (error_code != 0) {
		return (int32)error_code;
	}

	strncpy_safe((int8 *)put_mbp_info->asset_tag, output, REST_ASSET_TAG_LEN, REST_ASSET_TAG_LEN - 1);

	return RESULT_OK;
}

result_t libwrap_put_mbp(int idx, const put_mbp_t put_mbp_info)
{
	int lid = idx;
	int rc = 0;
	memdb_integer node_id = get_subnode_id_by_lid(lid, MC_NODE_ROOT, MC_TYPE_CM);

	if (node_id == 0) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, node_id, MBP_DESCRIPT_STR,
						  0x0, (char*)put_mbp_info.descr, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	rc = libdb_attr_set_string(DB_RMM, node_id, MBP_ASSET_TAG_STR,
						  0x0, (char*)put_mbp_info.asset_tag, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}

static result_t get_mbp_by_idx(int idx, mbp_member_t *mbp_member)
{
	memdb_integer node_id = 0;
	int ip_addr;
	int lid = idx;
	int8 buff[RMM_NAME_LEN] = {0};

	node_id = get_subnode_id_by_lid(lid, MC_NODE_ROOT, MC_TYPE_CM);
	if (node_id == 0) {
		return RESULT_ATTR_ERR;
	}

	mbp_member->be.id = 
		get_db_info_num(DB_RMM, node_id, MBP_LOC_ID_STR);
	get_db_info_string(DB_RMM, node_id, 
		MBP_UUID_STR, mbp_member->be.uuid, UUID_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_NAME_STR, mbp_member->be.name, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_DESCRIPT_STR, mbp_member->be.desc, DESCRIPTION_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_CREATE_DATE_STR, mbp_member->be.create_date, DATE_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_UPDATE_DATE_STR, mbp_member->be.update_date, DATE_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_MBPID_STR, mbp_member->MBPID, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_HW_ADDR_STR, mbp_member->MBPHWAddress, REST_RACK_STRING_LEN);
	libdb_attr_get_int(DB_RMM, node_id, MBP_IP_ADDR_STR, &ip_addr, LOCK_ID_NULL);
	snprintf_s_iiii((char *)mbp_member->MBPIPAddress, REST_RACK_STRING_LEN, "%d.%d.%d.%d", ip_addr&0xff,
			(ip_addr>>8)&0xff,
			(ip_addr>>16)&0xff,
			(ip_addr>>24)&0xff);
	get_db_info_string(DB_RMM, node_id,
		MBP_FW_STATE, mbp_member->fw_state, DATE_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_ASSET_TAG_STR, mbp_member->asset.asset_tag, REST_ASSET_TAG_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_SER_NUM_STR, mbp_member->asset.fru.serial_num, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_MANUFACT_STR, mbp_member->asset.fru.manufacture, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_MODEL_STR, mbp_member->asset.fru.model, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_PART_NUM_STR, mbp_member->asset.fru.part_num, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, node_id, 
		MBP_FW_VER_STR, mbp_member->asset.fru.fw_ver, REST_RACK_STRING_LEN);

	snprintf_s_s((char *)(mbp_member->av_action[0].action), RMM_NAME_LEN, "%s", RMM_JSON_RESET);
	snprintf_s_s((char *)(mbp_member->av_action[0].cap[0].property), RMM_NAME_LEN, "%s", RMM_JSON_RESET_TYPE);
	get_db_info_string(DB_RMM, node_id, 
		MBP_AV_RST_TYPE_1_STR, mbp_member->av_action[0].cap[0].av, RMM_NAME_LEN);
	snprintf_s_s((char *)(mbp_member->av_action[1].action), RMM_NAME_LEN, "%s", RMM_JSON_SETUARTTARGET);
	snprintf_s_s((char *)(mbp_member->av_action[1].cap[0].property), RMM_NAME_LEN, "%s", RMM_JSON_TARGET_INDEX);
	snprintf_s_s((char *)(mbp_member->av_action[2].action), RMM_NAME_LEN, "%s", RMM_JSON_UPDATE);
	snprintf_s_s((char *)(mbp_member->av_action[2].cap[0].property), RMM_NAME_LEN, "%s", RMM_JSON_IMAGE);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, node_id, UNITS, buff, RMM_NAME_LEN);
	mbp_member->loc.units = (uint32)atoi((const char *)buff);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, node_id, MBP_X_LOC_STR, buff, RMM_NAME_LEN);
	mbp_member->loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, node_id, MBP_U_LOC_STR, buff, RMM_NAME_LEN);
	mbp_member->loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, node_id, MBP_U_HEIGHT_STR, buff, RMM_NAME_LEN);
	mbp_member->loc.u_height = (uint32)atoi((const char *)buff);
	return RESULT_OK;
}

result_t libwrap_get_mbp_by_idx(int idx, mbp_member_t *mbp_member)
{
	return get_mbp_by_idx(idx, mbp_member);
}

static result_t get_mbp_coll(collections_t *mbp, uint32 *number, int8 *host)
{
    struct node_info *subnode = NULL;
	int subnode_num = 0;
	int i;
	char value[WRAP_DB_MAX_VALUE_LEN] = {0};
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};
	int32 mbp_num = 0;

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_CM, MC_TYPE_CM, &subnode_num, memdb_filter, LOCK_ID_NULL);
	if (subnode_num == 0) {
		libdb_free_node(subnode);
		return RESULT_NO_NODE;
	}

	for (i = 0; i < subnode_num; i++) {
		mbp->id = 
			(uint32)get_db_info_num(DB_RMM, subnode[i].node_id, MBP_LOC_ID_STR);

		if (mbp->id < 1) {
			mbp->id = 0;
			continue;
		}

		get_db_info_string(DB_RMM, subnode[i].node_id, MBP_UUID_STR, mbp->uuid, UUID_LEN);
		if (strnlen_s(mbp->uuid, sizeof(mbp->uuid)) == 0)
			continue;

		get_db_info_string(DB_RMM, subnode[i].node_id, MBP_NAME_STR, mbp->name, RMM_NAME_LEN);
		memset(prefix, 0, PREFIX_LEN);
		memset(new_link, 0, MAX_URL + 48);
		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		insert_str(new_link, "http://%s:%d/Rack/MBPCollection/%d/", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/
		snprintf_s_sii((char *)(mbp->href), DESCRIPTION_LEN, new_link,
					host,  rmm_cfg_get_port(RESTD_PORT), mbp->id);
		mbp++;
		mbp_num += 1;
	}

	*number = mbp_num;
	libdb_free_node(subnode);

	return RESULT_OK;
}

result_t libwrap_get_mbp_coll(collections_t* mbp, uint32 *number, int8 *host)
{
	return get_mbp_coll(mbp, number, host);
}


