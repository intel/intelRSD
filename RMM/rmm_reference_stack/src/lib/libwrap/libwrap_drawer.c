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



result_t get_drawer_id_by_idx(int dzone_idx, int drawer_idx, memdb_integer *node_id)
{
	memdb_integer cm_node_id;
	memdb_integer dzone_node_id;
	memdb_integer drawer_node_id;
	int cm_lid = 0;
	int dzone_lid = 0;
	int drawer_lid = drawer_idx;

	cm_lid = get_cm_lid(MC_TYPE_TZONE, dzone_idx);
	dzone_lid = get_zone_lid(MC_TYPE_TZONE, dzone_idx);
	cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	if (cm_node_id == 0) {
		return RESULT_NO_NODE;
	}

	dzone_node_id = get_subnode_id_by_lid(dzone_lid, cm_node_id, MC_TYPE_DZONE);
	if (dzone_node_id == 0) {
		return RESULT_NO_NODE;
	}

	drawer_node_id = get_subnode_id_by_lid(drawer_lid, dzone_node_id, MC_TYPE_DRAWER);
	if (drawer_node_id == 0) {
		return RESULT_NO_NODE;
	}

	*node_id = drawer_node_id;
	return RESULT_OK;
}


static int get_dzone_by_id(uint32 rack_dzone_idx, struct dzone_member *dz_number)
{
	int cm_lid = get_cm_lid(MC_TYPE_DZONE, rack_dzone_idx);
	int dzone_lid = get_zone_lid(MC_TYPE_DZONE, rack_dzone_idx);
	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer dzone_node_id = get_subnode_id_by_lid(dzone_lid, cm_node_id, MC_TYPE_DZONE);
	char buff[RMM_NAME_LEN] = {0};

	dz_number->be.id = 
		get_db_info_num(DB_RMM, dzone_node_id, DZONE_LOC_ID_STR);
	get_db_info_string(DB_RMM, dzone_node_id, 
		DZONE_UUID_STR, dz_number->be.uuid, UUID_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, 
		TZONE_NAME_STR, dz_number->be.name, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, 
		TZONE_DESCRIPT_STR, dz_number->be.desc, DESCRIPTION_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, 
		TZONE_CREATE_DATE_STR, dz_number->be.create_date, DATE_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, 
		TZONE_UPDATE_DATE_STR, dz_number->be.update_date, DATE_LEN);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, X_LOC, buff, RMM_NAME_LEN);
	dz_number->dzone_loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, U_LOC, buff, RMM_NAME_LEN);
	dz_number->dzone_loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, dzone_node_id, U_HEIGHT, buff, RMM_NAME_LEN);
	dz_number->dzone_loc.u_height = (uint32)atoi((const char *)buff);

	memset(dz_number->presence, 0, REST_PRESENCE_LEN);
	get_present_string(dzone_node_id, dz_number->presence, MAX_DRAWER_NUM, REST_PRESENCE_LEN);

	return 0;
}



static result_t get_drawerzone_collection(collections_t* dzone, uint32 *number, int8 *host)
{
    struct node_info *subnode = NULL;
	int subnode_num = 0;
	int i;
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};
	int cm_lid = 0;
	int max_dz_num = 0;
	int dz_lid = 0;

	subnode = libdb_list_node_by_type(MC_NODE_ROOT, MC_TYPE_DZONE, MC_TYPE_DZONE, &subnode_num, memdb_filter,LOCK_ID_NULL);

	for (i = 0; i < subnode_num; i++) {
		cm_lid = get_db_info_num(DB_RMM, subnode[i].parent, MBP_LOC_ID_STR);
		max_dz_num = MAX_TZONE_NUM;
		dz_lid = get_db_info_num(DB_RMM, subnode[i].node_id, TZONE_LOC_ID_STR);
		dzone->id = dz_lid + (cm_lid -1) * max_dz_num;
		if (dzone->id < 1) {
			dzone->id = 0;
			continue;
		}

		get_db_info_string(DB_RMM, subnode[i].node_id, DZONE_UUID_STR, dzone->uuid, UUID_LEN);
		get_db_info_string(DB_RMM, subnode[i].node_id, DZONE_NAME_STR, dzone->name, RMM_NAME_LEN);
		memset(prefix, 0, PREFIX_LEN);
		memset(new_link, 0, MAX_URL + 48);
		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		insert_str(new_link, "http://%s:%d/redfish/v1/Chassis/Drawers/%d/", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/
		snprintf_s_sii((char *)(dzone->href), DESCRIPTION_LEN, new_link,
					host, rmm_cfg_get_port(RESTD_PORT), dzone->id);
		dzone++;
	}

	*number = subnode_num;
	libdb_free_node(subnode);

	return RESULT_OK;
}

result_t libwrap_get_dzone_by_idx(uint32 tzone_idx, dzone_member_t *tzone_member)
{
	return get_dzone_by_id(tzone_idx, tzone_member);
}


result_t libwrap_get_dzone_coll(collections_t *dzone, uint32 *number, int8 *host)
{
	return get_drawerzone_collection(dzone, number, host);
}

result_t libwrap_pre_put_drawer(uint32 dzone_idx, uint32 drawer_idx, put_drawer_t *put_drawer_info)
{
	memdb_integer drawer_node_id;
	result_t rs = RESULT_OK;
	char output[128] = {0};
	int64 error_code = 0;

	rs = get_drawer_id_by_idx(dzone_idx, drawer_idx, &drawer_node_id);
	if (RESULT_OK != rs)
		return rs;

	libdb_attr_get_string(DB_RMM, drawer_node_id, DRAWER_DESCRIPT_STR, output, 128, LOCK_ID_NULL);
	if (error_code != 0) {
		return (int)error_code;
	}	
	strncpy_safe((char*)put_drawer_info->descr, output, DESCRIPTION_LEN, DESCRIPTION_LEN - 1);

	return RESULT_OK;
}

result_t libwrap_put_drawer(int32 dzone_idx, int32 drawer_idx, const put_drawer_t put_drawer_info)
{

	memdb_integer drawer_id = 0;
	get_drawer_id_by_idx(dzone_idx, drawer_idx, &drawer_id);
	if (libdb_attr_set_string(DB_RMM, drawer_id, DRAWER_DESCRIPT_STR,
						  0x0, (char*)put_drawer_info.descr, SNAPSHOT_NEED, LOCK_ID_NULL) == -1) {
		return RESULT_ATTR_ERR;
	}

	return RESULT_OK;
}

static result_t get_drawer_by_idx(int32 dzone_idx, int32 drawer_idx, drawer_member_t *drawer_member, int8 *host)
{
	int cm_lid = get_cm_lid(MC_TYPE_DZONE, dzone_idx);
	int dzone_lid = get_zone_lid(MC_TYPE_DZONE, dzone_idx);
	int drawer_lid = drawer_idx;

	memdb_integer cm_node_id = get_subnode_id_by_lid(cm_lid, 0, MC_TYPE_CM);
	memdb_integer dzone_node_id = get_subnode_id_by_lid(dzone_lid, cm_node_id, MC_TYPE_DZONE);
	memdb_integer drawer_node_id = get_subnode_id_by_lid(drawer_lid, dzone_node_id, MC_TYPE_DRAWER);

	char buff[RMM_NAME_LEN] = {0};
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};
	char rack_puid[REST_RACK_STRING_LEN] = {0};

	if (0 == drawer_node_id )
		return RESULT_NO_NODE;

	drawer_member->be.id = 
		get_db_info_num(DB_RMM, drawer_node_id, DRAWER_LOC_ID_STR) + (cm_lid - 1) * dzone_lid * MAX_DRAWER_NUM;
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_UUID_STR, drawer_member->be.uuid, UUID_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_NAME_STR, drawer_member->be.name, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_DESCRIPT_STR, drawer_member->be.desc, DESCRIPTION_LEN);

	rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
	insert_str(new_link, "http://%s:%d/redfish/v1/Drawers/%s", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/

	snprintf_s_sis((char *)(drawer_member->href), DESCRIPTION_LEN, new_link,
			host, rmm_cfg_get_port(RESTD_PORT), (char *)(drawer_member->be.uuid));

	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_CREATE_DATE_STR, drawer_member->be.create_date, DATE_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_UPDATE_DATE_STR, drawer_member->be.update_date, DATE_LEN);
    memset(drawer_member->TMC_ip_addr, 0, REST_RACK_STRING_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_TMC_IP_STR, drawer_member->TMC_ip_addr, REST_RACK_STRING_LEN);

	//get_db_info_string(DB_RMM, drawer_node_id, 
		//DRAWER_PUID_STR, drawer_member->tray_puid, REST_RACK_STRING_LEN);
	drawer_member->tray_ruid = 
		(uint32)get_db_info_num(DB_RMM, drawer_node_id, DRAWER_RUID_STR);

	memset(buff, 0, RMM_NAME_LEN);
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, RACK_PUID_STR, (char *)(drawer_member->tray_puid), sizeof(drawer_member->tray_puid), LOCK_ID_NULL);	
	snprintf_s_i(buff, sizeof(buff), "-%d", drawer_member->tray_ruid);
	strncat_s((char *)(drawer_member->tray_puid), sizeof(drawer_member->tray_puid), buff, strnlen_s(buff, sizeof(buff)));
	
	drawer_member->power_consumption = 
		(uint32)get_db_info_num(DB_RMM, drawer_node_id, DRAWER_PWR_CONSUM_STR);

	snprintf_s_s((char *)(drawer_member->av_action.action), RMM_NAME_LEN, "%s", RMM_JSON_RESET);
	snprintf_s_s((char *)(drawer_member->av_action.cap[0].property), RMM_NAME_LEN, "%s", RMM_JSON_RESET_TYPE);
	snprintf_s_s((char *)(drawer_member->av_action.cap[1].property), RMM_NAME_LEN, "%s", RMM_JSON_RESET_TYPE);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_AV_RST_TYPE_1_STR, drawer_member->av_action.cap[0].av, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, 
		DRAWER_AV_RST_TYPE_2_STR, drawer_member->av_action.cap[1].av, RMM_NAME_LEN);

	drawer_member->pres_temp = 
		(uint32)get_db_info_num(DB_RMM, drawer_node_id, DRAWER_PRES_TEMP);
	memset(drawer_member->pod, 0, REST_RACK_STRING_LEN);
	//memcpy_s(drawer_member->pod, "", 0);
	memset(drawer_member->rack, 0, REST_RACK_STRING_LEN);
    get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_UUID_STR, drawer_member->rack, UUID_LEN);
    
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, UNITS, buff, RMM_NAME_LEN);
	drawer_member->loc.units = (uint32)atoi((const char *)buff);

	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, X_LOC, buff, RMM_NAME_LEN);
	drawer_member->loc.x_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, U_LOC, buff, RMM_NAME_LEN);
	drawer_member->loc.u_location = (uint32)atoi((const char *)buff);
	memset(buff, 0, RMM_NAME_LEN);
	get_db_info_string(DB_RMM, drawer_node_id, U_HEIGHT, buff, RMM_NAME_LEN);
	drawer_member->loc.u_height = (uint32)atoi((const char *)buff);
	
	return RESULT_OK;
}



result_t libwrap_get_drawer_by_idx(int32 dzone_idx, int32 drawer_idx, drawer_member_t *drawer_member, int8 *host)
{
	return get_drawer_by_idx(dzone_idx, drawer_idx, drawer_member, host);
}


result_t libwrap_pack_drawer_coll_to_json(json_t *output, struct rest_uri_param *param)
{
	collections_t *drawer_collections = NULL;
	int8 *members = NULL;
	uint32 drawer_num = 0;
	result_t rs = RESULT_OK;
	int32 i;
	int32 drawers_num = 0;

	if ((output == NULL) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	drawers_num = libwrap_get_asset_num(MC_TYPE_DRAWER);
	if (drawers_num == 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		goto end;
	}

	drawer_collections = (collections_t *)malloc(drawers_num * sizeof(collections_t));
	if (drawer_collections == NULL)
		return RESULT_MALLOC_ERR;

	memset(drawer_collections, 0, drawers_num * sizeof(collections_t));

	members = (int8 *)malloc(drawers_num * HREF_URL_LEN);
	if (members == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_NO_NODE;
	}

	memset(members, 0, drawers_num * HREF_URL_LEN);

	rs = libwrap_get_drawer_coll(drawer_collections, &drawer_num, param->host);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_GET_COLL_ERR;
	}

	for (i = 0; i < drawers_num; i++) {
		snprintf_s_si((int8 *)drawer_collections[i].href, HREF_URL_LEN, "%s/%d", (char *)(param->url), drawer_collections[i].id);
		memcpy_s(members + i * HREF_URL_LEN, HREF_URL_LEN, drawer_collections[i].href, HREF_URL_LEN);
	}
end:
	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_DRAWER_COLL, TRUE);
	add_json_string(output, RMM_JSON_NAME, "Drawer Collection");
	json_add_multi_member(output, drawers_num, members);

	if (drawer_collections)
		free(drawer_collections);

	if (members)
		free(members);

	return RESULT_OK;
}


result_t libwrap_pack_drawer_to_json(json_t *output, drawer_member_t *drawer_member, struct rest_uri_param *param)
{
	json_t *loc = NULL;
	json_t *power_thermal = NULL;
	json_t *oem = NULL;
	json_t *rsa = NULL;
	json_t *links = NULL;
	json_t *containedby = NULL;
	int8 odata_id_str[ODATA_ID_LEN] = {};

	if ((output == NULL) || (drawer_member == NULL) || (NULL == param)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_CHASSIS, TRUE);

	add_json_string(output, RMM_JSON_CHASSISTYPE, "Drawer");

	oem = json_object();
	if (oem != NULL) {
		rsa = json_object();
		if (rsa != NULL) {
			add_json_string(rsa, RMM_JSON_ODATA_TYPE, "http://rackscale.intel.com/schema#Intel.Oem.DrawerChassis");
			add_json_integer(rsa, RMM_JSON_DRAWER_RUID, drawer_member->tray_ruid);
			loc = json_object();
			if (loc == NULL) {
				return RESULT_NONE_POINTER;
			}
			
			add_json_string(loc, RMM_JSON_RACK_UNITS, RMM_JSON_RACK_UNITS_OU);
			add_loc_info(loc, drawer_member->loc.x_location, RMM_JSON_XLOC);
			add_loc_info(loc, drawer_member->loc.u_location, RMM_JSON_ULOC);
			add_loc_info(loc, drawer_member->loc.u_height, RMM_JSON_UHEIGHT);
			json_object_add(rsa, RMM_JSON_LOC, loc);
			json_object_add(oem, RMM_JSON_OEM_INTEL_RSA, rsa);
		}

		json_object_add(output, RMM_JSON_OEM, oem);
	}

	links = json_object();
	if (links == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	containedby = json_object();
	if (containedby == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_s(odata_id_str, ODATA_ID_LEN, "%s", "/redfish/v1/Chassis/Rack");
	add_json_string(containedby, RMM_JSON_ODATA_ID, odata_id_str);
	json_object_add(links, RMM_JSON_CONTAINEDBY, containedby);
	json_object_add(output, RMM_JSON_RF_LINKS, links);

	return RESULT_OK;
}

result_t libwrap_update_put_drawer_info(json_t *req, put_drawer_t *put_info, int32 dzone_idx, int32 drawer_idx)
{
	result_t rs = RESULT_OK;

	if ((req == NULL) || (put_info == NULL)) {
		return RESULT_NONE_POINTER;
	}

	if (put_prepare_str(req, put_info->descr, DESCRIPTION_LEN, RMM_JSON_DESC) == -1)
		return RESULT_JSON_ARR_ERR;

	rs = libwrap_put_drawer(dzone_idx, drawer_idx, *put_info);
	if (rs != RESULT_OK) {
		return RESULT_MEMDB_SET_ERR;
	}

	return RESULT_OK;
}

static result_t get_drawer_coll(collections_t *drawer_coll, uint32 *number, int8 *host)
{
    struct node_info *subnode = NULL;
	struct node_info *parent = NULL;
	int cm_node_id = 0;
	int dzone_node_id = 0;
	int drawer_node_id = 0;
	int subnode_num  = 0;
	int i;
	int cm_lid = 0;
	char prefix[PREFIX_LEN] = {0};
	char new_link[MAX_URL + 48] = {0};

	subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_DRAWER, MC_TYPE_DRAWER, &subnode_num, memdb_filter, LOCK_ID_NULL);
	if (subnode_num == 0) {
		libdb_free_node(subnode);
		return RESULT_NO_NODE;
	}

	for (i = 0; i < subnode_num; i++) {
		parent = libdb_get_node_by_node_id(DB_RMM, subnode[i].parent, LOCK_ID_NULL);
		if (parent == NULL)
			return RESULT_NO_NODE;

		cm_lid = (uint32)get_db_info_num(DB_RMM, parent[0].parent, MBP_LOC_ID_STR);
		drawer_coll->id = (cm_lid - 1) * MAX_DRAWER_NUM +
			(uint32)get_db_info_num(DB_RMM, subnode[i].node_id, DRAWER_LOC_ID_STR);
		if (drawer_coll->id < 1) {
			drawer_coll->id = 0;
			continue;
		}

		get_db_info_string(DB_RMM, subnode[i].node_id, DRAWER_UUID_STR, drawer_coll->uuid, UUID_LEN);
		get_db_info_string(DB_RMM, subnode[i].node_id, DRAWER_NAME_STR, drawer_coll->name, RMM_NAME_LEN);
		get_db_info_string(DB_RMM, subnode[i].node_id, DRAWER_TMC_IP_STR, host, 20);
		memset(prefix, 0, PREFIX_LEN);
		memset(new_link, 0, MAX_URL + 48);
		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		insert_str(new_link, "http://%s:%d/redfish/v1/Drawers/%d/", 12, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/
		snprintf_s_sii((char *)(drawer_coll->href), DESCRIPTION_LEN, new_link,
					host, rmm_cfg_get_port(RESTD_PORT), (drawer_coll->id));
		drawer_coll++;
	}

	*number = subnode_num;
	libdb_free_node(subnode);

	return RESULT_OK;
}

result_t libwrap_get_drawer_coll(collections_t *drawer, uint32 *number, int8 *host)
{
	return get_drawer_coll(drawer, number, host);
}

result_t libwrap_set_memdb_rack_puid()
{
	int64 rack_puid = 0;

	rack_puid = get_db_info_num(DB_RMM, MC_NODE_ROOT, RACK_PUID_STR);

	am_set_rack_puid(rack_puid);

	return 0;
}


