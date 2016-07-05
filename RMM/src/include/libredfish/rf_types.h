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


#ifndef __LIBUTILS_RF_TYPE_H__
#define __LIBUTILS_RF_TYPE_H__

#include "libutils/types.h"

#define RF_MSG_MAX_LEN					256
#define MAX_MODULE_NAME_LEN				128
#define MAX_FUNC_NAME_LEN				128
#define BUF_SIZE						25600
#define MAX_RF_EVT_MSG_LOG_COUNT		1024
#define MAX_IP_LEN						64

#define RF_CONTEXT_LEN      256
#define RF_URL_LEN          256
#define RF_PROTOCOL_LEN     64
#define RF_NAME_LEN         64
#define RF_DESC_LEN         128

#define MAX_EVT_ACTION_NUM	32
#define MAX_EVT_SOURCE_NUM	10
#define MAX_RF_EVT_NUM		(MAX_EVT_ACTION_NUM*MAX_EVT_SOURCE_NUM)

#define RF_EVENT_TYPE_RACK_CHANGE_STR		"RackStatusChange"
#define RF_EVENT_TYPE_RACK_UPDATE_STR		"RackResourceUpdated"
#define RF_EVENT_TYPE_PZONE_ADD_STR			"PowerZoneResourceAdded"
#define RF_EVENT_TYPE_PZONE_REMOVE_STR		"PowerZoneResourceRemoved"
#define RF_EVENT_TYPE_PSU_CHANGE_STR		"PSUStatusChange"
#define RF_EVENT_TYPE_PSU_ADD_STR			"PSUResourceAdded"
#define RF_EVENT_TYPE_PSU_REMOVE_STR		"PSUResourceRemoved"

#define RF_EVENT_TYPE_TZONE_ADD_STR			"ThermalZoneResourceAdded"
#define RF_EVENT_TYPE_TZONE_REMOVE_STR		"ThermalZoneResourceRemoved"
#define RF_EVENT_TYPE_TZONE_UPDATE_STR		"ThermalZoneResourceUpdated"
#define RF_EVENT_TYPE_FAN_ADD_STR			"FanResourceAdded"
#define RF_EVENT_TYPE_FAN_REMOVE_STR		"FanResourceRemoved"
#define RF_EVENT_TYPE_FAN_CHANGE_STR		"FanStatusChange"

#define RF_EVENT_TYPE_DRAWER_CHANGE_STR		"DrawerStatusChange"
#define RF_EVENT_TYPE_DRAWER_ADD_STR		"DrawerResourceAdded"
#define RF_EVENT_TYPE_DRAWER_REMOVE_STR		"DrawerResourceRemoved"
#define RF_EVENT_TYPE_DRAWER_ALERT_STR		"DrawerAlert"

#define RF_EVENT_TYPE_MBP_CHANGE_STR		"MBPStatusChange"
#define RF_EVENT_TYPE_MBP_ADD_STR			"MBPResourceAdded"
#define RF_EVENT_TYPE_MBP_REMOVE_STR		"MBPResourceRemoved"

#define RF_EVENT_TYPE_ADD_STR				"ResourceAdded"
#define RF_EVENT_TYPE_DELETE_STR			"ResourceRemoved"
#define RF_EVENT_TYPE_UPDATE_STR			"ResourceUpdated"
#define RF_EVENT_TYPE_CHANGE_STR			"StatusChange"
#define RF_EVENT_TYPE_ALERT_STR				"Alert"

#define RF_EVENT_LISTENER_NAME_STR          "name"
#define RF_EVENT_LISTENER_DESC_STR          "description"
#define RF_EVENT_LISTENER_DEST_STR          "dest"
#define RF_EVENT_LISTENER_CONTEXT_STR       "context"
#define RF_EVENT_LISTENER_PROTOCOL_STR      "protocol"
#define RF_EVENT_LISTENER_INDEXES_STR       "indexes"

#define RF_EVENT_TYPE_ADD_BIT			0x01
#define RF_EVENT_TYPE_DELETE_BIT		(RF_EVENT_TYPE_ADD_BIT << 1)
#define RF_EVENT_TYPE_UPDATE_BIT		(RF_EVENT_TYPE_ADD_BIT << 2)
#define RF_EVENT_TYPE_CHANGE_BIT		(RF_EVENT_TYPE_ADD_BIT << 3)
#define RF_EVENT_TYPE_ALERT_BIT			(RF_EVENT_TYPE_ADD_BIT << 4)

#define RF_EVENT_MAX_MSG_ARG_NUM		8
#define RF_EVENT_MAX_CAP_NUM			4
#define RF_EVENT_MAX_TYPE_NUM			5
#define RF_EVENT_MAX_LISTENER_NUM		16

#define RF_EVENT_MASK_ALL				0xFFFFFFFF

#define RF_EVENT_URL_FMT_HDR            "http://%s:%d"
#define RF_EVENT_ROOT_FMT               "/"
#define RF_EVENT_RACK_FMT               "%s/Chassis/Rack/"
#define RF_EVENT_DRAWER_FMT             "%s/Chassis/Drawer%s/"
#define RF_EVENT_MBP_FMT                "%s/Chassis/Rack/MBPs/%s/"
#define RF_EVENT_PZONE_FMT              "%s/Chassis/Rack/PowerZones/%s/"
#define RF_EVENT_PSU_FMT                "%s/Chassis/Rack/PowerZones/%s/PowerZone#/PowerSupplies/%s/"
#define RF_EVENT_TZONE_FMT              "%s/Chassis/Rack/ThermalZones/%s/"
#define RF_EVENT_FAN_FMT                "%s/Chassis/Rack/ThermalZones/%s/ThermalZone#/Fans/%s/"

#define RF_EVENT_SERVICE_STR			"EventService"
#define RF_EVENT_SERVICE_NAME			"Event Service"
#define RF_EVENT_LISTENERS_NAME			"Event Subscriptions Collection"
#define RF_EVENT_LISTENER_NAME			"EventSubscription"
#define RF_EVENT_CONTEXT_STR			"WebUser3"
#define RF_EVENT_PROTOCOL_STR			"Redfish"
#define RF_EVENT_ODATA_CONTEXT_STR		"$metadata#EventService"
#define RF_EVENT_ODATA_TYPE_EVT			"#EventService.1.0.0.EventService"
#define RF_HEALTH_STATUS_OK				"OK"
#define RF_ENABLE_STATUS_EN				"Enabled"
#define RF_SERVICE_ENABLED_FALSE		"False"
#define RF_EVENT_SUBSCRIBE_STR			"/Subscriptions"
#define RF_EVENT_EVT_MEMBER_STR			"/Members/Events/$entity"
#define RF_EVENT_LISTENER_MEMBER_STR	"/Members/Subscriptions/Members/$entity"
#define RF_EVENT_ODATA_TYPE_DEST_COLL	"#EventDestinationCollection.EventDestinationCollection"
#define RF_EVENT_ODATA_TYPE_DEST		"#EventDestination.1.0.0.EventDestination"


#define RF_COPY_RIGHT					"Copyright(c) 2014-2015 Distributed Management Task Force, Inc. (DMTF). All rights reserved."

union ip{
	int8 c[4]; 
	int64 src;
} IP;

/*redfish log define*/
enum RF_EVT {
	RF_MSG_REQ			= 0,
	RF_GET_LOG_BY_LIMIT	= 1,
	RF_SET_TRAP_IP		= 2
};

enum SNMP_TRAP
{
	SNMP_TRAP_ADD 		= 0,
	SNMP_TRAP_REMOVE	= 1
};

struct rf_log_req_info {
	int32		type;
	union {
		struct {
			int32	msg_sn;
			int32	is_event;
			int8	str_args[RF_MSG_MAX_LEN];
			int32	level;
			int8	module_name[MAX_MODULE_NAME_LEN];
			int8	func_name[MAX_FUNC_NAME_LEN];
		} fmt1;
		struct {
			int32 count;
		} fmt2;
		struct {
			int32 is_remove;
			int8 ip[MAX_IP_LEN];
			int32 port;
		} fmt3;
	} data;
};

typedef struct rf_log_desc {
	int32 count;
	int32 length;
} rf_log_dsc_t;

typedef struct rf_msg_info {
	rf_log_dsc_t dsc;
	int8 data[RF_MSG_MAX_LEN * MAX_RF_EVT_MSG_LOG_COUNT];
} rf_msg_info_t;
/*redfish log define end*/

typedef struct listener_dest {
	int32	idx_mask;
	int8	context[RF_CONTEXT_LEN];
	int8	dest[RF_URL_LEN];
	int8	protocol[RF_PROTOCOL_LEN];
	int8    name[RF_NAME_LEN];
	int8    description[RF_DESC_LEN];
} listener_dest_t;

typedef struct evt_source_info {
	int8 prefix_msg[16];					//!< prefix messgae about the msg id str
	int32 node_type;							//!< node id for event source
	int32 is_evt_exist[MAX_EVT_ACTION_NUM];	//!< Initialize action for event source(such as change, add, remove and so on.).
} evt_source_info_t;

typedef struct rf_status {
	int8 state[16];
	int8 health[16];
} rf_status_t;

typedef struct rf_fixed {
	int8 copy_right[256];
	int8 odata_type[64];
	int8 id[32];
	int8 name[32];
	rf_status_t status;
	bool service_enabled;
	int32 retry;
	int32 retry_interval;
} rf_fixed_t;

typedef struct rf_link {
	int8 listeners[RF_EVENT_MAX_LISTENER_NUM][256];
	int8 subscriptions[128];
	int8 odata_context[64];
	int8 odata_id[64];
} rf_link_t;

typedef struct rf_capability {
	int8 event_type[32];
	int32 event_id;
	int8 event_timestamp[128];
	int8 severity[32];
	int8 message[256];
	int32  msg_id;
	int8 msg_args[RF_EVENT_MAX_MSG_ARG_NUM][32];
	int8 origin_cond[64];
} rf_capability_t;

typedef struct rf_av_action {
	int8 action[64];
	rf_capability_t cap[RF_EVENT_MAX_CAP_NUM];
} rf_av_action_t;

typedef struct rf_evt_svc {
	rf_fixed_t      fixed;
	rf_link_t       links;
/*	rf_status_t     status;             */
/*	int             retry_attemp;       */
/*	int             retry_interval;     */
/*	char            removal_action[32]; */
/*	int             removal_interval;   */
	int8            event_type[RF_EVENT_MAX_TYPE_NUM][32];
/*	rf_av_action_t  av_action; */
} rf_evt_svc_t;

typedef struct rf_log_svc {
	rf_fixed_t  fixed;
	int8        self_link[256];
	int32       MaxNumberOfRecords;
	int8        OverWritePolicy[32];
} rf_log_svc_t;

typedef struct id_evt_type {
	int32	node_type;
	int8	type_name[64];
} id_evt_type_t;

#endif
