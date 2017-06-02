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

#ifndef __ASSET_MONITOR_GENERAL_H__
#define __ASSET_MONITOR_GENERAL_H__

#include "libutils/rmm.h"

struct br_info{
	unsigned long  target_addr;
	unsigned short target_channel;
};

#define FUNC_FAIL  			0
#define FUNC_SUCCESS   		1

#define IP_MAX_STR_LEN  32
#define PSU_FRU_MAX_STR_LEN     128

#define STATE_NOT_READY         0
#define STATE_READY             1

#define MAX_CERT_LEN	8000
#define CERT_HASH_LEN	16
 
enum {
	CM_NODE,
	PZONE_NODE,
	PSUS_NODE,
	TZONE_NODE,
	FANS_NODE,
	DZONE_NODE,
	DRAWERS_NODE
};

enum {
	OFFLINE,
	ONLINE
};

enum {
	FAIL,
	SUCCESS
};

enum {
	IDLE,
	BUSY
};

typedef void (*set_status_fn)(void *nptr, int state);
typedef void (*clear_data_fun)(void *nptr);

typedef struct am_node {
    struct list_head    list;
    int                 type;
    int                 node_status;                    ///<Fail Ready Success.
    void                *priv_data;

    set_status_fn       set_status;
    clear_data_fun      clr_fn;

    struct list_head    children;
    struct am_node      *parent;
} am_node_t;

typedef void (*am_req_fn) (am_node_t *cm_node);

typedef struct func_tbl {
	int         	timeout;
	/* @once:    The function just need run once
	 **/
	bool					once;
	am_req_fn				func;
	jipmi_rsp_callback_fn	callback;
} func_tbl_t;

typedef struct version {
	uint8 low;
	uint8 high;
} version_t;

typedef struct fn_cond {
	int32  status;
	int64  expire;
} fn_cond_t;

typedef struct cm_data {
	char       serial_dev_name[MAX_NAME_LEN];
	int32      loc_id;
	int32      serial_fd;
	uint32     ip_address;
	version_t  cm_ver;

	fn_cond_t  open_serial;
	fn_cond_t  get_device_id;
	fn_cond_t  set_default_pwm;
	fn_cond_t  get_cm_ip;
	fn_cond_t  get_PODMCert_file;
	fn_cond_t  get_RMMCert_file;

	int32      func_idx;
	bool       loop_state;
	int64      loop_expire;
	int8       uuid[UUID_MAX_LEN];
} cm_data_t;

typedef struct psus_data {
	fn_cond_t  get_psus_power_in;
	fn_cond_t  get_psus_power_out;
	fn_cond_t  get_psus_current_out;
	fn_cond_t  get_psu_serial;
	fn_cond_t  get_psu_manufacture;
	fn_cond_t  get_psu_model;
	fn_cond_t  get_psu_fw_ver;
	fn_cond_t  get_psu_power_cap;
	fn_cond_t  get_psu_health_status;
	int32      power_in[MAX_PSU_NUM];
	int32      power_out[MAX_PSU_NUM];
	int32      health_status[MAX_PSU_NUM];
	int32      current_out[MAX_PSU_NUM];
	int32      total_power_cap;
	int32      power_cap[MAX_PSU_NUM];
	int8       serial[MAX_PSU_NUM][PSU_FRU_MAX_STR_LEN];
	int8       model[MAX_PSU_NUM][PSU_FRU_MAX_STR_LEN];
	int8       fw_ver[MAX_PSU_NUM][PSU_FRU_MAX_STR_LEN];
	int8       manufacture[MAX_PSU_NUM][PSU_FRU_MAX_STR_LEN];
	int32      pos;
	int8       uuid[MAX_PSU_NUM][UUID_MAX_LEN];
} psus_data_t;

typedef struct pzone_data {
	fn_cond_t  get_pzone_present;
	fn_cond_t  get_psus_present;
	int8       pzone_present;
	int8       psu_present;
	int32      add_flag[MAX_PSU_FRU_ATTR_NUM+MAX_PSU_PWR_DATA_NUM][MAX_PSU_NUM];   //< serial, model, fw_ver, manufacture, powercap. power in, current out, power out
	int32      remove_flag[MAX_PSU_FRU_ATTR_NUM+MAX_PSU_PWR_DATA_NUM+MAX_PSU_HEALTH_NUM][MAX_PSU_NUM];   //< serial, model, fw_ver, manufacture, powercap. power in, current out, power out
	int8       uuid[MAX_PZONE_NUM][UUID_MAX_LEN];
	int32      pos;
} pzone_data_t;

typedef struct fans_data {
	fn_cond_t  get_fans_speed;
	int32      speed[MAX_PWM_NUM];
	int32      pos;
	int8       uuid[MAX_PWM_NUM][UUID_MAX_LEN];
} fans_data_t;

typedef struct tzone_data {
	fn_cond_t  get_fans_present;
	fn_cond_t  set_tzone_pwm;
	fn_cond_t  update_tzone_pwm;
	int8       pwm[MAX_TZONE_NUM];
	int8       present;
	int8       uuid[MAX_TZONE_NUM][UUID_MAX_LEN];
	int32      pos;
	int32      avg_temp;
} tzone_data_t;

typedef struct cert_vars {
	bool     cert_need_update;
	int16    cur_pos;
	int8     cur_chunk_num;
} cert_vars_t;

typedef struct cert_data {	
	bool	Cert_exist;
	uint8	Cert[MAX_CERT_LEN];
	uint16	CertLen;
	uint8	CertHash[CERT_HASH_LEN];
	time_t	Cert_mod_time;
	
	/* for retry mechanism */
	uint8	retry_cnt;
	jipmi_serial_msg_t req;	
} cert_data_t;

enum {
	SLED_TYPE_HSW = 0x0,
	SLED_TYPE_BDX_DE = 0x01
};

typedef struct sled_thermal {
	uint8 sled_type;
	uint8 desired_pwm;
	uint8 inlet_temp;
} sled_thermal_t;

typedef struct sleds_data {
	sled_thermal_t thermal_data[MAX_SLED_NUM];
	uint16 sled_power[MAX_SLED_NUM];
} sleds_data_t;


typedef struct drawers_data {
	fn_cond_t  get_drawers_temp;
	fn_cond_t  set_drawers_tray_ruid;
	fn_cond_t  set_drawers_rack_bpid;
	fn_cond_t  set_drawers_rack_puid;
	fn_cond_t  get_drawers_rack_puid;
	fn_cond_t  get_drawers_ip;
	fn_cond_t  get_drawers_ip_source;
	fn_cond_t  get_drawers_subnet_mask;
	fn_cond_t  get_drawers_thermal_sensor;
	fn_cond_t  get_drawers_health_sensor;
	fn_cond_t  get_drawers_power;
	fn_cond_t  get_drawers_lancfg;
	fn_cond_t  get_RMMCert_file;
	fn_cond_t  get_drawers_RMMCert;
	fn_cond_t  get_drawers_RMMCert_hash;
	fn_cond_t  get_drawers_PODMCert;
	fn_cond_t  get_PODMCert_file;
	fn_cond_t  get_drawers_PODMCert_hash;
	fn_cond_t  set_drawers_RMMCert;
	fn_cond_t  set_drawers_PODMCert;
	fn_cond_t  calc_dzone_results;
	fn_cond_t  process_ptas;
	uint8      temps[MAX_DRAWER_NUM];
	uint8      health_st[MAX_DRAWER_NUM];
	//int8       ips[MAX_DRAWER_NUM][IP_MAX_STR_LEN];
	int32      pos;
	int8       uuid[MAX_DRAWER_NUM][UUID_MAX_LEN];
	int        puid[MAX_DRAWER_NUM];

	/* ip configurations */
	uint32 ips[MAX_DRAWER_NUM];
	uint8 ip_sources[MAX_DRAWER_NUM];
	uint8 subnet_masks[MAX_DRAWER_NUM];
	
	/* certificate */
	cert_data_t RMMCertData;
	cert_data_t PODMCertData;

	cert_vars_t RMMCert_vars[MAX_DRAWER_NUM];
	//uint8        RMMCert[MAX_DRAWER_NUM][MAX_CERT_LEN];
	//uint8		RMMCertHash[MAX_DRAWER_NUM][CERT_HASH_LEN];
	cert_vars_t PODMCert_vars[MAX_DRAWER_NUM];
	//uint8        PODMCert[MAX_DRAWER_NUM][MAX_CERT_LEN];
	//uint8		PODMCertHash[MAX_DRAWER_NUM][CERT_HASH_LEN];

	/* sleds */
	int8       sled_present[MAX_DRAWER_NUM];
	int8       sled_uuid[MAX_DRAWER_NUM][MAX_SLED_NUM][UUID_MAX_LEN];
	sleds_data_t sled_data[MAX_DRAWER_NUM];

	int32      drawer_power[MAX_DRAWER_NUM];
} drawers_data_t;



typedef struct dzone_data {
	fn_cond_t  get_drawers_alert;
	fn_cond_t  get_drawer_present;
	int8       alert;
	int8       present;
	int8       fresh_present;	/* newly added drawer bitmap */
	int8       uuid[MAX_DZONE_NUM][UUID_MAX_LEN];
	int32      pos;
} dzone_data_t;





extern am_node_t *get_cm_node(am_node_t *nptr);

extern bool func_not_ready(am_node_t *nptr, fn_cond_t cond, bool once);

 /**
 * @brief get psu fru info
 *
 * @param  cm           the pointer of cm.
 * 
 */
extern void get_psus_serials(am_node_t *cm);

extern void get_psu_serial(am_node_t *cm);

extern void get_psus_manufactures(am_node_t *cm);

extern void get_psu_manufacture(am_node_t *cm);

extern void get_psus_models(am_node_t *cm);

extern void get_psu_model(am_node_t *cm);

extern void get_psus_fw_vers(am_node_t *cm);

extern void get_psu_fw_ver(am_node_t *cm);

extern void get_psus_power_cap(am_node_t *cm);

extern void get_psu_power_cap(am_node_t *cm);

extern void get_psus_health_status(am_node_t *cm);

extern void get_psu_health_status(am_node_t *cm);

extern uint64 get_current_tick(void);;

extern bool find_lid_from_uuid(am_node_t **root, int32 type, char *uuid, int32 *lid, unsigned int *host, int32 *serial_fd);
extern bool record_fan_pwm(am_node_t **root, int8 *uuid, int8 pwm);
extern void record_rack_puid(int puid);
extern int get_rack_puid();

extern int32 convert_pwm_to_rpm(int32 pwm);
extern char am_rmcp_username[RMCP_USERNAME_LEN];
extern char am_rmcp_password[RMCP_PASSWORD_LEN];
//extern bool cert_in_progress;

#endif

