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
#include "libutils/types.h"
#include "libsecurec/safe_lib.h"

static float linear_to_float(unsigned short raw)
{
	char exponent = raw >> 11;
	short mantissa = raw & 0x7ff;

	if (exponent > 0x0F)
		exponent |= 0xE0;

	if (mantissa > 0x03FF)
		mantissa |= 0xF800;

	return mantissa * pow(2, exponent);
}

static int convert_rsp_to_int(unsigned char low, unsigned char high)
{
	short result = (high<<8)|low;
	float tmp = linear_to_float(result);

	return (int)(tmp + 0.5);
}

static void update_pzone_state(am_node_t *pzone)
{
	pzone_data_t *pz_data = (pzone_data_t *)(pzone->priv_data);

	if (pz_data->get_pzone_present.status == FUNC_SUCCESS &&
		pz_data->get_psus_present.status == FUNC_SUCCESS) {
		set_node_status(pzone, ONLINE, false);
	}
}

static void present_changed(pzone_data_t *pz_data, int old_present, int present, int max)
{
	int i = 0;
	int j = 0;
	bool old_bit = 0;
	bool new_bit = 0;
	int  add_status = 0;
	int  remove_status = 0;

	for (i = 0; i < max; i++) {
		old_bit = (old_present >> i) & 1;
		new_bit = (present >> i) & 1;
		if (old_bit != new_bit) {
			if (new_bit) {
				add_status = 1;
				remove_status = 0;
			} else {
				add_status = 0;
				remove_status = 1;
			}

			for( j = 0; j < sizeof(pz_data->add_flag)/sizeof(pz_data->add_flag[0]); j++ ){
				pz_data->add_flag[j][i] = add_status;
			}

			for( j = 0; j < sizeof(pz_data->remove_flag)/sizeof(pz_data->remove_flag[0]); j++ ){
				pz_data->remove_flag[j][i] = remove_status;
			}
		}
	}
}


int get_pzone_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *pz;
	pzone_data_t *pz_data;
	am_node_t *cm;
	int present;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	pz = (am_node_t *)cb_data;
	pz_data = (pzone_data_t *)(pz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;
		
	rmm_log(DBG, "PZone present[%d:%d] uuid[%s]\n", pz_data->pzone_present, rsp[1], pz_data->uuid);	
	present = rsp[1];
	if (pz_data->pzone_present != present)
		notify_pzone_present_change(pz, present);

	pz_data->pzone_present = present;
	pz_data->get_pzone_present.status = FUNC_SUCCESS;
	update_pzone_state(pz);
	process_next_func(get_cm_node(pz));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_pzone_present_change(pz, 0);
	set_fn_status(pz, FUNC_FAIL);
	set_node_status(pz, OFFLINE, true);
	process_next_func(get_cm_node(pz));

	return -1;
}

void get_pzone_present(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };
	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(pz, pz_data->get_pzone_present, func.once))
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		PZONE_PRESENCE_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	pz_data->get_pzone_present.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, pz, JIPMI_NON_SYNC);

	return;

next:
	process_next_func(cm);
	return;
}

int get_psus_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *pz;
	pzone_data_t *pz_data;
	int present;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	pz = (am_node_t *)cb_data;
	pz_data = (pzone_data_t *)(pz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;

    rmm_log(DBG, "PSU present[%d:%d] uuid[%s]\n", pz_data->psu_present, rsp[1], pz_data->uuid);
	present = rsp[1];
	if (pz_data->psu_present != present) {
		notify_psu_present_change(pz, present);
		present_changed(pz_data, pz_data->psu_present, present, MAX_PSU_NUM);
	}
	pz_data->psu_present = present;

	pz_data->get_psus_present.status = FUNC_SUCCESS;
	update_pzone_state(pz);
	process_next_func(get_cm_node(pz));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_present_change(pz, 0);
	set_fn_status(pz, FUNC_FAIL);
	//set_node_status(pz, OFFLINE, true);
	process_next_func(get_cm_node(pz));

	return -1;
}

void get_psus_present(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };
	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(pz, pz_data->get_psus_present, func.once))
		goto next;
	if (pz_data->get_pzone_present.status == FUNC_FAIL)
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		PSU_PRESENCE_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	pz_data->get_psus_present.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, pz, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

int get_psu_power_in_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	int pin;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;
	if (IPMI_CC_OK != rsp[0])
		goto err;

	pin = convert_rsp_to_int(rsp[2], rsp[1]);
	rmm_log(DBG, "PSU %d in[%d:%d] uuid[%s]\n", psus_data->pos, psus_data->power_in[psus_data->pos], pin, psus_data->uuid[psus_data->pos]);

	if (psus_data->power_in[psus_data->pos] != pin) {
		psus_data->power_in[psus_data->pos] = pin;
		notify_psu_power_in_change(psus, pin, pz_data->psu_present);
	}

	pos = get_next_psu(psus_data->pos, pz_data->psu_present, MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_power_in.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}

	psus_data->pos = pos;
	get_psu_power_in(get_cm_node(psus));

	return 0;
err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_power_in_change(psus, 0, pz_data->psu_present);
	set_fn_status(psus, FUNC_FAIL);
	process_next_func(get_cm_node(psus));

	return -1;
}

void get_psu_power_in(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		PSU_READ_PIN_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	1);
	req.data[0] = psus_data->pos;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_power_in_rsp_cb, psus, JIPMI_NON_SYNC);
}

void get_psus_power_in(am_node_t *cm)
{
	bool fn_cond;
	int pos;
	int i;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psus_power_in, func.once))
		goto next;

	/* handle psu remove scenario for power in */
	pos = -1;
	while(1) {
		pos = get_next_removed_psu(pos, pz_data->remove_flag[5], MAX_PSU_NUM);

		if (pos == -1)
			break;
		
		psus_data->power_in[pos] = 0;
		pz_data->remove_flag[5][pos] = 0;
	}

	pos = get_next_psu(-1, pz_data->psu_present, MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_power_in.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	psus_data->get_psus_power_in.expire = get_current_tick() + func.timeout;
	get_psu_power_in(cm);
	return;

next:
	process_next_func(cm);
	return;
}

int get_psu_current_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int current_out;
	int pos;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;
	current_out = convert_rsp_to_int(rsp[2], rsp[1]);
    rmm_log(DBG, "PSU %d out[%d:%d] uuid[%s]\n", psus_data->pos, psus_data->current_out[psus_data->pos], current_out, psus_data->uuid[psus_data->pos]);
    
	if (psus_data->current_out[psus_data->pos] != current_out)
		notify_psu_current_out_change(psus, current_out, pz_data->psu_present);

	psus_data->current_out[psus_data->pos] = current_out;

	pos = get_next_psu(psus_data->pos, pz_data->psu_present , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_current_out.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	get_psu_current_out(get_cm_node(psus));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_current_out_change(psus, 0, pz_data->psu_present);
	set_fn_status(psus, FUNC_FAIL);
	process_next_func(get_cm_node(psus));

	return -1;
}

void get_psu_current_out(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		PSU_READ_IOUT_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	req.data[0] = psus_data->pos;
	FILL_INT(req.data_len,	0x01);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_current_rsp_cb, psus, JIPMI_NON_SYNC);
}

void get_psus_current_out(am_node_t *cm)
{
	bool fn_state;
	int pos;
	int i;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(cm, psus_data->get_psus_current_out, func.once))
		goto next;

	/* handle psu remove scenario for current out */
	pos = -1;
	while(1) {
		pos = get_next_removed_psu(pos, pz_data->remove_flag[6], MAX_PSU_NUM);

		if (pos == -1)
			break;
		
		psus_data->current_out[pos] = 0;
		pz_data->remove_flag[6][pos] = 0;
	}
	
	pos = get_next_psu(-1, pz_data->psu_present, MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_current_out.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	psus_data->get_psus_current_out.expire = get_current_tick() + func.timeout;
	get_psu_current_out(cm);
	return;

next:
	process_next_func(cm);
	return;
}


int get_psu_power_out_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int power_out;
	int pos;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;
	power_out = convert_rsp_to_int(rsp[1], rsp[2]);
    rmm_log(DBG, "PSU %d power out[%d:%d] uuid[%s]\n", psus_data->pos, psus_data->power_out[psus_data->pos], power_out, psus_data->uuid[psus_data->pos]);
    
	if (psus_data->power_out[psus_data->pos] != power_out) {
		psus_data->power_out[psus_data->pos] = power_out;
		notify_psu_power_out_change(psus, power_out, pz_data->psu_present);
	}

	pos = get_next_psu(psus_data->pos, pz_data->psu_present , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_power_out.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	get_psu_power_out(get_cm_node(psus));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_power_out_change(psus, 0, pz_data->psu_present);
	set_fn_status(psus, FUNC_FAIL);
	process_next_func(get_cm_node(psus));

	return -1;
}

void get_psu_power_out(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_WORD;
	req.data[2] = PSU_OFFSET_POWER_OUT;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_power_out_rsp_cb, psus, JIPMI_NON_SYNC);
}

void get_psus_power_out(am_node_t *cm)
{
	bool fn_state;
	int pos;
	int i;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(cm, psus_data->get_psus_power_out, func.once))
		goto next;

	/* handle psu remove scenario for power out */
	pos = -1;
	while(1) {
		pos = get_next_removed_psu(pos, pz_data->remove_flag[7], MAX_PSU_NUM);

		if (pos == -1)
			break;
		
		psus_data->power_out[pos] = 0;
		pz_data->remove_flag[7][pos] = 0;
	}

	pos = get_next_psu(-1, pz_data->psu_present, MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psus_power_out.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	psus_data->get_psus_power_out.expire = get_current_tick() + func.timeout;
	get_psu_power_out(cm);
	return;

next:
	process_next_func(cm);
	return;
}


int get_psu_serial_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	char buff[128] = {0};		//testtest
	char tmp[8] = {0};
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	for (i = 1; i < rsp_len; i++) {
		snprintf_s_c(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, sizeof(tmp)-1));
	}

	if(strnlen_s(buff, sizeof(buff)-1) == 0 || strnlen_s(buff, sizeof(buff)-1) > PSU_FRU_MAX_STR_LEN)
		goto err;

	rmm_log(DBG, "PSU serial[%s:%s] uuid[%s] pos[%d]\n", psus_data->serial[psus_data->pos], buff, psus_data->uuid[psus_data->pos], psus_data->pos);

	pz_data->add_flag[0][psus_data->pos] = 0;
	
	notify_psu_serial_change(psus, buff);
	if (strcmp(psus_data->serial[psus_data->pos], buff)) {
		memset(psus_data->serial[psus_data->pos], 0, sizeof(psus_data->serial[psus_data->pos]));
		memcpy_s(psus_data->serial[psus_data->pos], sizeof(psus_data->serial[psus_data->pos]), buff, strnlen_s(buff, sizeof(buff)-1));
	}

	pos = get_next_added_psu(psus_data->pos, pz_data->add_flag[0] , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_serial.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	//pz_data->add_flag[0][pos] = 0;
	get_psu_serial(get_cm_node(psus));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_serial_change(psus, "N/A");
	set_fn_status(psus, FUNC_FAIL);
	//set_node_status(psus, OFFLINE, true);
	process_next_func(get_cm_node(psus));
	return -1;
}

void get_psu_serial(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_SERIAL;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_serial_cb, psus, JIPMI_NON_SYNC);

	return;
}

void get_psus_serials(am_node_t *cm)
{
	bool fn_state;
	int pos;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psu_serial, func.once))
		goto next;

	pos = get_next_added_psu(-1, pz_data->add_flag[0], MAX_PSU_NUM);

	if (pos == -1) {
		psus_data->get_psu_serial.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	//pz_data->add_flag[0][pos] = 0;
	psus_data->get_psu_serial.expire = get_current_tick() + func.timeout;

	get_psu_serial(cm);
	return;

next:
	process_next_func(cm);
	return;

}

static int get_psu_manufacture_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	for (i = 1; i < rsp_len; i++) {
		snprintf_s_c(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, sizeof(tmp)-1));
	}

	if(strnlen_s(buff, sizeof(buff)-1) == 0 || strnlen_s(buff, sizeof(buff)-1) > PSU_FRU_MAX_STR_LEN)
		goto err;

	rmm_log(DBG, "PSU manufacture [%s:%s] uuid[%s]\n", psus_data->manufacture[psus_data->pos], buff, psus_data->uuid[psus_data->pos]);

	pz_data->add_flag[1][psus_data->pos] = 0;
	
	notify_psu_manufacture_change(psus, buff);
	if (strcmp(psus_data->manufacture[psus_data->pos], buff)) {
		memset(psus_data->manufacture[psus_data->pos], 0, sizeof(psus_data->manufacture[psus_data->pos]));
		memcpy_s(psus_data->manufacture[psus_data->pos], sizeof(psus_data->manufacture[psus_data->pos]), buff, strnlen_s(buff, sizeof(buff)-1));
	}

	pos = get_next_added_psu(psus_data->pos, pz_data->add_flag[1] , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_manufacture.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	//pz_data->add_flag[1][pos] = 0;
	get_psu_manufacture(get_cm_node(psus));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_manufacture_change(psus, "N/A");
	set_fn_status(psus, FUNC_FAIL);
	//set_node_status(psus, OFFLINE, true);
	process_next_func(get_cm_node(psus));
	return -1;
}


void get_psu_manufacture(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_MANUFACTURE;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_manufacture_cb, psus, JIPMI_NON_SYNC);

	return;
}


void get_psus_manufactures(am_node_t *cm)
{
	bool fn_state;
	int pos;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psu_manufacture, func.once))
		goto next;

	pos = get_next_added_psu(-1, pz_data->add_flag[1], MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_manufacture.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}
	//pz_data->add_flag[1][pos] = 0;
	psus_data->pos = pos;
	psus_data->get_psu_manufacture.expire = get_current_tick() + func.timeout;
	get_psu_manufacture(cm);
	return;

next:
	process_next_func(cm);
	return;
}

int get_psu_model_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	for (i = 1; i < rsp_len; i++) {
		snprintf_s_c(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, sizeof(tmp)-1));
	}

	if(strnlen_s(buff, sizeof(buff)-1) == 0 || strnlen_s(buff, sizeof(buff)-1) > PSU_FRU_MAX_STR_LEN)
		goto err;

	rmm_log(DBG, "PSU model[%s:%s] uuid[%s]\n", psus_data->model[psus_data->pos], buff, psus_data->uuid[psus_data->pos]);

	pz_data->add_flag[2][psus_data->pos] = 0;
	
	notify_psu_model_change(psus, buff);
	if (strcmp(psus_data->model[psus_data->pos], buff)) {
		memset(psus_data->model[psus_data->pos], 0, sizeof(psus_data->model[psus_data->pos]));
		memcpy_s(psus_data->model[psus_data->pos], sizeof(psus_data->model[psus_data->pos]), buff, strnlen_s(buff, sizeof(buff)-1));
	}

	pos = get_next_added_psu(psus_data->pos, pz_data->add_flag[2] , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_model.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	//pz_data->add_flag[2][pos] = 0;
	get_psu_model(get_cm_node(psus));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_model_change(psus, "N/A");
	set_fn_status(psus, FUNC_FAIL);
	//set_node_status(psus, OFFLINE, true);
	process_next_func(get_cm_node(psus));
	return -1;
}


void get_psu_model(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_MODEL;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_model_cb, psus, JIPMI_NON_SYNC);

	return;
}

void get_psus_models(am_node_t *cm)
{
	bool fn_state;
	int pos;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psu_model, func.once))
		goto next;

	pos = get_next_added_psu(-1, pz_data->add_flag[2], MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_model.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	//pz_data->add_flag[2][pos] = 0;
	psus_data->get_psu_model.expire = get_current_tick() + func.timeout;
	get_psu_model(cm);
	return;

next:
	process_next_func(cm);
	return;
}

int get_psu_fw_ver_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;
	
	for (i = 1; i < rsp_len; i++) {
		snprintf_s_c(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, sizeof(tmp)-1));
	}

	if(strnlen_s(buff, sizeof(buff)-1) == 0 || strnlen_s(buff, sizeof(buff)-1) > PSU_FRU_MAX_STR_LEN)
		goto err;
	
    rmm_log(DBG, "PSU fw ver[%s:%s] uuid[%s]\n", psus_data->fw_ver[psus_data->pos], buff, psus_data->uuid[psus_data->pos]);

	pz_data->add_flag[3][psus_data->pos] = 0;
	
	notify_psu_fw_ver_change(psus, buff);
	if (strcmp(psus_data->fw_ver[psus_data->pos], buff)) {
		memset(psus_data->fw_ver[psus_data->pos], 0, sizeof(psus_data->fw_ver[psus_data->pos]));
		memcpy_s(psus_data->fw_ver[psus_data->pos], sizeof(psus_data->fw_ver[psus_data->pos]), buff, strnlen_s(buff, sizeof(buff)-1));
	}

	pos = get_next_added_psu(psus_data->pos, pz_data->add_flag[3], MAX_PSU_NUM);

	if (pos == -1) {
		psus_data->get_psu_fw_ver.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	//pz_data->add_flag[3][pos] = 0;
	get_psu_fw_ver(get_cm_node(psus));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_fw_ver_change(psus, "N/A");
	set_fn_status(psus, FUNC_FAIL);
	//set_node_status(psus, OFFLINE, true);
	process_next_func(get_cm_node(psus));
	return -1;
}


void get_psu_fw_ver(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_REVISION;

	FILL_INT(req.netfn, 	IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_fw_ver_cb, psus, JIPMI_NON_SYNC);

	return;
}


void get_psus_fw_vers(am_node_t *cm)
{
	bool fn_state;
	int pos;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psu_fw_ver, func.once))
		goto next;

	pos = get_next_added_psu(-1, pz_data->add_flag[3], MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_fw_ver.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	//pz_data->add_flag[3][pos] = 0;
	psus_data->get_psu_fw_ver.expire = get_current_tick() + func.timeout;
	get_psu_fw_ver(cm);
	return;

next:
	process_next_func(cm);
	return;
}


void update_psu_total_power_cap(am_node_t *psus)
{
	int i = 0;
	int32 power_cap_sum = 0;
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	
	/* calculate power cap */
	for(i=0; i<MAX_PSU_NUM; i++)
		power_cap_sum += psus_data->power_cap[i];

	if (power_cap_sum != psus_data->total_power_cap) {
		rmm_log(INFO, "notify psu total power cap to %d.\n", power_cap_sum);
		notify_psu_total_power_cap_change(psus, power_cap_sum);
		psus_data->total_power_cap = power_cap_sum;
	}
}


int get_psu_power_cap_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int pos;
	int power_cap = 0;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	power_cap= convert_rsp_to_int(rsp[1], rsp[2]);
	rmm_log(DBG, "PSU power cap [%d:%d] uuid[%s]\n", psus_data->power_cap[psus_data->pos], power_cap, buff, psus_data->uuid[psus_data->pos]);

	notify_psu_power_cap_change(psus, power_cap);
	if (psus_data->power_cap[psus_data->pos] != power_cap) {
		psus_data->power_cap[psus_data->pos] = power_cap;
	}

	pos = get_next_added_psu(psus_data->pos, pz_data->add_flag[4], MAX_PSU_NUM);

	if (pos == -1) {
		psus_data->get_psu_power_cap.status = FUNC_SUCCESS;
		update_psu_total_power_cap(psus);
		process_next_func(get_cm_node(psus));
		return 0;
	}

	psus_data->pos = pos;
	pz_data->add_flag[4][pos] = 0;
	get_psu_power_cap(get_cm_node(psus));
	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	notify_psu_power_cap_change(psus, 0);
	set_fn_status(psus, FUNC_FAIL);
	//set_node_status(psus, OFFLINE, true);
	process_next_func(get_cm_node(psus));
	return -1;
}


void get_psu_power_cap(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_WORD;
	req.data[2] = PSU_OFFSET_POWER_CAP;

	FILL_INT(req.netfn, 	IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_power_cap_cb, psus, JIPMI_NON_SYNC);

	return;
}


void get_psus_power_cap(am_node_t *cm)
{
	bool fn_state;
	int pos;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(psus, psus_data->get_psu_power_cap, func.once))
		goto next;

	/* handle psu remove scenario for pzone power cap */
	pos = -1;
	while(1) {
		pos = get_next_removed_psu(pos, pz_data->remove_flag[4], MAX_PSU_NUM);
		
		if (pos == -1) {
			update_psu_total_power_cap(psus);
			break;
		}

		psus_data->power_cap[pos] = 0;
		pz_data->remove_flag[4][pos] = 0;
	}

	pos = get_next_added_psu(-1, pz_data->add_flag[4], MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_power_cap.status = FUNC_SUCCESS;
		if (psus_data->total_power_cap == 0)
			update_psu_total_power_cap(psus);
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	pz_data->add_flag[4][pos] = 0;
	psus_data->get_psu_power_cap.expire = get_current_tick() + func.timeout;
	get_psu_power_cap(cm);
	return;

next:
	process_next_func(cm);
	return;
}


int get_psu_health_status_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *psus;
	am_node_t *cm;
	am_node_t *pz;
	psus_data_t *psus_data;
	pzone_data_t *pz_data;
	int health_status;
	int pos;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	psus = (am_node_t *)cb_data;
	psus_data = (psus_data_t *)psus->priv_data;
	pz = psus->parent;
	pz_data = (pzone_data_t *)pz->priv_data;

	if (IPMI_CC_OK != rsp[0])
		goto err;

	health_status = rsp[1]|(rsp[2]<<8);
	rmm_log(DBG, "PSU health status [%d:%d] uuid[%s]\n", psus_data->health_status[psus_data->pos], health_status, psus_data->uuid[psus_data->pos]);
	if (psus_data->health_status[psus_data->pos] != health_status) {
		psus_data->health_status[psus_data->pos] = health_status;
		notify_psu_health_status_change(psus, health_status, pz_data->psu_present);
	}

	pos = get_next_psu(psus_data->pos, pz_data->psu_present , MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_health_status.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return 0;
	}
	psus_data->pos = pos;
	get_psu_health_status(get_cm_node(psus));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC %d.\n", rsp[0]);
	set_fn_status(psus, FUNC_FAIL);
	process_next_func(get_cm_node(psus));

	return -1;
}

void get_psu_health_status(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	/* <Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	am_node_t *pz = node_lookup(cm, PZONE_NODE);
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;

	req.data[0] = psus_data->pos;
	req.data[1] = PSU_READ_WORD;
	req.data[2] = PSU_OFFSET_STATUS;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	3);

	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_psu_health_status_rsp_cb, psus, JIPMI_NON_SYNC);
}

void get_psus_health_status(am_node_t *cm)
{
	bool fn_state;
	int pos;
	int i;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	if (func_not_ready(cm, psus_data->get_psu_health_status, func.once))
		goto next;

	/* handle psu remove scenario for health status */
	pos = -1;
	while(1) {
		pos = get_next_removed_psu(pos, pz_data->remove_flag[8], MAX_PSU_NUM);

		if (pos == -1)
			break;
		
		psus_data->health_status[pos] = 0;
		pz_data->remove_flag[8][pos] = 0;
	}

	pos = get_next_psu(-1, pz_data->psu_present, MAX_PSU_NUM);
	if (pos == -1) {
		psus_data->get_psu_health_status.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(psus));
		return;
	}

	psus_data->pos = pos;
	psus_data->get_psu_health_status.expire = get_current_tick() + func.timeout;
	get_psu_health_status(cm);
	return;

next:
	process_next_func(cm);
	return;
}

