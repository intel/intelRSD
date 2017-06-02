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


#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libutils/sock.h"
#include "libutils/dump.h"
#include "libmemdb/memdb.h"
#include "liblog/log.h"
#include "libcfg/cfg.h"
#include "libjsonrpc/jsonrpc.h"
#include "libredfish/rf_event.h"
#include "libredfish/snmp_event.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/registerd_socket.h"
#include "libinit/libinit.h"

static int is_gami_reg_ready(int max_retry)
{
	while (1) {
		if (connect_registerd() != -1)
			return 0;

		if (max_retry == 0)
			return -1;
		else if (max_retry > 0)
			max_retry--;

		sleep(1);
	}
	return -1;
}

static int is_assetd_ready(int max_retry)
{
	int port = 0;

	port = rmm_cfg_get_port(ASSETD_PORT);
	
	while (1) {
		if (connect_assetd(port) != -1)
			return 0;

		if (max_retry == 0)
			return -1;
		else if (max_retry > 0)
			max_retry--;

		sleep(1);
	}
	return -1;
}

int rmm_modules_init(unsigned int modules, unsigned int jipmi_port_attr, unsigned int jsonrpcapi_bitmap)
{
#define INIT_MAX_RETRY    10

	int port = 0;
	int ret = -1;
	
	if (modules | MODULEINIT_LOG) {
		ret = rmm_log_init();
		if (ret == -1)
			return -1;
	}

	if (modules | MODULEINIT_COREDUMP)
		enable_core_dump();

	if (modules | MODULEINIT_REDFISHD) {
		port = rmm_cfg_get_port(REDFISHD_PORT);
		ret = rf_connect(INADDR_LOOPBACK, port);
		if (ret == -1)
			return -1;
		port = rmm_cfg_get_port(SNMPAGENTD_PORT);
		ret = snmp_subagentd_connect(INADDR_LOOPBACK, port);
		if (ret == -1)
			return -1;
	}

	if (modules | MODULEINIT_GAMI_REG) {
		ret = is_gami_reg_ready(INIT_MAX_RETRY);
		if (ret == -1)
			return -1;
	}

	if (modules | MODULEINIT_ASSETD) {
		ret = is_assetd_ready(INIT_MAX_RETRY);
		if (ret == -1)
			return -1;
	}

	if (jsonrpcapi_bitmap) {
		port  = rmm_cfg_get_port(jipmi_port_attr);
		libjsonrpcapi_init(JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI, port);
		if (jsonrpcapi_bitmap | JSONRPCINIT_MEMDB)
			ret = libdb_is_ready(DB_RMM, LOCK_ID_NULL, -1);

		if (ret == -1)
			return -1; 
	}

	return 0;
}


