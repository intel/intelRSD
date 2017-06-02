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


#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "libredfish/rf_event.h"
#include "libredfish/snmp_event.h"
#include "libredfish/mr_def.h"
#include "libutils/log_level.h"
#include "libcfg/cfg.h"


int32 main(int32 argc, int8 **argv)
{
	int32 port = rmm_cfg_get_port(REDFISHD_PORT);

	rf_connect(INADDR_LOOPBACK, port);
	return 1;
};
