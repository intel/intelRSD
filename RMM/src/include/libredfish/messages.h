/**
 * Copyright (c) 2017 Intel Corporation.
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

#ifndef __LIBREDFISH_MESSAGES_H__
#define __LIBREDFISH_MESSAGES_H__

#include "mr_def.h"
#include "msg_reg.h"

static const struct Message{
	char*	name;
	char*	desc;
	char*	msg;
	char*	severity;
	unsigned short	num_of_args;
	char*	types[MAX_PARAM_TYPES];
} static_messages[] = {
	{ "MSGStatusChange",
		"Resource Status Change",
		"Resource status change.\n",
		"N/A",
		0,
		{}
	},
	{ "MSGResourceUpdated",
		"Resource Update",
		"Resource updated.\n",
		"N/A",
		0,
		{}
	},
	{ "MSGResourceAdded",
		"Resource Add",
		"Resource add\n",
		"OK",
		0,
		{}
	},
	{ "MSGResourceRemoved",
		"Resource Remove",
		"Resource removed.\n",
		"OK",
		0,
		{}
	},
	{ "MSGAlert",
		"Resource Alert",
		"Resource alert.\n",
		"OK",
		0,
		{}
	},
	{ "MSGRackStatusChange",
		"Rack Status Change",
		"Rack status change [%string]\n",
		"N/A",
		1,
		{"string"}
	},
	{ "MSGRackResourceUpdated",
		"Rack Info Update",
		"Rack updated [%string].\n",
		"N/A",
		1,
		{"string"}
	},
	{ "MSGPSUStatusChange",
		"PSU Status Change",
		"PowerZone [%string] PSU [%string] status change [%string].\n",
		"N/A",
		3,
		{"string", "string", "string"}
	},
	{ "MSGPSUResourceUpdated",
		"PSU Info Update",
		"PowerZone [%string] PSU [%string] updated.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGPSUResourceAdded",
		"PSU Add",
		"PowerZone [%string] PSU [%string] add.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGPSUResourceRemoved",
		"PSU Remove",
		"PowerZone [%string] PSU [%string] Remove.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGFanResourceAdded",
		"FAN Add",
		"ThermalZone [%string] FAN [%string] add.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGFanResourceRemoved",
		"FAN Remove",
		"ThermalZone [%string] FAN [%string] Remove.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGFanStatusChange",
		"FAN Status CHange",
		"ThermalZone [%string] FAN [%string] Status Change.\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGMBPStatusChange",
		"MBP Status Change",
		"MBP [%string] status change [%string].\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGMBPResourceUpdated",
		"MBP Info update",
		"MBP [%string] updated.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGMBPResourceAdded",
		"MBP Add",
		"MBP [%string] add.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGMBPResourceRemoved",
		"MBP Remove",
		"MBP [%string] Remove.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGDrawerStatusChange",
		"Drawer Status Change",
		"Drawer [%string] status change [%string].\n",
		"OK",
		2,
		{"string", "string"}
	},
	{ "MSGDrawerResourceUpdated",
		"Drawer update",
		"Drawer [%string] updated.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGDrawerResourceAdded",
		"Drawer Add",
		"Drawer [%string] add\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGDrawerResourceRemoved",
		"Drawer Remove",
		"Drawer [%string] Remove.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGDrawerAlert",
		"Drawer Alert",
		"Drawer [%string] Alert.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGPZoneResourceUpdated",
		"Power Zone Info update",
		"Power Zone [%string] updated.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGPZoneResourceAdded",
		"Power Zone Add",
		"Power Zone [%string] add.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGPZoneResourceRemoved",
		"Power Zone Remove",
		"Power Zone [%string] Remove.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGTZoneResourceUpdated",
		"Thermal Zone Info update",
		"Thermal Zone [%string] updated.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGTZoneResourceAdded",
		"Thermal Zone Add",
		"Thermal Zone [%string] add.\n",
		"OK",
		1,
		{"string"}
	},
	{ "MSGTZoneResourceRemoved",
		"Thermal Zone Remove",
		"Thermal Zone [%string] Remove.\n",
		"OK",
		1,
		{"string"}
	},
	{ /* End of the messages indicator */ NULL,NULL,NULL,NULL,0,{} }
};

#endif
