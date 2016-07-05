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
#include <sys/stat.h>
#include <time.h>
#include "openssl/md5.h"
#include "libcfg/cfg.h"

static void set_drawer_id_field(am_node_t *cm, jipmi_rsp_callback_fn cb_fn, int drawer_lid, int instance);


#if 0
/* certificate */
static uint8	RMMCert[MAX_CERT_LEN];
static uint16	RMMCertLen;
static uint8	RMMCertHash[CERT_HASH_LEN];
static time_t	RMMCert_mod_time;

static uint8	PODMCert[MAX_CERT_LEN];
static uint16	PODMCertLen;
static uint8	PODMCertHash[CERT_HASH_LEN];
static time_t	PODMCert_mod_time;
#endif

#define RMM_CERT_PATH 	"/etc/rmm/rmm.cert"
#define PODM_CERT_PATH	"/etc/rmm/podm.cert"

#define MAX_CERT_RETRY 3
static bool platform_BDC_R = false;

#define MAX_CERT_CHUNK_LEN	48
#define DRAWER_POS_TO_CH_ID(pos) (pos*2)

#define DEFUALT_DRAWER_POWER	70

struct br_info br_addrs[MAX_NODE_NUM] = {
	{CHASSIS_BR_ADDR0, CHASSIS_BR_CHAN0},
	{CHASSIS_BR_ADDR1, CHASSIS_BR_CHAN1},
	{CHASSIS_BR_ADDR2, CHASSIS_BR_CHAN2},
	{CHASSIS_BR_ADDR3, CHASSIS_BR_CHAN3},
	{CHASSIS_BR_ADDR4, CHASSIS_BR_CHAN4},
	{CHASSIS_BR_ADDR5, CHASSIS_BR_CHAN5},
	{CHASSIS_BR_ADDR6, CHASSIS_BR_CHAN6},
	{CHASSIS_BR_ADDR7, CHASSIS_BR_CHAN7}
};

static void update_dzone_state(am_node_t *dz)
{
	dzone_data_t *dz_data = NULL;

	/* sanity check */
	if (!dz) {
		rmm_log(ERROR, "dz is NULL\n");
		return;
	}

	dz_data = (dzone_data_t *)(dz->priv_data);
	if (!dz_data) {
		rmm_log(ERROR, "dz->priv_data is NULL\n");
		return;
	}

	if (dz_data->get_drawer_present.status == FUNC_SUCCESS) {
		dz->node_status = ONLINE;
	}
}


/*01010101 -> 1111*/
static int convert_8bit_to_4bit(int present)
{
	int bit0 = present & 1;
	int bit1 = present & (1 << 2) ? 1:0;
	int bit2 = present & (1 << 4) ? 1:0;
	int bit3 = present & (1 << 6) ? 1:0;

	return bit0 + bit1*2 + bit2*4 + bit3*8;
}

void check_platform_type()
{
	char platform[32] = {};

	if (rmm_cfg_get_platform(platform, sizeof(platform)) < 0)
		rmm_log(ERROR, "can not get platform info.\n");
	else {
		if (strncmp(platform, "BDC-R", strnlen_s("BDC-R", RSIZE_MAX_STR)) == 0)
			platform_BDC_R = true;
		else
			platform_BDC_R = false;
		rmm_log(DBG, "platform type is [%s], platform_BDC_R is [%d].\n", platform, platform_BDC_R);
	}
}


int get_drawer_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *dz;
	dzone_data_t *dz_data;
	unsigned char present;
	uint8 i = 0;
	bool old_bit = 0;
	bool new_bit = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	dz = (am_node_t *)cb_data;
	dz_data = (dzone_data_t *)(dz->priv_data);

	if (IPMI_CC_OK != rsp[0])
		goto err;

	rmm_log(DBG, "DZone present[0x%x:0x%x] uuid[%s]\n", dz_data->present, rsp[1], dz_data->uuid);

	present = rsp[1];
	if (dz_data->present != present)
		notify_drawer_present_change(dz, present);

#if 0
	/* set new drawer flag */
	for (i = 0; i < MAX_DRAWER_NUM; i++) {
		old_bit = (dz_data->present >> i) & 1;
		new_bit = (present >> i) & 1;
		if (old_bit != new_bit) {
			if (new_bit) {
				drawers_data->new_drawer[i] = true;
			}
		}
	}
#endif
	
	dz_data->fresh_present = convert_8bit_to_4bit(present^dz_data->present);

	if (present && dz_data->present==0)	/* drawer push in, check platform type(will improve in the future) */
		check_platform_type();

	rmm_log(DBG, "present:old [0x%x:0x%x] fresh_present set to [0x%x].\n", present, dz_data->present, dz_data->fresh_present);
	
	dz_data->present = present;
	dz_data->get_drawer_present.status = FUNC_SUCCESS;
	update_dzone_state(dz);
	process_next_func(get_cm_node(dz));

	return 0;

err:
	rmm_log(ERROR, "DZone response error IPMI CC result[%d] rsp_0[%d] len[%d], dz %s.\n", result, rsp[0], rsp_len, dz_data->uuid);
	notify_drawer_present_change(dz, 0);
	dz_data->present = 0;
	set_fn_status(dz, FUNC_FAIL);
	//set_node_status(dz, OFFLINE, true);
	process_next_func(get_cm_node(dz));

	return -1;
}



void get_drawer_present(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };

	am_node_t *dz = node_lookup(cm, DZONE_NODE);
	dzone_data_t  *dz_data = (dzone_data_t *)dz->priv_data;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];

	if (func_not_ready(dz, dz_data->get_drawer_present, func.once))
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		CHASSIS_PRESENCE_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	dz_data->get_drawer_present.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, dz, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}




int get_drawers_alert_rsp(int result, unsigned char *rsp, int rsp_len, void *cb_data)
{
	am_node_t *dz = NULL;
	dzone_data_t *dz_data;
	int alert = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	dz = (am_node_t *)cb_data;
	dz_data = (dzone_data_t *)(dz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;
    
    rmm_log(DBG, "DZone alert[%d:%d] uuid[%s]\n", dz_data->alert, rsp[1], dz_data->uuid);
    
	alert = rsp[1];
	if (dz_data->alert != alert)
		notify_drawer_alert_change(dz, alert);

	dz_data->alert = alert;
	dz_data->get_drawers_alert.status = FUNC_SUCCESS;

	update_dzone_state(dz);
	process_next_func(get_cm_node(dz));

	return 0;

err:
	rmm_log(ERROR, "DZone response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);

	notify_drawer_alert_change(dz, 0);
	set_fn_status(dz, FUNC_FAIL);
	//set_node_status(dz, OFFLINE, true);
	process_next_func(get_cm_node(dz));

	return -1;
}


void get_drawers_alert(am_node_t *cm)
{
	jipmi_msg_t req = { {0} };
	am_node_t *dz = NULL;
	dzone_data_t *dz_data = NULL;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];

	dz = node_lookup(cm, DZONE_NODE);
	if (!dz) {
		rmm_log(ERROR, "Fail to get DZONE_NODE\n");
		return;
	}
	dz_data = (dzone_data_t *)dz->priv_data;
	if (!dz_data) {
		rmm_log(ERROR, "dz->priv_data is NULL\n");
		return;
	}

	if (func_not_ready(dz, dz_data->get_drawers_alert, func.once))
		goto next;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		GET_ALERT_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	dz_data->get_drawers_alert.expire = get_current_tick() + func.timeout;
	libjipmi_rmcp_cmd(cm_data->ip_address, IPMI_RMCP_PORT, &req, func.callback, dz, JIPMI_NON_SYNC);
	return;

next:
	process_next_func(cm);
	return;
}

int get_drawers_temp_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int reading;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;

	rmm_log(DBG, "Drawer temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_temp.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_temp(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	notify_drawer_temp_change(drawers, 0);
	//set_fn_status(drawers, FUNC_FAIL);
	//set_node_status(drawers, OFFLINE, true);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_temp.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_temp(get_cm_node(drawers));

	return 0;
}


void get_drawer_temp(am_node_t *cm)
{
	//jipmi_msg_t req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

#if 0
	FILL_INT(req.netfn,		IPMI_APP_NETFN);
	FILL_INT(req.cmd,		CHASSIS_TEMP_CMD);
	FILL_STR(req.name,		am_rmcp_username);
	FILL_STR(req.password,	am_rmcp_password);
	FILL_INT(req.data_len,	0);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &req, get_drawers_temp_rsp_cb, drawers,
							br_addrs[pos].target_addr,
							br_addrs[pos].target_channel,
							0,
							0,
							IPMI_BRIDGE_MSG_SINGLE,
							JIPMI_NON_SYNC);
#endif

	FILL_INT(req.msg_base.netfn,		IPMI_APP_NETFN);
	FILL_INT(req.msg_base.cmd,		CHASSIS_TEMP_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	FILL_INT(req.msg_base.data_len,	0);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	libjipmi_serial_br_cmd(&req, get_drawers_temp_rsp_cb, drawers,
							br_addrs[pos].target_addr,
							br_addrs[pos].target_channel,
							0,
							0,
							IPMI_BRIDGE_MSG_SINGLE,
							JIPMI_NON_SYNC);
}

void get_drawers_temp(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_temp, func.once) || platform_BDC_R==true)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_temp.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_temp.expire = get_current_tick() + func.timeout;
	get_drawer_temp(cm);

	return;

next:
	process_next_func(cm);
	return;
}


static void get_drawer_id_field(am_node_t *cm, jipmi_rsp_callback_fn cb_fn, int pos, int instance)
{
	jipmi_serial_msg_t req = { };
	int field_type = 0;
	int byte_num = 0;
	char *uuid = NULL;
	int id_value = 0;
	int puid;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	req.msg_base.data[0] = instance;

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_ID_FIELD_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	FILL_INT(req.msg_base.data_len,	1);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	rmm_log(DBG, "-------------------get_drawer_id_field called to pos[%d], instance[%d].\n", pos, instance);

	libjipmi_serial_br_cmd(&req, cb_fn, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_SYNC);
}

int get_drawer_rack_puid_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int puid;
	int i;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	rmm_log(DBG, "get_drawer_rack_puid_rsp, max bytes for this filed [%d], num of bytes of this filed [%d].\n", rsp[1], rsp[2]);
	if (rsp[2] != 4) {
		rmm_log(ERROR, "num of bytes in this field error.\n");
		goto err;
		}

	puid = rsp[3]<<24 | rsp[4]<<16 | rsp[5]<<8 | rsp[6];
	rmm_log(DBG, "get_drawer_rack_puid_rsp, puid [0x%x].\n", puid);
	if (drawers_data->puid[drawers_data->pos] != puid) {
		rmm_log(DBG, "drawer pos [%d] restarted.\n", drawers_data->pos);
		dz_data->fresh_present |= 1<<drawers_data->pos;	/* psme reboot happened*/
		drawers_data->puid[drawers_data->pos] = puid;
		}
	
	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_rack_puid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_rack_puid(get_cm_node(drawers), pos);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d] on pos [%d].\n", result, rsp[0], rsp_len, drawers_data->pos);
	drawers_data->puid[drawers_data->pos] = 0;

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_rack_puid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_rack_puid(get_cm_node(drawers), pos);

	return 0;
}


void get_drawer_rack_puid(am_node_t *cm, int pos)
{
	get_drawer_id_field(cm, get_drawer_rack_puid_rsp, pos, INSTANCE_RACK_PUID);
}

void get_drawers_rack_puid(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_rack_puid, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_rack_puid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	
	drawers_data->pos = pos;
	drawers_data->get_drawers_rack_puid.expire = get_current_tick() + func.timeout;
	get_drawer_rack_puid(cm, pos);

	return;

next:
	process_next_func(cm);
	return;
}



int set_drawer_ruid_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial
	
	rmm_log(DBG, "set_drawer_ruid_rsp, max bytes for this filed [%d]\n", rsp[1]);

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_tray_ruid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	set_drawer_ruid(get_cm_node(drawers), pos);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_tray_ruid.status = FUNC_FAIL;
		set_fn_status(drawers, FUNC_FAIL);
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	set_drawer_ruid(get_cm_node(drawers), pos);

	return 0;
}


void set_drawer_ruid(am_node_t *cm, int pos)
{
	//rmm_log(DBG, "set_drawer_ruid called to pos[%d]\n", pos);
	set_drawer_id_field(cm, set_drawer_ruid_rsp, pos, INSTANCE_DRAWER_RUID);
}

void set_drawers_ruid(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_tray_ruid, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_tray_ruid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->set_drawers_tray_ruid.expire = get_current_tick() + func.timeout;
	set_drawer_ruid(cm, pos);

	return;

next:
	process_next_func(cm);
	return;
}


int set_drawer_rack_bpid_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial
	
	rmm_log(DBG, "set_drawer_rack_bpid_rsp, max bytes for this filed [%d]\n", rsp[1]);

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_rack_bpid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	set_drawer_rack_bpid(get_cm_node(drawers), pos);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_rack_bpid.status = FUNC_FAIL;
		set_fn_status(drawers, FUNC_FAIL);
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	set_drawer_rack_bpid(get_cm_node(drawers), pos);

	return 0;
}


void set_drawer_rack_bpid(am_node_t *cm, int pos)
{
	set_drawer_id_field(cm, set_drawer_rack_bpid_rsp, pos, INSTANCE_RACK_BPID);
}

void set_drawers_rack_bpid(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_rack_bpid, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->set_drawers_rack_bpid.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->set_drawers_rack_bpid.expire = get_current_tick() + func.timeout;
	set_drawer_rack_bpid(cm, pos);

	return;

next:
	process_next_func(cm);
	return;
}



int set_drawer_rack_puid_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int puid;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial
	
	rmm_log(DBG, "set_drawer_rack_puid_rsp, max bytes for this filed [%d]\n", rsp[1]);

	puid = get_rack_puid();
	pos = drawers_data->pos;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->set_drawers_rack_puid.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return 0;
		}

		if (drawers_data->puid[pos] != puid || dz_data->fresh_present & 1<<pos)
			break;
	}
	drawers_data->pos = pos;
	set_drawer_rack_puid(get_cm_node(drawers), pos);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d] on pos [%d].\n", result, rsp[0], rsp_len, drawers_data->pos);
	drawers_data->puid[drawers_data->pos] = 0;
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	puid = get_rack_puid();
	pos = drawers_data->pos;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		if (pos == -1) {
			set_fn_status(drawers, FUNC_FAIL);
			drawers_data->set_drawers_rack_puid.status = FUNC_FAIL;
			process_next_func(get_cm_node(drawers));
			return -1;
		}

		if (drawers_data->puid[pos] != puid || dz_data->fresh_present & 1<<pos)
			break;		
	}
	drawers_data->pos = pos;
	set_drawer_rack_puid(get_cm_node(drawers), pos);

	return 0;
}


void set_drawer_rack_puid(am_node_t *cm, int pos)
{
	set_drawer_id_field(cm, set_drawer_rack_puid_rsp, pos, INSTANCE_RACK_PUID);
}

void set_drawers_rack_puid(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_rack_puid, func.once) || platform_BDC_R==false)
		goto next;

	//pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	int puid = get_rack_puid();
	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->set_drawers_rack_puid.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		if (drawers_data->puid[pos] != puid || dz_data->fresh_present & 1<<pos)
			break;
	}
	
	drawers_data->pos = pos;
	drawers_data->set_drawers_rack_puid.expire = get_current_tick() + func.timeout;
	set_drawer_rack_puid(cm, pos);

	return;

next:
	process_next_func(cm);
	return;
}


static void set_drawer_id_field(am_node_t *cm, jipmi_rsp_callback_fn cb_fn, int pos, int instance)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int field_type = 0;
	int byte_num = 0;
	char *uuid = NULL;
	//int tray_ruid = 0;
	int id_value = 0;
	int puid;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	
	/* 
	Tray RUID: Tray Pos
	Rack PUID: derived from Database set by Pod Manager
	Rack BPID: CM Pos
	*/
	switch (instance) {
		case INSTANCE_DRAWER_RUID:
			id_value = pos+1+(cm_data->loc_id - 1) * MAX_DRAWER_NUM;
			break;
		case INSTANCE_RACK_BPID:
			id_value = cm_data->loc_id;
			break;
		case INSTANCE_RACK_PUID:
			puid = get_rack_puid();
			id_value = puid;
			drawers_data->puid[pos] = puid;
			break;
		default:
			rmm_log(ERROR, "Instance [%d] not correct.");
			return;
	}

	req.msg_base.data[byte_num++] = FIELD_TYPE_DEFAULT;		/* type of field*/
	req.msg_base.data[byte_num++] = instance;				/* instance of field */
	req.msg_base.data[byte_num++] = 4;						/* number of byte to set */
	req.msg_base.data[byte_num++] = (char)(id_value>>24);
	req.msg_base.data[byte_num++] = (char)(id_value>>16);
	req.msg_base.data[byte_num++] = (char)(id_value>>8);
	req.msg_base.data[byte_num++] = (char)id_value;

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		SET_ID_FIELD_CMD);
	FILL_STR(req.msg_base.name,		am_rmcp_username);
	FILL_STR(req.msg_base.password,	am_rmcp_password);
	FILL_INT(req.msg_base.data_len,	byte_num);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	rmm_log(DBG, "-------------------set_drawer_id_field called to pos[%d], set instance[%d], id value[%d], byte num[%d]\n", pos, instance, id_value, byte_num);

	libjipmi_serial_br_cmd(&req, cb_fn, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}



int get_drawer_thermal_sensor_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int i, j;
	uint8 attrs_need_init = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	#if 0
	rmm_log(DBG, "DZone temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;
	#endif

	if (drawers_data->sled_present[pos]==0 && rsp[1]!=0)	/* first run or first sled plug in, init variables */
		attrs_need_init = 1;
	
	if (drawers_data->sled_present[pos] != rsp[1]) {
		rmm_log(DBG, "sled [%d] present changed from [%x] to [%x].\n", pos, drawers_data->sled_present[pos], rsp[1]);
		notify_sled_present_change(drawers, rsp[1]);
	}
	drawers_data->sled_present[pos] = rsp[1];

	for (i=0, j=0; i<8; i++) {
		if (rsp[1] & 1<<i) {
			uint8 rsp_type = rsp[2+j*3];
			uint8 rsp_pwm = rsp[2+j*3+1];
			uint8 rsp_temp = rsp[2+j*3+2];
			if (attrs_need_init || drawers_data->sled_data[pos].thermal_data[i].sled_type != rsp_type) {
				notify_sled_type_change(drawers, i, rsp_type);
			}
			drawers_data->sled_data[pos].thermal_data[i].sled_type = rsp_type;

			if (drawers_data->sled_data[pos].thermal_data[i].desired_pwm!= rsp_pwm) {
				notify_sled_pwm_change(drawers, i, rsp_pwm);
			}
			drawers_data->sled_data[pos].thermal_data[i].desired_pwm = rsp_pwm;

			if (drawers_data->sled_data[pos].thermal_data[i].inlet_temp != rsp_temp) {
				notify_sled_temp_change(drawers, i, rsp_temp);
			}
			drawers_data->sled_data[pos].thermal_data[i].inlet_temp = rsp_temp;
			
			rmm_log(DBG, "drawer pos[%d] bitmap[0x%x] sled[%d] present.type[%d], pwm[%d], inlet[%d]\n", pos, rsp[1], i, 
				drawers_data->sled_data[pos].thermal_data[i].sled_type,
				drawers_data->sled_data[pos].thermal_data[i].desired_pwm,
				drawers_data->sled_data[pos].thermal_data[i].inlet_temp);
			j++;
		}
	}

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_thermal_sensor.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_thermal_sensor(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	//notify_drawer_temp_change(drawers, 0);
	drawers_data->sled_present[pos] = 0;
	notify_sled_present_change(drawers, 0);

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_thermal_sensor.status = FUNC_SUCCESS;
		set_fn_status(drawers, FUNC_FAIL);
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_thermal_sensor(get_cm_node(drawers));
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	return 0;
}


void get_drawer_thermal_sensor(am_node_t *cm)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

#if 0
	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		GET_AGGREGATED_THERMAL_SENSOR);
	FILL_STR(ipmi_req.name, 	am_rmcp_username);
	FILL_STR(ipmi_req.password, am_rmcp_password);
	FILL_INT(ipmi_req.data_len, 0);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &ipmi_req, get_drawer_thermal_sensor_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							pos,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
#endif
	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_AGGREGATED_THERMAL_SENSOR);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	FILL_INT(req.msg_base.data_len, 0);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	libjipmi_serial_br_cmd(&req, get_drawer_thermal_sensor_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_thermal_sensor(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_thermal_sensor, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_thermal_sensor.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_thermal_sensor.expire = get_current_tick() + func.timeout;
	
	get_drawer_thermal_sensor(cm);

	return;

next:
	process_next_func(cm);
	return;
}



int get_drawer_subnet_mask_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int i, j;
	//uint8 attrs_need_init = 0;
	unsigned int mask;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial
	
	mask = (rsp[IP_RSP_OFFSET]&0xFF)|
				  ((rsp[IP_RSP_OFFSET+1]&0xFF)<<8)|
				  ((rsp[IP_RSP_OFFSET+2]&0xFF)<<16)|
				  ((rsp[IP_RSP_OFFSET+3]&0xFF)<<24);

	if (drawers_data->subnet_masks[pos] != mask) {
		rmm_log(DBG, "drawer [%d] subnet mask changed from [%x] to [%x].\n", pos, drawers_data->subnet_masks[pos], mask);
		notify_drawer_subnet_mask_change(drawers, mask);
	}
	drawers_data->subnet_masks[pos] = mask;


	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_subnet_mask.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_subnet_mask(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	notify_drawer_subnet_mask_change(drawers, 0);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		drawers_data->get_drawers_subnet_mask.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_subnet_mask(get_cm_node(drawers));

	return 0;
}


void get_drawer_subnet_mask(am_node_t *cm)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	FILL_INT(req.msg_base.netfn,	IPMI_LANCFG_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_LANCFG_CMD);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	req.msg_base.data[0] = GET_LANCFG_DATA0;
	req.msg_base.data[1] = GET_LANCFG_SUBNET_MASK_DATA1;
	req.msg_base.data[2] = GET_LANCFG_DATA2;
	req.msg_base.data[3] = GET_LANCFG_DATA3;
	FILL_INT(req.msg_base.data_len, GET_LANCFG_DATA_LEN);

	libjipmi_serial_br_cmd(&req, get_drawer_subnet_mask_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_subnet_mask(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_subnet_mask, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_subnet_mask.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_subnet_mask.expire = get_current_tick() + func.timeout;
	get_drawer_subnet_mask(cm);

	return;

next:
	process_next_func(cm);
	return;
}

int get_drawer_ip_source_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int i, j;
	//uint8 attrs_need_init = 0;
	uint8 ip_src = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	if (rsp[IP_RSP_OFFSET] > 4) {
		rmm_log(ERROR, "invalid ip source\n");
		goto err;
	}

	ip_src = rsp[IP_RSP_OFFSET];

	if (drawers_data->ip_sources[pos] != ip_src) {
		rmm_log(DBG, "drawer [%d] ip source changed from [%x] to [%x].\n", pos, drawers_data->ip_sources[pos], ip_src);
		notify_drawer_ip_source_change(drawers, ip_src);
	}
	drawers_data->ip_sources[pos] = ip_src;


	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_ip_source.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_ip_source(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	notify_drawer_ip_source_change(drawers, 0);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		drawers_data->get_drawers_ip_source.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_ip_source(get_cm_node(drawers));

	return 0;
}


void get_drawer_ip_source(am_node_t *cm)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	FILL_INT(req.msg_base.netfn,	IPMI_LANCFG_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_LANCFG_CMD);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	req.msg_base.data[0] = GET_LANCFG_DATA0;
	req.msg_base.data[1] = GET_LANCFG_IPSRC_DATA1;
	req.msg_base.data[2] = GET_LANCFG_DATA2;
	req.msg_base.data[3] = GET_LANCFG_DATA3;
	FILL_INT(req.msg_base.data_len, GET_LANCFG_DATA_LEN);

	libjipmi_serial_br_cmd(&req, get_drawer_ip_source_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_ip_source(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_ip_source, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_ip_source.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_ip_source.expire = get_current_tick() + func.timeout;
	get_drawer_ip_source(cm);

	return;

next:
	process_next_func(cm);
	return;
}

int get_drawer_ip_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int i, j;
	//uint8 attrs_need_init = 0;
	unsigned int drawer_ipaddr;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	//if (IPMI_CC_OK != rsp[0])
		//goto err;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	if ((rsp[IP_RSP_OFFSET]&0xFF) == 0||
		(rsp[IP_RSP_OFFSET]&0xFF) == 0xFF||
		(rsp[IP_RSP_OFFSET+3]&0xFF) == 0||
		(rsp[IP_RSP_OFFSET+3]&0xFF) == 0xFF) {
		rmm_log(ERROR, "invalid ip addr\n");
		goto err;
	}
	
	drawer_ipaddr = (rsp[IP_RSP_OFFSET]&0xFF)|
				  ((rsp[IP_RSP_OFFSET+1]&0xFF)<<8)|
				  ((rsp[IP_RSP_OFFSET+2]&0xFF)<<16)|
				  ((rsp[IP_RSP_OFFSET+3]&0xFF)<<24);

	if (drawers_data->ips[pos] != drawer_ipaddr) {
		rmm_log(DBG, "drawer [%d] ip changed from [%x] to [%x].\n", pos, drawers_data->ips[pos], drawer_ipaddr);
		notify_drawer_ip_change(drawers, drawer_ipaddr);
	}
	drawers_data->ips[pos] = drawer_ipaddr;


	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_ip.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_ip(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	notify_drawer_ip_change(drawers, 0);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		drawers_data->get_drawers_ip.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_ip(get_cm_node(drawers));

	return 0;
}


void get_drawer_ip(am_node_t *cm)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	FILL_INT(req.msg_base.netfn,	IPMI_LANCFG_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_LANCFG_CMD);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	req.msg_base.data[0] = GET_LANCFG_DATA0;
	req.msg_base.data[1] = GET_LANCFG_DATA1;
	req.msg_base.data[2] = GET_LANCFG_DATA2;
	req.msg_base.data[3] = GET_LANCFG_DATA3;
	FILL_INT(req.msg_base.data_len, GET_LANCFG_DATA_LEN);

	libjipmi_serial_br_cmd(&req, get_drawer_ip_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_ip(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_ip, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->fresh_present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_ip.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_ip.expire = get_current_tick() + func.timeout;
	get_drawer_ip(cm);

	return;

next:
	process_next_func(cm);
	return;
}





int get_drawer_health_sensor_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	uint8 reading;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	
	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	rmm_log(DBG, "DZone health status [%d:%d] uuid[%s]\n", drawers_data->health_st[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);
	
	reading = rsp[1];
	if (dz_data->fresh_present & 1<<drawers_data->pos ||
		drawers_data->health_st[drawers_data->pos] != reading) {
		notify_drawer_health_status_change(drawers, reading);
	}
	drawers_data->health_st[drawers_data->pos] = reading;

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_health_sensor.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_health_sensor(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	notify_drawer_health_status_change(drawers, 0x80);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		drawers_data->get_drawers_health_sensor.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_health_sensor(get_cm_node(drawers));

	return 0;
}


void get_drawer_health_sensor(am_node_t *cm)
{
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	#if 0
	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		GET_AGGREGATED_HEALTH_SENSOR);
	FILL_STR(ipmi_req.name, 	am_rmcp_username);
	FILL_STR(ipmi_req.password, am_rmcp_password);
	FILL_INT(ipmi_req.data_len, 0);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &ipmi_req, get_drawer_health_sensor_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							pos,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
	#endif

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_AGGREGATED_HEALTH_SENSOR);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	FILL_INT(req.msg_base.data_len, 0);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	libjipmi_serial_br_cmd(&req, get_drawer_health_sensor_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_health_sensor(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_health_sensor, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_health_sensor.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_health_sensor.expire = get_current_tick() + func.timeout;
	get_drawer_health_sensor(cm);

	return;

next:
	process_next_func(cm);
	return;
}




int get_drawer_power_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int reading;
	int i, j;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	//if (IPMI_CC_OK != rsp[0])
	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	rsp = rsp+SERIAL_DUALBRIDGE_COMP_OFFSET; //debug, using serial

	#if 0
	rmm_log(DBG, "DZone temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;
	#endif

	if (drawers_data->sled_present[pos] != rsp[1]) {		
		rmm_log(DBG, "sled [%d] present changed from [%x] to [%x].\n", pos, drawers_data->sled_present[pos], rsp[1]);
		notify_sled_present_change(drawers, rsp[1]);
	}
	drawers_data->sled_present[pos] = rsp[1];

	//rmm_log(DBG, "------------!!!!!!!!!!!--- rsp len %d. rsp1[%x].\n", rsp_len, rsp[1]);

	for (i=0, j=0; i<8; i++) {
		if (rsp[1] & 1<<i) {
			rmm_log(DBG, "-----------rsp 2:%x, rsp3:%x..be:%x\n", rsp[2+j*2], rsp[3+j*2], (rsp[3+j*2]<<8));
			uint16 rsp_power = rsp[2+j*2] + (rsp[3+j*2]<<8);

			if (drawers_data->sled_data[pos].sled_power[i] != rsp_power) {
				notify_sled_power_change(drawers, i, rsp_power);
			}
			drawers_data->sled_data[pos].sled_power[i]= rsp_power;
			rmm_log(DBG, "drawer pos[%d] bitmap[0x%x] sled[%d] present. power[%d].\n", pos, rsp[1], i, drawers_data->sled_data[pos].sled_power[i]);
			j++;
		}
	}

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_power.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_power(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	//notify_drawer_temp_change(drawers, 0);
	drawers_data->sled_present[pos] = 0;
	notify_sled_present_change(drawers, 0);
	//set_fn_status(drawers, FUNC_FAIL);
	//process_next_func(get_cm_node(drawers));

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		drawers_data->get_drawers_power.status = FUNC_FAIL;
		process_next_func(get_cm_node(drawers));
		return -1;
	}
	drawers_data->pos = pos;
	get_drawer_power(get_cm_node(drawers));
	return 0;
}


void get_drawer_power(am_node_t *cm)
{
	//jipmi_msg_t ipmi_req = { {0} };
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

#if 0
	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		GET_TRAY_POWER);
	FILL_STR(ipmi_req.name, 	am_rmcp_username);
	FILL_STR(ipmi_req.password, am_rmcp_password);
	FILL_INT(ipmi_req.data_len, 0);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &ipmi_req, get_drawer_power_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							pos,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
#endif
	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		GET_TRAY_POWER);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	FILL_INT(req.msg_base.data_len, 0);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;

	libjipmi_serial_br_cmd(&req, get_drawer_power_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_power(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_power, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_power.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_power.expire = get_current_tick() + func.timeout;
	get_drawer_power(cm);

	return;

next:
	process_next_func(cm);
	return;
}


void calc_dzone_results(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	am_node_t *tzone = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tzone_data = (tzone_data_t *)tzone->priv_data;
	int pos = 0;
	int i = 0;
	int8 sled_present = 0;
	int dzone_sled_cnt = 0;
	int drawer_sled_cnt = 0;
	int avg_temp = 0;
	int tzone_temp = 0;
	int drawer_power = 0;
	int drawer_temp_total = 0;
	int drawer_temp_avg = 0;

	if (func_not_ready(drawers, drawers_data->calc_dzone_results, func.once) || platform_BDC_R==false)
		goto next;

	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			if(tzone_temp != 0 && dzone_sled_cnt != 0) {
				avg_temp = tzone_temp/dzone_sled_cnt;
				notify_tzone_temp_change(tzone_data->uuid[0], avg_temp);
			}
			drawers_data->calc_dzone_results.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		drawers_data->pos = pos;
		/* calc  dzone  total temp */
		if (drawers_data->sled_present[pos]) {
			for (i=0; i<MAX_SLED_NUM; i++) {
				if (drawers_data->sled_present[pos] & 1<<i) {
					dzone_sled_cnt++;
					drawer_sled_cnt++;
					tzone_temp += drawers_data->sled_data[pos].thermal_data[i].inlet_temp;
					drawer_power += drawers_data->sled_data[pos].sled_power[i];
					drawer_temp_total += drawers_data->sled_data[pos].thermal_data[i].inlet_temp;
				}

				/* sled browse finish */
				if (i == MAX_SLED_NUM-1) {
					/* drawer power add switch power */
					drawers_data->drawer_power[pos] += DEFUALT_DRAWER_POWER;
					/* notify drawer power */
					if (drawers_data->drawer_power[pos] != drawer_power) {
						notify_drawer_power_change(drawers, drawer_power);
						drawers_data->drawer_power[pos] = drawer_power;
					}

					/* calc drawer temp */
					if (drawer_sled_cnt != 0) {
						drawer_temp_avg = drawer_temp_total/drawer_sled_cnt;
						/* notify drawer temp */
						if (drawers_data->temps[pos] != drawer_temp_avg) {
							notify_drawer_temp_change(drawers, drawer_temp_avg);
							drawers_data->temps[pos] = drawer_temp_avg;
						}
					}
						
					/* clear up variables */
					drawer_power = 0;
					drawer_temp_total = 0;
					drawer_temp_avg = 0;
					drawer_sled_cnt = 0;
				}
				
			}
		}
		else {	// Drawer present but Sled present 0, consider PSME hang.
			/* notify drawer power */
			if (drawers_data->drawer_power[pos] != DEFUALT_DRAWER_POWER) {
				notify_drawer_power_change(drawers, DEFUALT_DRAWER_POWER);
				drawers_data->drawer_power[pos] = DEFUALT_DRAWER_POWER;
			}
			
			/* notify drawer temp */
			if (drawers_data->temps[pos] != 0) {
				notify_drawer_temp_change(drawers, 0);
				drawers_data->temps[pos] = 0;
			}
		}
	}
#if 0
	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->calc_dzone_results.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->calc_dzone_results.expire = get_current_tick() + func.timeout;
	get_drawer_power(cm);

	return;
#endif

next:
	process_next_func(cm);
	return;
}




/* 	return value:
	-1: get file stat error.
	0: file do not changed
	1: file changed
*/
int check_cert_file_timestamp(char *path, time_t *origin)
{
	struct stat buf = {};

	if( stat( path, &buf ) != -1 ) {
		if (*origin == 0 || buf.st_mtime != *origin) {
			rmm_log(INFO, "File last modified = %s. origin = %s.\n", ctime(&buf.st_mtime), ctime(origin));
			*origin = buf.st_mtime;
			return 1;	/* File exist and changed */
		}
	} else {
		rmm_log(ERROR, "Fetch File stat failed.\n"); /* File do not exist */
		return -1;
	}

	return 0;	/* File exist but not changed */
}

int read_cert_from_file(char *path, uint8 *cert, uint16 *cert_len)
{
	int rc;
	uint16 len;
	FILE *pFile=fopen(path,"r");
	
	if (pFile == NULL) {
	        rmm_log(ERROR, "Cert file do not exist.\n");
	        return -1;
	}
	fseek(pFile,0,SEEK_END);
	len = ftell(pFile);
	
	if (len <= 0) {
	        rmm_log(ERROR, "Cert file is empty or reading err.\n");
	        return -1;
	}
	
	rewind(pFile);
	rc = fread(cert,1,len,pFile);
	if (rc< len) {
	        rmm_log(ERROR, "Cert file reading err, fread return len %d, should be %d.\n", rc, len);
	        return -1;
	}
	cert[len]=0;

	*cert_len = len;

	//rmm_log(DBG, "Cert path %s read success, len is %d.\n", path, len);
	fclose(pFile);

	return 0;
}

int generate_cert_hash(uint8 *cert, uint16 cert_len, uint8 *md)
{
	MD5_CTX ctx;
	char buf[33]={'\0'};
	char tmp[3]={'\0'};
	int i;

	MD5_Init(&ctx);
	MD5_Update(&ctx,(unsigned char *)cert, cert_len);
	MD5_Final(md,&ctx);

	for( i=0; i<16; i++ ){
		snprintf_s_i(tmp, sizeof(tmp), "%02X",md[i]);
		strncat_s(buf, sizeof(buf), tmp, strnlen_s(tmp, RSIZE_MAX_STR));
	}
	rmm_log(DBG, "%s\n",buf);
	return 0;
}




#if 0
int get_drawer_RMMCert_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos = 0;
	int i = 0;
	int16 cert_len = 0;
	int16 cur_cert_loc = 0;
	int8 cur_chunk_num = 0;
	int8 cur_chunk_len = 0;	

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;

	#if 0
	rmm_log(DBG, "DZone temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;
	#endif
	cert_len = rsp[1] | rsp[2]<<8;
	cur_chunk_num = rsp[3];
	cur_chunk_len = rsp[4];
	cur_cert_loc = drawers_data->RMMCert_vars[pos].cur_pos;
	
	rmm_log(DBG, "DZone RMMCert, Cert Total Length[%d], Current Chunk Num[%d], Current Chunk Len[%d].", cert_len, cur_chunk_num, cur_chunk_len);

	for(i=0; i<cur_chunk_len; i++) {
		cur_cert_loc++;
		drawers_data->RMMCert[pos][cur_cert_loc] = rsp[i+5];
	}

	drawers_data->RMMCert_vars[pos].cur_pos = cur_cert_loc;
	drawers_data->RMMCertData.CertLen = cert_len;

	if (cur_cert_loc != cert_len) {
		get_drawer_RMMCert(get_cm_node(drawers), cur_chunk_num+1);
		return 0;
	}
	
	
	pos = get_next_drawer(drawers_data->pos, dz_data->present, MAX_DRAWER_NUM);
	if (pos == -1) {
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_RMMCert(get_cm_node(drawers), 0);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	//notify_drawer_temp_change(drawers, 0);
	set_fn_status(drawers, FUNC_FAIL);
	process_next_func(get_cm_node(drawers));

	return -1;
}


void get_drawer_RMMCert(am_node_t *cm, int8 chunk_num)
{
	jipmi_msg_t ipmi_req = { {0} };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	ipmi_req.data[0] = DRAWER_CERT_TYPE_RMM;
	ipmi_req.data[1] = chunk_num;
		
	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		DRAWER_GET_CERT);
	FILL_STR(ipmi_req.name, 	am_rmcp_username);
	FILL_STR(ipmi_req.password, am_rmcp_password);
	FILL_INT(ipmi_req.data_len, 2);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &ipmi_req, get_drawer_RMMCert_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							pos,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_RMMCert(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_RMMCert, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, dz_data->present, MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_RMMCert.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_RMMCert.expire = get_current_tick() + func.timeout;
	get_drawer_RMMCert(cm, 0);

	return;

next:
	process_next_func(cm);
	return;
}
#endif


int get_drawer_RMMCert_hash_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;

	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	/* debug */
	char buff[128] = {0};
	char tmp[8] = {0};
	for (i=0; i<CERT_HASH_LEN; i++) {
		snprintf_s_i(tmp, sizeof(tmp), " %x", rsp[i+SERIAL_DUALBRIDGE_DATA_OFFSET]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, RSIZE_MAX_STR));
	}
	rmm_log(INFO, "Hash value got is :%s\n", buff);
	/* debug end */

	for (i=0; i<CERT_HASH_LEN; i++) {
		if (rsp[i+SERIAL_DUALBRIDGE_DATA_OFFSET] != drawers_data->RMMCertData.CertHash[i]) {
			rmm_log(INFO, "Drawer[%d] RMM cert needs update.\n", pos);
			drawers_data->RMMCert_vars[pos].cert_need_update = true;
			break;
		} else if (i+1 == CERT_HASH_LEN)
			drawers_data->RMMCert_vars[pos].cert_need_update = false;
	}

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_RMMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}

#if 0
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->get_drawers_RMMCert_hash.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return 0;
		}

		/* newly added drawer */
		if (dz_data->fresh_present & 1<<pos) {
			drawers_data->RMMCert_vars[pos].cert_need_update = true;
			break;
		}
		
		/* check if need cert update */
		if (drawers_data->RMMCert_vars[pos].cert_need_update == true)
			break;
	}
#endif

	drawers_data->pos = pos;
	get_drawer_RMMCert_hash(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d] Drawer [%d].\n", result, rsp[0], rsp_len, pos);
	drawers_data->RMMCert_vars[pos].cert_need_update = false;

	/* loop the next drawer */
	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		process_next_func(get_cm_node(drawers));
		return -1;
	}

	drawers_data->pos = pos;
	get_drawer_RMMCert_hash(get_cm_node(drawers));
	return 0;
}


void get_drawer_RMMCert_hash(am_node_t *cm)
{
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	//rmm_log(DBG, "Fetch RMM cert Hash on pos[%d].\n", pos);

	req.msg_base.data[0] = DRAWER_CERT_TYPE_RMM;

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		DRAWER_GET_CERT_HASH);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;
	FILL_INT(req.msg_base.data_len, 1);

	libjipmi_serial_br_cmd(&req, get_drawer_RMMCert_hash_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_RMMCert_hash(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;
	
	if (func_not_ready(drawers, drawers_data->get_drawers_RMMCert_hash, func.once) || platform_BDC_R==false)
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL) 
		goto next;

#if 0
	if (cm_data->get_RMMCert_file.status == FUNC_FAIL)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1 || pos != 0) {	/* only pos 0 is available for debugging  */
		drawers_data->get_drawers_RMMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
#endif

#if 0
	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);

		//rmm_log(DBG, "check rmm cert hash [%d].\n", pos);
		
		if (pos == -1) {
			drawers_data->get_drawers_RMMCert_hash.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		/* newly added drawer */
		if (dz_data->fresh_present & 1<<pos) {
			//rmm_log(DBG, "fresh_present [%d] is set.\n", pos);
			drawers_data->RMMCert_vars[pos].cert_need_update = true;
			break;
		}
		
		/* check if need cert update */
		if (drawers_data->RMMCert_vars[pos].cert_need_update == true) {
			//rmm_log(DBG, "cert_need_update [%d] is set.\n", pos);
			break;
		}
	}
#endif

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {	/* only pos 0 is available for debugging  */
		drawers_data->get_drawers_RMMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	
	drawers_data->pos = pos;
	drawers_data->get_drawers_RMMCert_hash.expire = get_current_tick() + func.timeout;
	get_drawer_RMMCert_hash(cm);

	return;

next:
	process_next_func(cm);
	return;
}


void get_RMMCert_file(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	bool fn_state;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];
	int i;

	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;
	
	if (func_not_ready(cm, cm_data->get_RMMCert_file, func.once) || platform_BDC_R==false)
		goto next;

	if (check_cert_file_timestamp(RMM_CERT_PATH, &drawers_data->RMMCertData.Cert_mod_time) == 1 &&	/* cert file changed or first run*/
		read_cert_from_file(RMM_CERT_PATH, drawers_data->RMMCertData.Cert, &drawers_data->RMMCertData.CertLen) == 0 &&
		generate_cert_hash(drawers_data->RMMCertData.Cert, drawers_data->RMMCertData.CertLen, drawers_data->RMMCertData.CertHash) == 0) {
		cm_data->get_RMMCert_file.status = FUNC_SUCCESS;

		#if 0
		/* set update flags */
		for (i=0; i<MAX_DRAWER_NUM; i++)
			drawers_data->RMMCert_vars[i].cert_need_update = true;
		#endif
	}

#if 0
	/* Check Cert hash every time to avoid psme restart (Temporary solution) */
	for (i=0; i<MAX_DRAWER_NUM; i++) {
		//rmm_log(DBG, "-------------set rmm cert update flag for pos %d.\n", i);
		drawers_data->RMMCert_vars[i].cert_need_update = true;
	}
#endif

next:
	process_next_func(cm);
	return;
}



#if 0
int get_drawer_PODMCert_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos = 0;
	int i = 0;
	int16 cert_len = 0;
	int16 cur_cert_loc = 0;
	int8 cur_chunk_num = 0;
	int8 cur_chunk_len = 0;	

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	if (IPMI_CC_OK != rsp[0])
		goto err;

	#if 0
	rmm_log(DBG, "DZone temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;
	#endif
	cert_len = rsp[1] | rsp[2]<<8;
	cur_chunk_num = rsp[3];
	cur_chunk_len = rsp[4];
	cur_cert_loc = drawers_data->PODMCert_vars[pos].cur_pos;
	
	rmm_log(DBG, "DZone PODMCert, Cert Total Length[%d], Current Chunk Num[%d], Current Chunk Len[%d].", cert_len, cur_chunk_num, cur_chunk_len);

	for(i=0; i<cur_chunk_len; i++) {
		cur_cert_loc++;
		drawers_data->PODMCert[pos][cur_cert_loc] = rsp[i+5];
	}

	drawers_data->PODMCert_vars[pos].cur_pos = cur_cert_loc;
	drawers_data->PODMCertData.CertLen = cert_len;

	if (cur_cert_loc != cert_len) {
		get_drawer_PODMCert(get_cm_node(drawers), cur_chunk_num+1);
		return 0;
	}
	
	
	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		process_next_func(get_cm_node(drawers));
		return 0;
	}
	drawers_data->pos = pos;
	get_drawer_PODMCert(get_cm_node(drawers), 0);

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	//notify_drawer_temp_change(drawers, 0);
	set_fn_status(drawers, FUNC_FAIL);
	process_next_func(get_cm_node(drawers));

	return -1;
}


void get_drawer_PODMCert(am_node_t *cm, int8 chunk_num)
{
	jipmi_msg_t ipmi_req = { {0} };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	ipmi_req.data[0] = DRAWER_CERT_TYPE_PODM;
	ipmi_req.data[1] = chunk_num;
		
	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_INT(ipmi_req.cmd,		DRAWER_GET_CERT);
	FILL_STR(ipmi_req.name, 	am_rmcp_username);
	FILL_STR(ipmi_req.password, am_rmcp_password);
	FILL_INT(ipmi_req.data_len, 2);

	libjipmi_rmcp_cmd_send_msg(cm_data->ip_address, IPMI_RMCP_PORT, &ipmi_req, get_drawer_PODMCert_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							pos,
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_PODMCert(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->get_drawers_PODMCert, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_PODMCert.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->get_drawers_PODMCert.expire = get_current_tick() + func.timeout;
	get_drawer_PODMCert(cm, 0);

	return;

next:
	process_next_func(cm);
	return;
}
#endif


int get_drawer_PODMCert_hash_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos;
	int reading;
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] )
		goto err;

	/* debug */
	char buff[128] = {0};
	char tmp[8] = {0};
	for (i=0; i<CERT_HASH_LEN; i++) {
		snprintf_s_i(tmp, sizeof(tmp), " %x", rsp[i+SERIAL_DUALBRIDGE_DATA_OFFSET]);
		strncat_s(buff, sizeof(buff), tmp, strnlen_s(tmp, RSIZE_MAX_STR));
	}
	rmm_log(ERROR, "Hash value got is :%s\n", buff);
	/* debug end */

	for (i=0; i<CERT_HASH_LEN; i++) {
		if (rsp[i+SERIAL_DUALBRIDGE_DATA_OFFSET] != drawers_data->PODMCertData.CertHash[i]) {
			rmm_log(INFO, "Drawer[%d] PODM cert needs update.\n", pos);
			drawers_data->PODMCert_vars[pos].cert_need_update = true;
			break;
		} else if (i+1 == CERT_HASH_LEN)
			drawers_data->PODMCert_vars[pos].cert_need_update = false;
	}

	#if 0
	rmm_log(DBG, "DZone temp[%d:%d] uuid[%s]\n", drawers_data->temps[drawers_data->pos], rsp[1], drawers_data->uuid[drawers_data->pos]);

	reading = rsp[1];
	if (drawers_data->temps[drawers_data->pos] != reading) {
		notify_drawer_temp_change(drawers, reading);
	}
	drawers_data->temps[drawers_data->pos] = reading;
	#endif

	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_PODMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return 0;
	}

	#if 0
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->get_drawers_PODMCert_hash.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return 0;
		}

		/* newly added drawer */
		if (dz_data->fresh_present & 1<<pos) {
			drawers_data->PODMCert_vars[pos].cert_need_update = true;
			break;
		}

		/* check if need cert update */
		if (drawers_data->PODMCert_vars[pos].cert_need_update == true)
			break;
	}
	#endif
	
	drawers_data->pos = pos;
	get_drawer_PODMCert_hash(get_cm_node(drawers));

	return 0;

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	drawers_data->PODMCert_vars[pos].cert_need_update = false;

	/* loop the next drawer */
	pos = get_next_drawer(drawers_data->pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		set_fn_status(drawers, FUNC_FAIL);
		process_next_func(get_cm_node(drawers));
		return -1;
	}

	drawers_data->pos = pos;
	get_drawer_PODMCert_hash(get_cm_node(drawers));
	return 0;
}


void get_drawer_PODMCert_hash(am_node_t *cm)
{
	jipmi_serial_msg_t req = { };
	int pos = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	pos = drawers_data->pos;

	rmm_log(DBG, "Fetch PODM cert Hash on pos[%d].\n", pos);

	req.msg_base.data[0] = DRAWER_CERT_TYPE_PODM;

	FILL_INT(req.msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req.msg_base.cmd,		DRAWER_GET_CERT_HASH);
	FILL_STR(req.msg_base.name, 	am_rmcp_username);
	FILL_STR(req.msg_base.password, am_rmcp_password);
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = cm_data->serial_fd;
	FILL_INT(req.msg_base.data_len, 1);

	libjipmi_serial_br_cmd(&req, get_drawer_PODMCert_hash_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_NON_SYNC);
}


void get_drawers_PODMCert_hash(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;
	
	if (func_not_ready(drawers, drawers_data->get_drawers_PODMCert_hash, func.once) || platform_BDC_R==false)
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;

	if (drawers_data->PODMCertData.Cert_exist == false)
		goto next;

#if 0
	if (cm_data->get_PODMCert_file.status == FUNC_FAIL)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1 || pos != 0) {	/* only pos 0 is available for debugging  */
		drawers_data->get_drawers_PODMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
#endif

#if 0
	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->get_drawers_PODMCert_hash.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		/* newly added drawer */
		if (dz_data->fresh_present & 1<<pos) {
			drawers_data->PODMCert_vars[pos].cert_need_update = true;
			break;
		}

		/* check if need cert update */
		if (drawers_data->PODMCert_vars[pos].cert_need_update == true)
			break;
	}
#endif

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_PODMCert_hash.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	
	drawers_data->pos = pos;
	drawers_data->get_drawers_PODMCert_hash.expire = get_current_tick() + func.timeout;
	get_drawer_PODMCert_hash(cm);

	return;

next:
	process_next_func(cm);
	return;
}


void get_PODMCert_file(am_node_t *cm)
{
	jipmi_serial_msg_t req = {};
	bool fn_state;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t  func = fn_tbl[cm_data->func_idx];
	int i;
	int rc = 0;

	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;
	
	if (func_not_ready(cm, cm_data->get_PODMCert_file, func.once) || platform_BDC_R==false)
		goto next;

	rc = check_cert_file_timestamp(PODM_CERT_PATH, &drawers_data->PODMCertData.Cert_mod_time);

	if (rc == -1)
		drawers_data->PODMCertData.Cert_exist = false;
	else
		drawers_data->PODMCertData.Cert_exist = true;
	
	if (rc == 1 &&	/* cert file changed or first run*/
		read_cert_from_file(PODM_CERT_PATH, drawers_data->PODMCertData.Cert, &drawers_data->PODMCertData.CertLen) == 0 &&
		generate_cert_hash(drawers_data->PODMCertData.Cert, drawers_data->PODMCertData.CertLen, drawers_data->PODMCertData.CertHash) == 0) {
		cm_data->get_PODMCert_file.status = FUNC_SUCCESS;
		rmm_log(INFO, "PODM Cert file renewed!\n");
		
		#if 0
		/* set update flags */
		for (i=0; i<MAX_DRAWER_NUM; i++)
			drawers_data->PODMCert_vars[i].cert_need_update = true;
		#endif
	}
	
next:
	process_next_func(cm);
	return;
}



#if 0
int get_drawers_lan_cfg_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}

	return 0;
}


void get_drawer_lan_cfg(am_node_t *cm)
{
	struct jipmi_msg ipmi_req = {};
	int host= 0;
	int i;
	int64 error_code = 0;
	memdb_integer node_id;
	int field_instance;

	FILL_INT(ipmi_req.netfn,	IPMI_CM_NETFN);
	FILL_STR(ipmi_req.name,		am_rmcp_username);
	FILL_STR(ipmi_req.password,	am_rmcp_password);
//	FILL_INT(ipmi_req.cmd,		GET_ID_FIELD_CMD);
//	FILL_INT(ipmi_req.data_len,	0);

	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &ipmi_req, get_drawers_lan_cfg_rsp, &node_id, JIPMI_NON_SYNC);
}

void get_drawers_lan_cfg(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_rack_puid, func.once) || platform_BDC_R==false)
		goto next;

	pos = get_next_drawer(-1, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
	if (pos == -1) {
		drawers_data->get_drawers_lancfg.status = FUNC_SUCCESS;
		process_next_func(get_cm_node(drawers));
		return;
	}
	drawers_data->pos = pos;
	drawers_data->set_drawers_rack_puid.expire = get_current_tick() + func.timeout;
	set_drawer_rack_puid(cm, pos);

	return;

next:
	process_next_func(cm);
	return;
}
#endif


int set_drawer_RMMCert_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos = 0;
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] ) {
		if (drawers_data->RMMCertData.retry_cnt < MAX_CERT_RETRY)
			goto retry;
		else
			goto err;
	}

	/* clear retry cnt */
	if (drawers_data->RMMCertData.retry_cnt)
		drawers_data->RMMCertData.retry_cnt = 0;

	rmm_log(DBG, "DZone RMMCert, Drawer [%d] Cert Total Length[%d], Current Chunk Num[%d], Current Pos [%d].",
		drawers_data->pos,
		drawers_data->RMMCertData.CertLen, 
		drawers_data->RMMCert_vars[pos].cur_chunk_num,
		drawers_data->RMMCert_vars[pos].cur_pos);

	if (drawers_data->RMMCert_vars[pos].cur_pos < drawers_data->RMMCertData.CertLen) {
		set_drawer_RMMCert(get_cm_node(drawers), drawers_data->RMMCert_vars[pos].cur_chunk_num+1);
		return 0;
	}

	//rmm_log(DBG, "---------set pos [%d] RMM cert success.\n", pos);
	/* finished and clean variables */
	//cert_in_progress = false;
	drawers_data->RMMCert_vars[pos].cert_need_update = false;
	drawers_data->RMMCert_vars[pos].cur_pos = 0;
	drawers_data->RMMCert_vars[pos].cur_chunk_num= 0;

	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);

		if (pos == -1) {
			drawers_data->set_drawers_RMMCert.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return 0;
		}

		/* check if need cert update */
		if (drawers_data->RMMCert_vars[pos].cert_need_update == true)
			break;
	}
	
	drawers_data->pos = pos;
	set_drawer_RMMCert(get_cm_node(drawers), 0);
	return 0;

retry:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d]. RMM Cert Retry Cnt %d. Drawer [%d]\n", result, rsp[0], rsp_len, drawers_data->RMMCertData.retry_cnt, drawers_data->pos);
	drawers_data->RMMCertData.retry_cnt++;
	libjipmi_serial_br_cmd(&drawers_data->RMMCertData.req, set_drawer_RMMCert_rsp_cb, drawers,
		0x10,
		0x2,
		0x24,
		DRAWER_POS_TO_CH_ID(pos),
		IPMI_BRIDGE_MSG_DUAL,
		JIPMI_NON_SYNC);
	return 0;
	

err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);/* clear retry cnt */
	/* clear retry cnt */
	if (drawers_data->RMMCertData.retry_cnt)
		drawers_data->RMMCertData.retry_cnt = 0;
	//cert_in_progress = false; /* error */
	drawers_data->RMMCert_vars[pos].cur_pos = 0;

	/* loop the next drawer */
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);

		if (pos == -1) {
			drawers_data->set_drawers_RMMCert.status = FUNC_FAIL;
			process_next_func(get_cm_node(drawers));
			return 0;
		}
		rmm_log(INFO, "###############pos [%d] update flag [%d].\n", pos, drawers_data->RMMCert_vars[pos].cert_need_update);

		/* check if need cert update */
		if (drawers_data->RMMCert_vars[pos].cert_need_update == true)
			break;
	}

	drawers_data->pos = pos;
	set_drawer_RMMCert(get_cm_node(drawers), 0);

	return 0;
}


void set_drawer_RMMCert(am_node_t *cm, int8 chunk_num)
{
	int pos = 0;
	int cur_chunk_len = 0;
	int left_len = 0;
	int i = 0;
	int data_len = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	jipmi_serial_msg_t *req = &drawers_data->RMMCertData.req;
	pos = drawers_data->pos;
	
	memset(req, 0, sizeof(jipmi_serial_msg_t));
	req->msg_base.data[data_len++] = DRAWER_CERT_TYPE_RMM;
	req->msg_base.data[data_len++] = drawers_data->RMMCertData.CertLen;
	req->msg_base.data[data_len++] = drawers_data->RMMCertData.CertLen>>8;
	req->msg_base.data[data_len++] = chunk_num;
	drawers_data->RMMCert_vars[pos].cur_chunk_num = chunk_num;

	left_len = drawers_data->RMMCertData.CertLen - drawers_data->RMMCert_vars[pos].cur_pos;

	if (left_len >= MAX_CERT_CHUNK_LEN)
		cur_chunk_len = MAX_CERT_CHUNK_LEN;
	else
		cur_chunk_len = left_len;

	req->msg_base.data[data_len++] = cur_chunk_len;

#if 0
	for (i=0; i<cur_chunk_len; i++) {
		//req.msg_base.data[data_len++] = drawers_data->RMMCert[pos][drawers_data->RMMCert_vars[pos].cur_pos++]; /* commented for debug */
		req->msg_base.data[data_len++] = i;	/*  debug, should be deleted */
		drawers_data->RMMCert_vars[pos].cur_pos++;	/*  debug, should be deleted */
	}
#endif

	for (i=0; i<cur_chunk_len; i++) {
		req->msg_base.data[data_len++] = drawers_data->RMMCertData.Cert[drawers_data->RMMCert_vars[pos].cur_pos++];
	}

	rmm_log(DBG, "Drawer [%d] Cert Len [%d] left len[%d] cur chunk len[%d] data len[%d].\n", 
		drawers_data->pos, drawers_data->RMMCertData.CertLen, left_len, cur_chunk_len, data_len);
		
	FILL_INT(req->msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req->msg_base.cmd,		DRAWER_SET_CERT);
	FILL_STR(req->msg_base.name, 	am_rmcp_username);
	FILL_STR(req->msg_base.password, am_rmcp_password);
	req->serial_flag = IPMI_SERIAL_OPERATION;
	req->serial_fd = cm_data->serial_fd;
	FILL_INT(req->msg_base.data_len, data_len);

	libjipmi_serial_br_cmd(req, set_drawer_RMMCert_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_SYNC);
}


void set_drawers_RMMCert(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_RMMCert, func.once) || platform_BDC_R==false)
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;

	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->set_drawers_RMMCert.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		/* check if need cert update */
		if (drawers_data->RMMCert_vars[pos].cert_need_update == true)
			break;
	}

	drawers_data->pos = pos;
	drawers_data->set_drawers_RMMCert.expire = get_current_tick() + func.timeout;
	//cert_in_progress = true;
	set_drawer_RMMCert(cm, 0);

	return;

next:
	process_next_func(cm);
	return;
}




int set_drawer_PODMCert_rsp_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	am_node_t *drawers;
	drawers_data_t *drawers_data;
	am_node_t *cm;
	am_node_t *dz;
	dzone_data_t *dz_data;
	int pos = 0;
	int i = 0;

	if (NULL == cb_data) {
		rmm_log(ERROR, "cb_data is NULL\n");
		exit(-1);
	}

	drawers = (am_node_t *)cb_data;
	drawers_data = (drawers_data_t *)(drawers->priv_data);
	dz = drawers->parent;
	dz_data = (dzone_data_t *)(dz->priv_data);
	pos = drawers_data->pos;
	
	if (IPMI_CC_OK != rsp[0] || rsp_len <= SERIAL_DUALBRIDGE_COMP_OFFSET || IPMI_CC_OK != rsp[SERIAL_DUALBRIDGE_COMP_OFFSET] ) {
		if (drawers_data->PODMCertData.retry_cnt < MAX_CERT_RETRY)
			goto retry;
		else
			goto err;
	}

	/* clear retry cnt */
	if (drawers_data->PODMCertData.retry_cnt)
		drawers_data->PODMCertData.retry_cnt = 0;

	rmm_log(DBG, "DZone PODMCert, Drawer [%d], Cert Total Length[%d], Current Chunk Num[%d], Current Pos [%d].\n", 
		drawers_data->pos,
		drawers_data->PODMCertData.CertLen, 
		drawers_data->PODMCert_vars[pos].cur_chunk_num,
		drawers_data->PODMCert_vars[pos].cur_pos);

	if (drawers_data->PODMCert_vars[pos].cur_pos < drawers_data->PODMCertData.CertLen) {
		set_drawer_PODMCert(get_cm_node(drawers), drawers_data->PODMCert_vars[pos].cur_chunk_num+1);
		return 0;
	}

	rmm_log(DBG, "---------set pos [%d] PODM cert success.\n", pos);
	/* finished and clean variables */
	//cert_in_progress = false;
	drawers_data->PODMCert_vars[pos].cert_need_update = false;
	drawers_data->PODMCert_vars[pos].cur_pos = 0;
	drawers_data->PODMCert_vars[pos].cur_chunk_num= 0;
	
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->set_drawers_PODMCert.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return 0;
		}

		/* check if need cert update */
		if (drawers_data->PODMCert_vars[pos].cert_need_update == true)
			break;
	}
	
	drawers_data->pos = pos;
	set_drawer_PODMCert(get_cm_node(drawers), 0);

	return 0;

retry:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d]. PODM Retry Cnt %d Drawer [%d].\n", result, rsp[0], rsp_len, drawers_data->PODMCertData.retry_cnt, drawers_data->pos);
	drawers_data->PODMCertData.retry_cnt++;
	libjipmi_serial_br_cmd(&drawers_data->PODMCertData.req, set_drawer_PODMCert_rsp_cb, drawers,
		0x10,
		0x2,
		0x24,
		DRAWER_POS_TO_CH_ID(pos),
		IPMI_BRIDGE_MSG_DUAL,
		JIPMI_NON_SYNC);
	return 0;
	
err:
	rmm_log(ERROR, "response error IPMI CC result[%d] rsp_0[%d] len[%d].\n", result, rsp[0], rsp_len);
	/* clear retry cnt */
	if (drawers_data->PODMCertData.retry_cnt)
		drawers_data->PODMCertData.retry_cnt = 0;
	//cert_in_progress = false; /* error */
	drawers_data->PODMCert_vars[pos].cur_pos = 0;

	/* loop the next drawer */
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);

		if (pos == -1) {
			drawers_data->set_drawers_PODMCert.status = FUNC_FAIL;
			process_next_func(get_cm_node(drawers));
			return -1;
		}
		rmm_log(INFO, "###############pos [%d] update flag [%d].\n", pos, drawers_data->PODMCert_vars[pos].cert_need_update);

		/* check if need cert update */
		if (drawers_data->PODMCert_vars[pos].cert_need_update == true)
			break;
	}

	drawers_data->pos = pos;
	set_drawer_PODMCert(get_cm_node(drawers), 0);
	return 0;
}


void set_drawer_PODMCert(am_node_t *cm, int8 chunk_num)
{
	int pos = 0;
	int cur_chunk_len = 0;
	int left_len = 0;
	int i = 0;
	int data_len = 0;

	/* Get vailidate node. */
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;

	jipmi_serial_msg_t *req = &drawers_data->PODMCertData.req;
	pos = drawers_data->pos;

	memset(req, 0, sizeof(jipmi_serial_msg_t));
	req->msg_base.data[data_len++] = DRAWER_CERT_TYPE_PODM;
	req->msg_base.data[data_len++] = drawers_data->PODMCertData.CertLen;
	req->msg_base.data[data_len++] = drawers_data->PODMCertData.CertLen>>8;
	req->msg_base.data[data_len++] = chunk_num;
	drawers_data->PODMCert_vars[pos].cur_chunk_num = chunk_num;

	left_len = drawers_data->PODMCertData.CertLen - drawers_data->PODMCert_vars[pos].cur_pos;

	if (left_len >= MAX_CERT_CHUNK_LEN)
		cur_chunk_len = MAX_CERT_CHUNK_LEN;
	else
		cur_chunk_len = left_len;

	req->msg_base.data[data_len++] = cur_chunk_len;

	rmm_log(DBG, "Drawer [%d] Cert Len [%d] left len[%d] cur chunk num [%d] chunk len[%d] data len[%d].\n", 
		drawers_data->pos, drawers_data->PODMCertData.CertLen, left_len, chunk_num, cur_chunk_len, data_len);

	for (i=0; i<cur_chunk_len; i++) {
		req->msg_base.data[data_len++] = drawers_data->PODMCertData.Cert[drawers_data->PODMCert_vars[pos].cur_pos++];
	}
		
	FILL_INT(req->msg_base.netfn,	IPMI_CM_NETFN);
	FILL_INT(req->msg_base.cmd,		DRAWER_SET_CERT);
	FILL_STR(req->msg_base.name, 	am_rmcp_username);
	FILL_STR(req->msg_base.password, am_rmcp_password);
	req->serial_flag = IPMI_SERIAL_OPERATION;
	req->serial_fd = cm_data->serial_fd;
	FILL_INT(req->msg_base.data_len, data_len);

	libjipmi_serial_br_cmd(req, set_drawer_PODMCert_rsp_cb, drawers,
							0x10,
							0x2,
							0x24,
							DRAWER_POS_TO_CH_ID(pos),
							IPMI_BRIDGE_MSG_DUAL,
							JIPMI_SYNC);
}


void set_drawers_PODMCert(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	int pos = 0;

	if (func_not_ready(drawers, drawers_data->set_drawers_PODMCert, func.once) || platform_BDC_R==false)
		goto next;

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;

	if (drawers_data->PODMCertData.Cert_exist == false)
		goto next;

	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1) {
			drawers_data->set_drawers_PODMCert.status = FUNC_SUCCESS;
			process_next_func(get_cm_node(drawers));
			return;
		}

		/* check if need cert update */
		if (drawers_data->PODMCert_vars[pos].cert_need_update == true)
			break;
	}
	
	drawers_data->pos = pos;
	drawers_data->set_drawers_PODMCert.expire = get_current_tick() + func.timeout;
	//cert_in_progress = true;
	set_drawer_PODMCert(cm, 0);

	return;

next:
	process_next_func(cm);
	return;
}



