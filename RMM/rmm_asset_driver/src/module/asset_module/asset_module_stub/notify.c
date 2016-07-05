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
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "notify.h"
#include "libutils/utils.h"

int notify_tzone_add(char *uuid, char *parent_uuid, int64 *tz_lid)
{
	assetd_tzone_add(tz_lid, uuid, parent_uuid);
	assetd_tzone_attr_init(uuid, 1, tz_lid);

	return 0;
}

int notify_dzone_add(char *uuid, char *parent_uuid, int64 *dz_lid)
{
	assetd_dzone_add(dz_lid, uuid, parent_uuid);
	assetd_dzone_attr_init(uuid, dz_lid);

	return 0;
}

int notify_cm_add(char *uuid, int64 *cm_lid)
{
	assetd_cm_add(cm_lid, uuid);
	assetd_cm_attr_init(uuid, cm_lid);

	return 0;
}

int notify_cm_ip_change(char *uuid, unsigned int ip_addr)
{
	int64 data = ip_addr;
	int64 ver_high = 10;
	int64 ver_low = 18;

	return assetd_cm_ip_change(&data, &ver_high, &ver_low, uuid);
}

int notify_pzone_present_change(char *uuid, char *parent_uuid)
{
	int64 pz_lid = 1;

	assetd_pzone_add(&pz_lid, uuid, parent_uuid);

	return 0;
}

static void present_change(int64 type, int old_present, int present, int max, char *uuids, char *parent_uuid, int cm_lid)
{
	int64 i = 0;
	bool old_bit = 0;
	bool new_bit = 0;
	char *curr_uuid  = NULL;

	for (i = 0; i < max; i++) {
		old_bit = (old_present >> i) & 1;
		new_bit = (present >> i) & 1;
		curr_uuid = uuids + i * UUID_MAX_LEN;
		if (old_bit != new_bit) {
			if (new_bit) {
				gen_uuid_v4(curr_uuid);
				switch (type) {
				case PSU_TYPE:
					assetd_psu_add(i, curr_uuid, parent_uuid);
					assetd_psu_attr_init(curr_uuid, cm_lid, i);
					break;
				case FAN_TYPE:
					assetd_fan_add(i, curr_uuid,  parent_uuid);
					assetd_fan_attr_init(curr_uuid, cm_lid, i);
					break;
				case DRAWER_TYPE:
					assetd_drawer_add(i, curr_uuid, parent_uuid);
					assetd_drawer_attr_init(curr_uuid, cm_lid, i);
					break;
				case SLED_TYPE:
					assetd_sled_add(i, curr_uuid, parent_uuid);
					assetd_sled_attr_init(curr_uuid, cm_lid, i);
					break;
				default:
					break;
				}
			} else {
				switch (type) {
				case PSU_TYPE:
					assetd_psu_remove(curr_uuid);
					break;
				case FAN_TYPE:
					assetd_fan_remove(curr_uuid);
					break;
				case DRAWER_TYPE:
					assetd_drawer_remove(curr_uuid);
					break;
				case SLED_TYPE:
					assetd_sled_remove(curr_uuid);
					break;
				default:
					break;
				}
			}
		}
	}
}

int notify_psu_present_change(char *uuid, char *parent_uuid, int present)
{
	int cm_lid = 1;
	int64 type = PSU_TYPE;

	present_change(type, 0, present, MAX_PSU_NUM, uuid, parent_uuid, cm_lid);

	return 0;
}

int notify_psu_power_in_change(char *uuid, int pin)
{
	int64 data = pin;
	int64 total = pin;

	return assetd_psu_power_in_change(&data, &total, uuid);
}

int notify_fan_present_change(char *uuid, char *parent_uuid, int *present)
{
	int cm_lid = 1;
	int64 type = FAN_TYPE;

	present_change(type, 0, *present, MAX_PWM_NUM, uuid, parent_uuid, cm_lid);

	return assetd_fan_present(present, parent_uuid);
}


int notify_fans_speed_change(char *uuid, unsigned int *speed)
{
	assetd_fans_speed_change(speed, uuid);
	return 0;
}

int notify_drawer_present_change(char *uuid, char *parent_uuid, int *present)
{
	int64 type = DRAWER_TYPE;
	int cm_lid = 1;

	present_change(type, 0, *present, MAX_DRAWER_NUM, uuid, parent_uuid, cm_lid);

	return assetd_drawer_present(present, parent_uuid);
}




