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

#include "notify.h"
#include "libutils/sock.h"
#include "libsecurec/safe_lib.h"


void update_cm_node_status(am_node_t *cm)
{
	cm_data_t *data = (cm_data_t *)(cm->priv_data);

	if (data->open_serial.status == FUNC_SUCCESS &&
		data->get_device_id.status == FUNC_SUCCESS &&
		data->get_cm_ip.status == FUNC_SUCCESS) {
		set_node_status(cm, ONLINE, false);
	}
}

int open_serial_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *cm = NULL;
	int        serial_fd = 0;
	cm_data_t *cm_data = NULL;

	/* sanity check */
	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	cm = (am_node_t *)cb_data;
	cm_data = (cm_data_t *)(cm->priv_data);

	if (IPMI_CC_OK != rsp[0]) {
		rmm_log(ERROR, "rsp[0]: %d.\n", rsp[0]);
		goto err;
	}
	if (rsp_len != 5) {
		rmm_log(ERROR, "rsp_len: %d.\n", rsp_len);
		goto err;
	}

	memcpy_s(&serial_fd, sizeof(serial_fd),rsp+1, rsp_len-1);
	if (serial_fd == INVALID_FD) {
		goto err;
	}

	cm_data->serial_fd = serial_fd;
	cm_data->open_serial.status = FUNC_SUCCESS;

	update_cm_node_status(cm);

	process_next_func(cm);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d, cm idx is %d, uuid is %s.\n", rsp[0], cm_data->loc_id, cm_data->uuid);
	notify_cm_remove(cm);
	set_fn_status(cm, FUNC_FAIL);
	set_node_status(cm, OFFLINE, true);
	/** once serial open fail, wait a long period for retry*/
	cm_data->loop_expire += SERIAL_ERROR_WAIT_TIME; 
	process_next_func(cm);

	return -1;
}

void open_serial(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	cm_data_t *cm_data = NULL;
	func_tbl_t  func = {};

	/* sanity check */
	if (!cm || !cm->priv_data) {
		rmm_log(ERROR, "cm or cm->priv_data is NULL\n");
		exit(-1);
	}

	cm_data = (cm_data_t *)cm->priv_data;
	func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(cm, cm_data->open_serial, func.once))
		goto next;

	cm_data->open_serial.expire = get_current_tick() + func.timeout;

	FILL_INT(req.msg_base.netfn,	OEM_IPMI_NETFN);
	FILL_INT(req.msg_base.cmd,		OEM_SERIAL_OPEN_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPEN_DEV;
	memcpy_s(req.msg_base.data, IPMI_MAX_DATA_LENGTH, cm_data->serial_dev_name, strnlen_s(cm_data->serial_dev_name, sizeof(cm_data->serial_dev_name)-1));
	FILL_INT(req.msg_base.data_len,	strnlen_s(cm_data->serial_dev_name, sizeof(cm_data->serial_dev_name)-1));

	libjipmi_serial_cmd(&req, func.callback, cm, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

int get_device_id_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *cm;
	cm_data_t *cm_data;
	version_t  cm_ver;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	cm = (am_node_t *)cb_data;
	cm_data = (cm_data_t *)(cm->priv_data);

	if (IPMI_CC_OK != rsp[0])
		goto err;

	cm_ver.low = rsp[4];
	cm_ver.high = rsp[3];
    
	rmm_log(DBG, "CM old_ver[%d:%d] new_ver[%d:%d] uuid[%s]\n", 
        cm_data->cm_ver.low, cm_data->cm_ver.high, cm_ver.low, cm_ver.high, cm_data->uuid);
	if (cm_ver.low != cm_data->cm_ver.low || cm_ver.high != cm_data->cm_ver.high)
		notify_cm_ver_change(cm, &cm_ver);
	cm_data->cm_ver.low = cm_ver.low;
	cm_data->cm_ver.high = cm_ver.high;
	cm_data->get_device_id.status = FUNC_SUCCESS;
	update_cm_node_status(cm);
	process_next_func(cm);

	return 0;
err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_cm_remove(cm);
	set_fn_status(cm, FUNC_FAIL);
	set_node_status(cm, OFFLINE, true);
	process_next_func(cm);

	return -1;
}

void get_device_id(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(cm, cm_data->get_device_id, func.once))
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;

	cm_data->get_device_id.expire = get_current_tick() + func.timeout;

	FILL_INT(req.msg_base.netfn,	IPMI_APP_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_DEVID_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;
	FILL_INT(req.msg_base.data_len,	0);

	libjipmi_serial_cmd(&req, func.callback, cm, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

#if 0
int set_default_pwm_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *cm;
	cm_data_t *cm_data;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	cm = (am_node_t *)cb_data;
	cm_data = (cm_data_t *)(cm->priv_data);

	if (IPMI_CC_OK != rsp[0]) {
		rmm_log(ERROR, "rsp[0]: %d.\n", rsp[0]);
		goto err;
	}

	rmm_log(DBG, "CM status[%d:%d:%d] uuid[%s]\n", 
				cm_data->open_serial.status, 
				cm_data->get_device_id.status,
				cm_data->get_cm_ip.status,
				cm_data->uuid);

	cm_data->set_default_pwm.status = FUNC_SUCCESS;
	update_cm_node_status(cm);
	process_next_func(cm);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_cm_remove(cm);
	set_fn_status(cm, FUNC_FAIL);
	set_node_status(cm, OFFLINE, true);
	process_next_func(cm);

	return -1;
}

void set_default_pwm(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	bool fn_state;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *tz = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tz_data = (tzone_data_t *)tz->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];
	int i = 0;

	if (func_not_ready(cm, cm_data->set_default_pwm, func.once))
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL || cm_data->get_device_id.status == FUNC_FAIL)
		goto next;

	cm_data->set_default_pwm.expire = get_current_tick() + func.timeout;

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		SET_DEFAULT_PWM_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;
	req.msg_base.data[0] = 0xff;  /* all fans */
	req.msg_base.data[1] = 0x28;  /* 40% */

	for (i = 0; i < MAX_TZONE_NUM; i++) {
		tz_data->pwm[i] = 0x28;
	}

	FILL_INT(req.msg_base.data_len,	2);

	libjipmi_serial_cmd(&req, func.callback, cm, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}
#endif

int get_cm_ip_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *cm;
	cm_data_t *cm_data;
	unsigned int mbp_ipaddr;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	cm = (am_node_t *)cb_data;
	cm_data = (cm_data_t *)(cm->priv_data);

	if (IPMI_CC_OK != rsp[0]) {
		goto err;
	}

	if ((rsp[IP_RSP_OFFSET]&0xFF) == 0||
		(rsp[IP_RSP_OFFSET]&0xFF) == 0xFF||
		(rsp[IP_RSP_OFFSET+3]&0xFF) == 0||
		(rsp[IP_RSP_OFFSET+3]&0xFF) == 0xFF) {
		rmm_log(ERROR, "invalid ip addr\n");
		goto err;
	}

	mbp_ipaddr = (rsp[IP_RSP_OFFSET]&0xFF)|
				  ((rsp[IP_RSP_OFFSET+1]&0xFF)<<8)|
				  ((rsp[IP_RSP_OFFSET+2]&0xFF)<<16)|
				  ((rsp[IP_RSP_OFFSET+3]&0xFF)<<24);

	if (cm_data->ip_address != mbp_ipaddr)
		notify_cm_ip_change(cm, mbp_ipaddr);

	/* Since there is no hardware method to detect the presence of thermal zone
	 * and drawer zone, we add tzone and dzone when opening CM serial successfully.
	 */
	if (cm_data->ip_address == INVALID_IP) {
		notify_cm_add(cm);
		notify_tzone_add(cm);
		notify_dzone_add(cm);
		notify_cm_ip_change(cm, mbp_ipaddr);
	}

	cm_data->ip_address = mbp_ipaddr;
	cm_data->get_cm_ip.status = FUNC_SUCCESS;
	update_cm_node_status(cm);
	process_next_func(cm);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d, cm idx is %d\n", rsp[0], cm_data->loc_id);
	notify_cm_remove(cm);
	set_fn_status(cm, FUNC_FAIL);
	set_node_status(cm, OFFLINE, true);
	process_next_func(cm);

	return -1;
}

void get_cm_ip(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(cm, cm_data->get_cm_ip, func.once))
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL||
		cm_data->get_device_id.status == FUNC_FAIL)
		goto next;

	FILL_INT(req.msg_base.netfn, IPMI_LANCFG_NETFN);
	FILL_INT(req.msg_base.cmd, GET_LANCFG_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	req.msg_base.data[0] = GET_LANCFG_DATA0;
	req.msg_base.data[1] = GET_LANCFG_DATA1;
	req.msg_base.data[2] = GET_LANCFG_DATA2;
	req.msg_base.data[3] = GET_LANCFG_DATA3;
	FILL_INT(req.msg_base.data_len, GET_LANCFG_DATA_LEN);

	cm_data->get_cm_ip.expire = get_current_tick() + func.timeout;
	libjipmi_serial_cmd(&req, func.callback, cm, JIPMI_NON_SYNC);

	return;

next:
	process_next_func(cm);
	return;
}

