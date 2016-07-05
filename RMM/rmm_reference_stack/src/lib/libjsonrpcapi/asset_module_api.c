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


#include <stdio.h>
#include "libjson/json.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/registerd_socket.h"
#include "libjsonrpcapi/asset_module_api.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpcapi/utils.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"

int asset_module_set_gami_attr(int *ip_addr, int *port)
{
	jrpc_req_pkg_t req_pkg = {};
	jrpc_rsp_pkg_t rsp_pkg = {};
	int rc;
	int64 tmp_ip = *ip_addr;
	int64 tmp_port = *port;

	fill_param(&req_pkg, JRPC_GAMI_REG_IP, &tmp_ip, JSON_INTEGER);
	fill_param(&req_pkg, JRPC_GAMI_REG_PORT, &tmp_port, JSON_INTEGER);

	rc = send_msg_to_asset_module(&req_pkg, &rsp_pkg, REG_APP);
	jrpc_rsp_pkg_free(&rsp_pkg);

	return rc;
}


