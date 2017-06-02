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


#include <stdio.h>
#include "libjson/json.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/registerd_socket.h"
#include "libjsonrpcapi/asset_module_api.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/utils.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include <string.h>
#include <sys/socket.h>

#define GAMI_VERSION	"1.3"
#define GAMI_VENDOR	"Intel"
int gami_register_req(gami_reg_t *reg_info)
{
	jrpc_req_pkg_t req_pkg = {};
	json_t *cap_array = NULL;
	json_t *cap = NULL;
	int i = 0;
	int rc;
	char *ip = NULL;
	int64 port = reg_info->port;
	json_t *capabilities_array = NULL;
	json_t *cap_div = NULL;
	struct in_addr net_addr;

	printf("gami_register, name %s, ip %d, port %d.\n", reg_info->name, reg_info->ip_addr, reg_info->port);
	fill_param(&req_pkg, JRPC_GAMI_REG_VERSION, GAMI_VERSION, JSON_STRING);
	fill_param(&req_pkg, JRPC_GAMI_REG_VENDOR, GAMI_VENDOR, JSON_STRING);
	fill_param(&req_pkg, JRPC_GAMI_REG_NAME, reg_info->name, JSON_STRING);
	net_addr.s_addr = reg_info->ip_addr;
	ip = inet_ntoa(net_addr);
	fill_param(&req_pkg, JRPC_GAMI_REG_IP, ip, JSON_STRING);
	fill_param(&req_pkg, JRPC_GAMI_REG_PORT, &port, JSON_INTEGER);

	capabilities_array = json_array();
	if (capabilities_array == NULL) {
		return -1;
	}

	/*mbp*/
	cap_div = json_object();
	if (cap_div == NULL) {
		return -1;
	}

	cap_array = json_array();
	if (cap_array == NULL) {
		return -1;
	}

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (strnlen_s((char *)reg_info->capability.cap_mbp[i], sizeof(reg_info->capability.cap_mbp[i])-1) != 0) {
			cap = NULL;
			cap = json_object();
			if (cap == NULL) {
				return -1;
			}
			add_json_string(cap, JRPC_GAMI_CAP_NAME, reg_info->capability.cap_mbp[i]);
			json_array_add(cap_array, cap);
		}
	}
	json_object_add(cap_div, JRPC_GAMI_REG_CAP_MBP, cap_array);
	json_array_add(capabilities_array, cap_div);

	/*psu*/
	cap_div = json_object();
	if (cap_div == NULL) {
		return -1;
	}

	cap_array = json_array();
	if (cap_array == NULL) {
		return -1;
	}

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (strnlen_s((char *)reg_info->capability.cap_psu[i], sizeof(reg_info->capability.cap_psu[i])-1) != 0) {
			cap = NULL;
			cap = json_object();
			if (cap == NULL) {
				return -1;
			}
			add_json_string(cap, JRPC_GAMI_CAP_NAME, reg_info->capability.cap_psu[i]);
			json_array_add(cap_array, cap);
		}
	}
	json_object_add(cap_div, JRPC_GAMI_REG_CAP_PSU, cap_array);
	json_array_add(capabilities_array, cap_div);


	/*fan*/
	cap_div = json_object();
	if (cap_div == NULL) {
		return -1;
	}

	cap_array = json_array();
	if (cap_array == NULL) {
		return -1;
	}

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (strnlen_s((char *)reg_info->capability.cap_fan[i], sizeof(reg_info->capability.cap_fan[i])-1) != 0) {
			cap = NULL;
			cap = json_object();
			if (cap == NULL) {
				return -1;
			}
			add_json_string(cap, JRPC_GAMI_CAP_NAME, reg_info->capability.cap_fan[i]);
			json_array_add(cap_array, cap);
		}
	}
	json_object_add(cap_div, JRPC_GAMI_REG_CAP_FAN, cap_array);
	json_array_add(capabilities_array, cap_div);



	/*drawer*/
	cap_div = json_object();
	if (cap_div == NULL) {
		return -1;
	}

	cap_array = json_array();
	if (cap_array == NULL) {
		return -1;
	}

	for (i = 0; i < GAMI_MAX_CAP_NUM; i++) {
		if (strnlen_s((char *)reg_info->capability.cap_drawer[i], sizeof(reg_info->capability.cap_drawer[i])-1) != 0) {
			cap = NULL;
			cap = json_object();
			if (cap == NULL) {
				return -1;
			}
			add_json_string(cap, JRPC_GAMI_CAP_NAME, reg_info->capability.cap_drawer[i]);
			json_array_add(cap_array, cap);
		}
	}
	json_object_add(cap_div, JRPC_GAMI_REG_CAP_DRAWER, cap_array);
	json_array_add(capabilities_array, cap_div);

	fill_param(&req_pkg, JRPC_GAMI_CAPABILITIES, capabilities_array, JSON_ARRAY);

	rc = send_msg_to_registerd(&req_pkg, REG_APP);

	return rc;
}



