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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "handler.h"
#include <pthread.h>
#include "libcfg/cfg.h"
#include "libredfish/msg_reg.h"
#include "libutils/time.h"
#include "libutils/base64.h"
#include "libsecurec/safe_lib.h"

static struct rest_handler service_root_handler;
static struct rest_handler chassis_handler;
static struct rest_handler metadata_handler;
static struct rest_handler rack_handler;
static struct rest_handler rack_drawer_reset_handler;
static struct rest_handler log_handler;
static struct rest_handler rf_log_handler;
static struct rest_handler log_service_handler;
static struct rest_handler rack_snmp_listener_handler;
static struct rest_handler rack_platform_handler;

void register_rack_handler(void)
{
	register_handler("",						&service_root_handler);
	register_handler(CHASSIS_URL,				&chassis_handler);
	register_handler(METADATA_URL,				&metadata_handler);
	register_handler(RACK_URL,					&rack_handler);
	register_handler(RACK_DRAWER_RESET_URL,		&rack_drawer_reset_handler);
	register_handler(LOG_URL,					&log_handler);
	register_handler(RF_LOG_URL,				&rf_log_handler);
	register_handler(RF_LOG_SERVICE_URL,		&log_service_handler);
	register_handler(RACK_SNMP_LISTENER_URL,	&rack_snmp_listener_handler);
	register_handler(PLATFORM_URL,				&rack_platform_handler);
}

static json_t *service_root_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	rs = libwrap_pack_service_root_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack service root fail, result is %d\n", rs);
		return NULL;
	}

	return result;
}

static json_t *chassis_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("request json object fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	rs = libwrap_pack_chassis_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack chassis fail, result is %d\n", rs);
		return NULL;
	}

	return result;
}

static json_t *metadata_get(struct rest_uri_param *param)
{
	send_file(param->fd, param->url);

	return NULL;
}


static json_t *rack_get(struct rest_uri_param *param)
{
	rack_info_t rack_info = { {0} };
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	rs = libwrap_get_rack(&rack_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("get rack info fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("request json object fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	rs = libwrap_pack_rack_info_to_json(result, &rack_info, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack rack fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *rack_patch(struct rest_uri_param *param)
{
	json_t *req;
	result_t rs = RESULT_OK;
	put_rack_info_t put_rack_info = { {0} };
	int32 rc = 0;

	rs = libwrap_pre_put_rack(&put_rack_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pre put fail, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		HTTPD_ERR("parse json data fail");
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	rs = libwrap_update_put_rack_info(req, &put_rack_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("libwrap_update_put_rack_info fail, result is %d\n", rs);
		json_free(req);
		return NULL;
	}
	json_free(req);

	rs = libwrap_put_rack(put_rack_info);
	if (rs != RESULT_OK) {
		HTTPD_ERR("rack put fail, result is %d\n", rs);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	rc = libwrap_update_rack_mtime();
	if (rc != RESULT_OK) {
		rmm_log(ERROR, "memdb set rack update date fail:%d\n", rc);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return rack_get(param);
	}

	rf_log(INFO, MSGRackResourceUpdated, RMM_JSON_UPDATED_BIG);

	return rack_get(param);
}

static input_attr_t drawer_reset_attrs[] = {
    {"TargetIndex", NULL},
    {"ResetType", NULL}
};

static json_t *rack_post(struct rest_uri_param *param)
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
	json_t *result = NULL;
	int8 *type = NULL;
	json_t *drawer_num_json = NULL;
	int32 drawer_num = 0;
	uint32 ary_size = sizeof(drawer_reset_attrs)/sizeof(input_attr_t);

	#if 0
	rs = get_drawer_asset_idx(param, &dzone_idx, &drawer_idx);
	if (rs != RESULT_OK) {
		HTTPD_ERR("drawer index get fail\n");
		return NULL;
	}
	#endif

	req = json_parse(param->json_data);
	if (req == NULL) {
		HTTPD_ERR("json parse error\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	if (libwrap_check_input_attrs(drawer_reset_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	if (NULL == (drawer_num_json = json_object_get(req, RMM_JSON_TARGET_INDEX))) {
		HTTPD_ERR("fail to get json field.\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	drawer_num = json_integer_value(drawer_num_json);
	if (drawer_num == -1) {
		HTTPD_ERR("fail to get json number.\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		goto err;
	}

	if (drawer_num<1 || drawer_num>(MAX_DRAWER_NUM*MAX_MBP_PRESENCE)) {
		HTTPD_ERR("drawer_num is incorrect.\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	if (drawer_num%MAX_DRAWER_NUM != 0) {
		dzone_idx = drawer_num/MAX_DRAWER_NUM+1;
		drawer_idx = drawer_num%MAX_DRAWER_NUM;
	} else {
		dzone_idx = drawer_num/MAX_DRAWER_NUM;
		drawer_idx = MAX_DRAWER_NUM;
	}

	if (NULL == (type_json = json_object_get(req, RMM_JSON_RESET_TYPE))) {
		HTTPD_ERR("fail to get json field.\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	type = json_string_value(type_json);
	if (type == NULL) {
		HTTPD_ERR("fail to get json string.\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		goto err;
	}

	HTTPD_ERR("-----------------num %d, dzone_idx %d, drawer_idx %d, type:%s\n", 
		drawer_num, dzone_idx, drawer_idx, type);

	if(0 == strcmp(type, RMM_JSON_RESET_SOFT_STR))
		rs = libwrap_drawer_soft_reset(dzone_idx, drawer_idx, &reset_drawer_result);
	else if(0 == strcmp(type, RMM_JSON_RESET_HARD_STR))
		rs = libwrap_drawer_hard_reset(dzone_idx, drawer_idx, &reset_drawer_result);
	else {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	if (rs == RESULT_OK)
		update_response_info(param, HTTP_NO_CONTENT);
	else if (rs == RESULT_NO_NODE) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		goto err;
	} else {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		goto err;
	}

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", (dzone_idx -1)* MAX_DRAWER_NUM + drawer_idx);
	rf_snmp_evt(INFO, MSGDrawerStatusChange, buff, "Reseted");

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	return result;

err:
	json_free(req);
	return NULL;
	
}


static json_t *log_get(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *log_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *log_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *rf_log_get(struct rest_uri_param *param) {
	json_t* result = NULL;
	json_t* json_log = NULL;
	int8* p_count;
	rf_msg_info_t* info = NULL;
	void* offset = NULL;
	int32 count, index = 0;

	update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
	p_count = rest_query_value(param, "count");

	if (p_count == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto expt_end;
	}

	info = (rf_msg_info_t*) malloc(sizeof(rf_msg_info_t));

	if (!info) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		goto expt_end;
	}

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("result json object request error\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		goto expt_end;
	}

	count = strtoul(p_count, 0, 10);

	rf_log_get_by_limit(count, &info->dsc, info->data);

	json_object_add(result, RMM_JSON_COUNT, json_integer(info->dsc.count));
	json_object_add(result, RMM_JSON_LEN, json_integer(info->dsc.length));

	json_log = json_array();
	if (NULL != json_log) {
		if (info->dsc.count < 1024) {
			offset = (void*) info->data;
			while (index < info->dsc.count) {
				json_array_add(json_log, json_string((int8*) (offset + index * 256)));
				index++;
			}
			json_object_add(result, RMM_JSON_LOG_INFO, json_log);

		}
		else {
			json_array_add(json_log, json_string("log request size is too long"));
			json_object_add(result, RMM_JSON_LOG_INFO, json_log);
		}

		free(info);
		update_response_info(param, HTTP_OK);
		return result;
	}

	expt_end:
	if (info) {
		free(info);
	}
	if (result) {
		json_free(result);
	}
	if (json_log) {
		json_free(json_log);
	}
	HTTPD_ERR("file %s line %d: %s fail\n", __FILE__, __LINE__, __func__);

	return NULL;
}

static json_t *rf_log_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *rf_log_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *log_service_get(struct rest_uri_param *param)
{
	rf_log_svc_t service = { { {0} } };
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("result json object request error\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *rack_snmp_listener_get(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return	NULL;
}

static json_t *rack_snmp_listener_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *rack_snmp_listener_post(struct rest_uri_param *param)
{
	json_t *req = NULL;

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	update_response_info(param, HTTP_OK);

	return process_snmp_listener(req);
}

static json_t *rack_platform_get(struct rest_uri_param *param)
{
	json_t *result = NULL;
	int32 reg = 0;
	int8 *platform;

	if (is_platform_bdcr())
		platform = PLATFORM_NAME_BDC_R;
	else
		platform = PLATFORM_NAME_BDC_A;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request error\n");
		return NULL;
	}

	add_json_string(result, RMM_JSON_PLATFORM, platform);

	update_response_info(param, HTTP_OK);
	return result;
}

static struct rest_handler chassis_handler = {
	.get	= chassis_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler metadata_handler = {
	.get	= metadata_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};


static struct rest_handler rack_handler = {
	.get	= rack_get,
	.put	= NULL,
	.patch	= rack_patch,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler rack_drawer_reset_handler = {
	.get	= NULL,
	.put	= NULL,
	.patch	= NULL,
	.post	= rack_post,
	.del	= NULL,
};


static struct rest_handler service_root_handler = {
	.get	= service_root_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler log_handler = {
	.get	= log_get,
	.put	= NULL,
	.patch	= log_patch,
	.post	= log_post,
	.del	= NULL,
};


static struct rest_handler rf_log_handler = {
	.get	= rf_log_get,
	.put	= NULL,
	.patch	= rf_log_patch,
	.post	= rf_log_post,
	.del	= NULL,
};

static struct rest_handler log_service_handler = {
	.get	= log_service_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler rack_platform_handler = {
	.get	= rack_platform_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler rack_snmp_listener_handler = {
	.get	= rack_snmp_listener_get,
	.put	= NULL,
	.patch	= rack_snmp_listener_patch,
	.post	= rack_snmp_listener_post,
	.del	= NULL,
};

