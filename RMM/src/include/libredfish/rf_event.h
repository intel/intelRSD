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


#ifndef __LIBREDFISH_RF_EVENT_H__
#define __LIBREDFISH_RF_EVENT_H__

#include "libredfish/rf_types.h"
#include "libredfish/snmp_event.h"

#define RF_LOG                  0
#define RF_EVENT                1

/**
 * @brief connect to redfishd.
 *
 * @param  host 		host ip of redfishd.
 * @param  port 		port of redfishd.
 */
int32 rf_connect(uint32 host, int32 port);

/**
 * @brief send message to redfishd
 *
 * @param  func_name 		func name.
 * @param  level 			log level of this message.
 * @param  is_evt 			0: only write to log.
 * 							1: write to log and tigger the redfish event.
 * @param  msg_sn 			msg number.
 */
int32 rf_msg_send(const int8 *func_name, int32 level, int32 is_evt, int32 msg_sn, ...);

/**
 * @brief get redfish event log.
 *
 * @param  count 			The count of lines.
 * @param  desc_rsp 		the description of get log response.
 * @param  data				the pointer of log.
 */
int32 rf_log_get_by_limit(int32 count, struct rf_log_desc *desc_rsp, int8 *data);

#define rf_log(level, msg_sn, args...) rf_msg_send(__func__, level, RF_LOG,  msg_sn, ##args)

int32 rf_get_fixed_info(rf_fixed_t *fixed);
int32 rf_get_evt_support(int32 *evt_types);

extern struct evt_source_info evt_source_infos[];
extern const id_evt_type_t rf_evt_msg[];
extern int32 evt_source_infos_cnt;

#endif
