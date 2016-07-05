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
#include <string.h>
#include <netinet/in.h>

#include "libredfish/rf_event.h"
#include "libredfish/snmp_event.h"
#include "libredfish/mr_def.h"
#include "libutils/log_level.h"
#include "libcfg/cfg.h"


int32 main(int32 argc, int8 **argv)
{
	int32 port = rmm_cfg_get_port(SNMPAGENTD_PORT);

	snmp_subagentd_connect(INADDR_LOOPBACK, port);
	//snmp_subagentd_set(0, "10.239.196.224", 162);
	//snmp_subagentd_set(0, "10.239.196.224", 162);
	
	//snmp_subagentd_evt(INFO, MSGRackUpdate, "Rack update");
	//snmp_subagentd_set(1, "10.239.196.224", 162);
	
	/*rf_snmp_evt(INFO, MSGPsuChange, 1, "Rack change");
	rf_snmp_evt(INFO, MSGPsuUpdate, 1);
	rf_snmp_evt(INFO, MSGPsuAdd, 2);
	rf_snmp_evt(INFO, MSGPsuRemove, 2);


	rf_snmp_evt(INFO, MSGFanChange, 2, "reset");
	rf_snmp_evt(INFO, MSGFanUpdate, 2);
	rf_snmp_evt(INFO, MSGFanAdd, 2);
	rf_snmp_evt(INFO, MSGFanRemove, 2);


	rf_snmp_evt(INFO, MSGMbpChange, 2, "reset");
	rf_snmp_evt(INFO, MSGMbpUpdate, 1);
	rf_snmp_evt(INFO, MSGMbpAdd, 2);
	rf_snmp_evt(INFO, MSGMbpRemove, 2);


	rf_snmp_evt(INFO, MSGDrawerChange, 2 , "reset");
	rf_snmp_evt(INFO, MSGDrawerUpdate, 2);
	rf_snmp_evt(INFO, MSGDrawerAdd, 2);
	rf_snmp_evt(INFO, MSGDrawerRemove, 2);
	rf_snmp_evt(INFO, MSGDrawerAlert, 2);

	rf_snmp_evt(INFO, MSGPZoneUpdate, 2);
	rf_snmp_evt(INFO, MSGPZoneAdd, 2);
	rf_snmp_evt(INFO, MSGPZoneRemove, 2);

	rf_snmp_evt(INFO, MSGTZoneUpdate, 2);
	rf_snmp_evt(INFO, MSGTZoneAdd, 2);
	rf_snmp_evt(INFO, MSGTZoneRemove, 2);

	rf_snmp_evt(INFO, MSGDZoneUpdate, 2);
	rf_snmp_evt(INFO, MSGDZoneAdd, 2);
	rf_snmp_evt(INFO, MSGDZoneRemove, 2);*/

	return 1;
};
