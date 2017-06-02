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

#ifndef RMM_GAMI_REG
#define RMM_GAMI_REG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <errno.h>
#include <libutils/dump.h>

#include "libutils/rmm_json.h"
#include "libutils/sock.h"
#include "libutils/ip.h"
#include "libutils/string.h"
#include "liblog/log.h"
#include "libcfg/cfg.h"
#include "libjsonrpcapi/asset_module_api.h"
#include "libjsonrpcapi/registerd_api.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/parser.h"
#include "libinit/libinit.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libassetmodule/am_action.h"
#include "libsecurec/safe_lib.h"
#include "libassetd/assetd_type.h"

extern bool cm_runnable;

extern void generate_gami_reg_info(gami_reg_t *reg_info);
extern int process_register(jrpc_req_pkg_t *req, json_t *resp);
extern int gami_register(void);

#endif