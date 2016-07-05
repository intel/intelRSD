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

#include "config.h"

#ifdef RACK_PTAS

#include "notify.h"
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "libcfg/cfg.h"

#include "ptas_sdk/ptas_sdk.h"

static bool rack_ptas_init = false;
static bool rack_ptas_daemon_start = false;
static PTAS_Output_t output_cache = {};
static uint64 rack_ptas_seq_num = 0;


/*01010101 -> 1111*/
static int convert_8bit_to_4bit(int present)
{
	int bit0 = present & 1;
	int bit1 = present & (1 << 2) ? 1:0;
	int bit2 = present & (1 << 4) ? 1:0;
	int bit3 = present & (1 << 6) ? 1:0;

	return bit0 + bit1*2 + bit2*4 + bit3*8;
}

#define BDC_R_TRAY_SLED_BITMAP_TOPLEFT			(1<<0)
#define BDC_R_TRAY_SLED_BITMAP_TOPRIGHT 		(1<<2)
#define BDC_R_TRAY_SLED_BITMAP_BOTTOMLEFT 		(1<<1)
#define BDC_R_TRAY_SLED_BITMAP_BOTTOMRIGHT 		(1<<3)
#define BDC_R_TRAY_LEFT_SLED					0
#define BDC_R_TRAY_MIDDLE_SLED					1
#define BDC_R_TRAY_RIGHT_SLED					2

void process_drawer_data(am_node_t *cm, PTAS_Statistic_t *ptas_data)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	int8 present = 0;
	
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;

	int pos = 0;
	int virt_drawer_pos = 0;
	int i = 0;
	SLED_statistic_t *sled_st_ptr = NULL;
	
	/* process drawer&sled data */
	rmm_log(DBG, "ptas\n");
	rmm_log(DBG, "ptas\n");
	rmm_log(DBG, "ptas\n");
	rmm_log(DBG, "ptas\n");
	rmm_log(DBG, "ptas: Checking CM %d, tray present 0x%x.\n", cm_data->loc_id,dz_data->present);
	
	pos = -1;
	while(1) {
		pos = get_next_drawer(pos, convert_8bit_to_4bit(dz_data->present), MAX_DRAWER_NUM);
		
		if (pos == -1)
			break;

		rmm_log(DBG, "ptas: checking pos %d, sled present is %d.\n", pos, drawers_data->sled_present[pos]);

		/* BDC-R have 4 sleds per tray, we treat the hw configuration as below:
		1 Tray consider to be 2 drawer. Each drawer have 3 Sleds, and the middle sled is persistent present(RRC). */
		virt_drawer_pos = pos*2;
		
		ptas_data->tray_statistic[virt_drawer_pos].present = true;
		ptas_data->tray_statistic[virt_drawer_pos+1].present = true;
		
		ptas_data->tray_statistic[virt_drawer_pos].switch_power = 70/2;	/* fixed value */
		ptas_data->tray_statistic[virt_drawer_pos+1].switch_power = 70/2;	/* fixed value */

		/* handler top middle SLED */
		sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos].sled_statis[BDC_R_TRAY_MIDDLE_SLED]);
		sled_st_ptr->present = true;
		sled_st_ptr->temperature = 0;
		/* handler bottom middle SLED */
		sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos+1].sled_statis[BDC_R_TRAY_MIDDLE_SLED]);
		sled_st_ptr->present = true;
		sled_st_ptr->temperature = 0;
		
		rmm_log(DBG, "ptas: tray %d SLED present 0x%x.\n", pos, drawers_data->sled_present[pos]);

		if (drawers_data->sled_present[pos]) {
			if (drawers_data->sled_present[pos] & BDC_R_TRAY_SLED_BITMAP_TOPLEFT) {
				sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos].sled_statis[BDC_R_TRAY_LEFT_SLED]);
				sled_st_ptr->present = true;
				sled_st_ptr->temperature = drawers_data->sled_data[pos].thermal_data[0].inlet_temp;
				rmm_log(DBG, "ptas virtual: tray %d SLED %d, temp %d.\n", 
					virt_drawer_pos, BDC_R_TRAY_LEFT_SLED, sled_st_ptr->temperature);
			}

			if (drawers_data->sled_present[pos] & BDC_R_TRAY_SLED_BITMAP_TOPRIGHT) {
				sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos].sled_statis[BDC_R_TRAY_RIGHT_SLED]);
				sled_st_ptr->present = true;
				sled_st_ptr->temperature = drawers_data->sled_data[pos].thermal_data[2].inlet_temp;
				rmm_log(DBG, "ptas virtual: tray %d SLED %d, temp %d.\n", 
					virt_drawer_pos, BDC_R_TRAY_RIGHT_SLED, sled_st_ptr->temperature);
			}

			if (drawers_data->sled_present[pos] & BDC_R_TRAY_SLED_BITMAP_BOTTOMLEFT) {
				sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos+1].sled_statis[BDC_R_TRAY_LEFT_SLED]);
				sled_st_ptr->present = true;
				sled_st_ptr->temperature = drawers_data->sled_data[pos].thermal_data[1].inlet_temp;
				rmm_log(DBG, "ptas virtual: tray %d SLED %d, temp %d.\n", 
					virt_drawer_pos+1, BDC_R_TRAY_LEFT_SLED, sled_st_ptr->temperature);
			}

			if (drawers_data->sled_present[pos] & BDC_R_TRAY_SLED_BITMAP_BOTTOMRIGHT) {
				sled_st_ptr = &(ptas_data->tray_statistic[virt_drawer_pos+1].sled_statis[BDC_R_TRAY_RIGHT_SLED]);
				sled_st_ptr->present = true;
				sled_st_ptr->temperature = drawers_data->sled_data[pos].thermal_data[3].inlet_temp;
				rmm_log(DBG, "ptas virtual: tray %d SLED %d, temp %d.\n", 
					virt_drawer_pos+1, BDC_R_TRAY_RIGHT_SLED, sled_st_ptr->temperature);
			}
		}
	}
	return;
}


void process_fan_data(am_node_t *cm, PTAS_Statistic_t *ptas_data)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;

	am_node_t *tzone = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tzone_data = (tzone_data_t *)tzone->priv_data;
	am_node_t *fans = node_lookup(cm, FANS_NODE);
	fans_data_t *fans_data = (fans_data_t *)fans->priv_data;

	int pos = 0;
	int i = 0;
	
	/* process thermal data */
	if (tzone_data->present) {
		for (i=0; i<MAX_PWM_NUM; i++) {
			if (tzone_data->present & 1<<i) {
				ptas_data->fan_rpm[i].present = true;
				ptas_data->fan_rpm[i].rpm = fans_data->speed[i];
			}
		}	
	}

	return;
}


void process_psu_data(am_node_t *cm, PTAS_Statistic_t *ptas_data)
{
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;

	am_node_t *psus = node_lookup(cm, PSUS_NODE);
	psus_data_t *psus_data = (psus_data_t *)psus->priv_data;
	pzone_data_t *pz_data = (pzone_data_t *)psus->parent->priv_data;

	int pos = 0;
	int i = 0;

	/* process psu data */
	pos = -1;
	while(1) {
		pos = get_next_psu(pos, pz_data->psu_present, MAX_PSU_NUM);
		if (pos == -1)
			break;

		ptas_data->glb_power_sum += psus_data->power_in[pos];
	}

	return;
}


int check_ptasd_daemon(void)
{
	FILE* fp;
	int count;
	char buf[300] = {};
	char command[300] = {};

	sprintf(command, "ps -ef | grep ptasd | grep -v grep | wc -l" );

	if((fp = popen(command,"r")) == NULL)
		return -1;

	if((fgets(buf, 300, fp))!= NULL){
		count = atoi(buf);
		if(count == 0)
			rmm_log(DBG, "ptasd not running.\n");
		else
			rmm_log(DBG, "process :ptasd total is %d\n",count);
	}
	pclose(fp);

	return count;
}

#define PTASD_PATH "/usr/local/bin/ptasd"
void process_rack_ptas(am_node_t *cm)
{	
	struct PTAS_Statistic PTAS_data = {};
	PTAS_config_t p_config = {6789, MAX_PWM_NUM, MAX_DRAWER_NUM*2};
	PTAS_Output_t output = {};
	int rc = 0;

	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	func_tbl_t func = fn_tbl[cm_data->func_idx];
	am_node_t *tzone = node_lookup(cm, TZONE_NODE);
	tzone_data_t *tz_data = (tzone_data_t *)tzone->priv_data;
	am_node_t *drawers = node_lookup(cm, DRAWERS_NODE);
	drawers_data_t *drawers_data = (drawers_data_t *)drawers->priv_data;
	dzone_data_t *dz_data = (dzone_data_t *)drawers->parent->priv_data;
	

	if (cm_data->open_serial.status == FUNC_FAIL)
		goto next;

	if (func_not_ready(drawers, drawers_data->process_ptas, func.once))
		goto next;
	drawers_data->process_ptas.expire = get_current_tick() + func.timeout;
	
	if (rack_ptas_daemon_start != true) {
		/* check daemon status */
		rc = check_ptasd_daemon();

		if (rc<0) {
			rmm_log(ERROR, "check ptasd status failed.\n");
			goto next;
		}
		else if (rc > 1) {
			rmm_log(ERROR, "%d ptasd instance is running.\n", rc);
			goto next;
		}
		else if (rc == 1)
			rmm_log(DBG, "ptasd already running.\n");
		#if 0
		else if (access(PTASD_PATH, X_OK) == 0) {
			system("cd /usr/local/bin && ./ptasd &");
			goto next; /* 1 second to start */
		}
		#endif
	}
	rack_ptas_daemon_start = true;

	if (rack_ptas_init != true) {
		libptas_init(&p_config, NULL);
		rack_ptas_init = true;
	}

	/* check if data ready */
	if (dz_data->present == 0) {
		rmm_log(DBG, "data not ready. Skip this time.\n");
		goto next;
	}
	
	/* process data */
	process_drawer_data(cm, &PTAS_data);
	process_fan_data(cm, &PTAS_data);
	process_psu_data(cm, &PTAS_data);

	PTAS_data.sequence_num = rack_ptas_seq_num++;
	rc = PTAS_wait_timeout(&PTAS_data, &output, 1);
	if (rc == 0)
		rmm_log(DBG, "PTAS_wait_timeout PTAS Output: rc %x, seq %x, Q %lf, outlet %f.\n", 
	        output.rc,
	        output.sequence_num,
	        output.Q,
	        output.outlet_temp);
	else
		rmm_log(DBG, "PTAS_wait_timeout, timeout happened.\n");

	if (output.rc != 0) {
		rmm_log(ERROR, "Invalid Rack PTAS return value [0x%x].\n", output.rc);
		goto next;
	}
	
	if (output.outlet_temp != output_cache.outlet_temp) {
		output_cache.outlet_temp = output.outlet_temp;
		if (notify_tzone_outlet_temp_change(tz_data->uuid[0], output.outlet_temp) != 0)
			goto next;
	}

	if (output.Q != output_cache.Q) {
		output_cache.Q = output.Q;
		if (notify_tzone_airflow_change(tz_data->uuid[0], output.Q) != 0)
			goto next;
	}

next:
	process_next_func(cm);
	return;
}

#endif

