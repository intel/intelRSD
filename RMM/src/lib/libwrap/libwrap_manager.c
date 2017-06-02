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


#define LIBWRAP_MANAGER
result_t libwrap_pack_manager_coll_to_json(json_t *output, struct rest_uri_param *param)
{
	json_t *members_arr = NULL;
	json_t *member = NULL;
	int32 nodenum = 0;
	int32 i = 0;
	struct node_info *pnode = NULL;
	int8 buf[128] = {0};
	int8 uuid[UUID_LEN] = {};
	int32 cm_lid = 0;
	memdb_integer rc = -1;

	if ((output == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	pack_odata_head(output, param, RMM_JSON_ODATA_TYPE_MGR_COLL, TRUE);

	add_json_string(output, RMM_JSON_NAME, "Manager Collection");
	pnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM, &nodenum, NULL, LOCK_ID_NULL);

	members_arr = json_array();
	if (members_arr == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	member = json_object();
	if (member != NULL) {
		add_json_string(member, RMM_JSON_ODATA_ID, "/redfish/v1/Managers/RMC");
		json_array_add(members_arr, member);
	}

	if (pnode != NULL) {
		for (i = 0; i < nodenum; i++) {
			member = json_object();
			if (member != NULL) {
				rc = libdb_attr_get_string(DB_RMM, pnode[i].node_id, MBP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL);
				if (rc != 0 || strnlen_s(uuid, sizeof(uuid)) == 0) {
					continue;
				}

				rc = libdb_attr_get_int(DB_RMM, pnode[i].node_id, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
				if (rc != 0) 
					continue;

				snprintf_s_i(buf, sizeof(buf), "/redfish/v1/Managers/MBPC%d", cm_lid);
				add_json_string(member, RMM_JSON_ODATA_ID, buf);
				json_array_add(members_arr, member);
			}
		}
	}
	add_json_integer(output, RMM_JSON_MEMBERS_COUNT, json_array_size(members_arr));
	json_object_add(output, RMM_JSON_MEMBERS, members_arr);

	return RESULT_OK;
}

result_t libwrap_pack_rmc_to_json(json_t *output, struct rest_uri_param *param)
{
	json_t *action = NULL;
	json_t *action_reset = NULL;
	json_t *status = NULL;
	json_t *network = NULL;
	json_t *ethernet = NULL;
	json_t *managerfor = NULL;
	json_t *links = NULL;
	json_t *rmc_obj = NULL;
	json_t *av_action = NULL;
	json_t *act_obj = NULL;
	json_t *act_oem = NULL;
	json_t *rsa = NULL;
	json_t *act_type = NULL;
	int8 act_target[REST_MAX_ODATA_LEN] = {0};
	int8 act_str[REST_MAX_ACTION_LEN] = {0};
	int8 act_comp_str[REST_MAX_ACTION_LEN] = {0};
	int8 odata[REST_MAX_ODATA_LEN] = {0};
	int8 uuid[UUID_LEN]= {};
	int8 fw_ver[REST_RACK_STRING_LEN] = {};
	int8 time[REST_RACK_STRING_LEN] = {};
	int8 tz[REST_RACK_STRING_LEN] = {};
	struct node_info *pnode = NULL;
	int32 nodenum = 0;
	memdb_integer root_service_nid = 0;
	int8 odata_id_str[ODATA_ID_LEN] = {};
	int8 attr_str[REST_RACK_STRING_LEN];
	json_t *image_array = NULL;

	if ((output == NULL) || (param == NULL)) {
		return RESULT_NONE_POINTER;
	}

	snprintf_s_s(odata, REST_MAX_ODATA_LEN, "%s", "/redfish/v1/$metadata#Managers/Members/$entity");
	add_json_string(output, RMM_JSON_ODATA_CONTEXT, odata);
	if (RESULT_OK == gen_odata_id(param, odata, REST_MAX_ODATA_LEN))
		add_json_string(output, RMM_JSON_ODATA_ID, odata);
	if (RESULT_OK == gen_odata_type(param, odata, REST_MAX_ODATA_LEN, RMM_JSON_ODATA_TYPE_MGR, TRUE))
		add_json_string(output, RMM_JSON_ODATA_TYPE, odata);

	/* By now, there is always only one RMC, so hardcode it here. */
	add_json_string(output, RMM_JSON_ID, "1");

	pnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_V1, &nodenum, NULL, LOCK_ID_NULL);
	if (pnode == NULL) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	root_service_nid = pnode[0].node_id;
	pnode = libdb_list_subnode_by_type(DB_RMM, root_service_nid, MC_TYPE_RMC, &nodenum, NULL, LOCK_ID_NULL);
	if (pnode == NULL) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	if (libdb_attr_get_string(DB_RMM, pnode->node_id, WRAP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	add_json_string(output, RMM_JSON_UUID, uuid);
	add_json_string(output, RMM_JSON_NAME, "Manager");
	add_json_string(output, RMM_JSON_MANAGER_TYPE, "RackManager");
	add_json_string(output, RMM_JSON_DESC, "RackScale RMC");

	bzero(uuid, sizeof(uuid));
	if (libdb_attr_get_string(DB_RMM, root_service_nid, WRAP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	add_json_string(output, RMM_JSON_SERVICE_ENTRY_POINT_UUID, uuid);

	add_json_string(output, RMM_JSON_MODEL, "RackScale RMC 1.0");

	libutils_get_time(time);
	add_json_string(output, RMM_JSON_DATETIME, time);
	libutils_get_tz(tz);
	add_json_string(output, RMM_JSON_DATETIME_LOCAL_OFFSET, tz);

	get_db_info_string(DB_RMM, MC_NODE_ROOT, RACK_FW_VER_STR, fw_ver, REST_RACK_STRING_LEN);
	add_json_string(output, RMM_JSON_FIRMWARE_VERSION, fw_ver);

	av_action = json_object();
	if (av_action == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	act_oem = json_object();
	if (act_oem == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	rsa = json_object();
	if (rsa == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	/*Update Start*/
	act_obj = json_object();
	if (act_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss((int8 *)act_target, REST_MAX_ODATA_LEN, "%s/%s/%s.%s", (char *)(param->url), RMM_JSON_OEM_POST_ACTIONS, RMM_JSON_RMC, RMM_JSON_UPDATE);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);
	act_type = json_array();
	if (act_type == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	json_array_add(act_type, json_string(UPGRADE_DEB_FILE));
	snprintf_s_ss((int8 *)act_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_COMPONENT_TYPE, RMM_JSON_ALLOWABLE_VALUES);
	json_object_add(act_obj, act_str, act_type);

	snprintf_s_ss((char *)attr_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_IMAGE, RMM_JSON_ALLOWABLE_VALUES);
	image_array= json_array();
	if (image_array != NULL) {
		json_object_add(act_obj, (char *)attr_str, image_array);
	}

	snprintf_s_ss((int8 *)act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_RMC, RMM_JSON_UPDATE);
	json_object_add(rsa, act_comp_str, act_obj);
	/*Update End*/

	/*LoadFactoryDefault Start*/
	act_obj = json_object();
	if (act_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss((int8 *)act_target, REST_MAX_ODATA_LEN, "%s/%s/%s.%s", (char *)(param->url), RMM_JSON_OEM_POST_ACTIONS, RMM_JSON_RMC, RMM_JSON_LOAD_FACTORY_DEFAULT);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);

	snprintf_s_ss((int8 *)act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_RMC, RMM_JSON_LOAD_FACTORY_DEFAULT);
	json_object_add(rsa, act_comp_str, act_obj);
	/*LoadFactoryDefault End*/

	json_object_add(act_oem, RMM_JSON_OEM_INTEL_RSA, rsa); 
	json_object_add(av_action, RMM_JSON_OEM, act_oem);

	/*Reset Start*/
	action_reset = json_object();
	if (action_reset != NULL) {
		add_json_string(action_reset, RMM_JSON_TARGET, "/redfish/v1/Managers/RMC/Actions/Manager.Reset");
	}
	json_object_add(av_action, RMM_JSON_MANAGER_RESET, action_reset);
	/*Reset End*/

	json_object_add(output, RMM_JSON_POST_ACTIONS, av_action);

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_MANAGER_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_MANAGER_HEALTH, "OK");
	}

	json_object_add(output, RMM_JSON_STATUS, status);

	network = json_object();
	if (network != NULL) {
		add_json_string(network, RMM_JSON_ODATA_ID, "/redfish/v1/Managers/RMC/NetworkProtocol");
	}
	json_object_add(output, RMM_JSON_MANAGER_NETWORK_PROTOCOL, network);

	ethernet = json_object();
	if (ethernet != NULL) {
		add_json_string(ethernet, RMM_JSON_ODATA_ID, "/redfish/v1/Managers/RMC/EthernetInterfaces");
	}
	json_object_add(output, RMM_JSON_MANAGER_ETH_INTERFACES, ethernet);

	links = json_object();
	if (links == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	managerfor = json_array();
	if (managerfor == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	rmc_obj = json_object();
	if (rmc_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	add_json_integer(links, RMM_JSON_MANAGERFORCHASSIS_ODATA_COUNT, 1);
	snprintf_s_s(odata_id_str, ODATA_ID_LEN, "%s", "/redfish/v1/Chassis/Rack");
	add_json_string(rmc_obj, RMM_JSON_ODATA_ID, odata_id_str);
	json_array_add(managerfor, rmc_obj);
	json_object_add(links, RMM_JSON_MAGAGER_FOR_CHASSIS, managerfor);
	json_object_add(output, RMM_JSON_RF_LINKS, links);

	return RESULT_OK;
}

result_t libwrap_pack_mbpc_to_json(json_t *output, struct rest_uri_param *param)
{
	json_t *status = NULL;
	json_t *managerfor = NULL;
	json_t *links = NULL;
	json_t *mbp_obj = NULL;
	json_t *oem_obj = NULL;
	json_t *rsa_obj = NULL;
	int8 odata[REST_MAX_ODATA_LEN] = {0};
	int8 uuid[UUID_LEN]= {};
	int8 fw_ver[REST_RACK_STRING_LEN] = {};
	int8 id[REST_RACK_STRING_LEN] = {};
	int8 name[REST_RACK_STRING_LEN] = {};
	int32 mbp_lid = 0;
	int32 nodenum = 0;
	int url_len;
	memdb_integer root_service_nid = 0;
	memdb_integer mbpc_nid = 0;
	memdb_integer mbp_nid = 0;
	struct node_info *pnode = NULL;
	int8 odata_id_str[ODATA_ID_LEN] = {};
	json_t *av_action = NULL;
	json_t *act_obj = NULL;
	int8 act_target[REST_MAX_ODATA_LEN] = {0};
	//json_t *act_type = NULL;
	int8 act_str[REST_MAX_ACTION_LEN] = {0};
	int8 act_comp_str[REST_MAX_ACTION_LEN] = {0};

	if ((output == NULL) || (param == NULL))
		return RESULT_NONE_POINTER;

	snprintf_s_s(odata, REST_MAX_ODATA_LEN, "%s", "/redfish/v1/$metadata#Managers/Members/$entity");
	add_json_string(output, RMM_JSON_ODATA_CONTEXT, odata);
	if (RESULT_OK == gen_odata_id(param, odata, REST_MAX_ODATA_LEN))
		add_json_string(output, RMM_JSON_ODATA_ID, odata);
	if (RESULT_OK == gen_odata_type(param, odata, REST_MAX_ODATA_LEN, RMM_JSON_ODATA_TYPE_MGR, TRUE))
		add_json_string(output, RMM_JSON_ODATA_TYPE, odata);

	url_len = strnlen_s(param->url, sizeof(param->url));
	if (url_len != 0) {
		sscanf(param->url, "/redfish/v1/Managers/MBPC%d", &mbp_lid);
	}

	bzero(uuid, sizeof(uuid));
	if (libdb_attr_get_string(DB_RMM, root_service_nid, WRAP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	pnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_V1, &nodenum, NULL, LOCK_ID_NULL);
	if (pnode == NULL) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	root_service_nid = pnode[0].node_id;
	mbpc_nid = get_subnode_id_by_lid(mbp_lid, root_service_nid, MC_TYPE_MBPC);

	if (mbpc_nid == INVALID_NODE_ID) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	if (libdb_attr_get_string(DB_RMM, mbpc_nid, WRAP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	mbp_nid = get_subnode_id_by_lid(mbp_lid, MC_NODE_ROOT, MC_TYPE_CM);
	if (mbp_nid == INVALID_NODE_ID) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_NO_NODE;
	}

	if (libdb_attr_get_string(DB_RMM, mbp_nid, MBP_FW_VER_STR, fw_ver, sizeof(fw_ver), LOCK_ID_NULL) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return RESULT_ATTR_ERR;
	}

	snprintf_s_i(id, sizeof(id), "%d", mbp_lid);
	add_json_string(output, RMM_JSON_ID, id);
	add_json_string(output, RMM_JSON_UUID, uuid);
	add_json_string(output, RMM_JSON_FIRMWARE_VERSION, fw_ver);
	snprintf_s_s(name, sizeof(name), "%s", "MBPC");
	add_json_string(output, RMM_JSON_NAME, name);
	add_json_string(output, RMM_JSON_MANAGER_TYPE, "ManagementController");
	add_json_string(output, RMM_JSON_DESC, "Management Backplane Controller");

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_MANAGER_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_MANAGER_HEALTH, "OK");
	}

	json_object_add(output, RMM_JSON_STATUS, status);

	av_action = json_object();
	if (av_action == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	/*Reset Start*/
	act_obj = json_object();
	if (act_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	snprintf_s_ssss(act_target, REST_MAX_ODATA_LEN, "%s/%s/%s.%s", (int8 *)param->url, RMM_JSON_POST_ACTIONS, RMM_JSON_ODATA_TYPE_MGR, RMM_JSON_RESET);
	add_json_string(act_obj, RMM_JSON_TARGET, act_target);

#if 0
	act_type = json_array();
	if (act_type == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	json_array_add(act_type, json_string(RMM_JSON_RESET_SOFT_STR));
	snprintf_s_ss(act_str, REST_MAX_ACTION_LEN, "%s@%s", RMM_JSON_RESET_TYPE, RMM_JSON_ALLOWABLE_VALUES);
	json_object_add(act_obj, act_str, act_type);
#endif

	snprintf_s_ss(act_comp_str, REST_MAX_ACTION_LEN, "#%s.%s", RMM_JSON_ODATA_TYPE_MGR, RMM_JSON_RESET);
	json_object_add(av_action, act_comp_str, act_obj);
	/*Reset End*/

	json_object_add(output, RMM_JSON_POST_ACTIONS, av_action);

	links = json_object();
	if (links == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	managerfor = json_array();
	if (managerfor == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	mbp_obj = json_object();
	if (mbp_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	oem_obj = json_object();
	if (oem_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	rsa_obj = json_object();
	if (rsa_obj == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return RESULT_MALLOC_ERR;
	}

	add_json_integer(links, RMM_JSON_MANAGERFORMBP_ODATA_COUNT, 1);
	snprintf_s_si(odata_id_str, ODATA_ID_LEN, "%s%d", "/redfish/v1/Chassis/Rack/MBPs/", mbp_lid);
	add_json_string(mbp_obj, RMM_JSON_ODATA_ID, odata_id_str);
	json_array_add(managerfor, mbp_obj);

	json_object_add(rsa_obj, RMM_JSON_MAGAGER_FOR_MBP, managerfor);
	json_object_add(oem_obj, RMM_JSON_OEM_INTEL_RSA, rsa_obj);
	json_object_add(links, RMM_JSON_OEM, oem_obj);
	json_object_add(output, RMM_JSON_RF_LINKS, links);

	return RESULT_OK;
}

