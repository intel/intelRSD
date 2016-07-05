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


int get_fans_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *tz;
	tzone_data_t *tz_data;
	int present = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	tz = (am_node_t *)cb_data;
	tz_data = (tzone_data_t *)(tz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;
    rmm_log(DBG, "TZone present[%d:%d] uuid[%s]\n", tz_data->present, rsp[1], tz_data->uuid);
	present = rsp[1];
	if (tz_data->present != present)
		notify_fan_present_change(tz, present);
	tz_data->present = present;
	tz->node_status = ONLINE;
	process_next_func(get_cm_node(tz));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_fan_present_change(tz, 0);
	tz_data->present = 0;
	set_fn_status(tz, FUNC_FAIL);
	//set_node_status(tz, OFFLINE, true);
	process_next_func(get_cm_node(tz));

	return -1;
}

void get_fans_present(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	am_node_t *tz = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tz_data = (tzone_data_t *)tz->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(tz, tz_data->get_fans_present, func.once))
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		FAN_PRESENCE_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	tz_data->get_fans_present.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, tz, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

int get_fans_speed_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *fans;
	fans_data_t *fans_data;
	int i;
	unsigned int speed[MAX_PWM_NUM] = {0};
	bool is_changed = false;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	fans = (am_node_t *)cb_data;
	fans_data = (fans_data_t *)(fans->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;

	rmm_log(DBG, "TZone speed[%d][%d][%d][%d][%d][%d] uuid[%s]\n",
				(rsp[0*2+2])|(rsp[0*2+1]<<8),
				(rsp[1*2+2])|(rsp[1*2+1]<<8),
				(rsp[2*2+2])|(rsp[2*2+1]<<8),
				(rsp[3*2+2])|(rsp[3*2+1]<<8),
				(rsp[4*2+2])|(rsp[4*2+1]<<8),
				(rsp[5*2+2])|(rsp[5*2+1]<<8),
				fans_data->uuid);

	for (i = 0; i < MAX_PWM_NUM; i++) {
		speed[i]  = (rsp[i*2+2])|(rsp[i*2+1]<<8);
		if (speed[i] != fans_data->speed[i]) {
			is_changed = true;
		}
	}

	if (is_changed)
		notify_fans_speed_change(fans, speed);

	for (i = 0; i < MAX_PWM_NUM; i++) {
		fans_data->speed[i] = speed[i];
	}

	fans->node_status = ONLINE;
	process_next_func(get_cm_node(fans));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_fans_speed_change(fans, speed);
	set_fn_status(fans, FUNC_FAIL);
	//set_node_status(fans, OFFLINE, true);
	process_next_func(get_cm_node(fans));

	return -1;
}

void get_fans_speed(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };
	am_node_t *fans = node_lookup(cm, FANS_NODE);
	fans_data_t *fans_data = (fans_data_t *)fans->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(fans, fans_data->get_fans_speed, func.once))
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		FAN_TACH_METER_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);
	fans_data->get_fans_speed.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, fans, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

int set_tzone_pwm_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *tz;
	tzone_data_t *tz_data;
	int32 tz_num = 1;
	int32 rpm = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	tz = (am_node_t *)cb_data;
	tz_data = (tzone_data_t *)tz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	rpm = convert_pwm_to_rpm(tz_data->pwm[tz_num - 1]);

	if (rpm == -1)
		goto err;

	if (notify_tzone_rpm_change(tz_data->uuid[tz_num - 1], rpm) != 0)
		return -1;

	process_next_func(get_cm_node(tz));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	set_fn_status(tz, FUNC_FAIL);
	process_next_func(get_cm_node(tz));

	return -1;
}

static int32 update_pwm_ready(tzone_data_t *tzone_data)
{
	int64 curr_tick = get_current_tick();

	if (tzone_data->update_tzone_pwm.expire <= curr_tick) {
		tzone_data->update_tzone_pwm.expire = curr_tick + 10000;
		return true;
	}

	return false;
}

void set_tzone_pwm(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };
	am_node_t *tzone = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tzone_data = (tzone_data_t *)tzone->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	int32 tz_num = 1;

	if (func_not_ready(tzone, tzone_data->set_tzone_pwm, func.once))
		goto next;

	if (update_pwm_ready(tzone_data) == false)
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SET_DEFAULT_PWM_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	2);
	req.data[0] = 0xff;
	req.data[1] = tzone_data->pwm[tz_num - 1];

	rmm_log(DBG, "set pwm is %d\n", tzone_data->pwm[0]);

	tzone_data->set_tzone_pwm.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, tzone, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

