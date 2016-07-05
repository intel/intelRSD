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


#include "handler.h"
#include "libwrap/wrap.h"

static struct rest_handler drawer_coll_handler;
static struct rest_handler drawer_handler;
static struct rest_handler drawer_reset_handler;

void register_drawer_handler(void)
{
	register_handler(DRAWER1_URL, &drawer_handler);
	register_handler(DRAWER2_URL, &drawer_handler);
	register_handler(DRAWER3_URL, &drawer_handler);
	register_handler(DRAWER4_URL, &drawer_handler);
	register_handler(DRAWER5_URL, &drawer_handler);
	register_handler(DRAWER6_URL, &drawer_handler);
	register_handler(DRAWER7_URL, &drawer_handler);
	register_handler(DRAWER8_URL, &drawer_handler);

	//register_handler(DRAWER_ACTION_RESET_URL, &drawer_reset_handler);
}

static int32 get_drawer_asset_idx(const struct rest_uri_param *param, int32 *zone_idx, int32 *idx)
{
	memdb_integer node_id;
	struct node_info *drawer_info;
	struct node_info *dzone_info;
	int32 temp = 0;
	int32 cm_lid = 0;
	int32 drawer_lid = 0;
	int8 result[64] = {0};
	int32 url_len = 0;

	url_len = strnlen_s(param->url, sizeof(param->url));
	if (url_len != 0) {
		sscanf(param->url, "/redfish/v1/Chassis/Drawer%d", &temp);
	}

	*zone_idx = (temp-1) / MAX_DRAWER_NUM + 1;
	*idx = (temp > MAX_DRAWER_NUM)?(temp - MAX_DRAWER_NUM):temp;

	return 0;
}

static json_t *drawer_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	rs = libwrap_pack_drawer_coll_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack drawer collection fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;

}

static json_t *drawer_coll_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *drawer_coll_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *drawer_get(struct rest_uri_param *param)
{
	drawer_member_t drawer_member = { {0} };
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	int32 dzone_idx = 0;
	int32 drawer_idx = 0;

	rs = get_drawer_asset_idx(param, &dzone_idx, &drawer_idx);
	if ((rs != RESULT_OK) || (drawer_idx <= 0)) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("drawer zone index get fail\n");
		return NULL;
	}

	if (format_uuid(param->url, strnlen_s(param->url, sizeof(param->url))) == -1) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("format url uuid fail\n");
		return NULL;
	}

	rs = libwrap_get_drawer_by_idx(dzone_idx, drawer_idx, &drawer_member, param->host);
	if (rs != RESULT_OK) {
		HTTPD_ERR("get drawer by idx fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request error\n");
		return NULL;
	}

	rs = libwrap_pack_drawer_to_json(result, &drawer_member, param);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack drawer fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *drawer_patch(struct rest_uri_param *param)
{
	json_t *req = NULL;
	result_t rs = RESULT_OK;
	put_drawer_t put_drawer_info = { {0} };
	json_t *rs_json = NULL;
	int32 dzone_idx = 0;
	int32 drawer_idx = 0;
	int32 index = 0;

	rs = get_drawer_asset_idx(param, &dzone_idx, &drawer_idx);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("drawer index get fail\n");
		return NULL;
	}

	rs = libwrap_pre_put_drawer(dzone_idx, drawer_idx, &put_drawer_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("drawer pre put fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	rs = libwrap_update_put_drawer_info(req, &put_drawer_info, dzone_idx, drawer_idx);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("drawer put fail, result is %d\n", rs);
		json_free(req);
		return NULL;
	}

	json_free(req);
	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", (dzone_idx - 1) * MAX_DRAWER_NUM + index);
	rf_log(INFO, MSGDrawerResourceUpdated, buff);

	update_response_info(param, HTTP_OK);
	return drawer_get(param);
}

#if 0
static json_t *drawer_reset_post(struct rest_uri_param *param)
{
	json_t *req;
	result_t rs = RESULT_OK;
	int32 reset_drawer_result = 0;
	memdb_integer node_id = 0;
	int32 index = 0;
	int32 dzone_idx = 0;
	int32 drawer_idx = 0;
	json_t *action_json = NULL;
	json_t *type_json = NULL;
	int8 *type = NULL;

	update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
	rs = get_drawer_asset_idx(param, &dzone_idx, &drawer_idx);
	if (rs != RESULT_OK) {
		HTTPD_ERR("drawer index get fail\n");
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	if (NULL == (type_json = json_object_get(req, RMM_JSON_RESET_TYPE))) {
		HTTPD_ERR("fail to get json field.\n");
		goto err;
	}

	type = json_string_value(type_json);
	if (type == NULL) {
		HTTPD_ERR("fail to get json string.\n");
		goto err;
	}

	if(0 == strcmp(type, RMM_JSON_RESET_SOFT_STR))
		libwrap_drawer_soft_reset(dzone_idx, drawer_idx, &reset_drawer_result);
	else if(0 == strcmp(type, RMM_JSON_RESET_HARD_STR))
		libwrap_drawer_hard_reset(dzone_idx, drawer_idx, &reset_drawer_result);
	else
		goto err;

	if (reset_drawer_result == RESULT_OK)
		update_response_info(param, HTTP_OK);
	else
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", (dzone_idx -1)* MAX_DRAWER_NUM + drawer_idx);
	rf_snmp_evt(INFO, MSGDrawerStatusChange, buff, "Reseted");
	return NULL;

err:
	json_free(req);
	return NULL;
	
}
#endif

static struct rest_handler drawer_coll_handler = {
	.get    = drawer_coll_get,
	.put    = NULL,
	.patch  = drawer_coll_patch,
	.post   = drawer_coll_post,
	.del    = NULL,
};

static struct rest_handler drawer_handler = {
	.get    = drawer_get,
	.put    = NULL,
	.patch  = drawer_patch,
	.post   = NULL,
	.del    = NULL,
};

#if 0
static struct rest_handler drawer_reset_handler = {
	.get    = NULL,
	.put    = NULL,
	.patch  = NULL,
	.post   = drawer_reset_post,
	.del    = NULL,
};
#endif

