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

#include "libwrap.h"


result_t cm_hard_reset(int cm_idx, int *result)
{
	return am_peripheral_hard_reset(cm_idx, 0, result);
}

result_t cm_uart_switch(int cm_idx, int tray_idx, int tgt_idx)
{
	return am_uart_switch(cm_idx, tray_idx, tgt_idx);
}

result_t drawer_hard_reset(int32 dzone_idx, int32 drawer_idx, int* result)
{
//	drawer_idx =  (dzone_idx - 1) * MAX_DRAWER_NUM + drawer_idx;
	int peripheral_id = drawer_idx * 2 - 1;
	return am_peripheral_hard_reset(dzone_idx, peripheral_id, result);
}

result_t psme_soft_reset(int32 dzone_idx, int32 drawer_idx, int* result)
{
	//drawer_idx =  (dzone_idx - 1) * MAX_DRAWER_NUM + drawer_idx;
	//int peripheral_id = drawer_idx * 2 - 1;
	return am_psme_soft_reset(dzone_idx, drawer_idx, result);
}

result_t rmm_factory_reset(int* result)
{
	return assetd_rmm_factory_rest(result);
}

/* According to RMM-PSME spec v0.8
	libwrap_set_id_field/libwrap_get_id_field should not be called by web.
	thermal sensors, health sensors and tray power have already been set in memdb.
*/
#if 0
result_t libwrap_set_id_field(memdb_integer *node_id, int32 field_type, int32 field_instance, int32 byte_num, int32 *data)
{
	return am_set_id_field(node_id, field_type, field_instance, byte_num, data);
}

result_t libwrap_get_id_field(memdb_integer *node_id, int32 field_instance)
{
	return am_get_id_field(node_id, field_instance);
}

result_t libwrap_get_aggregate_thermal_sensor(memdb_integer *node_id)
{
	return am_get_aggregate_thermal_sensor(node_id);
}

result_t libwrap_get_aggregate_health_sensor(memdb_integer *node_id)
{
	return am_get_aggregate_health_sensor(node_id);
}

result_t libwrap_get_tray_power(memdb_integer *node_id)
{
	return am_get_tray_power(node_id);
}
#endif


result_t libwrap_drawer_hard_reset(int cm_idx, int drawer_idex, int *result)
{
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_DRAWER, RMM_JSON_RESET)) != RESULT_OK)
		return rc;

	return drawer_hard_reset(cm_idx, drawer_idex, result);
}

result_t libwrap_psme_soft_reset(int cm_idx, int drawer_idex, int *result)
{
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_DRAWER, RMM_JSON_RESET)) != RESULT_OK)
		return rc;

	return psme_soft_reset(cm_idx, drawer_idex, result);
}

result_t libwrap_rmm_factory_reset(int *result)
{
	return rmm_factory_reset(result);
}

result_t libwrap_drawer_soft_reset(int cm_idx, int drawer_idex, int *result)
{
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_DRAWER, RMM_JSON_RESET)) != RESULT_OK)
		return rc;

	return psme_soft_reset(cm_idx, drawer_idex, result);
}

result_t libwrap_get_log(int begin, int count, void* info)
{
    return -1;
}


result_t libwrap_mbp_hard_reset(int idx, int *result)
{
	result_t rc;
	
	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_MBP, RMM_JSON_RESET)) != RESULT_OK)
		return rc;
	return cm_hard_reset(idx, result);
}

result_t libwrap_mbp_soft_reset(int idx, int *result)
{
	result_t rc;
	
	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_MBP, RMM_JSON_RESET)) != RESULT_OK)
		return rc;
	
	return -1;
}

result_t libwrap_mbp_uart_switch(int cm_idx, int tray_idx, int tgt_idx)
{
	result_t rc;

	if ((rc = check_module_capability(JRPC_GAMI_REG_CAP_MBP, RMM_JSON_SETUARTTARGET)) != RESULT_OK)
		return rc;

	return cm_uart_switch(cm_idx, tray_idx, tgt_idx);
}


