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

#ifndef __ASSET_MONITOR_ACTION_H__
#define __ASSET_MONITOR_ACTION_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>

#include "libutils/types.h"
#include "libassetd/assetd_type.h"
#include "libutils/rmm.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libjipmi/common.h"
#include "libjipmi/libjipmi.h"

#include "libassetd/assetd_jrpc_def.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libjsonrpcapi/assetd_api.h"

#include "libassetmodule/am_action.h"
#include "libwrap/wrap.h"
#include "cm_update.h"
#include "cm.h"


#define MAX_ACTION_HANDLER_PARAMS 6

typedef int (*do_action_handler_fn) ();
typedef struct handler_tbl {
	bool   set_flag;
	do_action_handler_fn do_fn;
	uint8  param_num;
	jrpc_data_integer params[MAX_ACTION_HANDLER_PARAMS];
} handler_tbl_t;

extern handler_tbl_t action_handler_tbl[];
void set_handler_fn(uint8 fn_id, int8 param_num, ...);
int get_handler_fn(uint8 fn_id, jrpc_data_integer *params);
int get_array_elem(jrpc_req_pkg_t *req_pkg, char *attr_name, json_type type, void *value_ptr);

void process_actions(am_node_t *cm);
int action_set_tzone_pwm(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args);
int post_psu_by_node_id(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args);
int do_post_psu_by_node_id();
int peripheral_hard_reset(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args);
int do_peripheral_hard_reset();
int psme_soft_reset(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args);
int do_psme_soft_reset();
int uart_switch(jrpc_req_pkg_t *req, json_t *resp, void *args);
int do_uart_switch();
int set_rack_puid(jrpc_req_pkg_t *req_pkg, json_t *resp, void *args);


#endif

