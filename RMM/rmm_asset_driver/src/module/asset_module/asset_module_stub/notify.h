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

#ifndef __ASSET_MONITOR_NOTIFY_H__
#define __ASSET_MONITOR_NOTIFY_H__

#include <stdio.h>
#include "cm.h"

int notify_tzone_add(char *uuid, char *parent_uuid, int64 *tz_lid);
int notify_dzone_add(char *uuid, char *parent_uuid, int64 *dz_lid);
int notify_cm_add(char *uuid, int64 *cm_id);
int notify_cm_ip_change(char *uuid, unsigned int ip_addr);
int notify_pzone_present_change(char *uuid, char *parent_uuid);
int notify_psu_present_change(char *uuid, char *parent_uuid, int present);
int notify_psu_power_in_change(char *uuid, int pin);
int notify_fan_present_change(char *uuid, char *parent_uuid, int *present);
int notify_fans_speed_change(char *uuid, unsigned int *speed);
int notify_drawer_present_change(char *uuid, char *parent_uuid, int *present);


#endif

