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


#include "handler.h"
#include "libcfg/cfg.h"
#include "libwrap/wrap.h"

#include <stdlib.h>  
#include <unistd.h> 

static struct rest_handler mbp_coll_handler;
static struct rest_handler mbp_handler;
static struct rest_handler mbp_uart_handler;
static struct rest_handler mbp_update_handler;

void register_mbp_handler(void)
{
	register_handler(MBP_COLL_URL,						&mbp_coll_handler);
	register_handler(MBP_URL,							&mbp_handler);
	register_handler(MBP_ACTION_SET_UART_TARGET_URL,	&mbp_uart_handler);
	register_handler(MBP_ACTION_UPDATE_URL,				&mbp_update_handler);
}

static int32 process_firmware_update(int32 idx, int8 *dev, int8 *data, int32 length)
{
	int32 rc = -1;
	int8 *w_buf = NULL;
	int32 w_fd = -1;
	int32 len = 0;
	int32 offset = 0;
	int32 decode_size = 0;
	int32 buff_len = 0;
	struct stat sb;
	int8 file_name[128] = {0};

	if ((data == NULL) || (length <= 0))
		return -1;

	snprintf_s_ss(file_name, sizeof(file_name), "%s/%s", FIRMWARE_FILE_PATH, FIRMWARE_FILE_NAME);

	if (stat(FIRMWARE_FILE_PATH, &sb) == -1) {
		if (errno == ENOENT)
			mkdir(FIRMWARE_FILE_PATH, 0777);
	}

	w_fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (w_fd < 0) {
		perror("process update file w");
		return -1;
	}

	buff_len = length;

	while (offset < buff_len) {
		/* Decode the file first. */
		decode_size = ((offset + MAX_READ_BASE64_SIZE)
					   < buff_len ? MAX_READ_BASE64_SIZE
					   : (buff_len - offset));

		w_buf = base64_decode(data + offset, decode_size, &len);
		if (NULL == w_buf)
			goto err;

		offset += decode_size;

		rc = write(w_fd, w_buf, len);
		if (rc < 0)
			goto err;

		free(w_buf);
		w_buf = NULL;
	}

	close(w_fd);

	return libwrap_update_mbp_firmware(idx, dev, FIRMWARE_FILE_NAME);

err:
	if (w_buf != NULL) {
		free(w_buf);
		w_buf = NULL;
	}

	close(w_fd);
	remove(file_name);

	return -1;
}

static json_t *mbp_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

 	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	rs = libwrap_pack_mbp_coll_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack mbp collection fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *mbp_coll_patch(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *mbp_coll_post(struct rest_uri_param *param)
{
	update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
	return NULL;
}

static json_t *mbp_get(struct rest_uri_param *param)
{
	int32 idx = 0;
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mbp_member_t mbp_member = { {0} };

	idx = get_asset_idx(param, "mbp_id", MC_TYPE_CM);
	if (idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get mbp index fail\n");
		return NULL;
	}

	if (format_uuid(param->url, strnlen_s(param->url, sizeof(param->url))) == -1) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("format url uuid fail\n");
		return NULL;
	}

	rs = libwrap_get_mbp_by_idx(idx, &mbp_member);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("get mbp info fail, result is %d\n", rs);
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	rs = libwrap_pack_mbp_to_json(result, &mbp_member, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack mbp fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *mbp_patch(struct rest_uri_param *param)
{
	json_t *req;
	result_t rs = RESULT_OK;
	put_mbp_t put_mbp_info = { {0} };
	int32 idx;

	update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);

	idx = get_asset_idx(param, "mbp_id", MC_TYPE_CM);
	if (idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get mbp index fail\n");
		return NULL;
	}

	rs = libwrap_pre_put_mbp(idx, &put_mbp_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("pre put error, result is %d\n", rs);
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	req = json_parse(param->json_data);
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("parse json data error\n");
		return NULL;
	}

	rs = libwrap_update_put_mbp_info(req, &put_mbp_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("libwrap_update_put_mbp_info fail, result is %d\n", rs);
		json_free(req);
		return NULL;
	}
	json_free(req);

	rs = libwrap_put_mbp(idx, put_mbp_info);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("libwrap_put_mbp error, result is %d\n", rs);
		return NULL;
	}

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", idx);
	rf_log(INFO, MSGMBPResourceUpdated, buff, RMM_JSON_UPDATE);
	update_response_info(param, HTTP_OK);
	return mbp_get(param);
}

static json_t *mbp_set_uart_target(json_t *req, int32 cm_lid, struct rest_uri_param *param)
{
	int64 reset_type = 0;
	result_t rs = RESULT_OK;
	int32 reset_mode = 0;
	int32 reset_sys_result;
	int8  *tgt_component;
	int32 tray_idx = 0;
	int32 tgt_idx = 0;
	json_t *result = NULL;

	update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);

	if (get_subnode_id_by_lid(cm_lid, MC_NODE_ROOT, MC_TYPE_CM) == 0) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		return NULL;
	}

	if (is_platform_bdcr()) {
		tray_idx = json_integer_value(json_object_get(req, RMM_JSON_TARGET_TRAY));
		if (tray_idx == -1 || tray_idx<1 || tray_idx>(MAX_DRAWER_NUM)) {
			update_response_info(param, HTTP_BAD_REQUEST);
			HTTPD_ERR("get tray index error\n");
			json_free(req);
			return NULL;
		}
	}

	tgt_component = json_string_value(json_object_get(req, RMM_JSON_TARGET_COMPONET));
	if (tgt_component == NULL) {
		HTTPD_ERR("get target component error\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(req);
		return NULL;
	}

	if (strcmp(tgt_component, "MMP") == 0)
		tgt_idx = 0;
	else if (strcmp(tgt_component, "SLED1") == 0)
		tgt_idx = 1;
	else if (strcmp(tgt_component, "SLED2") == 0)
		tgt_idx = 2;
	else if (strcmp(tgt_component, "SLED3") == 0)
		tgt_idx = 3;
	else if (strcmp(tgt_component, "SLED4") == 0)
		tgt_idx = 4;
	else if (strcmp(tgt_component, "CPP") == 0)
		tgt_idx = 5;
	else {
		HTTPD_ERR("Invalid input\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(req);
		return NULL;
	}

	rs = libwrap_mbp_uart_switch(cm_lid, tray_idx, tgt_idx);

	if (rs != RESULT_OK) {
		if (rs == RESULT_NO_NODE)
			update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		else
			update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("set uart switch fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	json_free(req);

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d",cm_lid);
//	rf_snmp_evt(INFO, MSGMbpStatusChange, buff, "set uart target");

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	json_object_add(result, RMM_JSON_RESULT, json_string("Success"));
	return result;
}

static json_t *mbp_process_update(json_t *req, int32 idx, struct rest_uri_param *param)
{
	json_t *result = NULL;
	int8 *image_data = NULL;
	json_t *image_obj = NULL;
	int32 rc = -1;
	int32 cm_lid = 0;
	int8 cm_dev[64] = {0};

	/* check update capability */
	if(libwrap_check_update_capability() != RESULT_OK) {
		update_response_info(param, HTTP_METHOD_NOT_ALLOWED);
		HTTPD_ERR("firmware update is not supported.\n");
		return NULL;
	}

	if (!libwrap_get_firmware_update_status()) {
		HTTPD_ERR("get firmware update status fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	libwrap_set_firmware_update_status(0);

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		libwrap_set_firmware_update_status(1);
		return NULL;
	}
	rc = libwrap_check_tftp_service();
	if (rc == -1) {
		json_free(req);
		json_object_add(result, RMM_JSON_FRU_RESULT, json_string("tftp server not ready"));
		libwrap_set_firmware_update_status(1);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return result;
	}

	image_obj = json_object_get(req, RMM_JSON_IMAGE);
	if (image_obj == NULL) {
		json_free(req);
		json_object_add(result, RMM_JSON_FRU_RESULT, json_string("invalid image"));
		update_response_info(param, HTTP_BAD_REQUEST);
		libwrap_set_firmware_update_status(1);
		return result;
	}

	image_data = json_string_value(image_obj);
	if (image_data == NULL) {
		json_free(req);
		json_object_add(result, RMM_JSON_FRU_RESULT, json_string("invalid image"));
		update_response_info(param, HTTP_BAD_REQUEST);
		libwrap_set_firmware_update_status(1);
		return result;
	}

	cm_lid = idx;
	if ((cm_lid == 1) || (cm_lid == 2)) {
		snprintf_s_i(cm_dev, sizeof(cm_dev), "/dev/ttyCm%dIPMI", cm_lid);

		if (access(cm_dev, R_OK) == 0)
			rc = process_firmware_update(cm_lid, cm_dev, image_data, strnlen_s(image_data, RSIZE_MAX_STR));
		else {
			HTTPD_ERR("cm not exist\n");
			update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
			json_free(req);
			libwrap_set_firmware_update_status(1);
			return NULL;
			}
	} else {
		HTTPD_ERR("invalid cm loc id\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		json_free(req);
		libwrap_set_firmware_update_status(1);
		return NULL;
	}

	if (rc == 0) {
		json_object_add(result, RMM_JSON_FRU_RESULT, json_string(RMM_JSON_UPDATING));
		update_response_info(param, HTTP_ACCEPTED);
	} else {
		libwrap_set_firmware_update_status(1);
	}

	json_free(req);

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", idx);
	rf_snmp_evt(INFO, MSGMBPStatusChange, buff, RMM_JSON_UPDATE_BIG);

#ifdef REST_HTTPS
	send_json_reply_redirect(param->fd, param->https_url);
#else
	send_json_reply_redirect(param->fd, param->url);
#endif
	return NULL;
}

static json_t *mbp_update_get(struct rest_uri_param *param)
{
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	if (!libwrap_get_firmware_update_status()) {
		json_object_add(result, RMM_JSON_STATUS, json_string(RMM_JSON_UPDATING));
	} else {
		json_object_add(result, RMM_JSON_STATUS, json_string(RMM_JSON_UPDATED));
	}

	update_response_info(param, HTTP_OK);

	return result;
}

static input_attr_t mbp_update_attrs[] = {
	{"Image", NULL}
};

static json_t *mbp_update_post(struct rest_uri_param *param)
{
	json_t *req;
	int8 *action = NULL;
	int32 cm_lid;
	uint32 ary_size = sizeof(mbp_update_attrs)/sizeof(input_attr_t);

	cm_lid = get_asset_idx(param, "mbp_id", MC_TYPE_CM);
	if (cm_lid == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get cm loc lid fail\n");
		return NULL;
	}

	if (cm_lid < 1 || cm_lid>MAX_MBP_PRESENCE) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("CM lid %d not valid.\n", cm_lid);
		return NULL;
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	if (libwrap_check_input_attrs(mbp_update_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	HTTPD_INFO("processing mbp update\n");
	return mbp_process_update(req, cm_lid, param);

err:
	if (req)
		json_free(req);

	return NULL;

}

static input_attr_t switch_uart_attrs[] = {
	{"TargetComponent", NULL},
	{"TargetTray", NULL}
};

static json_t *mbp_uart_post(struct rest_uri_param *param)
{
	json_t *req;
	int32 cm_lid;
	uint32 ary_size = sizeof(switch_uart_attrs)/sizeof(input_attr_t);

	cm_lid = get_asset_idx(param, "mbp_id", MC_TYPE_CM);
	if (cm_lid == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get cm loc lid fail\n");
		return NULL;
	}

	if (cm_lid < 1 || cm_lid>MAX_MBP_PRESENCE) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("CM lid %d not valid.\n", cm_lid);
		return NULL;
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json parse error\n");
		return NULL;
	}

	if (libwrap_check_input_attrs(switch_uart_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	HTTPD_INFO("processing mbp set uart target\n");
	return mbp_set_uart_target(req, cm_lid, param);

err:
	if (req)
		json_free(req);

	return NULL;

}


static struct rest_handler mbp_coll_handler = {
	.get    = mbp_coll_get,
	.put    = NULL,
	.patch  = mbp_coll_patch,
	.post   = mbp_coll_post,
	.del    = NULL,
};

static struct rest_handler mbp_handler = {
	.get    = mbp_get,
	.put    = NULL,
	.patch  = mbp_patch,
	.post   = NULL,
	.del    = NULL,
};

static struct rest_handler mbp_update_handler = {
	.get    = mbp_update_get,
	.put    = NULL,
	.patch  = NULL,
	.post   = mbp_update_post,
	.del    = NULL,
};

static struct rest_handler mbp_uart_handler = {
	.get    = NULL,
	.put    = NULL,
	.patch  = NULL,
	.post   = mbp_uart_post,
	.del    = NULL,
};


