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

#include <assert.h>
#include "handler.h"
#include "libwrap/wrap.h"

static struct rest_handler tzone_coll_handler;
static struct rest_handler tzone_handler;
static struct rest_handler tzone_pwm_handler;
static struct rest_handler fan_coll_handler;
static struct rest_handler fan_handler;

static json_t *tzone_set_speed(json_t *req, int32 tzone_idx/*, int32 fan_idx*/, struct rest_uri_param *param);

void register_tzone_handler(void)
{
	register_handler(TZONE_COLL_URL, &tzone_coll_handler);
	register_handler(TZONE_URL, &tzone_handler);
	register_handler(TZONE_ACTION_SET_PWM_URL, &tzone_pwm_handler);
}

#if 0
void  register_fan_handler(void)
{
	char action_url[REST_MAX_ODATA_LEN] = {0};

	register_handler(FAN_COLL_URL, &fan_coll_handler);
	register_handler(FAN_URL, &fan_handler);
}
#endif

static json_t *tzone_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request error\n");
		return NULL;
	}

	rs = libwrap_pack_tzone_coll_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack tzone collection fail, result is %d\n", rs);
		return NULL;
	}

	return result;
}

static json_t *tzone_coll_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *tzone_coll_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *tzone_get(struct rest_uri_param *param)
{
	tzone_member_t tzone_member = { {0} };
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	int32 tzone_idx = 0;

	tzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_TZONE);
	if (tzone_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get cooling zone index fail\n");
		return NULL;
	}

	if (format_uuid(param->url, strnlen_s(param->url, sizeof(param->url))) == -1) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("format url uuid fail\n");
		return NULL;
	}

	rs = libwrap_get_tzone_by_idx(tzone_idx, &tzone_member);
	if (rs != RESULT_OK) {
		HTTPD_ERR("get cooling zone fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	rs = libwrap_pack_tzone_to_json(result, &tzone_member, param);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack tzone fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *tzone_patch(struct rest_uri_param *param)
{
	json_t *req = NULL;
	result_t rs = RESULT_OK;
	put_tzone_t put_tzone_info = { {0} };
	int8 *description = NULL;
	json_t *rs_json = NULL;
	int32 tzone_idx = 0;
	json_t *obj = NULL;

	tzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_TZONE);
	if (tzone_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get cooling zone index fail\n");
		return NULL;
	}

	rs = libwrap_pre_put_tzone_by_idx(tzone_idx, &put_tzone_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("cooling zone pre put fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	rs = libwrap_update_put_tzone_info(req, &put_tzone_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("libwrap_update_put_tzone_info fail, result is %d\n", rs);
		json_free(req);
		return NULL;
	}
	json_free(req);

	rs = libwrap_put_tzone_by_idx(tzone_idx, put_tzone_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("cooling zone put error, result is %d\n", rs);
		return NULL;
	}

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", tzone_idx);
	rf_log(INFO, MSGTZoneResourceUpdated, buff);
	return tzone_get(param);
}

static input_attr_t tz_pwm_attrs[] = {
    {"DesiredSpeedPWM", NULL}
};

static json_t *tzone_pwm_post(struct rest_uri_param *param)
{
	json_t *req;
	int8 *action = NULL;
	int32 tzone_idx = 0;
	uint32 ary_size = sizeof(tz_pwm_attrs)/sizeof(input_attr_t);

	tzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_TZONE);
	if (tzone_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get cooling zone index fail\n");
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	if (libwrap_check_input_attrs(tz_pwm_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		free(req);
		return NULL;
	}

	return tzone_set_speed(req, tzone_idx, param);
}

static json_t *tzone_set_speed(json_t *req, int32 tzone_idx/*, int32 fan_idx*/, struct rest_uri_param *param)
{
	int8 *unit_type = NULL;
	int64 speed = 0;
	uint32 u_type = 0;
	result_t rs = RESULT_OK;
	int len = 0;
	json_t *result = NULL;

	speed = json_integer_value(json_object_get(req, RMM_JSON_PWM));
	if (-1 != speed) {
		u_type = 1;
		if (speed > MAX_PWM_VALUE) {
			HTTPD_ERR("Get PWM failed. SPeed is %d.\n", speed);
			update_response_info(param, HTTP_BAD_REQUEST);
			goto err;
		}
	} else {
		u_type = 0;
		speed = json_integer_value(json_object_get(req, RMM_JSON_RPM));
		if (-1 == speed) {
			HTTPD_ERR("Get RPM failed. SPeed is %d.\n", speed);
			update_response_info(param, HTTP_BAD_REQUEST);
			goto err;
		}
	}

	HTTPD_ERR("--------------post fan set speed at idx %d, speed %d.\n", tzone_idx, speed);

	rs = libwrap_post_fan_set_speed(tzone_idx/*, fan_idx*/, u_type, speed);
	if (rs != RESULT_OK) {
		HTTPD_ERR("fan speed post fail, result is %d\n", rs);
		if (rs == RESULT_NO_NODE)
			update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		else
			update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);

		goto err;
	} else {
		update_response_info(param, HTTP_OK);
	}

	int8 fan_lid[32] = {};
	int8 tz_lid[32] = {};
	int8 pwm[32] = {};
	snprintf_s_i(fan_lid, sizeof(fan_lid), "%d", 0);
	snprintf_s_i(tz_lid, sizeof(tz_lid), "%d", tzone_idx);
	//snprintf(pwm, sizeof(pwm), "%s %lld", unit_type, speed);
	len = snprintf_s_s(pwm, sizeof(pwm), "%s", unit_type);
	if (len < 0) {
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	len += snprintf_s_ll(pwm+len, sizeof(pwm)-len, " %lld", speed);
	if (len < 0) {
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	rf_snmp_evt(INFO, MSGTZoneResourceUpdated, tz_lid, fan_lid, pwm);

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	json_object_add(result, RMM_JSON_RESULT, json_string("Success"));
	return result;
err:
	HTTPD_ERR("Failed to set Fan speed.\n");
	json_free(req);
	return NULL;
}


/* removed fan available actions */
#if 0
static json_t *fan_handle_change_state(json_t *req, int32 tzone_idx, int32 fan_idx, struct rest_uri_param *param)
{
	int64 enabled_state = 0;
	result_t rs = RESULT_OK;

	enabled_state = json_integer_value(json_object_get(req, RMM_JSON_ENABLE_STATE));

	if (!((enabled_state == FAN_STATE_ENABLED) || (enabled_state == FAN_STATE_DISABLED))) {
		HTTPD_ERR("unknown fan state\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		json_free(req);
		return NULL;
	}

	rs = libwrap_post_fan_change_state(tzone_idx, fan_idx, enabled_state);
	if (rs != RESULT_OK) {
		HTTPD_ERR("fan state post fail, result is %d\n", rs);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
	} else {
		update_response_info(param, HTTP_OK);
	}
	json_free(req);

	int8 fan_lid[32] = {};
	int8 tz_lid[32] = {};
	int8 state[32] = {};
	snprintf(fan_lid, sizeof(fan_lid), "%d", fan_idx);
	snprintf(tz_lid, sizeof(tz_lid), "%d", tzone_idx);
	libwrap_get_state_str(enabled_state, state, sizeof(state));
	rf_snmp_evt(INFO, MSGFanStatusChange, tz_lid, fan_lid, state);

	return NULL;
}


static json_t *fan_post(struct rest_uri_param *param)
{
	json_t *req;
	int8 *action = NULL;
	int32 tzone_idx = 0, fan_idx = 0;

	tzone_idx = get_asset_idx(param, "zone_id", MC_TYPE_TZONE);
	if (tzone_idx == -1) {
		HTTPD_ERR("get cooling zone index fail\n");
		return NULL;
	}

	fan_idx = get_asset_idx(param, "fan_id", MC_TYPE_FAN);
	if (fan_idx == -1) {
		HTTPD_ERR("get fan index fail\n");
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	action = param->action_name;

	if (0 == strcmp(action, RMM_JSON_REQ_STATE_CHANGE))
		return fan_handle_change_state(req, tzone_idx, fan_idx, param);
	/*
	else if (0 == strcmp(action, RMM_JSON_SET_DESIRED_SPEED))
		return fan_handle_set_speed(req, tzone_idx, fan_idx, param);
	*/

	json_free(req);

	return NULL;
}
#endif

static struct rest_handler tzone_coll_handler = {
	.get    = tzone_coll_get,
	.put    = NULL,
	.patch  = tzone_coll_patch,
	.post   = tzone_coll_post,
	.del    = NULL,
};

static struct rest_handler tzone_handler = {
	.get    = tzone_get,
	.put    = NULL,
	.patch  = tzone_patch,
	.post   = NULL,
	.del    = NULL,
};

static struct rest_handler tzone_pwm_handler = {
	.get    = NULL,
	.put    = NULL,
	.patch  = NULL,
	.post   = tzone_pwm_post,
	.del    = NULL,
};
#if 0
static struct rest_handler fan_coll_handler = {
	.get    = fan_coll_get,
	.put    = NULL,
	.patch  = fan_coll_patch,
	.post   = fan_coll_post,
	.del    = NULL,
};

static struct rest_handler fan_handler = {
	.get    = fan_get,
	.put    = NULL,
	.patch  = fan_patch,
	.post   = NULL,
	.del    = NULL,
};
#endif
