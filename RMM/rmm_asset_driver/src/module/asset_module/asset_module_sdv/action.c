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

#include "action.h"
#include "notify.h"
#include "liblog/log.h"
#include "general.h"
#include "libsecurec/safe_lib.h"
#include <pthread.h>


handler_tbl_t action_handler_tbl[] = {
	[AMACTION_SET_FAN_PWM] = {false, NULL, 0, {0}},
	[AMACTION_RESET_HARD_RESET] = {false, do_peripheral_hard_reset, 0, {0}},
	[AMACTION_POST_PSU_BY_NODE_ID] = {false, do_post_psu_by_node_id, 0, {0}},
	[AMACTION_MBP_FW_UPDATE] = {false, do_cm_update_mbp_firmware, 0, {0}},
	[AMACTION_MBP_UART_SWITCH] = {false, do_uart_switch, 0, {0}},
	[AMACTION_PSME_SOFT_RESET] = {false, do_psme_soft_reset, 0, {0}},
	[AMACTION_SET_RACK_PUID] = {false, NULL, 0, {0}},
};



pthread_mutex_t action_hdl_lock;

void lock_handler_tbl()
{
	pthread_mutex_lock(&action_hdl_lock);
}

void unlock_handler_tbl()
{
	pthread_mutex_unlock(&action_hdl_lock);
}

void set_handler_fn(uint8 fn_id, int8 param_num, ...)
{
	uint8 i;
	lock_handler_tbl();
	va_list args;
	va_start(args, param_num);
	for (i=0; i<param_num; i++) {
		action_handler_tbl[fn_id].params[i] = va_arg(args, jrpc_data_integer);
	}
	va_end(args);
	action_handler_tbl[fn_id].param_num = param_num;
	action_handler_tbl[fn_id].set_flag = true;
	unlock_handler_tbl();
}

int get_handler_fn(uint8 fn_id, jrpc_data_integer *params)
{
	int i;
	lock_handler_tbl();
	for (i=0; i<action_handler_tbl[fn_id].param_num; i++) {
		*(params+i) = action_handler_tbl[fn_id].params[i];
	}

	/* clean */
	action_handler_tbl[fn_id].param_num = 0;
	memset(action_handler_tbl[fn_id].params, 0, sizeof(action_handler_tbl[fn_id].params));
	action_handler_tbl[fn_id].set_flag = false;
	unlock_handler_tbl();

	return i;
}



#if 0
void set_handler_fn_arg(int8 fn_id, int8 param_id, jrpc_data_integer value)
{
	action_handler_tbl[fn_id].params[param_id] = value;
}

jrpc_data_integer get_hander_fn_arg(int8 fn_id, int8 param_id)
{
	return action_handler_tbl[fn_id].params[param_id];
}

void set_handler_fn_flag(int8 fn_id)
{
	action_handler_tbl[fn_id].set_flag = true;
}

void clear_handler_fn_flag(int8 fn_id)
{
	action_handler_tbl[fn_id].set_flag = false;
}
#endif

struct reset_param {
	int host;
	int cm_lid;
};


struct uart_param {
	int host;
	int tgt_lid;
	int tgt_tray;
};


int get_array_elem(jrpc_req_pkg_t *req_pkg, char *attr_name, json_type type, void *value_ptr)
{
	int i = 0;
	json_t *array = NULL;
	int array_size = 0;
	char *name = NULL;

	json_t *elem = json_object();
	json_t *value = json_object();

	jrpc_get_named_param_value(req_pkg->json, JRPC_ACTION_ATTR, JSON_ARRAY, (void *)&array);
	array_size = json_array_size(array);

	for (; i<array_size; i++) {
		elem = json_array_get(array, i);
		name = json_string_value(json_object_get(elem, JRPC_ACTION_ATTR_NAME));

		if (name && strcmp(name, attr_name) == 0) {
			value = json_object_get(elem, JRPC_ACTION_VALUE);

			switch(type) {
				case JSON_INTEGER:
					*(jrpc_data_integer *)value_ptr = json_integer_value(json_object_get(value, JRPC_ACTION_VALUE_NUM));
					break;
				case JSON_STRING:
					*(jrpc_data_string *)value_ptr = json_string_value(json_object_get(value, JRPC_ACTION_VALUE_STR));
					break;
				case JSON_TRUE:
					*(jrpc_data_integer *)value_ptr = json_integer_value(json_object_get(value, JRPC_ACTION_VALUE_NUM));
					break;

				default:
					return -1;
			}

			return 0 ;
		}
		else
			continue;
	}

	return -1;
}

#if 0
int get_fan_pwm_by_node_id_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	char buff[32] = {0};
	json_t *status = NULL;
	json_t * resp = (json_t *)cb_data;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}

	status = json_object();
	if (status == NULL) {
		return -1;
	}
	add_json_string(status, JRPC_STATE, (unsigned char *)JRPC_STATE_ENABLED);
	add_json_string(status, JRPC_HEALTH, (unsigned char *)JRPC_HEALTH_OK);
	json_object_add(resp, JRPC_STATUS, status);

	snprintf(buff, sizeof(buff), "%d", rsp[1]);
	json_object_add(resp, JRPC_FAN_PWM, json_string(buff));

	return 0;
}


int get_fan_pwm_by_id(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	struct jipmi_msg ipmi_req = {};
	unsigned int host = 0;
	int64 error_code = 0;
	int fan_lid;
	am_node_t **nptrs = NULL;
	int8 *uuid = NULL;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;

	if (jrpc_get_named_param_value(req_pkg->json, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS) {
		rmm_log(ERROR, "get uuid fain\n");
		return -1;
	}

	if (false == find_lid_from_uuid(nptrs, FANS_NODE, uuid, &fan_lid, &host, NULL)) {
		rmm_log(ERROR, "find fan lid fail\n");
		return -1;
	}

	ipmi_req.data[0] = fan_lid - 1;

	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		GET_FAN_PWM_CMD);
	FILL_STR(ipmi_req.name,		am_rmcp_username);
	FILL_STR(ipmi_req.password,	am_rmcp_password);
	FILL_INT(ipmi_req.data_len,	1);

	return libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &ipmi_req, get_fan_pwm_by_node_id_cb, resp, JIPMI_SYNC);
}
#endif

int set_fan_pwm_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}
	
	return 0;
}

void process_actions(am_node_t *cm)
{
	uint8 i;
	uint32 set_flags = 0;

	/* form bitmap */
	lock_handler_tbl();
	for (i=0; i<sizeof(action_handler_tbl)/sizeof(action_handler_tbl[0]); i++) {
		if (action_handler_tbl[i].set_flag && action_handler_tbl[i].do_fn!=NULL)
			set_flags |= 1<<i;
	}
	unlock_handler_tbl();

	/* call handler function */
	for (i=0; i<sizeof(action_handler_tbl)/sizeof(action_handler_tbl[0]); i++) {
		if (set_flags & 1<<i) {
			action_handler_tbl[i].do_fn();
			rmm_log(DBG, "action available on id %d.\n", i);
		}
	}

	process_next_func(cm);
	return;
}


int action_set_tzone_pwm(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	struct jipmi_msg ipmi_req = {};
	unsigned int  host = 0;
	int64 error_code = 0;
	int32 fan_lid;
	jrpc_data_integer pwm;
	am_node_t **nptrs = NULL;
	int8 *uuid = NULL;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;

	if (get_array_elem(req_pkg, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS)
		return -1;

	if (get_array_elem(req_pkg, JRPC_PWM, JSON_INTEGER, &pwm) != 0)
		return -1;

	if (false == record_fan_pwm(nptrs, uuid, (char)pwm))
		return -1;

	return RESULT_OK;
}


int set_rack_puid(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	struct jipmi_msg ipmi_req = {};
	unsigned int  host = 0;
	int64 error_code = 0;
	jrpc_data_integer puid;

	if (NULL == args)
		return -1;

	if (get_array_elem(req_pkg, JRPC_RACK_PUID, JSON_INTEGER, &puid) != 0)
		return -1;
	
	record_rack_puid((int)puid);

	return RESULT_OK;
}



int serial_inf_close_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	return 0;
}


int set_psu_by_node_id_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	struct jipmi_retry_msg *retry_msg = (struct jipmi_retry_msg *)cb_data;

	if (retry_msg == NULL)
		return -1;
#if 0
	printf("set psu by node id RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++) {
		printf("%02X ", rsp[i]);
	}
	printf("\n");
#endif
	if ((rsp[0] != IPMI_CC_OK) && (retry_msg->retry != 0)) {
		retry_msg->retry -= 1;
		libjipmi_rmcp_cmd(retry_msg->host, IPMI_RMCP_PORT, &retry_msg->msg, set_psu_by_node_id_cb, &retry_msg, JIPMI_SYNC);
	}

	if (rsp[0] == IPMI_CC_OK)
		printf("PSU operation successful\n");

	return 0;
}


int session_close_rsp_handler(int result, unsigned char *rsp, int rsp_len,
										  void *cb_data)
{
	int rc = -1;
	struct reset_param *param = (struct reset_param *)cb_data;

	if(rsp[0] == IPMI_CC_OK) {
		struct jipmi_serial_msg req = {};
		char cm_dev[64] = {0};
		int64 error_code = 0;

		req.serial_flag = IPMI_SERIAL_CLOSE_DEV;
		FILL_INT(req.msg_base.netfn,	OEM_IPMI_NETFN);
		FILL_INT(req.msg_base.cmd,		OEM_SERIAL_CLOSE_CMD);

		snprintf_s_i(cm_dev, sizeof(cm_dev), "/dev/ttyCm%dIPMI", param->cm_lid);
		memcpy_s(req.msg_base.data, sizeof(req.msg_base.data), cm_dev, strnlen_s(cm_dev, sizeof(cm_dev)-1));
		FILL_INT(req.msg_base.data_len,	strnlen_s(cm_dev, sizeof(cm_dev)-1));

		rc = libjipmi_serial_cmd(&req, serial_inf_close_rsp_cb, NULL, JIPMI_SYNC);
		free(param);
		return rc;
	} else {
		free(param);
		return -1;
	}
}

int peripheral_reset_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int rc = -1;
	int host;
	struct reset_param *param = (struct reset_param *)cb_data;
	int64 error_code = 0;
	struct jipmi_msg ipmi_req = {};

	if(rsp[0] == IPMI_CC_OK) {
		FILL_INT(ipmi_req.netfn,	IPMI_RMCP_CLOSE_SESSION_NETFN);
		FILL_INT(ipmi_req.cmd,		IPMI_RMCP_CLOSE_SESSION_CMD);
		FILL_STR(ipmi_req.name,		am_rmcp_username);
		FILL_STR(ipmi_req.password,	am_rmcp_password);
		FILL_INT(ipmi_req.data_len,	0);

		rc = libjipmi_rmcp_close_session(param->host, IPMI_RMCP_PORT, &ipmi_req, session_close_rsp_handler, param, JIPMI_SYNC);
		rmm_log(INFO, "resp is %d\n", rsp[0]);
		return rc;
	} else {
		rmm_log(ERROR, "resp is %d\n", rsp[0]);
		return -1;
	}
}

int peripheral_hard_reset(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	int64 error_code = 0;
	struct jipmi_msg ipmi_req = {};
	int32 cm_lid;
	jrpc_data_integer peripheral_id;
	int32 serial_fd;
	unsigned int  host = 0;
	int8 *uuid = NULL;
	am_node_t **nptrs = NULL;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;
	
	if (get_array_elem(req_pkg, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS)
			return -1;

	if (false == find_lid_from_uuid(nptrs, CM_NODE, uuid, &cm_lid, &host, &serial_fd))
		return -1;

	if (host == 0)
		rmm_log(INFO, "uuid[%s] host[%d] serial[%d]\n", uuid, host, serial_fd);

	if (get_array_elem(req_pkg, JRPC_PERI_ID, JSON_INTEGER, &peripheral_id) != 0)
		return -1;

	set_handler_fn(AMACTION_RESET_HARD_RESET, 4, host, cm_lid, peripheral_id, serial_fd);

	rmm_log(DBG, "Peripheral reset, host %d, cm_lid %d, peripheral_id %d.\n", host, cm_lid, peripheral_id);

	return RESULT_OK;
}


int do_peripheral_hard_reset()
{
	int rc = 0;
	struct jipmi_msg ipmi_req = {};
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
	struct _fm_update_info *fm_update = NULL;
	unsigned int host = 0;
	int32 cm_lid;
	char peripheral_id;
	int32 serial_fd;

	if (get_handler_fn(AMACTION_RESET_HARD_RESET, params) != 4) {
		rmm_log(ERROR, "do actions params error.\n");
		return -1;
	}

	host = (typeof(host))params[0];
	cm_lid = (typeof(cm_lid))params[1];
	peripheral_id = (typeof(peripheral_id))params[2];
	serial_fd = (typeof(serial_fd))params[3];

	rmm_log(DBG, "Do peripheral reset, host %d, cm_lid %d, peripheral_id %d, serial fd %d\n", host, cm_lid, peripheral_id, serial_fd);
	
	struct reset_param *param = (struct reset_param *)malloc(sizeof(struct reset_param));

	if (param == NULL)
		return -1;

	memset(param, 0, sizeof(struct reset_param));

	param->host = host;
	param->cm_lid = cm_lid;

	ipmi_req.data[0] = peripheral_id;

	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		CHASSIS_RESET_CMD);
	FILL_STR(ipmi_req.name,		am_rmcp_username);
	FILL_STR(ipmi_req.password,	am_rmcp_password);
	FILL_INT(ipmi_req.data_len,	1);

	if (is_platform_bdcr() && peripheral_id != 0) {
		jipmi_serial_msg_t req = { };
		uint16 channel = peripheral_id - 1;

		FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
		FILL_INT(req.msg_base.cmd,		CHASSIS_RESET_CMD);
		FILL_STR(req.msg_base.name,		am_rmcp_username);
		FILL_STR(req.msg_base.password,	am_rmcp_password);
		FILL_INT(req.msg_base.data_len,	1);
		req.serial_flag = IPMI_SERIAL_OPERATION;
		req.serial_fd = serial_fd;
		req.msg_base.data[0] = 0x05;

		libjipmi_serial_br_cmd(&req, peripheral_reset_cb, param,
							0x24,
							channel,
							0x00,
							0x00,
							IPMI_BRIDGE_MSG_SINGLE,
							JIPMI_SYNC);
//		libjipmi_rmcp_cmd_send_msg(host, IPMI_RMCP_PORT, &ipmi_req, peripheral_reset_cb, param,
//									0x24,
//									peripheral_id,
//									0x00,
//									0x00,
//									IPMI_BRIDGE_MSG_SINGLE,
//									JIPMI_SYNC);
	} else
		rc = libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &ipmi_req, peripheral_reset_cb, param, JIPMI_SYNC);

	return rc;
}


int psme_soft_reset_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int rc = -1;
	int host;
	struct reset_param *param = (struct reset_param *)cb_data;
	int64 error_code = 0;

	rmm_log(INFO, "resp is %d\n", rsp[0]);
	
	if(rsp[0] == IPMI_CC_OK)
		return 0;
	else
		return -1;
}

int psme_soft_reset(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	int64 error_code = 0;
	struct jipmi_msg ipmi_req = {};
	int32 cm_lid;
	jrpc_data_integer drawer_lid;
	int32 serial_fd;
	unsigned int  host = 0;
	int8 *uuid = NULL;
	am_node_t **nptrs = NULL;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;
	
	if (get_array_elem(req_pkg, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS)
			return -1;

	if (false == find_lid_from_uuid(nptrs, CM_NODE, uuid, &cm_lid, &host, &serial_fd))
		return -1;

	if (host == 0)
		rmm_log(INFO, "uuid[%s] host[%d] serial[%d]\n", uuid, host, serial_fd);

	if (get_array_elem(req_pkg, JRPC_DRAWER_LID, JSON_INTEGER, &drawer_lid) != 0)
		return -1;

	set_handler_fn(AMACTION_PSME_SOFT_RESET, 4, host, cm_lid, drawer_lid, serial_fd);

	rmm_log(DBG, "PSME SOFT reset, host %d, cm_lid %d, drawer_lid %d, serial_fd %d.\n", host, cm_lid, drawer_lid, serial_fd);

	return RESULT_OK;
}


int do_psme_soft_reset()
{
	int rc = 0;
	//struct jipmi_msg ipmi_req = {};
	jipmi_serial_msg_t req = { };
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
	struct _fm_update_info *fm_update = NULL;
	unsigned int host = 0;
	int32 cm_lid;
	uint8 drawer_lid;
	int32 serial_fd;

	if (get_handler_fn(AMACTION_PSME_SOFT_RESET, params) != 4) {
		rmm_log(ERROR, "do actions params error.\n");
		return -1;
	}

	host = (typeof(host))params[0];
	cm_lid = (typeof(cm_lid))params[1];
	drawer_lid = (typeof(drawer_lid))params[2];
	serial_fd = (typeof(serial_fd))params[3];

	rmm_log(DBG, "PSME SOFT reset, host %d, cm_lid %d, drawer lid %d, serial fd %d.\n", host, cm_lid, drawer_lid, serial_fd);

	FILL_INT(req.msg_base.netfn,	IPMI_APP_NETFN);
	FILL_INT(req.msg_base.cmd,		WARM_RESET_CMD);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	FILL_INT(req.msg_base.data_len, 0);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = serial_fd;

	libjipmi_serial_br_cmd(&req, psme_soft_reset_cb, NULL,
							0x10,
							0x2,
							0x24,
							(drawer_lid-1)*2,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_SYNC);

	return rc;
}


int post_psu_by_node_id(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	struct jipmi_msg ipmi_req = {};
	unsigned char opt_status;
	unsigned int  host, host1, host2;
	int64 error_code = 0;
	int psu_lid;
	jrpc_data_integer request_enabled_state;
	jrpc_data_integer pz_rack_index;
	am_node_t **nptrs = NULL;
	int8 *uuid = NULL;
	int32 psu_bitmap = 0;
	int32 status_bitmap = 0;
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;

	if (get_array_elem(req_pkg, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS)
			return -1;

	if (false == find_lid_from_uuid(nptrs, PSUS_NODE, uuid, &psu_lid, &host, NULL))
		return -1;

	if (get_array_elem(req_pkg, JRPC_PSU_ENABLE_STATE, JSON_INTEGER, &request_enabled_state) != 0)
		return -1;

	if (get_array_elem(req_pkg, JRPC_PZ_LID, JSON_INTEGER, &pz_rack_index) != 0)
		return -1;

	/* get bitmaps first */
	if (get_handler_fn(AMACTION_POST_PSU_BY_NODE_ID, params) == 4) {
		host1 = (typeof(host1))params[0];
		host2 = (typeof(host2))params[1];
		psu_bitmap = (typeof(psu_bitmap))params[2];
		status_bitmap = (typeof(status_bitmap))params[3];
	}

	if (request_enabled_state == PSU_STATE_ENABLED)
		opt_status = 0x1;
	else if (request_enabled_state == PSU_STATE_DISABLED)
		opt_status = 0x0;
	else
		opt_status = 0x1;

	if (pz_rack_index == 1) {
		host1 = host;
	} else {
		host2 = host;
		psu_lid += MAX_PSU_NUM;
	}

	psu_bitmap |= (1<<(psu_lid-1));
	status_bitmap = (status_bitmap & ~(1<<(psu_lid-1))) | (opt_status<<(psu_lid-1));

#if 0
	if (request_enabled_state == PSU_STATE_ENABLED)
		opt_code = 0x80;
	else if (request_enabled_state == PSU_STATE_DISABLED)
		opt_code = 0x00;
	else
		opt_code = 0x80;
#endif

	set_handler_fn(AMACTION_POST_PSU_BY_NODE_ID, 4, host1, host2, psu_bitmap, status_bitmap);

	printf("post PSU, host %d, psu_lid %d, psu bitmap 0x%x, status bitmap 0x%x.\n", host, psu_lid, psu_bitmap, status_bitmap);

	return RESULT_OK;
}

int do_post_psu_by_node_id()
{
	unsigned int host = 0, host1, host2;
	int psu_pos;
	int psu_id;
	unsigned char opt_code;
	int32 psu_bitmap = 0;
	int32 status_bitmap = 0;
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
	struct jipmi_msg ipmi_req = {};
	jipmi_retry_msg_t retry_msg = {};

	if (get_handler_fn(AMACTION_POST_PSU_BY_NODE_ID, params) != 4) {
		rmm_log(ERROR, "do actions params error.\n");
		return -1;
	}

	host1 = (typeof(host1))params[0];
	host2 = (typeof(host2))params[1];
	psu_bitmap = (typeof(psu_bitmap))params[2];
	status_bitmap = (typeof(status_bitmap))params[3];

	rmm_log(DBG, "post PSU, host %d, psu bitmap 0x%x, status bitmap 0x%x.\n", host, psu_bitmap, status_bitmap);

	for (psu_pos = 0; psu_pos < (MAX_PSU_NUM * 2); psu_pos++) {
		if (psu_bitmap & 1<<psu_pos) {
			if (psu_pos < MAX_PSU_NUM) {
				host = host1;
				psu_id = psu_pos;
			} else {
				psu_id = psu_pos - MAX_PSU_NUM;
				host = host2;
			}
			ipmi_req.data[0] = psu_id;
			ipmi_req.data[1] = PSU_WRITE_BYTE;
			ipmi_req.data[2] = 0x01;/* operation code for psu pmbus */
			ipmi_req.data[3] = BYTE_LEN;
			if (status_bitmap & 1<<psu_pos)
				ipmi_req.data[4] = 0x80;
			else
				ipmi_req.data[4] = 0x00;

			FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
			FILL_INT(ipmi_req.cmd,		SEND_PSU_CMD);
			FILL_STR(ipmi_req.name,		am_rmcp_username);
			FILL_STR(ipmi_req.password,	am_rmcp_password);
			FILL_INT(ipmi_req.data_len,	5);

			retry_msg.host = host;
			memcpy_s(&retry_msg.msg, sizeof(retry_msg.msg), &ipmi_req, sizeof(ipmi_req));
			retry_msg.retry = 5;
			rmm_log(DBG, "post PSU, pos %d, opt_code 0x%x.\n", psu_pos, ipmi_req.data[4]);
			printf("*********************host is %x, psu pos is %d, opt code is 0x%x\n", host, psu_pos,ipmi_req.data[4]);
			libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &ipmi_req, set_psu_by_node_id_cb, &retry_msg, JIPMI_SYNC);
		}
	}

	return 0;
}


static int uart_switch_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	struct jipmi_msg ipmi_req = {};
	struct uart_param *param = (struct uart_param *)cb_data;

	if(rsp[0] == IPMI_CC_OK || rsp[0] == TRUE) {
		if (is_platform_bdcr()) {
			int8 tgt_tray = ((2 * (param->tgt_tray - 1)) + 1);
			ipmi_req.data[0] = tgt_tray;

			FILL_INT(ipmi_req.netfn,    IPMI_CM_NETFN);
			FILL_INT(ipmi_req.cmd,      UART_SWITCH_CMD);
			FILL_STR(ipmi_req.name,     am_rmcp_username);
			FILL_STR(ipmi_req.password, am_rmcp_password);
			FILL_INT(ipmi_req.data_len, 1);
			libjipmi_rmcp_cmd(param->host, IPMI_RMCP_PORT, &ipmi_req, NULL, NULL, JIPMI_SYNC);
		}
		rmm_log(INFO, "resp is %d\n", rsp[0]);
		return 0;
	} else {
		rmm_log(ERROR, "resp is %d\n", rsp[0]);
		return -1;
	}
}

int uart_switch(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args)
{
	int64 error_code = 0;
	struct jipmi_msg ipmi_req = {};
	unsigned int host = 0;
	jrpc_data_integer tgt_lid = 0;
	jrpc_data_integer tray_lid = 0;
	int32 serial_fd = 0;
	am_node_t **nptrs = NULL;
	int8 *uuid = NULL;
	int32 mbp_lid = 0;

	if (NULL == args)
		return -1;
	nptrs = (am_node_t **)args;

	if (get_array_elem(req_pkg, JRPC_UUID, JSON_STRING, &uuid) != JSONRPC_SUCCESS)
			return -1;

	if (false == find_lid_from_uuid(nptrs, CM_NODE, uuid, &mbp_lid, &host, &serial_fd))
		return -1;

	if (get_array_elem(req_pkg, JRPC_TARGET_IDX, JSON_INTEGER, &tgt_lid) != 0)
		return -1;

	if (is_platform_bdcr()) {
		if (get_array_elem(req_pkg, JRPC_TRAY_IDX, JSON_INTEGER, &tray_lid) != 0)
			return -1;
	}

	set_handler_fn(AMACTION_MBP_UART_SWITCH, 4, host, tgt_lid, tray_lid, serial_fd);

	rmm_log(DBG, "Uart switch, host %d, tgt_lid %d, tray_lid %d.\n", host, tgt_lid, tray_lid);
	
	return RESULT_OK;
}

int do_uart_switch()
{
	unsigned int host = 0;
	jrpc_data_integer tgt_lid = 0;
	jrpc_data_integer tray_lid = 0;
	jrpc_data_integer serial_fd = 0;
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
	struct jipmi_msg ipmi_req = {};

	if (get_handler_fn(AMACTION_MBP_UART_SWITCH, params) != 4) {
		rmm_log(ERROR, "do actions params error.\n");
		return -1;
	}

	host = (typeof(host))params[0];
	tgt_lid = (typeof(tgt_lid))params[1];
	tray_lid = (typeof(tray_lid))params[2];
	serial_fd = (typeof(serial_fd))params[3];

	rmm_log(DBG, "Do uart switch, host %d, tgt_lid %d, tray_lid %d.\n", host, tgt_lid, tray_lid);

	struct uart_param *param = (struct uart_param *)malloc(sizeof(struct uart_param));

	if (param == NULL)
		return -1;

	memset(param, 0, sizeof(struct uart_param));

	ipmi_req.data[0] = (char)tgt_lid;

	param->host = (int)host;
	param->tgt_lid = (int)tgt_lid;
	param->tgt_tray = (int)tray_lid;

	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		UART_SWITCH_CMD);
	FILL_STR(ipmi_req.name,		am_rmcp_username);
	FILL_STR(ipmi_req.password,	am_rmcp_password);
	FILL_INT(ipmi_req.data_len,	1);
	if (is_platform_bdcr()) {
		jipmi_serial_msg_t req = { };
		unsigned short channel = 0;
		channel = (unsigned short)((tray_lid - 1) * 2);

		FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
		FILL_INT(req.msg_base.cmd,		UART_SWITCH_CMD);
		FILL_STR(req.msg_base.name,		am_rmcp_username);
		FILL_STR(req.msg_base.password,	am_rmcp_password);
		req.msg_base.data[0] = (char)tgt_lid;
		FILL_INT(req.msg_base.data_len,	1);
		req.serial_flag = IPMI_SERIAL_OPERATION;
		req.serial_fd = serial_fd;

//		libjipmi_rmcp_cmd_send_msg(host, IPMI_RMCP_PORT, &ipmi_req, uart_switch_cb, param,
//							0x24,
//							channel,
//							0x00,
//							0x00,
//							IPMI_BRIDGE_MSG_SINGLE,
//							JIPMI_SYNC);
		libjipmi_serial_br_cmd(&req, uart_switch_cb, param,
							0x24,
							channel,
							0,
							0,
							IPMI_BRIDGE_MSG_SINGLE,
							JIPMI_SYNC);
	} else
		libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &ipmi_req, uart_switch_cb, NULL, JIPMI_SYNC);

	return RESULT_OK;
}



