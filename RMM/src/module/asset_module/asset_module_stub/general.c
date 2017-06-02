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

#include "libutils/string.h"
#include "cm.h"
#include "notify.h"

static int has_run = 0;

static void get_cm_status(void)
{
	char cm_uuid[64] = {};
	char tz_uuid[64] = {};
	char dz_uuid[64] = {};
	int64 cm_lid = 1;
	int64 tz_lid = 1;
	int64 dz_lid = 1;
	unsigned int mbp_ipaddr = 0xc8a0010b;

	sprintf(cm_uuid, "%s", CM_UUID);
	sprintf(tz_uuid, "%s", TZ_UUID);
	sprintf(dz_uuid, "%s", DZ_UUID);

	notify_cm_add(cm_uuid, &cm_lid);
	notify_tzone_add(tz_uuid, cm_uuid, &tz_lid);
	notify_dzone_add(dz_uuid, cm_uuid, &dz_lid);
	notify_cm_ip_change(cm_uuid, mbp_ipaddr); 
}

static void get_power_zone_status()
{
	int present = 0x01;
	int pin = 100;
	char cm_uuid[64] = {};
	char psu_uuid[64] = {};
	char pz_uuid[64] = {};

	sprintf(cm_uuid, "%s", CM_UUID);
	sprintf(pz_uuid, "%s", PZ_UUID);
	sprintf(psu_uuid, "%s", PSU1_UUID);

	notify_pzone_present_change(pz_uuid, cm_uuid);
	notify_psu_present_change(psu_uuid, pz_uuid, present);
	notify_psu_power_in_change(psu_uuid, pin);
}

static void get_thermal_zone_status()
{
	unsigned int speed = 2728;
	int present = 1;
	char fan_uuid[64] = {};
	char tz_uuid[64] = {};

	sprintf(tz_uuid, "%s", TZ_UUID);
	sprintf(fan_uuid, "%s", FAN1_UUID);

	notify_fan_present_change(fan_uuid, tz_uuid, &present);
	notify_fans_speed_change(fan_uuid, &speed);
}

static void get_drawer_status()
{
	int present = 1;
	char drawer_uuid[64] = {};
	char dz_uuid[64] = {};

	sprintf(dz_uuid, "%s", DZ_UUID);
	sprintf(drawer_uuid, "%s", DRAWER1_UUID);

	notify_drawer_present_change(drawer_uuid, dz_uuid, &present);
}

void am_run(void)
{
	if (has_run == 0) {
		get_cm_status();
		get_power_zone_status();
		get_thermal_zone_status();
		get_drawer_status();
		has_run = 1;
	}
}
