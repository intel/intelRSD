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
#include <pthread.h>
#include "libcfg/cfg.h"
#include "libredfish/msg_reg.h"
#include "libutils/time.h"
#include "libutils/base64.h"

static struct rest_handler rf_evt_handler;
static struct rest_handler rf_listener_handler;
static struct rest_handler rf_listeners_handler;
static struct rest_handler rf_mr_handler;

void register_redfish_handler(void)
{
	register_handler(RF_EVT_URL,			&rf_evt_handler);
	register_handler(RF_LISTENERS_URL, 		&rf_listeners_handler);
	register_handler(RF_LISTENER_URL,		&rf_listener_handler);
	register_handler(MSG_REG_URL,			&rf_mr_handler);
}

static json_t *rf_evt_get(struct rest_uri_param *param)
{
	rf_evt_svc_t service = {};
	json_t *result = NULL;
	int32 rf_evt_support[MAX_EVT_ACTION_NUM] = {};

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request error\n");
		return NULL;
	}

	if (rf_evt_info_init(&service, rf_evt_support, RF_EVENT_ROOT_FMT)!= 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("get redfish evt info error\n");
		return NULL;
	}

	if (rf_evt_pack_json(result, rf_evt_support, &service) != 0) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack redfish evt info error\n");
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *rack_evt_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static input_attr_t sub_listener_attrs[] = {
	{"Name",         NULL},
	{"Description",  NULL},
	{"Destination",  NULL},
	{"EventTypes",   NULL},
	{"Context",      NULL},
	{"Protocol",     NULL}
};

static json_t *rf_listeners_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *obj = NULL;
	int32 mask = 0;
	listener_dest_t listener;
	int32 listener_idx = 0;
	uint32 ary_size = sizeof(sub_listener_attrs)/sizeof(input_attr_t);

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	if (libwrap_check_input_attrs(sub_listener_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto out;
	}

	obj = libwrap_get_attr_json(sub_listener_attrs, ary_size, "Protocol");
	if (obj) {
		int8 *input = NULL;
		input = json_string_value(obj);
		if (input && check_str_len(input, REST_EVENT_PROTOCOL)) {
			if (strncmp("Redfish", input, strnlen_s("Redfish", RSIZE_MAX_STR)) !=0 ) {
				update_response_info(param, HTTP_BAD_REQUEST);
				goto out;
			}
		} else {
			update_response_info(param, HTTP_BAD_REQUEST);
			goto out;
		}
	}

	mask |= RF_EVENT_MASK_ALL;

	if (process_listener(req, &listener, mask) != 0) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("process listener error\n");
		goto out;
	}

	listener_idx = libwrap_get_evt_listener_idx(mask, listener.dest);
	snprintf_s_si(param->rsp, HREF_URL_LEN, "http://%s/redfish/v1/EventService/Subscriptions/%d", param->host, listener_idx);
	update_response_info(param, HTTP_CREATED);

out:
	if (req)
		json_free(req);
	return NULL;
}

static json_t *rf_listeners_get(struct rest_uri_param *param)
{
	evt_listeners_t listeners = {};
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json object request fail\n");
		return NULL;
	}

	if (evt_listeners_init(&listeners, RF_EVENT_MASK_ALL, RF_EVENT_ROOT_FMT) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("mbp coll listeners info init fail\n");
		return NULL;
	}

	if (evt_listeners_pack_json(result, &listeners) != 0) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("mbp coll listeners pack json fail\n");
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *rf_listener_get(struct rest_uri_param *param)
{
	int8 *p_id = NULL;
	int32 listener_id = 0;
	int8 buff[8] = {0};
	result_t rs = RESULT_OK;
	evt_listener_t listener = {};
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json object request fail\n");
		return NULL;
	}

	p_id = rest_path_value(param, "listener_id");
	if (p_id == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get value fail\n");
		return NULL;
	}
	listener_id  = (int32)str2int(p_id);

	if (evt_listener_init(&listener, RF_EVENT_MASK_ALL, listener_id, RF_EVENT_ROOT_FMT) != 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("mbp listener info init fail\n");
		return NULL;
	}

	if (evt_listener_pack_json(result, &listener) != 0) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("mbp listener pack json fail\n");
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *rf_listener_del(struct rest_uri_param *param)
{
	int8 *p_id = NULL;
	int32 listener_id = 0;
	int8 buff[8] = {0};
	result_t rs = RESULT_OK;
	evt_listener_t listener = {};

	p_id = rest_path_value(param, "listener_id");
	if (p_id == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get value fail\n");
		return NULL;
	}
	listener_id  = (int32)str2int(p_id);

	if (libwrap_del_evt_listener(RF_EVENT_MASK_ALL, listener_id) != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("mbp listener info del fail\n");
		return NULL;
	}

	update_response_info(param, HTTP_NO_CONTENT);
	return NULL;
}

static json_t *rf_mr_get(struct rest_uri_param *param)
{
	json_t *result = NULL;

	update_response_info(param, HTTP_OK);
	result = rf_get_standard_msg_reg_json();
	return result;
}

static struct rest_handler rf_evt_handler = {
	.get	= rf_evt_get,
	.put	= NULL,
	.patch	= rack_evt_patch,
	.post	= NULL,
	.del	= NULL,
};

static struct rest_handler rf_listeners_handler = {
	.get	= rf_listeners_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= rf_listeners_post,
	.del	= NULL,
};

static struct rest_handler rf_listener_handler = {
	.get	= rf_listener_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= rf_listener_del,
};

static struct rest_handler rf_mr_handler = {
	.get	= rf_mr_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= NULL,
	.del	= NULL,
};


