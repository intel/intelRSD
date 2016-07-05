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

#ifndef __ASSET_MONITOR_CM_H__
#define __ASSET_MONITOR_CM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>

#include "libassetd/assetd_type.h"
#include "libutils/rmm.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libjipmi/libjipmi.h"
#include "general.h"
#include "config.h"


typedef struct cm_attr {
	char      		serial_dev_name[MAX_NAME_LEN];
	int				loc_id;
} cm_attr_t;


typedef struct cm_data_header {
	char			serial_dev_name[MAX_NAME_LEN];
	int 			index;
} cm_data_header_t;


extern func_tbl_t fn_tbl[];
extern struct br_info br_addrs[MAX_NODE_NUM];

#define SERIAL_ERROR_WAIT_TIME     2000

/* Return the child node by node type */
am_node_t *node_lookup(am_node_t *nptr, int type);


/**
 * @brief get cm node
 */
am_node_t *get_cm_node(am_node_t *nptr);

int get_next_availible_asset(int pos, char present, int max);

int get_next_availible_changed_asset(int pos, int *add_flag, int max);

void process_next_func(am_node_t *cm);


void set_fn_status(am_node_t *nptr, int status);


/**
 * @brief 	set node status.
 *
 * @param nptr 		the pointer of am_node.
 * @param status 	the node status
 * 
 */
void set_node_status(am_node_t *nptr, int status, bool set_subnode);


/**
 * @brief initialize asset moudule node, and build the tree struct dependence.
 *
 * @param  cm_attr_t 		information of cm atrribute.
 * 
 * @return  the root pointer of cm.
 */
am_node_t *init_zone_node(cm_attr_t *cptr);

/**
 * @brief run the asset module.
 *
 * @param  nptrs 	the array of cm tree node.
 * @param  cnt 		the number of cm.
 * 
 */
void am_run(am_node_t **nptrs, int cnt);

/**
 * @brief serial inf open the cm.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void open_serial(am_node_t *cm);

int open_serial_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief  set default fan pwm (40%).
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void set_default_pwm(am_node_t *cm);

int set_default_pwm_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);
/**
 * @brief get device id.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_device_id(am_node_t *cm);

int get_device_id_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief get lan config.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_cm_ip(am_node_t *cm);

int get_cm_ip_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief get drawer present. 
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawer_present(am_node_t *cm);

int get_drawer_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);



/**
 * @brief get fan present.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_fans_present(am_node_t *cm);

int get_fans_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief get power zone present.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_pzone_present(am_node_t *cm);

int get_pzone_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);
/**
 * @brief get psu present.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_psus_present(am_node_t *cm);

int get_psus_present_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief get drawer alert state.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_alert(am_node_t *cm);

int get_drawers_alert_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);
/**
 * @brief set tray ruid.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void set_drawers_ruid(am_node_t *cm);
void set_drawer_ruid(am_node_t *cm, int pos);


//int set_drawers_ruid_rsp(int result, unsigned char *rsp, int rsp_len,
//			 void *cb_data);

 /**
 * @brief set rack bpid.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void set_drawers_rack_bpid(am_node_t *cm);
void set_drawer_rack_bpid(am_node_t *cm, int pos); 

//int set_drawers_rack_bpid_rsp(int result, unsigned char *rsp, int rsp_len,
//			 void *cb_data);


  /**
  * @brief get rack puid.
  *
  * @param	cm		 the pointer of cm.
  *
  */
 void get_drawers_rack_puid(am_node_t *cm);
 void get_drawer_rack_puid(am_node_t *cm, int pos);
 

 /**
 * @brief set rack puid.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void set_drawers_rack_puid(am_node_t *cm);
void set_drawer_rack_puid(am_node_t *cm, int pos);


/**
* @brief set drawer subnet mask.
*
* @param  cm	   the pointer of cm.
*
*/
void get_drawer_subnet_mask(am_node_t *cm);
void get_drawers_subnet_mask(am_node_t *cm);


/**
* @brief set drawer ip source.
*
* @param  cm	   the pointer of cm.
*
*/
void get_drawer_ip_source(am_node_t *cm);
void get_drawers_ip_source(am_node_t *cm);

/**
* @brief set drawer ip.
*
* @param  cm	   the pointer of cm.
*
*/
void get_drawer_ip(am_node_t *cm);
void get_drawers_ip(am_node_t *cm);


//int set_drawers_rack_puid_rsp(int result, unsigned char *rsp, int rsp_len,
//			 void *cb_data);

 /**
 * @brief get aggregated thermal sensor.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void get_drawers_thermal_sensor(am_node_t *cm);

//int get_drawers_thermal_sensor_rsp(int result, unsigned char *rsp, int rsp_len,
//			void *cb_data);

 /**
 * @brief get aggregated health sensor.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void get_drawers_health_sensor(am_node_t *cm);

//int get_drawers_health_sensor_rsp(int result, unsigned char *rsp, int rsp_len,
//			 void *cb_data);

 /**
 * @brief get tray power state.
 *
 * @param  cm 		the pointer of cm.
 *
 */
void get_drawers_power(am_node_t *cm);

//int get_drawers_power_rsp(int result, unsigned char *rsp, int rsp_len,
//			 void *cb_data);

 /**
 * @brief get tray lan cfg.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_lan_cfg(am_node_t *cm);

int get_drawers_lan_cfg_rsp(int result, unsigned char *rsp, int rsp_len,
			 void *cb_data);

/**
* @brief calc_dzoe results.
*
* @param  cm	   the pointer of cm.
* 
*/
void calc_dzone_results(am_node_t *cm);


#ifdef RACK_PTAS
/**
* @brief process rack ptas data.
*
* @param  cm	   the pointer of cm.
* 
*/
void process_rack_ptas(am_node_t *cm);
#endif

/**
 * @brief get fan speed.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_fans_speed(am_node_t *cm);

int get_fans_speed_rsp(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);


/**
 * @brief set fan pwm.
 *
 * @param  cm 		the pointer of cm.
 *
 */

void set_tzone_pwm(am_node_t *cm);
int set_tzone_pwm_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data);

/**
 * @brief get psu power input.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_psus_power_in(am_node_t *cm);

void get_psu_power_in(am_node_t *cm);

/**
 * @brief get psu current output.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_psus_current_out(am_node_t *cm);
void get_psu_current_out(am_node_t *cm);

/**
 * @brief get psu power output.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_psus_power_out(am_node_t *cm);
void get_psu_power_out(am_node_t *cm);

/**
 * @brief get drawer temperature.
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_temp(am_node_t *cm);

void get_drawer_temp(am_node_t *cm);


/**
 * @brief get drawer thermal sensor
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_thermal_sensor(am_node_t *cm);

void get_drawer_thermal_sensor(am_node_t *cm);


/**
 * @brief get drawer health sensor
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_health_sensor(am_node_t *cm);

void get_drawer_health_sensor(am_node_t *cm);


/**
 * @brief get drawer power
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_power(am_node_t *cm);

void get_drawer_power(am_node_t *cm);


/**
 * @brief get cert from files
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_RMMCert_file(am_node_t *cm);

void get_PODMCert_file(am_node_t *cm);


/**
 * @brief set drawer RMM cert
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void set_drawers_RMMCert(am_node_t *cm);

void set_drawer_RMMCert(am_node_t * cm,int8 chunk_num);


/**
 * @brief get drawer RMM cert hash
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_RMMCert_hash(am_node_t *cm);

void get_drawer_RMMCert_hash(am_node_t * cm);


/**
 * @brief set drawer power
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void set_drawers_PODMCert(am_node_t *cm);

void set_drawer_PODMCert(am_node_t * cm,int8 chunk_num);


/**
 * @brief get drawer power hash
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void get_drawers_PODMCert_hash(am_node_t *cm);

void get_drawer_PODMCert_hash(am_node_t * cm);





/**
 * @brief Call the next function in the fuction list
 *
 * @param  cm 		the pointer of cm.
 * 
 */
void process_next_func(am_node_t *cm);


#define get_next_fan(pos, present, max)     get_next_availible_asset(pos, present, max)
#define get_next_psu(pos, present, max)     get_next_availible_asset(pos, present, max)
#define get_next_added_psu(pos, add_flag, max)     get_next_availible_changed_asset(pos, add_flag, max)
#define get_next_removed_psu(pos, remove_flag, max)     get_next_availible_changed_asset(pos, remove_flag, max)
#define get_next_drawer(pos, present, max)  get_next_availible_asset(pos, present, max)

#endif

