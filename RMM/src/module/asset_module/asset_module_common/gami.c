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

 #include "gami.h"

void generate_gami_reg_info(gami_reg_t *reg_info)
{
	char ip_str[64];

	bzero(ip_str, sizeof(ip_str));
	libutils_net_get_ip("eth0", ip_str, sizeof(ip_str));

	reg_info->ip_addr = inet_addr(ip_str);
	reg_info->port = rmm_cfg_get_port(ASSET_MODULE_PORT);
	strncpy_safe((char *)reg_info->name, __progname, MAX_NAME_LEN, MAX_NAME_LEN - 1);

	snprintf_s_s(reg_info->capability.cap_mbp[0], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_RESET);
	snprintf_s_s(reg_info->capability.cap_mbp[1], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_SETUARTTARGET);
	snprintf_s_s(reg_info->capability.cap_mbp[2], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_UPDATE);

	snprintf_s_s(reg_info->capability.cap_psu[0], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_REQ_STATE_CHANGE);

	snprintf_s_s(reg_info->capability.cap_fan[0], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_REQ_STATE_CHANGE);
	snprintf_s_s(reg_info->capability.cap_fan[1], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_SET_DESIRED_SPEED);

	snprintf_s_s(reg_info->capability.cap_drawer[0], GAMI_MAX_NAME_LEN, "%s", RMM_JSON_RESET);

}


int process_register(jrpc_req_pkg_t *req, json_t *resp)
{
	jrpc_data_integer port = 0;

	jrpc_get_named_param_value(req->json, JRPC_GAMI_REG_PORT, JSON_INTEGER, &port);
	if (port == 0)
		return -1;
	rmm_log(DBG, "%s port: %lld\n", __func__, port);

	if (connect_assetd((int)port) == 0) {
		cm_runnable = true;
	}
	return 0;

}


int gami_register(void)
{
	gami_reg_t reg_info = {{0}};

	generate_gami_reg_info(&reg_info);

	while (1) {
		if (0 == gami_register_req(&reg_info)) {
			rmm_log(INFO, "Sending gami registry request\n");
			break;
		} else {
			rmm_log(ERROR, "GAMI registry error\n");
			sleep(1);
		}
	}

	rmm_log(INFO, "GAMI registry finished\n");
	return 0;
}
