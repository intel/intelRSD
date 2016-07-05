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


#include <stdio.h>
#include "libjson/json.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "notify.h"
#include "libutils/utils.h"

int notify_tzone_add(am_node_t *cm)
{
	memdb_integer rsp_node_id;
	cm_data_t *cm_data = (cm_data_t *)(cm->priv_data);
	am_node_t *tz = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tzone_data = (tzone_data_t *)tz->priv_data;
	int64 tz_lid = 1;

	tzone_data->pos = tz_lid - 1;
	gen_uuid_v4(tzone_data->uuid[tzone_data->pos]);
	assetd_tzone_add(&tz_lid, tzone_data->uuid[tzone_data->pos], cm_data->uuid);
	assetd_tzone_attr_init(tzone_data->uuid[tzone_data->pos], cm_data->loc_id, &tz_lid);

	return 0;
}

int notify_dzone_add(am_node_t *cm)
{
	cm_data_t *data = (cm_data_t *)(cm->priv_data);
	am_node_t *dz = node_lookup(cm, DZONE_NODE);
	dzone_data_t *dzone_data = (dzone_data_t *)dz->priv_data;
	int64 dz_lid = 1;

	dzone_data->pos = dz_lid - 1;
	gen_uuid_v4(dzone_data->uuid[dzone_data->pos]);
	assetd_dzone_add(&dz_lid, dzone_data->uuid[dzone_data->pos], data->uuid);
	assetd_dzone_attr_init(dzone_data->uuid[dzone_data->pos], &dz_lid);

	return 0;
}

static int notifiy_cm_present_changed(am_node_t *cm, int evt_id)
{
	cm_data_t *data = (cm_data_t *)(cm->priv_data);
	int64 cm_lid = data->loc_id;

	if ((cm->node_status == ONLINE && evt_id == ADD_EVT)) {
		return 0;
	}

	if (evt_id == ADD_EVT) {
		gen_uuid_v4(data->uuid);
		assetd_cm_add(&cm_lid, data->uuid);
	} else if (evt_id == REMOVE_EVT) {
		if (is_str_uuid(data->uuid))
			assetd_cm_remove(data->uuid);
	} else
		return -1;

	return 0;
}


static int cm_attr_init(am_node_t *cm)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	int64 cm_lid = cm_data->loc_id;

	return assetd_cm_attr_init(cm_data->uuid, &cm_lid);
}


int notify_cm_add(am_node_t *cm)
{
	if (notifiy_cm_present_changed(cm, ADD_EVT) == -1)
		return -1;

	if (cm_attr_init(cm) == -1)
		return -1;

	return 0;
}

int notify_cm_remove(am_node_t *cm)
{
	return notifiy_cm_present_changed(cm, REMOVE_EVT);
}


int notify_cm_ip_change(am_node_t *cm, unsigned int ip_addr)
{
	cm_data_t *cm_data = (cm_data_t *)(cm->priv_data);
	int64 data = ip_addr;
	int64 ver_high = cm_data->cm_ver.high;
	int64 ver_low = cm_data->cm_ver.low;

	return assetd_cm_ip_change(&data, &ver_high, &ver_low, cm_data->uuid);
}

int notify_cm_ver_change(am_node_t *cm, version_t *new_ver)
{
	int evt_id = CHANGE_EVT;
	cm_data_t *cm_data = (cm_data_t *)(cm->priv_data);
	int64 ver_high = new_ver->high;
	int64 ver_low = new_ver->low;
	int64 data = cm_data->ip_address;

	return assetd_cm_ver_change(&data, &ver_high, &ver_low, cm_data->uuid);
}

int notify_pzone_present_change(am_node_t *pz, int present)
{
	int evt_id;
	cm_data_t *cm_data = (cm_data_t *)pz->parent->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;
	int64 type = PZ_TYPE;
	int64 pz_lid = 1;
	int cm_lid = cm_data->loc_id;

	if (present == 1)
		evt_id = ADD_EVT;
	else
		evt_id = REMOVE_EVT;

	if (pz->node_status == ONLINE && evt_id == ADD_EVT)
		return 0;

	if (evt_id == ADD_EVT) {
		pz_data->pos = pz_lid - 1;
		gen_uuid_v4(pz_data->uuid[pz_data->pos]);
		assetd_pzone_add(&pz_lid, pz_data->uuid[pz_data->pos], cm_data->uuid);
		assetd_pzone_attr_init(pz_data->uuid[pz_data->pos], cm_lid, &pz_lid);
	} else if (evt_id == REMOVE_EVT) {
		assetd_pzone_remove(pz_data->uuid[pz_data->pos]);
	} else
		return -1;

	return 0;
}

static void present_change(int64 type, int old_present, int present, int max, char *uuids, char *parent_uuid, int cm_lid)
{
	int64 i = 0;
	bool old_bit = 0;
	bool new_bit = 0;
	char *curr_uuid  = NULL;

	for (i = 0; i < max; i++) {
		old_bit = (old_present >> i) & 1;
		new_bit = (present >> i) & 1;
		curr_uuid = uuids + i * UUID_MAX_LEN;
		if (old_bit != new_bit) {
			if (new_bit) {
				gen_uuid_v4(curr_uuid);
				switch (type) {
				case PSU_TYPE:
					assetd_psu_add(i, curr_uuid, parent_uuid);
					assetd_psu_attr_init(curr_uuid, cm_lid, i);
					break;
				case FAN_TYPE:
					assetd_fan_add(i, curr_uuid,  parent_uuid);
					assetd_fan_attr_init(curr_uuid, cm_lid, i);
					break;
				case DRAWER_TYPE:
					assetd_drawer_add(i, curr_uuid, parent_uuid);
					assetd_drawer_attr_init(curr_uuid, cm_lid, i);
					break;
				case SLED_TYPE:
					assetd_sled_add(i, curr_uuid, parent_uuid);
					assetd_sled_attr_init(curr_uuid, cm_lid, i);
					break;
				default:
					break;
				}
			} else {
				switch (type) {
				case PSU_TYPE:
					assetd_psu_remove(curr_uuid);
					break;
				case FAN_TYPE:
					assetd_fan_remove(curr_uuid);
					break;
				case DRAWER_TYPE:
					assetd_drawer_remove(curr_uuid);
					break;
				case SLED_TYPE:
					assetd_sled_remove(curr_uuid);
					break;
				default:
					break;
				}
			}
		}
	}
}

int notify_psu_present_change(am_node_t *pz, int present)
{
	am_node_t *cm = pz->parent;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)pz->priv_data;
	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int64 type = PSU_TYPE;
	int old_present = pz_data->psu_present;
	int cm_lid = cm_data->loc_id;

	present_change(type, old_present, present, MAX_PSU_NUM, (char *)psus_data->uuid, pz_data->uuid[pz_data->pos], cm_lid);

	return 0;
}

int notify_psu_power_in_change(am_node_t *psus, int pin, uint8 presence)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int32 i = 0;
	int64 data = pin;
	int64 total = 0;

	for (i = 0; i < MAX_PSU_NUM; i++) {
		if (1<<i & presence)
			total += psus_data->power_in[i];
	}

	return assetd_psu_power_in_change(&data, &total, psus_data->uuid[psus_data->pos]);
}

int notify_psu_current_out_change(am_node_t *psus, int iout, uint8 presence)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int32 i = 0;
	int64 data = iout;
	int64 total = 0;

	for (i = 0; i < MAX_PSU_NUM; i++) {
		if (1<<i & presence)
			total += psus_data->current_out[i];
	}

	return assetd_psu_current_out_change(&data, &total, psus_data->uuid[psus_data->pos]);
}

int notify_psu_power_out_change(am_node_t *psus, int pout, uint8 presence)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int32 i = 0;
	int64 data = pout;
	int64 total = 0;

	for (i = 0; i < MAX_PSU_NUM; i++) {
		if (1<<i & presence)
			total += psus_data->power_out[i];
	}

	return assetd_psu_power_out_change(&data, &total, psus_data->uuid[psus_data->pos]);
}


int notify_psu_health_status_change(am_node_t *psus, int status, uint8 presence)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int32 i = 0;
	int64 data = status;
	int64 total = 0;

	return assetd_psu_health_status_change(&data, psus_data->uuid[psus_data->pos]);
}


int notify_psu_power_cap_change(am_node_t *psus, int cap)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	int32 i = 0;
	int64 data = cap;

	return assetd_psu_power_cap_change(&data, psus_data->uuid[psus_data->pos]);
}

int notify_psu_total_power_cap_change(am_node_t *psus, int cap)
{
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;
	int64 pz_lid = 1;
	int64 data = cap;
	
	pz_data->pos = pz_lid - 1;
	
	return assetd_psu_total_power_cap_change(&data, pz_data->uuid[pz_data->pos]);
}


int notify_psu_serial_change(am_node_t *psus, char *serial)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	return assetd_psu_serial_change(serial, psus_data->uuid[psus_data->pos]);
}

int notify_psu_manufacture_change(am_node_t *psus, char *manufacture)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	return assetd_psu_manufacture_change(manufacture, psus_data->uuid[psus_data->pos]);
}

int notify_psu_fw_ver_change(am_node_t *psus, char *fw_ver)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	return assetd_psu_fw_ver_change(fw_ver, psus_data->uuid[psus_data->pos]);
}

int notify_psu_model_change(am_node_t *psus, char *model)
{
	cm_data_t *cm_data = (cm_data_t *)(psus->parent->parent->priv_data);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;

	return assetd_psu_model_change(model, psus_data->uuid[psus_data->pos]);
}

int notify_fan_present_change(am_node_t *tz, int present)
{
	am_node_t *cm = tz->parent;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	tzone_data_t *tz_data = (tzone_data_t *)tz->priv_data;
	am_node_t *fans = node_lookup(cm, FANS_NODE);
	fans_data_t *fans_data = (fans_data_t *)fans->priv_data;
	int64 type = FAN_TYPE;
	int old_present = tz_data->present;
	int cm_lid = cm_data->loc_id;

	present_change(type, old_present, present, MAX_PWM_NUM, (char *)fans_data->uuid, tz_data->uuid[tz_data->pos], cm_lid);

	return assetd_fan_present(&present, tz_data->uuid[tz_data->pos]);
}

int notify_tzone_rpm_change(char *uuid, int32 rpm)
{
	return assetd_tzone_rpm_change(uuid, rpm);
}

int notify_tzone_temp_change(char *uuid, int32 temp)
{
	return assetd_tzone_temp_change(uuid, temp);
}


int notify_fans_speed_change(am_node_t *fans, unsigned int *data)
{
	am_node_t *cm = get_cm_node(fans);
	cm_data_t *cm_data = (cm_data_t *)(cm->priv_data);
	fans_data_t *fans_data = (fans_data_t *)fans->priv_data;
	int i = 0;

	for (i = 0; i < MAX_PWM_NUM; i++) {
		assetd_fans_speed_change(&data[i], fans_data->uuid[i]);
	}

	return 0;
}

int notify_drawer_alert_change(am_node_t *dz, int alert)
{
	cm_data_t *cm_data = (cm_data_t *)dz->parent->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)dz->priv_data;

	return assetd_drawer_alert_change(&alert, dz_data->uuid[dz_data->pos]);
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

int notify_drawer_present_change(am_node_t *dz, int present)
{
	am_node_t *cm = dz->parent;
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)dz->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	int64 type = DRAWER_TYPE;
	int old_present = convert_8bit_to_4bit(dz_data->present);
	int new_present = convert_8bit_to_4bit(present);
	int cm_lid = cm_data->loc_id;

	present_change(type, old_present, new_present, MAX_DRAWER_NUM, (char *)drawers_data->uuid, dz_data->uuid[dz_data->pos], cm_lid);

	return assetd_drawer_present(&new_present, dz_data->uuid[dz_data->pos]);
}

int notify_drawer_temp_change(am_node_t *drawers, int data)
{
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 temp = data;

	if (drawers_data->temps[drawers_data->pos] == data)
		return 0;

	return assetd_drawer_temp_change(&temp, drawers_data->uuid[drawers_data->pos]);
}

int notify_drawer_health_status_change(am_node_t *drawers, uint8 data)
{
	#if 0	//No use, commented temporarily
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 health = data;

	return assetd_drawer_health_status_change(&health, drawers_data->uuid[drawers_data->pos]);
	#endif

	return 0;
}


int notify_drawer_ip_change(am_node_t *drawers, unsigned int ip_addr)
{
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 data = ip_addr;

	if (drawers_data->ips[drawers_data->pos] != 0 &&
		drawers_data->ips[drawers_data->pos] == ip_addr)
		return 0;

	return assetd_drawer_ip_change(&data, drawers_data->uuid[drawers_data->pos]);
}

int notify_drawer_power_change(am_node_t *drawers, unsigned int power)
{
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 data = power;

	return assetd_drawer_power_change(&data, drawers_data->uuid[drawers_data->pos]);
}


int notify_drawer_ip_source_change(am_node_t *drawers, uint8 ip_src)
{
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 data = ip_src;

	if (drawers_data->ip_sources[drawers_data->pos] == ip_src)
		return 0;

	return assetd_drawer_ip_source_change(&data, drawers_data->uuid[drawers_data->pos]);
}

int notify_drawer_subnet_mask_change(am_node_t *drawers, unsigned int mask)
{
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	int64 data = mask;

	if (drawers_data->subnet_masks[drawers_data->pos] != 0 && 
		drawers_data->subnet_masks[drawers_data->pos] == mask)
		return 0;

	return assetd_drawer_subnet_mask_change(&data, drawers_data->uuid[drawers_data->pos]);
}



int notify_sled_present_change(am_node_t *drawers, uint8 present)
{
	#if 0	//No use, commented temporarily
	//am_node_t *cm = dz->parent;
	am_node_t *cm = get_cm_node(drawers);
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	//dzone_data_t *dz_data = (dzone_data_t *)dz->priv_data;
	//am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	int64 type = SLED_TYPE;
	int old_present = drawers_data->sled_present[drawers_data->pos];
	int new_present = present;
	int cm_lid = cm_data->loc_id;

	present_change(type, old_present, new_present, MAX_SLED_NUM, (char *)drawers_data->sled_uuid[drawers_data->pos], drawers_data->uuid[drawers_data->pos], cm_lid);

	return assetd_sled_present(&new_present, drawers_data->uuid[drawers_data->pos]);
	#endif

	return 0;
}

int notify_sled_type_change(am_node_t *drawers, uint8 sled_loc, uint8 sled_type)
{
	#if 0	//No use, commented temporarily
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);

	//if (drawers_data->sled_data[drawers_data->pos].thermal_data[sled_loc].sled_type == sled_type)
		//return 0;

	return assetd_sled_type_change(&sled_type, drawers_data->sled_uuid[drawers_data->pos][sled_loc]);
	#endif

	return 0;
}

int notify_sled_temp_change(am_node_t *drawers, uint8 sled_loc, uint8 temp)
{
	#if 0	//No use, commented temporarily
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);
	uint8 total, count = 0;
	uint8 avg = 0;
	uint8 i = 0;

	if (drawers_data->sled_data[drawers_data->pos].thermal_data[sled_loc].inlet_temp == temp)
		return 0;

	for (i = 0; i < MAX_SLED_NUM; i ++) {
		if (drawers_data->sled_data[drawers_data->pos].thermal_data[i].inlet_temp != 0) {
			total += drawers_data->sled_data[drawers_data->pos].thermal_data[i].inlet_temp;
			count ++;
		}
	}

	if (count != 0)
		avg = total/count;

	return assetd_sled_temp_change(&temp, drawers_data->sled_uuid[drawers_data->pos][sled_loc], &avg);
	#endif

	return 0;
}


int notify_sled_pwm_change(am_node_t *drawers, uint8 sled_loc, uint8 pwm)
{
	#if 0	//No use, commented temporarily
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);

	if (drawers_data->sled_data[drawers_data->pos].thermal_data[sled_loc].desired_pwm == pwm)
		return 0;

	return assetd_sled_pwm_change(&pwm, drawers_data->sled_uuid[drawers_data->pos][sled_loc]);
	#endif

	return 0;
}


int notify_sled_power_change(am_node_t *drawers, uint8 sled_loc, uint16 power)
{
	#if 0	//No use, commented temporarily
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	am_node_t *cm = get_cm_node(drawers);

	if (drawers_data->sled_data[drawers_data->pos].sled_power[sled_loc] == power)
		return 0;

	return assetd_sled_power_change(&power, drawers_data->sled_uuid[drawers_data->pos][sled_loc]);
	#endif

	return 0;
}

int notify_tzone_airflow_change(char *uuid, int32 q)
{
	return assetd_tzone_airflow_change(uuid, q);
}

int notify_tzone_outlet_temp_change(char *uuid, int32 temp)
{
	return assetd_tzone_outlet_temp_change(uuid, temp);
}




