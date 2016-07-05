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

#include "cm.h"
#include "libutils/string.h"
#include "action.h"

static bool is_cm_idle(am_node_t *cm);
static int32 get_min_fn_timeo(void);

	func_tbl_t fn_tbl[] = {
	/*timeout(ms),	once,	function,					callback */
	{1000,			true,	open_serial,				open_serial_rsp},
	{1000,			true,	get_device_id,				get_device_id_rsp},
	{1000,			false,	get_cm_ip,					get_cm_ip_rsp},

	{1000,			false,	process_actions,			NULL},
	{1000,			false,	get_drawer_present, 		get_drawer_present_rsp},
		
	{1000,			false,	get_fans_present,			get_fans_present_rsp},
	{1000,			false,	get_pzone_present,			get_pzone_present_rsp},
	{1000,			false,	get_psus_present,			get_psus_present_rsp},

	{500,			false,	get_drawers_alert,			get_drawers_alert_rsp},

	{1000,			false,	get_drawers_rack_puid,		NULL},
	{1000,			false,	set_drawers_ruid,			NULL},
	{1000,			false,	set_drawers_rack_bpid,		NULL},
	{1000,			false,	set_drawers_rack_puid,		NULL},

	#if 0	/* remove RMM cert functions */
	{30000,			false,	get_RMMCert_file,			NULL},
	{30000,			false,	get_drawers_RMMCert_hash,	NULL},
	{30000,			false,	set_drawers_RMMCert,		NULL},
	#endif

	{30000,			false,	get_PODMCert_file,			NULL},
	{30000,			false,	get_drawers_PODMCert_hash,	NULL},
	{30000,			false,	set_drawers_PODMCert,		NULL},

	{1000,			false,	get_drawers_thermal_sensor,	NULL},

	{1000,			false,	get_drawers_health_sensor,	NULL},
	{1000,			false,	get_drawers_power,			NULL},
	{1000,			false,	get_drawers_ip,				NULL},
	{1000,			false,	get_drawers_ip_source,		NULL},	
	{1000,			false,	get_drawers_subnet_mask,	NULL},	
		
	{1000,			false,	set_tzone_pwm, 				set_tzone_pwm_cb},

	{1000,			false,	get_fans_speed,				get_fans_speed_rsp},
	{1000,			false,	get_psus_power_in,			NULL},
	{1000,			false,	get_psus_power_out,			NULL},
	{1000,			false,	get_drawers_temp,			NULL},
	{1000,			false,	get_psus_serials,			NULL},
	{1000,			false,	get_psus_manufactures,		NULL},
	{1000,			false,	get_psus_models,			NULL},
	{1000,			false,	get_psus_fw_vers,			NULL},
	{1000,			false,	get_psus_power_cap,			NULL},
	{1000,			false,	get_psus_health_status,		NULL},
	{1000,			false,	calc_dzone_results,			NULL},

	#ifdef RACK_PTAS
	{3000,			false,	process_rack_ptas,			NULL},
	#endif
};

static void tzone_set_status(void *tzone, int32 status)
{
	tzone_data_t *data = (tzone_data_t *)((am_node_t *)tzone)->priv_data;

	rmm_log(DBG, "TZone uuid[%lld]\n", data->uuid);

	data->get_fans_present.status = status;
}

static void fans_set_status(void *fan, int32 status)
{
	fans_data_t *data = (fans_data_t *)((am_node_t *)fan)->priv_data;

	data->get_fans_speed.status = status;
}

static void tzone_clear_data(void *tz)
{
	memset(((am_node_t *)tz)->priv_data, 0, sizeof(tzone_data_t));
}

static void fans_clear_data(void *fan)
{
	memset(((am_node_t *)fan)->priv_data, 0, sizeof(fans_data_t));
}

static void pzone_set_status(void *pz, int32 status)
{
	pzone_data_t *data = (pzone_data_t *)((am_node_t *)pz)->priv_data;

	data->get_pzone_present.status = status;
	data->get_psus_present.status = status;
}

static void psus_set_status(void *psus, int32 status)
{
	psus_data_t *data = (psus_data_t *)((am_node_t *)psus)->priv_data;

	data->get_psus_power_in.status = status;
	data->get_psus_current_out.status = status;
}

static void pzone_clear_data(void *pz)
{
	memset(((am_node_t *)pz)->priv_data, 0, sizeof(pzone_data_t));
}

static void psus_clear_data(void *psus)
{
	memset(((am_node_t *)psus)->priv_data, 0, sizeof(psus_data_t));
}

static void dz_set_status(void *dz, int32 status)
{
	dzone_data_t *data = (dzone_data_t *)((am_node_t *)dz)->priv_data;

	data->get_drawer_present.status = status;
}

static void drawers_set_status(void *drawer, int32 status)
{
	drawers_data_t *data = (drawers_data_t *)((am_node_t *)drawer)->priv_data;

	data->get_drawers_temp.status = status;
}

static void dz_clear_data(void *dz)
{
	memset(((am_node_t *)dz)->priv_data, 0, sizeof(dzone_data_t));
}

static void drawers_clear_data(void *drawer)
{
	memset(((am_node_t *)drawer)->priv_data, 0, sizeof(drawers_data_t));
}

void am_run(am_node_t **nptrs, int32 cnt)
{
	int32 index = 0;
	uint64 time_ms;
	struct timeval curr_ts;
	am_node_t *cm;

	for (index = 0; index < cnt; index++) {
		cm = nptrs[index];
		if (is_cm_idle(cm)) {
			cm_data_t *data = (cm_data_t *)(cm->priv_data);
			/* check if the am_run has reached its period */
			gettimeofday(&curr_ts, NULL);
			time_ms = curr_ts.tv_sec * 1000 + curr_ts.tv_usec/1000;
			if (time_ms < data->loop_expire)
				continue;

			data->loop_expire = time_ms + get_min_fn_timeo();
			data->loop_state = BUSY;
			process_next_func(cm);
		}
	}
}

am_node_t *get_cm_node(am_node_t *nptr)
{
	am_node_t *tmp = nptr;

	while (tmp->parent != NULL) {
		tmp = tmp->parent;
	}
	return tmp;
}

uint64 get_current_tick(void)
{
	uint64 time_ms;
	struct timeval curr_ts;

	gettimeofday(&curr_ts, NULL);
	time_ms = curr_ts.tv_sec * 1000 + curr_ts.tv_usec/1000;
	return time_ms;
}

int32 get_next_availible_asset(int32 pos, int8 present, int32 max)
{
	int32 i = 0;
	int32 new_pos = -1;

	for (i = 0; i < max; i++) {
		if (((present>>i)&1) == 1) {
			/* If new pos bigger than pos, return, else return the first one. */
			if (i > pos)
				return i;
		}
	}
	return new_pos;
}

int32 get_next_availible_changed_asset(int32 pos, int32 *flag, int32 max)
{
	int32 i = 0;

	for (i = 0; i < max; i++) {
		if(flag[i] == 1)
			if(i > pos)
				return i;
	}
	return -1;
}

am_node_t *node_lookup(am_node_t *root, int32 type)
{
	am_node_t *node;
	am_node_t *find;

	if (root->type == type)
		return root;
	list_for_each_entry(node, &root->children, list) {
		find = node_lookup(node, type);
		if (find != NULL)
			return find;
	}

	return NULL;
}

void process_next_func(am_node_t *cm)
{

	cm_data_t *data = (cm_data_t *)cm->priv_data;

	data->func_idx++;
	if (data->func_idx == sizeof(fn_tbl) / (sizeof(func_tbl_t))) {
		data->func_idx = -1;
		data->loop_state = IDLE;
		return;
	}

	fn_tbl[data->func_idx].func(cm);
}

am_node_t *create_node(int32 type, set_status_fn status_fn, clear_data_fun clr_fn, am_node_t *parent)
{
	void *priv_data = NULL;
	am_node_t *nptr = (am_node_t *)malloc(sizeof(am_node_t));

	if (nptr == NULL) {
		rmm_log(ERROR, "malloc failed\n");
		return NULL;
	}

	INIT_LIST_HEAD(&nptr->children);
	nptr->type = type;
	int32 len = 0;

	switch (type) {
	case CM_NODE:
		len = sizeof(cm_data_t);
		break;
	case PZONE_NODE:
		len = sizeof(pzone_data_t);
		break;
	case PSUS_NODE:
		len = sizeof(psus_data_t);
		break;
	case TZONE_NODE:
		len = sizeof(tzone_data_t);
		break;
	case FANS_NODE:
		len = sizeof(fans_data_t);
		break;
	case DZONE_NODE:
		len = sizeof(dzone_data_t);
		break;
	case DRAWERS_NODE:
		len = sizeof(drawers_data_t);
		break;
	default:
		break;
	}

	priv_data = malloc(len);
	memset(priv_data, 0, len);

	nptr->parent = parent;
	nptr->set_status = status_fn;
	nptr->clr_fn = clr_fn;
	nptr->priv_data = priv_data;
	return nptr;
}

bool record_fan_pwm(am_node_t **root, int8 *uuid, int8 pwm)
{
	void *priv_data = NULL;
	am_node_t *node = NULL;
	int cm_index = 0;
	int i = 0;

	for (cm_index = 0; cm_index < MAX_CM_NUM; cm_index++) {
		node = NULL;
		node = node_lookup(root[cm_index], TZONE_NODE);
		if (node == NULL)
			return false;

		priv_data = NULL;
		priv_data = node->priv_data;
		if (priv_data == NULL)
			return false;

		for (i = 0; i < MAX_TZONE_NUM; i++) {
			if (0 == strcasecmp(((tzone_data_t *)priv_data)->uuid[i], uuid)) {
				((tzone_data_t *)priv_data)->pwm[i] = pwm;
				((tzone_data_t *)priv_data)->update_tzone_pwm.expire = get_current_tick();
				return true;
			}
		}
	}

	return false;
}

static int rack_puid = 1;
void record_rack_puid(int puid)
{
	rack_puid = puid;
}

int get_rack_puid()
{
	return rack_puid;
}


int32 convert_pwm_to_rpm(int32 pwm)
{
	FILE *fp;
	int8 buf[256];
	int32 pwm_r;
	int32 rpm;

	if (pwm < 0 || pwm > 100)
		return -1;

	fp = fopen("/etc/rmm/pwm_rpm.cfg", "r");
	if (fp == NULL)
		return -1;

	bzero(buf, sizeof(buf));

	/* Skip the title line */
	fgets(buf, sizeof(buf), fp);

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%d:%d", &pwm_r, &rpm) == 2) {
			if (pwm == pwm_r) {
				fclose(fp);
				return rpm;
			} else
				continue;
		}
	}

	fclose(fp);

	return -1;

}


bool find_lid_from_uuid(am_node_t **root, int32 type, char *uuid, int32 *lid, unsigned int *host, int32 *serial_fd)
{
	void *priv_data = NULL;
	am_node_t *node = NULL;
	int cm_index = 0;
	int i = 0;

	if (lid == NULL)
		return false;

	for (cm_index = 0; cm_index < MAX_CM_NUM; cm_index++) {
		node = NULL;
		node = node_lookup(root[cm_index], type);
		if (node == NULL)
			return false;

		priv_data = NULL;
		priv_data = node->priv_data;
		if (priv_data == NULL)
			return false;

		if (((cm_data_t *)(root[cm_index]->priv_data))->ip_address == 0)
			continue;

		if (host != NULL) {
			*host = ((cm_data_t *)(root[cm_index]->priv_data))->ip_address;
		}

		if (serial_fd != NULL)
			*serial_fd = ((cm_data_t *)(root[cm_index]->priv_data))->serial_fd;

		switch (type) {
		case CM_NODE:
			if (0 == strcasecmp(((cm_data_t *)priv_data)->uuid, uuid)) {
				*lid = i + 1;
				return true;
			}
			break;
		case PZONE_NODE:
			for (i = 0; i < MAX_PZONE_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((pzone_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		case PSUS_NODE:
			for (i = 0; i < MAX_PSU_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((psus_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		case TZONE_NODE:
			for (i = 0; i < MAX_TZONE_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((tzone_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		case FANS_NODE:
			for (i = 0; i < MAX_PWM_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((fans_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		case DZONE_NODE:
			for (i = 0; i < MAX_DZONE_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((dzone_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		case DRAWERS_NODE:
			for (i = 0; i < MAX_DRAWER_NUM; i++) {
				*lid = i + 1;
				if (0 == strcasecmp(((drawers_data_t *)priv_data)->uuid[i], uuid))
					return true;
			}
			break;
		default:
			break;
		}
	}

	return false;
}

static void cm_set_status(void *cm, int32 status)
{
	cm_data_t *data = (cm_data_t *)((am_node_t *)cm)->priv_data;

	data->get_cm_ip.status = status;
}

static void mc_clear_data(void *cm)
{
	int64 loop_expire = 0;
	cm_data_t *cm_data = (cm_data_t *)((am_node_t *)cm)->priv_data;

	loop_expire = cm_data->loop_expire;
	memset(((am_node_t *)cm)->priv_data + sizeof(cm_data_header_t), 0, sizeof(cm_data_t) - sizeof(cm_data_header_t));
	cm_data->loop_expire = loop_expire;
}

am_node_t *init_zone_node(cm_attr_t *attr)
{
	am_node_t *cm = create_node(CM_NODE, cm_set_status, mc_clear_data, NULL);

	if (cm == NULL) {
		rmm_log(ERROR, "failed to create cm node\n");
		return NULL;
	}

	cm_data_t *cm_data = (cm_data_t *)(cm->priv_data);
	if (cm_data == NULL) {
		rmm_log(ERROR, "priv_data is null\n");
		return NULL;
	}

	strncpy_safe(cm_data->serial_dev_name, attr->serial_dev_name, MAX_NAME_LEN, MAX_NAME_LEN - 1);
	cm_data->loc_id = attr->loc_id;
	cm_data->func_idx = -1;

	am_node_t *pz = create_node(PZONE_NODE, pzone_set_status, pzone_clear_data, cm);
	am_node_t *psus = create_node(PSUS_NODE, psus_set_status, psus_clear_data, pz);
	am_node_t *tz = create_node(TZONE_NODE, tzone_set_status, tzone_clear_data, cm);
	am_node_t *fans = create_node(FANS_NODE, fans_set_status, fans_clear_data, tz);
	am_node_t *dz = create_node(DZONE_NODE, dz_set_status, dz_clear_data, cm);
	am_node_t *drawers = create_node(DRAWERS_NODE, drawers_set_status, drawers_clear_data, dz);

	list_add_tail(&pz->list, &cm->children);
	list_add_tail(&tz->list, &cm->children);
	list_add_tail(&dz->list, &cm->children);

	list_add_tail(&psus->list, &pz->children);
	list_add_tail(&fans->list, &tz->children);
	list_add_tail(&drawers->list, &dz->children);

	return cm;
}


void set_node_status(am_node_t *nptr, int32 status, bool set_subnode)
{
	am_node_t *pa;
	am_node_t *cm = get_cm_node(nptr);
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;

	if (status == OFFLINE)
		(nptr->clr_fn)(nptr);

	nptr->node_status = status;
	if (set_subnode == false)
		return;

	list_for_each_entry(pa, &nptr->children, list) {
		set_node_status(pa, status, set_subnode);
	}
}

void set_fn_status(am_node_t *nptr, int32 status)
{
	if (nptr->set_status != NULL)
		(nptr->set_status)(nptr, status);
}

int32 get_fn_idx(am_node_t *nptr)
{
	am_node_t *cm = get_cm_node(nptr);
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;

	if (cm_data->func_idx < 0  ||
		cm_data->func_idx >= sizeof(fn_tbl) / sizeof(func_tbl_t))
		return -1;

	return cm_data->func_idx;
}

bool func_not_ready(am_node_t *nptr, fn_cond_t cond, bool once)
{
	am_node_t *parent = nptr->parent;
	am_node_t *cm = get_cm_node(nptr);
	cm_data_t *cm_data = (cm_data_t *)cm->priv_data;
	uint64 curr_tick = get_current_tick();

	/* check parent status */
	if (parent != NULL && parent->node_status == OFFLINE) {
		return true;
	}

	/* check once flag */
	if (once && cond.status == FUNC_SUCCESS) {
		return true;
	}

	if (cond.expire != 0) {
		/* update loop min expired time */
		cm_data->loop_expire = (cm_data->loop_expire < cond.expire ? cm_data->loop_expire : cond.expire);

		/* check self expired condition */
		if (cond.expire > curr_tick) {
			return true;
		}
	}

	return false;
}

static bool is_cm_idle(am_node_t *cm)
{
	cm_data_t *data = (cm_data_t *)cm->priv_data;

	if (data->loop_state == IDLE)
		return true;
	else
		return false;
}

static int32 get_min_fn_timeo(void)
{
	int32 i = 0, min = 10000000;

	for (i = 0; i < sizeof(fn_tbl)/sizeof(func_tbl_t); i++) {
		if (min > fn_tbl[i].timeout) {
			min = fn_tbl[i].timeout;
		}
	}

	return min;
}
