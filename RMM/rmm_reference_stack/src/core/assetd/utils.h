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


#ifndef __ASSETD_UTILS_H__
#define __ASSETD_UTILS_H__

#include <time.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libutils/list.h"
#include "libipmi/common.h"
#include "libmemdb/memdb.h"

#include "libjipmi/libjipmi.h"

#include "libredfish/rf_event.h"
#include "libutils/rmm.h"
#include "libwrap/wrap.h"

#if 0
extern result_t assetd_get_psu_serial(memdb_integer *node_id, int psu_idx, int pzone_idx);
extern result_t assetd_get_psu_manufacture(memdb_integer *node_id, int psu_idx, int pzone_idx);
extern result_t assetd_get_psu_model(memdb_integer *node_id, int psu_idx, int pzone_idx);
extern result_t assetd_get_psu_fw_ver(memdb_integer *node_id, int psu_idx, int pzone_idx);
extern result_t assetd_get_drawer_ip(memdb_integer* node_id, int32 dz_idx, int32 drawer_idx);
#endif
extern result_t assetd_gen_uuid(char *key, char *uuid);


#endif

