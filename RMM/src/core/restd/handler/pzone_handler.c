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

static struct rest_handler pzone_coll_handler;
static struct rest_handler pzone_handler;
static struct rest_handler psu_coll_handler;
static struct rest_handler psu_handler;
static struct rest_handler psu_change_handler;


void register_pzone_handler(void)
{
	register_handler(PZONE_COLL_URL,				&pzone_coll_handler);
	register_handler(PZONE_URL,						&pzone_handler);
}

void register_psu_handler(void)
{
//	register_handler(PSU_COLL_URL,					&psu_coll_handler);
//	register_handler(PSU_URL,						&psu_handler);
	register_handler(PSU_ACTION_STATUS_CHANGE_URL,	&psu_change_handler);
}

static json_t *pzone_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	rs = libwrap_pack_pzone_coll_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack pzone collection fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *pzone_coll_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *pzone_coll_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *pzone_get(struct rest_uri_param *param)
{
	pzone_member_t pzone_member = { {0} };
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	int32 pzone_idx = 0;

	pzone_idx = get_asset_idx(param, RMM_JSON_ZONE_ID, MC_TYPE_PZONE);
	if (pzone_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get power zone index fail\n");
		return NULL;
	}

	if (format_uuid(param->url, strnlen_s(param->url, sizeof(param->url))) == -1) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("format url uuid fail\n");
		return NULL;
	}

	rs = libwrap_get_pzone_by_idx(pzone_idx, &pzone_member);
	if (rs != RESULT_OK) {
		HTTPD_ERR("get power zone fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("result json object request fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	rs = libwrap_pack_pzone_to_json(result, &pzone_member, param);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack pzone fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *pzone_patch(struct rest_uri_param *param)
{
	json_t *req = NULL;
	result_t rs = RESULT_OK;
	put_pzone_t put_pzone_info = { {0} };
	int32 pzone_idx = 0;

	pzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_PZONE);
	if (pzone_idx == -1) {
		HTTPD_ERR("get power zone index fail\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	rs = libwrap_pre_put_pzone_by_idx(pzone_idx, &put_pzone_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("power zone pre put fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	rs = libwrap_update_put_pzone_info(req, &put_pzone_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("libwrap_update_put_pzone_info fail, result is %d\n", rs);
		json_free(req);
		return NULL;
	}
	json_free(req);

	rs = libwrap_put_pzone_by_idx(pzone_idx, put_pzone_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pz patch fail, result is %d\n", rs);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", pzone_idx);
	rf_log(INFO, MSGPZoneResourceUpdated, buff);
	update_response_info(param, HTTP_OK);
	return pzone_get(param);
}

static json_t *pzone_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static input_attr_t psu_change_attrs[] = {
    {"TargetIndex", NULL},
    {"EnabledState", NULL}
};

static json_t *psu_change_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *result = NULL;
	json_t *state = NULL;
	json_t *target_index = NULL;
	result_t rs = RESULT_OK;
	int8 *action = NULL;
	int8 *en_st_str = NULL;
	int32 en_st = 0;
	int32 pzone_idx, psu_idx;
	uint32 ary_size = sizeof(psu_change_attrs)/sizeof(input_attr_t);

	pzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_PZONE);
	if (pzone_idx == -1) {
		HTTPD_ERR("get power zone index fail\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("result json object request error\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		HTTPD_ERR("json parse error\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(result);
		return NULL;
	}

	if (libwrap_check_input_attrs(psu_change_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(req);
		json_free(result);
		return NULL;
	}

	target_index = json_object_get(req, RMM_JSON_TARGET_INDEX);
	state = json_object_get(req, RMM_JSON_ENABLE_STATE);
	
	if (state == NULL || target_index == NULL) {
		HTTPD_ERR("failed to get enable state/target_index, returning\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(req);
		json_free(result);
		return NULL;
	}

	psu_idx = json_integer_value(target_index);
	en_st_str = json_string_value(state);
	
	if (en_st_str == NULL || psu_idx == -1 || psu_idx > MAX_PSU_NUM) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json value get error\n");
		json_free(req);
		json_free(result);
		return NULL;
	}

	if (!((0 == strcmp(en_st_str, RMM_JSON_STATE_ENABLED)) || (0 == strcmp(en_st_str, RMM_JSON_STATE_DISABLED)))) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("bad enable state, returning\n");
		json_free(req);
		json_free(result);
		return NULL;
	}

	if (0 == strcmp(en_st_str, RMM_JSON_STATE_ENABLED))
		en_st = PSU_STATE_ENABLED;
	else if (0 == strcmp(en_st_str, RMM_JSON_STATE_DISABLED))
		en_st = PSU_STATE_DISABLED;
		
	rs = libwrap_post_pzone_psu_by_idx(pzone_idx, psu_idx, en_st);
	if (rs != RESULT_OK) {
		if (rs == RESULT_NO_NODE)
			update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		else
			update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);

		HTTPD_ERR("psu post fail, result is %d\n", rs);
		json_free(req);
		json_free(result);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	json_free(req);

	int8 psu_lid[32] = {};
	int8 pz_lid[32] = {};
	int8 state_str[32] = {};
	snprintf_s_i(psu_lid, sizeof(psu_lid), "%d", psu_idx);
	snprintf_s_i(pz_lid, sizeof(pz_lid), "%d", pzone_idx);
	libwrap_get_state_str(en_st, state_str, sizeof(state));
	rf_snmp_evt(INFO, MSGPSUStatusChange, pz_lid, psu_lid, state_str);

	json_object_add(result, RMM_JSON_RESULT, json_string("Success"));
	return result;
}

static struct rest_handler pzone_coll_handler = {
	.get    = pzone_coll_get,
	.put    = NULL,
	.patch  = pzone_coll_patch,
	.post   = pzone_coll_post,
	.del    = NULL,
};

static struct rest_handler pzone_handler = {
	.get    = pzone_get,
	.put    = NULL,
	.patch  = pzone_patch,
	.post   = pzone_post,
	.del    = NULL,
};

#if 0
static struct rest_handler psu_coll_handler = {
	.get    = psu_coll_get,
	.put    = NULL,
	.patch  = psu_coll_patch,
	.post   = psu_coll_post,
	.del    = NULL,
};

static struct rest_handler psu_handler = {
	.get    = psu_get,
	.put    = NULL,
	.patch  = psu_patch,
	.post   = NULL,
	.del    = NULL,
};
#endif

static struct rest_handler psu_change_handler = {
	.get    = NULL,
	.put    = NULL,
	.patch  = NULL,
	.post   = psu_change_post,
	.del    = NULL,
};

