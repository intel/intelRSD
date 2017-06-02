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


#ifndef __LIBCFG_CFG_H__
#define __LIBCFG_CFG_H__

#include "platform.h"

#define PROC_RMM                                "RMM"
#define PROC_RESTD                              "Restd"
#define PROC_MEMDBD                             "Memdbd"
#define PROC_IPMI_MODULE                        "IPMIModule"
#define PROC_RMM_LOGD                           "RMMLogd"
#define PROC_UPGRADED                           "Upgraded"
#define PROC_DB_SYNCD                           "DBSyncd"
#define PROC_REDFISHD                           "Redfishd"
#define PROC_SNMP_SUBAGENTD                     "SNMPSubagentd"
#define PROC_REGISTERD                          "Registerd"
#define PROC_ASSETD                             "Assetd"
#define PROC_ASSET_MODULE                       "AssetModule"
#define PROC_SENSORD                            "Sensord"
#define PROC_RMM_KEEPERD                        "RMMKeeperd"

#define PROC_AUTO_TEST                          "auto_test"
#define ATTR_VM_HEADER                          "vm"
#define ATTR_VM_ROOT_PASSWORD                   "root_password"

#define ATTR_PORT                               "Port"
#define ATTR_JRPC_PORT                          "JrpcPort"
#define ATTR_LOG_MODULES                        "LogModules"
#define ATTR_TTY_INFOS                          "TTYInfos"
#define ATTR_MAX_LINES                          "MaxNumberOfRecords"
#define ATTR_LOG_LEVEL                          "Level"
#define ATTR_RMCP_USERNAME                      "UserName"
#define ATTR_RMCP_PASSWORD                      "Password"
#define ATTR_SYNC_DB_VER_PORT                   "SyncDbVerPort"
#define ATTR_KEEPER_DDB_PORT                    "KeeperDdbPort"
#define ATTR_SNAPSHOT_SYNC_PORT                 "SnapshotSyncPort"
#define ATTR_REST_PREFIX                        "Prefix"

#define ATTR_RACK_PLATFORM                      "Platform"
#define ATTR_RACK_PLATFORM_BDCA                 "BDC-A"
#define ATTR_RACK_PLATFORM_BDCR                 "BDC-R"

#define ATTR_IPMID_SERIAL_BAUDRATE              "SerialBaudRate"
#define ATTR_IPMID_SERIAL_DATABITS              "SerialDataBits"
#define ATTR_IPMID_SERIAL_STOPBITS              "SerialStopBits"
#define ATTR_IPMID_SERIAL_PARITY                "SerialParity"
#define ATTR_IPMID_SERIAL_FLOWCONTROL           "SerialFlowControl"

#define ATTR_IPMI_RMCP_CLIENT_PORT              "RMCPClientPort"
#define ATTR_IPMI_IPMB_UDP_PORT                 "IPMBPort"
#define ATTR_IPMI_I2CD_UDP_PORT                 "I2cdPort"
#define ATTR_IPMI_LISTEN_PORT                   "IPMIListenPort"
#define ATTR_IPMI_COOLING_CTRL_PORT             "IPMICoolingCtrlPort"
#define ATTR_IPMI_POWER_MGMT_PORT               "IPMIPowerMgmtPort"
#define ATTR_IPMI_ASSET_MODULE_PORT             "IPMIAssetModulePort"
#define ATTR_IPMI_SENSOR_MONITOR_PORT           "IPMISensorMonitorPort"
#define ATTR_IPMI_TEST_PORT                     "IPMITestPort"
#define ATTR_IPMI_UART_TGT_CFG_PORT             "IPMIUartTgtCfgPort"
#define ATTR_IPMI_WEBSOCKET_PORT                "IPMIWebSocketPort"
#define ATTR_IPMI_JSON_RPC_SERVER_PORT          "IPMIJRpcServerPort"
#define ATTR_IPMI_JSON_RPC_IPMID_PORT           "IPMIJRpcIPMIdPort"
#define ATTR_IPMI_JSON_RPC_SNMP_SUBAGNETD_PORT  "IPMIJRpcSnmpSubAgentdPort"

#define RMM_LOG_MODULE_NAME_LEN 	128
#define MAX_LOG_MODULES_CNT 		11
#define RMM_TTY_INFO_LEN			128 
#define MAX_TTY_INFOS_CNT   		8
#define MAX_VM_PASSWORD_LEN			16

struct rmm_log_module {
	char name[RMM_LOG_MODULE_NAME_LEN];
	int level;
	int max_lines;
};

enum {
	ASSETD_PORT = 0,
	RESTD_PORT,
	MEMDBD_PORT,
	IPMIMODULE_PORT,
	REDFISHD_PORT,
	SNMPAGENTD_PORT,
	SENSORD_PORT,
	UPGRADED_PORT,
	LOGD_PORT,
	DBSYNCD_PORT,
	RMMKEEPERD_PORT,
	SYNCDB_VER_PORT,
	KEEPERDB_PORT,
	SNAPSHOT_SYNC_PORT,
	IPMIRMCPCLIENT_PORT,
	IPMIIPMB_PORT,
	IPMII2CD_PORT,
	IPMI_LISTEN_PORT,
	IPMICOOLINGCTRL_PORT,
	IPMIPOWERMGMT_PORT,
	IPMIASSETMODULE_PORT,
	IPMISENSORD_PORT,
	IPMITEST_PORT,
	IPMIUARTTGTCFG_PORT,
	IPMIWEBSOCKET_PORT,
	IPMIJSONRPC_SERVER_PORT,
	IPMIJSONRPC_IPMID_PORT,
	IPMIJSONRPC_SNMP_SUBAGENT_PORT,
	ASSETMGR_JSONRPC_PORT,
	REGISTERD_PORT,
	REGISTERD_JSONRPC_PORT,
	ASSET_MODULE_PORT,
};

/**
 * @brief rmm config get tty info
 *
 * @param  infos 			information of tty
 * @param  count 			tty count in config file.
 */
int rmm_cfg_get_tty_infos(char *infos, int *count);

/**
 * @brief get rmm log modules
 *
 * @param  modules 			log modules.
 * @param  count 			The count of log modules.
 */
int rmm_cfg_get_log_modules(struct rmm_log_module *modules, int *count);

int rmm_cfg_get_port(int port_attr);

int rmm_cfg_get_baudrate();
unsigned char rmm_cfg_get_databits(void);
unsigned char rmm_cfg_get_stopbits(void);
unsigned char rmm_cfg_get_parity(void);
unsigned char rmm_cfg_get_flow_control(void);

int rmm_cfg_get_rmcp_username(char *username, int max_len);
int rmm_cfg_get_rmcp_password(char *password, int max_len);

int rmm_cfg_get_vm_root_password(char *password, int max_len, int vm_idx);
int rmm_cfg_get_rest_prefix(char *prefix, int max_len);

int rmm_cfg_get_platform(char *platform, int max_len);
int is_platform_bdcr(void);

#endif
