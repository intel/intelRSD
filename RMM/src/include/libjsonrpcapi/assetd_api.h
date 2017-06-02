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


#ifndef __ASSETMGR_API_H__
#define __ASSETMGR_API_H__

#include <stdio.h>
#include "libutils/rmm.h"
#include "libutils/types.h"
#include "libjsonrpc/jsonrpc.h"

int fill_param(jrpc_req_pkg_t * req, char * name, void * value, json_type type);
int assetd_add(jrpc_req_pkg_t *req);
int assetd_remove(jrpc_req_pkg_t *req);
int assetd_update(jrpc_req_pkg_t *req);
int assetd_get_fan_pwm_by_id(memdb_integer* cm_node_id, memdb_integer* fan_node_id, int64 fan_idx);
int assetd_set_fan_pwm(int64 tzone_idx, int64 fan_idx, int64 pwm);
int assetd_peripheral_hard_reset(int64 cm_idx, int64 peripheral_id, int32 * result);
int assetd_rmm_factory_rest(int32 * result);

int assetd_set_id_field(memdb_integer *node_id, int64 field_type, int64 field_instance, int64 byte_num, int32 *data);
int assetd_get_id_field(memdb_integer *node_id, int64 field_instance);
int assetd_get_aggregate_thermal_sensor(memdb_integer *node_id);
int assetd_get_aggregate_health_sensor(memdb_integer *node_id);
int assetd_get_tray_power(memdb_integer *node_id);
int assetd_rmm_factory_rest(int32 * result);
int assetd_tzone_add(int64 *tz_lid, char *uuid, char *parent_uuid);
int assetd_tzone_remove(char *uuid);
int assetd_tzone_attr_init(char *uuid, int cm_lid, int64 *tz_lid);
int assetd_tzone_rpm_change(char *uuid, int32 rpm);
int assetd_tzone_temp_change(char *uuid, int32 temp);
int assetd_tzone_airflow_change(char *uuid, int32 q);
int assetd_tzone_outlet_temp_change(char *uuid, int32 temp);
int assetd_dzone_add(int64 *dz_lid, char *uuid, char *parent_uuid);
int assetd_dzone_attr_init(char *uuid, int64 *dz_lid);
int assetd_pzone_add(int64 *pz_lid, char *uuid, char *parent_uuid);
int assetd_pzone_attr_init(char *uuid, int cm_lid, int64 *pz_lid);
int assetd_cm_add(int64* cm_lid, char *uuid);
int assetd_cm_attr_init(char *uuid, int64 *cm_lid);
int assetd_cm_remove(char *uuid);
int assetd_cm_ip_change(int64 *ip_addr, int64 *ver_high, int64 *ver_low, char *uuid);
int assetd_cm_ver_change(int64 *ip_addr, int64 *ver_high, int64 *ver_low, char *uuid);
int assetd_pzone_remove(char *uuid);
int assetd_psu_add(int pos, char *uuid, char *parent_uuid);
int assetd_psu_attr_init(char *uuid, int cm_lid, int pos);
int assetd_psu_remove(char *uuid);
int assetd_psu_present(int *present, char *uuid);
int assetd_psu_power_out_change(int64 *pout, int64 *total, char * uuid);
int assetd_psu_health_status_change(int64 *status, char *uuid);
int assetd_psu_power_in_change(int64 *pin, int64 *total, char * uuid);
int assetd_psu_current_out_change(int64 *iout, int64 *total, char * uuid);
int assetd_psu_power_cap_change(int64 *cap, char *uuid);
int assetd_psu_total_power_cap_change(int64 *cap, char *uuid);
int assetd_psu_serial_change(char *serial, char * uuid);
int assetd_psu_manufacture_change(char *manufacture, char * uuid);
int assetd_psu_model_change(char *model, char * uuid);
int assetd_psu_fw_ver_change(char *fw_ver, char * uuid);
int assetd_fan_add(int pos, char *uuid, char *parent_uuid);
int assetd_fan_attr_init(char *uuid, int cm_lid, int pos);
int assetd_fan_remove(char * uuid);
int assetd_fan_present(int *present, char *uuid);
int assetd_fans_speed_change(unsigned int *data, char * uuid);
int assetd_drawer_add(int pos,char *uuid, char *parent_uuid);
int assetd_drawer_attr_init(char *uuid, int cm_lid, int pos);
int assetd_drawer_remove(char * uuid);
int assetd_drawer_alert_change(int *alert, char *uuid);
int assetd_drawer_present(int *present, char *uuid);
int assetd_drawer_temp_change(int64 *temp, char *uuid);
int assetd_drawer_health_status_change(int64 *health, char *uuid);
int assetd_drawer_ip_change(int64 *ip, char *uuid);
int assetd_drawer_power_change(int64 *power, char *uuid);
int assetd_drawer_ip_source_change(int64 *ip_src, char *uuid);
int assetd_drawer_subnet_mask_change(int64 *mask, char *uuid);
int assetd_sled_add(int pos, char *uuid, char *parent_uuid);
int assetd_sled_attr_init(char *uuid, int cm_lid, int pos);
int assetd_sled_remove(char * uuid);
int assetd_sled_present(int *present, char *uuid);
int assetd_sled_type_change(uint8 *sled_type, char *uuid);
int assetd_sled_temp_change(uint8 *temp, char *uuid, uint8 *avg);
int assetd_tz_temp_change(uint8 *avg, char *uuid);
int assetd_sled_pwm_change(uint8 *pwm, char *uuid);
int assetd_sled_power_change(uint16 *power, char *uuid);



#endif

