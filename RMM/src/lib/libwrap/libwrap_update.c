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

#include "libwrap.h"



static int var_firmware_update_ok = 1;
static int rmm_update_ok = 1;


void libwrap_set_firmware_update_status(int status)
{
	var_firmware_update_ok = status;
}

int libwrap_get_firmware_update_status(void)
{
	return var_firmware_update_ok;
}

void libwrap_set_rmm_update_status(int status)
{
	rmm_update_ok = status;
}

int libwrap_get_rmm_update_status(void)
{
	return rmm_update_ok;
}

result_t libwrap_check_update_capability(void)
{
	return check_module_capability(JRPC_GAMI_REG_CAP_MBP, JRPC_GAM_CAP_UPDATE);
}

/**
 * @brief: upgrade the CM firmware via IPMI.
 *         Steps:
 *         1. send the tftp server ip to the CM
 *         2. send the file name for upgrading to the CM
 *         3. send the command to start to do the upgradation
 *         4. check the serial port if disappeared for a while,
 *            if yes, the cm reboot itself and the update finished.
 *            or need to send the command "reboot" via serial console
 */
result_t libwrap_update_mbp_firmware(int idx, char *dev, char *file)
{
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	memdb_integer cm_node_id = 0;
	int host = 0;

	if (NULL == file)
		return RESULT_OTHER_ERR;

	cm_node_id = get_subnode_id_by_lid(idx, MC_NODE_ROOT, MC_TYPE_CM);
	libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);

	if (0 == host) {
		printf("cm host ip got 0. Error.\n");
		return RESULT_OTHER_ERR;
	}

	json_t *attr_array = json_array();

	fill_array_elem(attr_array, JRPC_ACTION_MBP_UPDATE_HOST, (void *)&host, JSON_INTEGER);
	fill_array_elem(attr_array, JRPC_ACTION_MBP_UPDATE_DEV, (void *)dev, JSON_STRING);
	fill_array_elem(attr_array, JRPC_ACTION_MBP_UPDATE_FILE, (void *)file, JSON_STRING);
	fill_jrpc_action_pkg(&req_pkg, AMACTION_MBP_FW_UPDATE, attr_array);

	send_msg_to_asset_module(&req_pkg, &rsp_pkg, DO_ACTION);

	libwrap_set_firmware_update_status(1);

	jrpc_req_pkg_free(&req_pkg);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return RESULT_OK;
}



