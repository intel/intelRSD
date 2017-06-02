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


#ifndef __COOLING_CONTRL_H__
#define __COOLING_CONTRL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "libutils/list.h"
#include "libutils/rmm.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libmemdb/memdb.h"
#include "libutils/string.h"

#define ASSET_MON_DEBUG 1
#define ASSET_MON_INFO  1

#if ASSET_MON_DEBUG
#define CCTRL_DEBUG(format, args...)	printf("[CCTRL_DEBUG]:"format"\n", ##args)
#else
#define CCTRL_DEBUG(format, args...)
#endif

#if ASSET_MON_INFO
#define CCTRL_INFO(format, args...)	printf("[CCTRL_INFO]:"format"\n", ##args)
#else
#define CCTRL_INFO(format, args...)
#endif

#define CCTRL_FAN_PRESENT_STR     "fan_present"
#define CCTRL_SHELF_PRESENT_STR   "shelf_present"
#define CCTRL_CM_THERMAL_STR      "cm-thermal"
#define CCTRL_TMC_THERMAL_STR     "tmc-thermal"


#define COOLING_POLICY            "cooling_policy"

#define POLICY_FIXED_PWM_STR         "fixed_pwm"
#define POLICY_AGGREGATED_PWM_STR     "aggregated_pwm"
#define POLICY_AGGREGATED_THERMAL_STR "aggregated_thermal"

#define TEST_APP_IPMI_NETFN		0x06
#define TEST_GET_DEVID_CMD		0x01

#define DEFAULT_FAN_PRESENT        0x01
#define DEFAULT_SHELF_PRESENT      0x03

struct cm_node {
	struct list_head list;
	memdb_integer node_id;
	int output_pwm;
};

struct tmc_node {
	struct list_head list;
	memdb_integer node_id;
};

struct cm_attr {
	struct list_head  list;
	memdb_integer     node_id;
	memdb_integer prefix_handler;
	int    aggregated_thermal;
	int    aggregated_pwm[MAX_PWM_NUM];
};

struct cooling_ctrl {
	char policy[32];
	struct list_head cm_head;
	struct list_head tmc_head;
	struct list_head attr_head;
	
	memdb_integer cm_create_handler;
	memdb_integer cm_delete_handler;
	
	memdb_integer tmc_create_handler;
	memdb_integer tmc_delete_handler;
};

enum cooling_policy {
	FIXED      = 0x00,
	SIMPLE     = 0x01,
	POLICY_MAX = 0x02,
};

struct cooling_ctrl* cooling_allocate();
void cooling_init(struct cooling_ctrl *ctrl);
void cooling_event_ops(struct event_info *evt, void *cb_data);

#endif /* ifndef __COOLING_CONTRL_H__ */
