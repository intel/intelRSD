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


#ifndef __LIBREDFISH_SNMP_EVENT_H__
#define __LIBREDFISH_SNMP_EVENT_H__

#include "libredfish/rf_types.h"

#define RF_LOG                  0
#define RF_EVENT                1

/**
 * @brief connect to redfishd.
 *
 * @param  host 		host ip of redfishd.
 * @param  port 		port of redfishd.
 */
int32 snmp_subagentd_connect(uint32 host, int32 port);
int32 snmp_subagentd_msg_send(const int8 *func_name, int32 level, int32 is_evt, int32 msg_sn, ...);

#define snmp_subagentd_evt(level, msg_sn, args...) snmp_subagentd_msg_send(__func__, level, RF_EVENT, msg_sn, ##args)

int32 snmp_subagentd_set(int32 type, int8 *ip, int32 port);

#endif
