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

#ifndef __ASSET_MONITOR_NOTIFY_H__
#define __ASSET_MONITOR_NOTIFY_H__

#include <stdio.h>
#include "cm.h"

int notify_tzone_add(am_node_t *cm);
int notify_tzone_rpm_change(char *uuid, int32 rm);
int notify_tzone_temp_change(char *uuid, int32 temp);


int notify_dzone_add(am_node_t *cm);

int notify_cm_add(am_node_t *cm);
int notify_cm_remove(am_node_t *cm);
int notify_cm_ip_change(am_node_t *cm, unsigned int new_host);
int notify_cm_ver_change(am_node_t *cm, version_t *new_ver);

int notify_pzone_present_change(am_node_t *pzone, int present);
int notify_psu_present_change(am_node_t *pzone, int present);
int notify_psu_power_in_change(am_node_t *psus, int pin, uint8 presence);
int notify_psu_power_out_change(am_node_t *psus, int pout, uint8 presence);
int notify_psu_current_out_change(am_node_t *psus, int data, uint8 presence);
int notify_psu_power_cap_change(am_node_t *psus, int cap);
int notify_psu_total_power_cap_change(am_node_t *psus, int cap);
int notify_psu_serial_change(am_node_t *psus, char *serial);
int notify_psu_manufacture_change(am_node_t *psus, char *manufacture);
int notify_psu_model_change(am_node_t *psus, char *model);
int notify_psu_fw_ver_change(am_node_t *psus, char *fw_ver);
int notify_psu_health_status_change(am_node_t *psus, int status, uint8 presence);

int notify_fan_present_change(am_node_t *tzone, int present);
int notify_fans_speed_change(am_node_t *fans, unsigned int *data);

int notify_drawer_alert_change(am_node_t *tz, int present);
int notify_drawer_present_change(am_node_t *tz, int present);
int notify_drawer_temp_change(am_node_t *tmcs, int data);
int notify_drawer_health_status_change(am_node_t * drawers,uint8 data);
int notify_drawer_ip_change(am_node_t *tmcs, unsigned int ip_addr);
int notify_drawer_power_change(am_node_t *drawers, unsigned int power);
int notify_drawer_ip_source_change(am_node_t *drawers, uint8 ip_src);
int notify_drawer_subnet_mask_change(am_node_t *drawers, unsigned int mask);


int notify_sled_present_change(am_node_t *drawers, uint8 present);
int notify_sled_type_change(am_node_t *drawers, uint8 sled_loc, uint8 sled_type);
int notify_sled_temp_change(am_node_t *drawers, uint8 sled_loc, uint8 temp);
int notify_sled_pwm_change(am_node_t *drawers, uint8 sled_loc, uint8 pwm);
int notify_sled_power_change(am_node_t *drawers, uint8 sled_loc, uint16 power);
int notify_tzone_airflow_change(char *uuid, int32 q);
int notify_tzone_outlet_temp_change(char *uuid, int32 temp);



#endif

