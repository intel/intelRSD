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


#ifndef __LIB_ASSET_MODULE_ACTION_H__
#define __LIB_ASSET_MODULE_ACTION_H__

#include <stdio.h>
#include "libjsonrpc/jsonrpc_err.h"
#include "libjsonrpcapi/libjsonrpcapi.h"

typedef struct _action_st {
	int  action_id;
	char *action_name;
} action_map_t;

enum AM_ACTIONS {
	AMACTION_SET_FAN_PWM,
	AMACTION_RESET_HARD_RESET,
	AMACTION_POST_PSU_BY_NODE_ID,
	AMACTION_MBP_FW_UPDATE,
	AMACTION_MBP_UART_SWITCH,
	AMACTION_PSME_SOFT_RESET,
	AMACTION_SET_RACK_PUID,
	MAX_ACTION
};


static action_map_t am_action_map[] = {
	{AMACTION_SET_FAN_PWM, JRPC_ACTION_SET_FAN_PWM},
	{AMACTION_RESET_HARD_RESET, JRPC_ACTION_HARD_RESET},
	{AMACTION_POST_PSU_BY_NODE_ID, JRPC_ACTION_POST_PSU},
	{AMACTION_MBP_FW_UPDATE, "mbp_fw_update"},
	{AMACTION_MBP_UART_SWITCH, JRPC_ACTION_UART_SWITCH},
	{AMACTION_PSME_SOFT_RESET, JRPC_ACTION_PSME_SOFT_RESET},
	{AMACTION_SET_RACK_PUID, JRPC_ACTION_SET_PUID}
	};


#endif

