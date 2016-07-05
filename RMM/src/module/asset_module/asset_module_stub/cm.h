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

#ifndef __ASSET_MONITOR_CM_H__
#define __ASSET_MONITOR_CM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>

#include "libassetd/assetd_type.h"
#include "libutils/rmm.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libjipmi/libjipmi.h"
#include "general.h"
#include "config.h"


#define SERIAL_ERROR_WAIT_TIME     2000

#define CM_UUID                    "dd2799ba-2bd4-43d4-b350-41d65de3f68c"
#define TZ_UUID                    "dfefca3e-eecb-41b2-a0ef-99b0b7850cbc"
#define FAN1_UUID                  "25e7de46-28c8-482f-b0e2-25dc9f46bdb1"
#define DZ_UUID                    "52cb9d8c-198a-47a6-a977-edf2c5443f5e"
#define DRAWER1_UUID               "f4fee293-d2da-427a-b7c1-5f6c16caa47b"
#define PZ_UUID                    "c7e74d1b-252d-43a0-8228-61763c12cdd8"
#define PSU1_UUID                  "5f5f4ddb-6d86-4367-aa2d-c99a95aee944"

extern void am_run(void);


#endif

