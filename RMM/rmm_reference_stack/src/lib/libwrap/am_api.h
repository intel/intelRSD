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


#ifndef __AM_API_H__
#define __AM_API_H__

int am_get_fan_pwm_by_id(memdb_integer* cm_node_id, memdb_integer* fan_node_id, int64 fan_idx);
int am_set_tzone_pwm(memdb_integer tzone_node_id, int64 pwm);
result_t am_peripheral_hard_reset(int64 cm_idx, int64 peripheral_id, int32 * result);
result_t am_psme_soft_reset(int64 dz_idx, int64 drawer_lid, int32 * result);
result_t am_post_psu_by_node_id(int64 pzone_idx, int64 psu_idx, int64 request_enabled_state);
result_t am_set_rack_puid(int64 puid);

#if 0
int am_set_id_field(memdb_integer *node_id, int64 field_type, int64 field_instance, int64 byte_num, int32 *data);
int am_get_id_field(memdb_integer *node_id, int64 field_instance);
int am_get_aggregate_thermal_sensor(memdb_integer *node_id);
int am_get_aggregate_health_sensor(memdb_integer *node_id);
int am_get_tray_power(memdb_integer *node_id);
#endif
int fill_array_elem(json_t *array, char *attr_name, void *value_ptr, json_type type);
void fill_jrpc_action_pkg(jrpc_req_pkg_t *req_pkg, int action_id, json_t *attr);

int am_uart_switch(int64 cm_lid, int64 tray_lid, int64 tgt_lid);

#endif

