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


#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "libjson/json.h"
#include "libutils/types.h"
#include "../rest.h"
#include "libutils/upgrade.h"
#include "libutils/sock.h"
#include "libredfish/rf_event.h"
#include "libredfish/mr_def.h"
#include "libutils/rmm_json.h"
#include "libcfg/cfg.h"
#include "libutils/base64.h"


#define MAX_READ_BASE64_SIZE	(64)


#define FIRMWARE_FILE_PATH		TFTP_SERVER_PATH
#define FIRMWARE_FILE_NAME		"F06e.bin"
#define INVAILD_IDX             0xF


#define RF_EVT_URL                       "/EventService"
#define RF_LISTENERS_URL                 "/EventService/Subscriptions"
#define RF_LISTENER_URL                  "/EventService/Subscriptions/{listener_id}"
#define MSG_REG_URL                      "/MessageRegistry"

#define METADATA_URL                     "/Metadata/{metadata_file}"
#define CHASSIS_URL                      "/Chassis"
#define RACK_URL                         "/Chassis/Rack"
#define RACK_DRAWER_RESET_URL            "/Chassis/Rack/Actions/Oem/RackChassis.DrawerReset"
#define LOG_URL                          "/Chassis/Rack/log"
#define RF_LOG_URL                       "/Chassis/Rack/rf_log"
#define RF_LOG_SERVICE_URL               "/Chassis/Rack/rf_logservice"
#define RACK_SNMP_LISTENER_URL           "/Chassis/Rack/snmp_listener"
#define PLATFORM_URL                     "/Chassis/Rack/platform"

#define MBP_COLL_URL                     "/Chassis/Rack/MBPs"
#define MBP_URL                          "/Chassis/Rack/MBPs/{mbp_id}"
#define MBP_ACTION_SET_UART_TARGET_URL   "/Chassis/Rack/MBPs/{mbp_id}/Actions/MBP.SetUartTarget"
#define MBP_ACTION_UPDATE_URL            "/Chassis/Rack/MBPs/{mbp_id}/Actions/MBP.Update"

#define PZONE_COLL_URL                   "/Chassis/Rack/PowerZones"
#define PZONE_URL                        "/Chassis/Rack/PowerZones/{zone_id}"
#define PZONE_POINTER_URL                "/Chassis/Rack/PowerZones/{zone_id}/PowerZone"

//#define PSU_COLL_URL                     "/Rack/powerZoneCollection/{zone_id}/PSUCollection"
//#define PSU_URL                          "/Rack/powerZoneCollection/{zone_id}/PSUCollection/{psu_id}"
#define PSU_ACTION_STATUS_CHANGE_URL     "/Chassis/Rack/PowerZones/{zone_id}/Actions/PowerZone.RequestStateChange"

#define TZONE_COLL_URL                   "/Chassis/Rack/ThermalZones"
#define TZONE_URL                        "/Chassis/Rack/ThermalZones/{zone_id}"
#define TZONE_ACTION_SET_PWM_URL         "/Chassis/Rack/ThermalZones/{zone_id}/Actions/ThermalZone.SetDesiredSpeedPWM"

#define TZONE_POINTER_URL                "/Chassis/Rack/ThermalZones/{zone_id}/ThermalZone"

//#define DRAWER_COLL_URL                  "/Chassis/Drawers"
#define DRAWER1_URL                       "/Chassis/Drawer1"
#define DRAWER2_URL                       "/Chassis/Drawer2"
#define DRAWER3_URL                       "/Chassis/Drawer3"
#define DRAWER4_URL                       "/Chassis/Drawer4"
#define DRAWER5_URL                       "/Chassis/Drawer5"
#define DRAWER6_URL                       "/Chassis/Drawer6"
#define DRAWER7_URL                       "/Chassis/Drawer7"
#define DRAWER8_URL                       "/Chassis/Drawer8"
#define DRAWER9_URL                       "/Chassis/Drawer9"
#define DRAWER10_URL                      "/Chassis/Drawer10"
#define DRAWER11_URL                      "/Chassis/Drawer11"
#define DRAWER12_URL                      "/Chassis/Drawer12"
#define DRAWER13_URL                      "/Chassis/Drawer13"
#define DRAWER14_URL                      "/Chassis/Drawer14"
#define DRAWER15_URL                      "/Chassis/Drawer15"
#define DRAWER16_URL                      "/Chassis/Drawer16"


#define MANAGER_COLL_URL                 "/Managers"
#define MANAGER_RMC_URL                  "/Managers/RMC"
#define MANAGER_MBPC1_URL                "/Managers/MBPC1"
#define MANAGER_MBPC2_URL                "/Managers/MBPC2"
#define MANAGER_NETWORK_SERVICE_URL      "/Managers/RMC/NetworkProtocol"
#define MANAGER_ETHERNET_INF_COLL_URL    "/Managers/RMC/EthernetInterfaces"
#define MANAGER_ETHERNET_INF_URL         "/Managers/RMC/EthernetInterfaces/{inf_id}"
#define MANAGER_VLANS_URL                "/Managers/RMC/EthernetInterfaces/{inf_id}/VLANS"
#define MANAGER_VLAN_URL                 "/Managers/RMC/EthernetInterfaces/{inf_id}/VLANS/{vlan_id}"
#define MANAGER_ACTION_RESET_URL         "/Managers/RMC/Actions/Manager.Reset"

//#define RACK_ACTION_RESET_URL            "/Managers/RMC/Actions/RACK.Reset"
#define RACK_ACTION_UPDATE_URL           "/Managers/RMC/Actions/Oem/RMC.Update"
#define RACK_ACTION_DEFAULT_URL          "/Managers/RMC/Actions/Oem/RMC.LoadFactoryDefault"
#define MBPC1_ACTION_RESET_URL           "/Managers/MBPC1/Actions/Manager.Reset"
#define MBPC2_ACTION_RESET_URL           "/Managers/MBPC2/Actions/Manager.Reset"

extern void register_rack_handler(void);
extern void register_redfish_handler(void);
extern void register_manager_handler(void);
extern void register_mbp_handler(void);
extern void register_drawer_handler(void);
extern void register_pzone_handler(void);
extern void register_psu_handler(void);
extern void register_tzone_handler(void);
extern void register_fan_handler(void);

extern void put_prepare_int(const json_t *elem, uint32 *output, const int8 *name);
extern int32 get_asset_idx(const struct rest_uri_param *param, const int8 *name, int32 type);

extern memdb_integer get_asset_node_id(const struct rest_uri_param *param, const int8 *name);
extern int32 rf_evt_info_init(rf_evt_svc_t *service, int32 *evt_types, const int8 *fmt, ...);
extern int32 rf_evt_pack_json(json_t *result, const int32 *evt_types, rf_evt_svc_t *service);
extern int32 process_listener(json_t *req, listener_dest_t *listener, int32 mask);
extern json_t *process_snmp_listener(json_t *req);
extern memdb_integer get_uniq_node_by_type(int32 type);
extern int32 duplicate_check_string(int8 *resource, int8 *string, int32 count, int32 max_len);
extern int32 evt_listeners_init(evt_listeners_t *listeners, int32 mask, const int8 * fmt, ...);
extern int32 evt_listeners_pack_json(json_t *result, evt_listeners_t *listeners);
extern int32 evt_listener_pack_json(json_t *result, evt_listener_t *listener);
extern int32 evt_listener_init(evt_listener_t *listener, int32 mask, int32 listener_id, const int8 *fmt, ...);
extern int32 subscribe_event(listener_dest_t *listener, int8 *evt_str);
extern int32 unsubscribe_event(listener_dest_t *listener, int8 *evt_str);
extern int32 format_uuid(char *url, int len);

#endif
