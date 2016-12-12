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


#ifndef __LIBWRAP_WRAP_H__
#define __LIBWRAP_WRAP_H__

#include <time.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libutils/list.h"
#include "libutils/utils.h"
#include "libjipmi/common.h"
#include "libmemdb/memdb.h"
#include "libjipmi/libjipmi.h"
#include "libredfish/rf_event.h"
#include "libutils/rmm.h"
#include "libutils/types.h"
#include "liblog/log.h"

#define REST_MAX_ACTION_LEN     64
#define REST_MAX_NODE_LEN       64
#define REST_MAX_ODATA_LEN      256

#define REST_MAX_NAME_LEN       128
#define REST_RACK_PODM_LEN      128
#define REST_RACK_STRING_LEN    64
#define REST_MAX_DESCR_LEN      64
#define REST_TIME_MAX_LEN       64
#define REST_ASSET_TAG_LEN      128
#define REST_PRESENCE_LEN       64

#define REST_EVENT_PROTOCOL     128

#define PSU_CAP_REQ_ST_SUP_NUM  3
#define MAX_REQ_PROCESS         32
#define IPMI_MAX_RESP_LEN       128

#define MAX_PWM_VALUE           100

#define REST_DESC_LEN           128
#define REST_DCUID_LEN          128
#define REST_GEO_TAG_LEN        128
#define REST_PODM_ADDR_LEN      128
#define REST_ASSET_TAG_LEN      128

#define RACK_STRING_LEN         64
#define RACK_TAG_LEN            128
#define DCUID_LEN               128
#define RACK_DCUID_LEN          256
#define UUID_LEN                64
#define RMM_NAME_LEN            32
#define DESCRIPTION_LEN         128
#define MAC_LEN                 6
#define VERSION_LEN             16
#define HREF_URL_LEN            128
#define CURR_PRESENT_LEN        8
#define TRAY_PUID_LEN           16
#define DATE_LEN                64

#define LOWER_POWER_ZONE_ID     1
#define UPPER_POWER_ZONE_IP     2

#define WRAP_RET_OK             0
#define WRAP_RET_TIMEOUT        1
#define WRAP_RET_NULL_NODE      2
#define WRAP_RET_ATTR_ERR       3
#define WRAP_RET_OTHER_ERR      4
#define WRAP_RET_INPUT_ERR      5

#define WRAP_LOC_ID_STR         "loc_id"
#define WRAP_UUID_STR           "uuid"
#define WRAP_MANAGER_UUID_STR   "manager_uuid"
#define WRAP_NAME_STR           "name"
#define WRAP_DESCRIPT_STR       "description"
#define WRAP_PUID_STR           "puid"
#define WRAP_DCUID_STR          "dcuid"
#define WRAP_ONLINE_STATE_STR   "online_state"
#define WRAP_CREATE_DATE_STR    "create_date"
#define WRAP_UPDATE_DATE_STR    "update_date"


#define RACK_LOC_ID_STR        WRAP_LOC_ID_STR
#define RACK_UUID_STR          WRAP_UUID_STR
#define RACK_NAME_STR          WRAP_NAME_STR
#define RACK_DESCRIPT_STR      WRAP_DESCRIPT_STR
#define RACK_CREATE_DATE_STR   WRAP_CREATE_DATE_STR
#define RACK_UPDATE_DATE_STR   WRAP_UPDATE_DATE_STR
#define RACK_PUID_STR          WRAP_PUID_STR
#define RACK_DCUID_STR         WRAP_DCUID_STR
#define RACK_GEOTAG_STR        "geo_tag"
#define RACK_API_VER_STR       "api_ver"
#define RACK_PODM_ADDR_STR     "podm_addr"
#define RACK_ASSET_TAG_STR     "asset_tag"
#define RACK_SER_NUM_STR       "serial_num"
#define RACK_MANUFACT_STR      "manufacture"
#define RACK_MODEL_STR         "rack_model"
#define RACK_PART_NUM_STR      "part_num"
#define RACK_FW_VER_STR        "fw_ver"
#define RACK_AV_RST_1_STR      "rack_av_resettype_1"
#define RACK_AV_RST_2_STR      "rack_av_resettype_2"
#define RACK_AV_UPDATE_1_STR   "rack_av_update_1"
#define RACK_AV_UPDATE_2_STR   "rack_av_update_2"
#define RACK_AV_DEFAULT_STR    "rack_av_default"
#define RACK_HREF_DRAWER_STR   "href_drawer"
#define RACK_HREF_PZONE_STR    "href_pzone"
#define RACK_HREF_TZONE_STR    "href_tzone"
#define RACK_HREF_EVENT_STR    "href_event"
#define RACK_HREF_MBP_STR      "href_mbp"
#define POD_DCUID_STR          "pod_dcuid"
#define RACK_RMM_PRES          "rmm_present"
#define RACK_DISAGGR           "rack_disaggr"
#define RACK_POD               "rack_pod"
#define RACK_ONLINE_STATE_STR  WRAP_ONLINE_STATE_STR

#define DZONE_LOC_ID_STR            WRAP_LOC_ID_STR
#define DZONE_UUID_STR              WRAP_UUID_STR
#define DZONE_NAME_STR              WRAP_NAME_STR
#define DZONE_DESCRIPT_STR          WRAP_DESCRIPT_STR
#define DZONE_CREATE_DATE_STR       WRAP_CREATE_DATE_STR
#define DZONE_UPDATE_DATE_STR       WRAP_UPDATE_DATE_STR
#define DZONE_X_LOC_STR             "x_location"
#define DZONE_U_LOC_STR             "u_location"
#define DZONE_U_HEIGHT_STR          "u_height"
#define DZONE_ONLINE_STATE_STR      WRAP_ONLINE_STATE_STR
#define DZONE_DRAWER_NUM            "drawer_num"

#define DRAWER_LOC_ID_STR              WRAP_LOC_ID_STR
#define DRAWER_UUID_STR                WRAP_UUID_STR
#define DRAWER_NAME_STR                WRAP_NAME_STR
#define DRAWER_DESCRIPT_STR            WRAP_DESCRIPT_STR
#define DRAWER_CREATE_DATE_STR         WRAP_CREATE_DATE_STR
#define DRAWER_UPDATE_DATE_STR         WRAP_UPDATE_DATE_STR
#define DRAWER_TMC_IP_STR              "tmc_ip_addr"
#define DRAWER_TMC_IP_SRC              "tmc_ip_src"
#define DRAWER_TMC_SUBNET_MASK         "tmc_subnet_mask"
#define DRAWER_RUID_STR                "drawer_ruid"
#define DRAWER_PUID_STR                "drawer_puid"
#define DRAWER_X_LOC_STR               "x_location"
#define DRAWER_U_LOC_STR               "u_location"
#define DRAWER_U_HEIGHT_STR            "u_height"
#define DRAWER_PWR_CONSUM_STR          "drawer_pwr_consum"
#define DRAWER_AV_RST_TYPE_1_STR       "drawer_av_resettype_1"
#define DRAWER_AV_RST_TYPE_2_STR       "drawer_av_resettype_2"
#define DRAWER_HEALTH_STR              "drawer_health_status"
#define DRAWER_POD                     "pod"
#define DRAWER_RACK                    "rack"
#define DRAWER_RMM_PRES                "rmm_present"
#define DRAWER_DISAGGR                 "rack_disaggr"
#define DRAWER_PRES_TEMP               "present_temperature"
#define DRAWER_ONLINE_STATE_STR        WRAP_ONLINE_STATE_STR

#define SLED_LOC_ID_STR              WRAP_LOC_ID_STR
#define SLED_UUID_STR                WRAP_UUID_STR
#define SLED_NAME_STR                WRAP_NAME_STR
#define SLED_DESCRIPT_STR            WRAP_DESCRIPT_STR
#define SLED_CREATE_DATE_STR         WRAP_CREATE_DATE_STR
#define SLED_UPDATE_DATE_STR         WRAP_UPDATE_DATE_STR


#define MBP_LOC_ID_STR              WRAP_LOC_ID_STR
#define MBP_UUID_STR                WRAP_UUID_STR
#define MBP_NAME_STR                WRAP_NAME_STR
#define MBP_DESCRIPT_STR            WRAP_DESCRIPT_STR
#define MBP_CREATE_DATE_STR         WRAP_CREATE_DATE_STR
#define MBP_UPDATE_DATE_STR         WRAP_UPDATE_DATE_STR
#define MBP_MBPID_STR               "mbp_mbpid"
#define MBP_HW_ADDR_STR             "mbp_hw_addr"
#define MBP_IP_ADDR_STR             "mbp_ip_addr"
#define MBP_FW_STATE                "mbp_fw_state"
#define MBP_X_LOC_STR               "x_location"
#define MBP_U_LOC_STR               "u_location"
#define MBP_U_HEIGHT_STR            "u_height"
#define MBP_ASSET_TAG_STR           "asset_tag"
#define MBP_SER_NUM_STR             "serial_num"
#define MBP_MANUFACT_STR            "manufacture"
#define MBP_MODEL_STR               "cm_model"
#define MBP_PART_NUM_STR            "part_num"
#define MBP_FW_VER_STR              "fw_ver"
#define MBP_AV_RST_TYPE_1_STR       "mbp_av_resettype_1"
#define MBP_AV_SET_UART_1_STR       "mbp_av_set_uart_1"
#define MBP_AV_UPDATE_1_STR         "mbp_av_update_1"
#define MBP_ONLINE_STATE_STR        WRAP_ONLINE_STATE_STR

#define X_LOC_STR                   "x_location"
#define U_LOC_STR                   "u_location"
#define U_HEIGHT_STR                "u_height"

#define PZONE_LOC_ID_STR            WRAP_LOC_ID_STR
#define PZONE_UUID_STR              WRAP_UUID_STR
#define PZONE_NAME_STR              WRAP_NAME_STR
#define PZONE_DESCRIPT_STR          WRAP_DESCRIPT_STR
#define PZONE_CREATE_DATE_STR       WRAP_CREATE_DATE_STR
#define PZONE_UPDATE_DATE_STR       WRAP_UPDATE_DATE_STR
#define PZONE_X_LOC_STR             X_LOC_STR
#define PZONE_U_LOC_STR             U_LOC_STR
#define PZONE_U_HEIGHT_STR          U_HEIGHT_STR
#define PZONE_TT_PWR_CONSUM_STR     "pzone_tt_pwr_consum"
#define PZONE_TT_PWR_CAP_STR        "pzone_tt_pwr_cap"
#define PZONE_TT_PWR_PROD_STR       "pzone_tt_pwr_prod"
#define PZONE_ONLINE_STATE_STR      WRAP_ONLINE_STATE_STR
#define PZONE_PSU_NUM               "psu_num"

#define PSU_LOC_ID_STR              WRAP_LOC_ID_STR
#define PSU_UUID_STR                WRAP_UUID_STR
#define PSU_NAME_STR                WRAP_NAME_STR
#define PSU_DESCRIPT_STR            WRAP_DESCRIPT_STR
#define PSU_CREATE_DATE_STR         WRAP_CREATE_DATE_STR
#define PSU_UPDATE_DATE_STR         WRAP_UPDATE_DATE_STR
#define PSU_ENABLE_ST_STR           "psu_enable_state"
#define PSU_REDUND_SET_STR          "psu_redundancy_set"
#define PSU_HEALTH_ST_STR           "psu_health_state"
#define PSU_TT_PWR_IN_STR           "psu_tt_pwr_in"
#define PSU_TT_PWR_CAP_STR          "psu_tt_pwr_cap"
#define PSU_TT_PWR_OUT_STR          "psu_tt_pwr_out"
#define PSU_ASSET_TAG_STR           "asset_tag"
#define PSU_SER_NUM_STR             "serial_num"
#define PSU_MANUFACT_STR            "manufacture"
#define PSU_MODEL_STR               "psu_model"
#define PSU_PART_NUM_STR            "part_num"
#define PSU_FW_VER_STR              "fw_ver"
#define PSU_AV_REQ_ST_1_STR         "psu_av_req_st_1"
#define PSU_AV_REQ_ST_2_STR         "psu_av_req_st_2"
#define PSU_AV_REQ_ST_3_STR         "psu_av_req_st_3"
#define PSU_X_LOC_STR               X_LOC_STR
#define PSU_U_LOC_STR               U_LOC_STR
#define PSU_U_HEIGHT_STR            U_HEIGHT_STR
#define PSU_ONLINE_STATE_STR        WRAP_ONLINE_STATE_STR


#define TZONE_LOC_ID_STR            WRAP_LOC_ID_STR
#define TZONE_UUID_STR              WRAP_UUID_STR
#define TZONE_NAME_STR              WRAP_NAME_STR
#define TZONE_DESCRIPT_STR          WRAP_DESCRIPT_STR
#define TZONE_CREATE_DATE_STR       WRAP_CREATE_DATE_STR
#define TZONE_UPDATE_DATE_STR       WRAP_UPDATE_DATE_STR
#define TZONE_X_LOC_STR             "x_location"
#define TZONE_U_LOC_STR             "u_location"
#define TZONE_U_HEIGHT_STR          "u_height"
#define TZONE_POLICY_STR            "tzone_policy"
#define TZONE_ONLINE_STATE_STR      WRAP_ONLINE_STATE_STR
#define TZONE_FAN_NUM               "fan_num"

#define FAN_LOC_ID_STR              WRAP_LOC_ID_STR
#define FAN_UUID_STR                WRAP_UUID_STR
#define FAN_NAME_STR                WRAP_NAME_STR
#define FAN_DESCRIPT_STR            WRAP_DESCRIPT_STR
#define FAN_CREATE_DATE_STR         WRAP_CREATE_DATE_STR
#define FAN_UPDATE_DATE_STR         WRAP_UPDATE_DATE_STR
#define FAN_ENABLE_STATE_STR        "enable_state"
#define FAN_HEALTH_STATE_STR        "health_state"
#define FAN_DESIRED_SPD_PWM_STR     "desired_speed_pwm"
#define FAN_DESIRED_SPD_RPM_STR     "desired_speed_rpm"
#define FAN_SPEED_PWM_RESTORE_STR   "pwm_restore"
#define FAN_TACH_READ_STR           "tach_read"
#define FAN_TACH_STATE_STR          "tach_state"
#define FAN_TACH_THRESH_LNC_STR     "tach_thresh_lnc"
#define FAN_TACH_THRESH_UNC_STR     "tach_thresh_unc"
#define FAN_TACH_THRESH_LC_STR      "tach_thresh_lc"
#define FAN_TACH_THRESH_UC_STR      "tach_thresh_uc"
#define FAN_TACH_PRESENT_STR        "tach_present"
#define FAN_ASSET_TAG_STR           "asset_tag"
#define FAN_SER_NUM_STR             "serial_num"
#define FAN_MANUFACT_STR            "manufacture"
#define FAN_MODEL_STR               "fan_model"
#define FAN_PART_NUM_STR            "part_num"
#define FAN_FW_VER_STR              "fw_ver"
#define FAN_AV_RSC_EN_VAL_1_STR     "rsc_en_val_1"
#define FAN_AV_RSC_EN_VAL_2_STR     "rsc_en_val_2"
#define FAN_AV_SDS_UNIT_1_STR       "sds_unit_1"
#define FAN_AV_SDS_UNIT_2_STR       "sds_unit_2"
#define FAN_AV_SDS_VAL_STR          "sds_val"
#define FAN_PWM_PREFIX_STR          "fan_pwm"
#define FAN_X_LOC_STR               X_LOC_STR
#define FAN_U_LOC_STR               U_LOC_STR
#define FAN_U_HEIGHT_STR            U_HEIGHT_STR
#define FAN_ONLINE_STATE_STR        WRAP_ONLINE_STATE_STR


#define WRAP_DB_MAX_KEY_LEN         32
#define WRAP_DB_MAX_VALUE_LEN       128

#define MAX_DRAWER_PRESENCE	        16
#define MAX_MBP_PRESENCE	        2

#define FAN_THRESH_MAX              20000
#define FAN_STATE_ENABLED           2
#define FAN_STATE_DISABLED          3
#define PSU_STATE_ENABLED           2
#define PSU_STATE_DISABLED          3
#define PSU_STATE_NOT_APP           5
#define PSU_STATE_EN_OFFLINE        6

#define TFTP_SERVER_PATH            "/srv/tftp"

#define RACK_OEM_ODATA_TYPE                         "#Intel.Oem.RmmRackChassis"
#define DRAWER_OEM_ODATA_TYPE 	                    "#Intel.Oem.RmmDrawerChassis"
#define POWER_SUPPLY_OEM_ODATA_TYPE 	            "#RmmPowerZone.PowerSupply"
#define THERMAL_ZONE_OEM_ODATA_TYPE 	            "#Intel.Oem.RmmThermalZone"
#define POWER_ZONE_OEM_ODATA_TYPE 	                "#Intel.Oem.RmmPowerZone"
#define VLAN_NETWORK_INTERFACE_ZONE_OEM_ODATA_TYPE 	"#VLanNetworkInterface.1.0.0.VLanNetworkInterface"

typedef enum {
	ACT_RESET   = 0,
	ACT_UPDATE  = 1,
	ACT_DEFAULT = 2,
	ACT_MAX     = 3,
} action_enum_t;

typedef enum{
	HREF_SYS    = 0,
	HREF_PZONE  = 1,
	HREF_CZONE  = 2,
	HREF_EVENT  = 3,
	HREF_MBP = 4,
	HREF_MAX    = 5,
} href_enum_t;

typedef enum{
	RESULT_OK                = 0,
	RESULT_NO_RESP           = 1,    //socket no response, timeout
	RESULT_NO_NODE           = 2,    //node not exist
	RESULT_ATTR_ERR          = 3,    //attributor name error
	RESULT_IPMI_ERR          = 4,    //IPMI command fail
	RESULT_MALLOC_ERR        = 5,
	RESULT_UNMATCHED_VERSION = 6,
	RESULT_SAME_VERSION      = 7,
	RESULT_LEN_ERR           = 8,
	RESULT_SKIP              = 9,
	RESULT_NONE_POINTER      = 10,
	RESULT_MEMDB_SET_ERR     = 11,
	RESULT_MEMDB_GET_ERR     = 12,
	RESULT_GET_ACTION_ERR    = 13,
	RESULT_GET_COLL_ERR      = 14,
	RESULT_JSON_ARR_ERR      = 15,
	RESULT_SIGNATURE_ERR     = 16,
	RESULT_OTHER_ERR         = 100    //other error
} result_t;

typedef enum{
	REQ_STATE_EN                = 2,    //Enabled
	REQ_STATE_DIS               = 3,    //Disabled
	REQ_STATE_NOT_APP           = 5,    //Not applicable
	REQ_STATE_EN_BUT_OFFLINE    = 6     //Enabled but offline(for redundancy design)
} request_enabled_state_t;

enum {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_ACCEPTED = 202,
	HTTP_NO_CONTENT = 204,
	HTTP_MOVED_PERMANENTLY = 301,
	HTTP_FOUND = 302,
	HTTP_NOT_MODIFIED = 304,
	HTTP_BAD_REQUEST = 400,
	HTTP_FORBIDDEN = 403,
	HTTP_UNAUTHORIZED = 401,
	HTTP_RESOURCE_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_NOT_ACCEPTABLE = 406,
	HTTP_CONFLICT = 409,
	HTTP_GONE = 410,
	HTTP_LENGTH_REQUIRED = 411,
	HTTP_PRECONDITION_FAILED = 412,
	HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_SERVICE_UNAVAILABLE = 503,

};

typedef struct input_attr {
	int8 *attr_name;
	json_t *value;
} input_attr_t;

typedef struct location{
	uint32 units;
	uint32 x_location;
	uint32 u_location;
	uint32 u_height;
} location_t;

typedef struct base_element{
	int     id;
	int8   uuid[UUID_LEN];
	int8   name[RMM_NAME_LEN];
	int8   desc[DESCRIPTION_LEN];
	int8   create_date[DATE_LEN];
	int8   update_date[DATE_LEN];
} base_element_t;

#define MAX_CAPABILITY		3

typedef struct capability{
	int8 property[RMM_NAME_LEN];
	int8 av[REST_RACK_STRING_LEN];
} capability_t;

typedef struct avail_action{
	int8 action[RMM_NAME_LEN];
	capability_t cap[MAX_CAPABILITY];
} avail_action_t;

typedef struct collections{
	uint32  id;
	int8   uuid[UUID_LEN];
	int8   name[RMM_NAME_LEN];
	int8   href[HREF_URL_LEN];
} collections_t;

typedef enum{
	RACK_UNITS_RO  = 0,
	RACK_UNITS_OU  = 1
} rack_units_t;

typedef struct fru_info{
	int8 serial_num[REST_RACK_STRING_LEN];
	int8 manufacture[REST_RACK_STRING_LEN];
	int8 model[REST_RACK_STRING_LEN];
	int8 part_num[REST_RACK_STRING_LEN];
	int8 fw_ver[REST_RACK_STRING_LEN];
} fru_info_t;

typedef struct psu_asset_info{
	int8 asset_tag[REST_ASSET_TAG_LEN];
	fru_info_t fru;
} asset_info_t;

typedef struct href{
	int8 drawer[HREF_URL_LEN];
	int8 powerzones[HREF_URL_LEN];
	int8 thermalzones[HREF_URL_LEN];
	int8 rf_event[HREF_URL_LEN];
	int8 mbps[HREF_URL_LEN];
} href_t;

typedef struct rack_info{
	base_element_t  be;
	int64   rack_puid;
	int8   pod_dcuid[DCUID_LEN];
	int8   rack_dcuid[RACK_DCUID_LEN];
	int8   geo_tag[RACK_TAG_LEN];
	int8   api_ver[VERSION_LEN];
	int8   podm_addr[REST_RACK_PODM_LEN];
	asset_info_t  asset;
	int8 drawer_pres[MAX_DRAWER_PRESENCE + 1];
	int8 mbp_pres[MAX_MBP_PRESENCE + 1];
	avail_action_t av_action[3];
	href_t  href;
	int64 rack_disaggr;
} rack_info_t;

typedef struct put_rack_info{
	int8 descr[REST_DESC_LEN];
	int64 rack_puid;
	int8 pod_dcuid[REST_DCUID_LEN];
	int8 geo_tag[REST_GEO_TAG_LEN];
	int8 podm_addr[REST_PODM_ADDR_LEN];
	int8 asset_tag[REST_ASSET_TAG_LEN];
} put_rack_info_t;

typedef struct put_drawer{
	int8 descr[REST_DESC_LEN];
} put_drawer_t;

typedef struct put_pzone{
	int8 descr[REST_DESC_LEN];
} put_pzone_t;

typedef struct put_psu{
	int8 descr[REST_DESC_LEN];
	int8 asset_tag[REST_ASSET_TAG_LEN];
} put_psu_t;

typedef struct put_tzone{
	int8 descr[REST_DESC_LEN];
} put_tzone_t;

typedef struct put_mbp{
	int8 descr[REST_DESC_LEN];
	int8 asset_tag[REST_ASSET_TAG_LEN];
} put_mbp_t;

typedef struct pzone_member{
	base_element_t  be;
	location_t power_zone_loc;
	uint32 tot_power_consumption;
	uint32 tot_power_cap;
	uint32 tot_power_production;
	int8 presence[REST_PRESENCE_LEN];
} pzone_member_t;

typedef struct psu_member{
	base_element_t  be;
	int32   enable_state;
	int8    redundancy[REST_RACK_STRING_LEN];
	int32   health_state;
	int32   power_in;
	int32   power_cap;
	int32   power_out;
	asset_info_t asset;
	avail_action_t av_action;
	location_t psu_loc;
} psu_member_t;

typedef struct tzone_member{
	base_element_t  be;
	location_t tzone_loc;
	int8 presence[REST_PRESENCE_LEN];
	int8 policy[RACK_STRING_LEN];
	int64 pres_temp;	//inlet temp
	int64 outlet_temp;
	int64 volumetric_airflow;
	int64 max_fan_sup;
	int64 desired_speed_pwm;
	int64 desired_speed_rpm;
} tzone_member_t;

typedef struct threshold{
	uint32 lower_non_critical;
	uint32 upper_non_critical;
	uint32 lower_critical;
	uint32 upper_critical;
} threshold_t;

typedef struct fan_tachometer{
	uint32 reading;
	threshold_t threshold;
} fan_tachometer_t;

typedef struct fan_member{
	base_element_t be;
	uint32  enable_state;
	uint32  health_state;
	fan_tachometer_t tachometer;
	int8 presence[REST_PRESENCE_LEN];
	asset_info_t asset;
	location_t fan_loc;
} fan_member_t;

typedef struct fan_pwm{
	memdb_integer node_id;
	uint32 fan_id;
	uint32 pwm;
} fan_pwm_t;

typedef struct dzone_member{
	base_element_t  be;
	location_t dzone_loc;
	int8 presence[REST_PRESENCE_LEN];
	int64 max_drawer_sup;
} dzone_member_t;

typedef struct drawer_member{
	base_element_t  be;
	int8 TMC_ip_addr[REST_RACK_STRING_LEN];
	uint32  tray_ruid;
	int8  tray_puid[REST_RACK_STRING_LEN];
	location_t loc;
	uint32  power_consumption;
	uint32  pres_temp;
	int8   pod[REST_RACK_STRING_LEN];
	int8   rack[REST_RACK_STRING_LEN];
	avail_action_t av_action;
	int8   href[HREF_URL_LEN];
} drawer_member_t;

typedef struct mbp_member{
	base_element_t  be;
	int8 MBPID[REST_RACK_STRING_LEN];
	int8 MBPHWAddress[REST_RACK_STRING_LEN];
	int8 MBPIPAddress[REST_RACK_STRING_LEN];
	int8 fw_state[REST_RACK_STRING_LEN];
	location_t loc;
	asset_info_t   asset;
	avail_action_t av_action[3];
} mbp_member_t;

typedef struct put_fan{
	int8   descr[DESCRIPTION_LEN];
	threshold_t threshold;
	int8 asset_tag[REST_ASSET_TAG_LEN];
} put_fan_t;

struct wrap_cb {
	int8 resp;
	int8  data[IPMI_MAX_RESP_LEN];
} wrap_cb_t;

struct wrap_ipmi {
	struct list_head  list;
	unsigned int host;
	struct jipmi_msg msg;
	ipmi_rsp_callback_fn cb_fn;
	void *cb_arg;
} wrap_ipmi_t;

typedef struct wrap_msg {
	struct list_head wrap_head;
	int curr_process;
} wrap_msg_t;

typedef struct id_tbl {
	int32 pid;
	int32 idx;
} id_tbl_t;

typedef result_t (*memdb_get_fn) (int8* name, int8* value, int32 size, va_list* valist);

typedef struct memdb_tbl {
	int8   key[WRAP_DB_MAX_KEY_LEN];
	uint8 snapshot_flag;
	int8   reset_flag;
	memdb_get_fn get_fn;
	int8   value[WRAP_DB_MAX_VALUE_LEN];
} memdb_tbl_t;

typedef struct json_map_tbl {
	int8  name[32];
	int8  submenu[32];
} json_map_tbl_t;

#define COPY_RIGHT_LEN   256
#define CONTEXT_LEN      256
#define DESCRIPTION_LEN  128
#define ODATA_ID_LEN     128
#define ODATA_TYPE_LEN   128
#define ID_LEN           128
#define NAME_LEN         128
#define TYPES_NUM        100
#define PROTOCOL_LEN     128
#define URL_LEN          256
#define MAC_ADDR_LEN     32
#define IPV4_ADDR_LEN    32
#define IPV6_ADDR_LEN    64
#define MAX_NAME_SERVER  16
#define MAX_INF_NUM      16

typedef struct listener {
	int8  url[MAX_URL];
	int8  context[CONTEXT_LEN];
	int8  protocol[PROTOCOL_LEN];
	int8  name[NAME_LEN];
	int8  description[DESCRIPTION_LEN];
	int32   event_types[TYPES_NUM];
	int32   evt_index;
	int32   listener_idx;
	struct listener *pnext;
} listener_t;

typedef struct listener_base {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 id[ID_LEN];
	int8 name[NAME_LEN];
} listener_base_t;

typedef struct evt_listeners {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 name[NAME_LEN];
	int64 num;
	int8 url[RF_EVENT_MAX_LISTENER_NUM][URL_LEN];
} evt_listeners_t;

typedef struct evt_listener {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 id[ID_LEN];
	int8 name[NAME_LEN];
	int8 description[NAME_LEN];
	int8 url[URL_LEN];
	int32  event_types[TYPES_NUM];
	int32  evt_index;
	int8 context[CONTEXT_LEN];
	int8 protocol[PROTOCOL_LEN];
} evt_listener_t;

struct rest_param_key {
	int8 *name;
	int8 *value;
};

typedef struct rest_uri_param {
	int32 httpmethod;
	int64 content_length;
	int32 fd;
	int32 num_path_keys;
	int32 num_query_keys;
	int8 *json_data;
	uint32 json_data_sz;
	int32 num_nodes;
	struct rest_param_key path_keys[MAX_KEYS_NUM];
	struct rest_param_key query_keys[MAX_KEYS_NUM];
	int8 *nodes[MAX_URI_NODES];
	int8 *host;
	int32 status;
	int8 url[HREF_URL_LEN];
	int8 https_url[HREF_URL_LEN];
	int8 rsp[HREF_URL_LEN];
} rest_uri_param_t;

typedef struct mngr_ipv4_addr {
	int8 address[IPV4_ADDR_LEN];
	int8 subnet_mask[IPV4_ADDR_LEN];
	int8 addr_origin[NAME_LEN];
	int8 gateway[IPV4_ADDR_LEN];
} mngr_ipv4_addr_t;

typedef struct mngr_ipv6_policy {
	int8 prefix[NAME_LEN];
	int32 precedence;
	int32 label;
} mngr_ipv6_policy_t;

typedef struct mngr_ipv6_addr {
	int8 address[IPV6_ADDR_LEN];
	int32 prefix_len;
	int8 addr_origin[NAME_LEN];
	int8 addr_state[NAME_LEN];
} mngr_ipv6_addr_t;

typedef struct mngr_eth_inf {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 id[ID_LEN];
	int8 name[NAME_LEN];
	int8 description[DESCRIPTION_LEN];
	int32 status;
	int8 factory_mac[MAC_ADDR_LEN];
	int8 mac_addr[MAC_ADDR_LEN];
	int8 link_tech[NAME_LEN];
	int32 speed_mbps;
	bool auto_sense;
	bool full_duplex;
	int32 frame_size;
	int8 hostname[NAME_LEN];
	int8 fqdn[NAME_LEN];
	int32 max_ipv6_static_num;
	bool vlan_enable;
	int32 vlan_id;
	mngr_ipv4_addr_t ipv4_addr;
	mngr_ipv6_policy_t ipv6_policy;
	mngr_ipv6_addr_t ipv6_static;
	mngr_ipv6_addr_t ipv6_addr;
	int8 ipv6_gateway[IPV6_ADDR_LEN];
	int8 name_servers[MAX_NAME_SERVER][NAME_LEN];
	int8 vlans[URL_LEN];
} mngr_eth_inf_t;


typedef struct mngr_eth_inf_coll {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 id[ID_LEN];
	int8 name[NAME_LEN];
	int8 description[DESCRIPTION_LEN];
	int32 inf_num;
} mngr_eth_inf_coll_t;

typedef struct mngr_eth_inf_list {
	int32 inf_id;
	int8  ifname[NAME_LEN];
	int32 vlan_num;
	int32 vlan_id[MAX_INF_NUM];
} mngr_eth_inf_list_t;

typedef struct mngr_eth_inf_id_map {
	int32 inf_num;
	mngr_eth_inf_list_t  list[MAX_INF_NUM];
} mngr_eth_inf_id_map_t;


typedef struct mngr_network_property {
	bool enabled;
	int32 port;
} mngr_network_property_t;

typedef struct mngr_network_srv {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 id[ID_LEN];
	int8 name[NAME_LEN];
	int8 description[DESCRIPTION_LEN];
	int32 status;
	int8 hostname[NAME_LEN];
	int8 fqdn[NAME_LEN];
	mngr_network_property_t http;
	mngr_network_property_t https;
	mngr_network_property_t ssh;
} mngr_network_srv_t;

typedef struct mngr_vlans {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 name[NAME_LEN];
	int64 num;
	int8 url[MAX_INF_NUM][URL_LEN];
}mngr_vlans_t;

typedef struct vlan_oem {
	int8 odata_type[ODATA_TYPE_LEN];
	bool tagged;
}vlan_oem_t;

typedef struct mngr_vlan {
	int8 copy_right[COPY_RIGHT_LEN];
	int8 odata_context[CONTEXT_LEN];
	int8 odata_id[ODATA_ID_LEN];
	int8 odata_type[ODATA_TYPE_LEN];
	int8 name[NAME_LEN];
	int8 id[ID_LEN];
	bool enable;
	int32 vlan_id;
	int8 description[DESCRIPTION_LEN];
	vlan_oem_t oem;
}mngr_vlan_t;



extern result_t libwrap_get_mbp_hw_addr(uint32 pos, int8 *hw_addr);

extern result_t libwrap_get_node_id_by_uuid(int8 *uuid, memdb_integer *node_id);

extern int32 pack_basic_element_info(const base_element_t *be, json_t *result);

extern int32 pack_oem_element_info(const base_element_t *be, json_t *result);

extern int32 pack_oem_loc_info(const location_t *loc_t, json_t *result);

extern int32 pack_fru_info(const fru_info_t *fru_info, json_t *fru);

extern void update_href_host(int8 *href, int32 len, int8 *host);

extern result_t libwrap_pack_rack_info_to_json(json_t *output, rack_info_t *rack_info, rest_uri_param_t *param);

extern result_t libwrap_get_rack(rack_info_t *rack_info);

extern result_t libwrap_update_put_rack_info(json_t *req, put_rack_info_t *put_info);

extern result_t libwrap_update_rack_mtime();

extern int32 put_prepare_str(const json_t *req, int8 *output, int32 len, const int8 *name);

extern int32 prepare_get_action(const avail_action_t *av_action, json_t *action, int32 index);

extern result_t libwrap_put_rack(const put_rack_info_t put_rack_info);

extern result_t libwrap_get_pzone_coll(collections_t *pzone, uint32 *number, int8 *host);

extern result_t libwrap_get_pzone_by_idx(uint32 pzone_idx, pzone_member_t *pzone_member);

extern result_t libwrap_get_pzone_psu_coll(uint32 pzone_idx, collections_t *psu, uint32 *number, int8 *host);

extern result_t libwrap_get_pzone_psu_by_idx(const uint32 pzone_idx, const uint32 psu_idx,  psu_member_t * psu_member);

extern result_t libwrap_post_pzone_psu_by_idx(uint32 pzone_idx, uint32 psu_idx, int32 state);

extern result_t libwrap_get_tzone_coll(collections_t *tzone, uint32 *number, int8 *host);

extern result_t libwrap_get_tzone_by_idx(uint32 tzone_idx, tzone_member_t *tz_member);

extern result_t libwrap_get_tzone_fan_coll(uint32 tzone_idx, collections_t *fan, uint32 *number, int8 *host);

extern result_t libwrap_get_tzone_fan_by_idx(uint32 tzone_idx, uint32 fan_idx, fan_member_t *fan_member);

extern result_t libwrap_get_dzone_coll(collections_t *dzone, uint32 *number, int8 *host);

extern result_t libwrap_get_dzone_by_idx(uint32 dzone_idx, dzone_member_t *dz_member);

extern result_t libwrap_get_drawer_coll(collections_t *drawer, uint32 *number, int8 *host);

extern result_t libwrap_set_memdb_rack_puid();

extern result_t libwrap_get_drawer_by_idx(int32 dzone_idx, int32 drawer_idx, drawer_member_t *drawer_member, int8 *host);

extern result_t libwrap_get_log(int32 begin, int32 count, void* info);

extern result_t libwrap_put_drawer(int32 dzone_idx, int32 drawer_idx, const put_drawer_t put_drawer_info);

extern result_t libwrap_check_update_capability();

extern void wrap_init(void);

extern result_t libwrap_put_fan(uint32 tzone_id, uint32 fan_idx, const put_fan_t put_fan_info);

extern result_t libwrap_get_mbp_by_idx(int32 id, mbp_member_t *mbp_member);

extern result_t libwrap_put_mbp(int32 id, const put_mbp_t put_mbp_info);

extern result_t libwrap_get_mbp_coll(collections_t* mbp, uint32 *number, int8 *host);

extern result_t libwrap_get_state_str(int32 state, int8 *output, int32 len);

extern memdb_integer get_subnode_id_by_lid(uint32 lid, memdb_integer node_id, int32 type);

extern result_t libwrap_put_psu(uint32 pzone_idx, uint32 psu_idx, put_psu_t *put_info);

extern result_t libwrap_put_pzone_by_idx(int32 idx, const put_pzone_t put_pzone_info);

extern int get_zone_lid(int32 zone_type, int32 zone_idx);

extern int get_cm_lid(int32 zone_type, int32 zone_idx);

extern int libwrap_get_asset_num(int32 zone_type);

extern result_t libwrap_put_tzone_by_idx(int32 idx, const put_tzone_t put_tzone_info);

extern result_t libwrap_get_psu_node_id_by_idx(int32 pzone_idx, int32 psu_idx, memdb_integer *node_id);

extern result_t libwrap_post_fan_change_state(uint32 tzone_idx, uint32 fan_idx, int64 enabled_state);

extern result_t libwrap_post_fan_set_speed(uint32 tzone_idx, uint32 type, int64 speed);

extern int32 libwrap_set_tzone_pwm(memdb_integer tz_node_id, int64 pwm);

extern result_t libwrap_drawer_hard_reset(int32 dzone_idx, int32 drawer_idx, int32 *result);

extern result_t libwrap_psme_soft_reset(int32 dzone_idx, int32 drawer_idx, int32 *result);

extern result_t libwrap_drawer_soft_reset(int32 dzone_idx, int32 drawer_idx, int32 *result);

extern result_t libwrap_mbp_soft_reset(int32 idx, int32 *result);

extern result_t libwrap_mbp_hard_reset(int32 idx, int32 *result);

#ifdef PLATFORM_BDC_R
extern result_t libwrap_mbp_uart_switch(int32 cm_idx, int32 tray_idx, int32 tgt_idx);
#else
extern result_t libwrap_mbp_uart_switch(int32 cm_idx, int32 tgt_idx);
#endif

extern result_t libwrap_rmm_factory_reset(int32 *result);

extern result_t libwrap_pre_put_rack(put_rack_info_t *put_rack_info);

extern result_t libwrap_pre_put_mbp(int32 id, put_mbp_t *put_mbp_info);

extern result_t libwrap_pre_put_pzone_by_idx(int32 idx, put_pzone_t *put_pzone_info);

extern result_t libwrap_pre_put_drawer(uint32 dzone_idx, uint32 drawer_idx, put_drawer_t *put_sys_info);

extern result_t libwrap_pre_put_psu(uint32 pzone_idx, uint32 psu_idx, put_psu_t *put_info);

extern result_t libwrap_pre_put_tzone_by_idx(int32 idx, put_tzone_t *put_tzone_info);

extern result_t libwrap_pre_put_fan(uint32 tzone_idx, uint32 fan_idx, put_fan_t *put_fan_info);

extern result_t libwrap_update_mbp_firmware(int32 idx, int8 *dev, int8 *file);

extern result_t libwrap_check_tftp_service(void);

extern int libwrap_get_firmware_update_status(void);

extern void libwrap_set_firmware_update_status(int32 status);

extern int libwrap_get_rmm_update_status(void);

extern void libwrap_set_rmm_update_status(int32 status);

extern result_t libwrap_get_evt_listeners_count(int32 mask, int32 *count);

extern result_t libwrap_get_evt_listener(int32 mask, evt_listener_t *listener, int32 listener_idx);

extern result_t libwrap_del_evt_listener(int32 mask, int32 listener_idx);

extern int32 libwrap_get_evt_listener_idx(int32 mask, int8 *url);

extern result_t libwrap_evt_subscribe(listener_dest_t *listener, memdb_integer listener_nid, memdb_integer evt_nid);

extern result_t libwrap_evt_unsubscribe(listener_dest_t *listener, memdb_integer listener_nid);

extern memdb_integer libwrap_get_uniq_node_by_type(int32 type);

extern memdb_integer libwrap_get_listener(int8 *dest, memdb_integer *evt_nid, int32 type);

extern memdb_integer libwrap_get_subnode_id_by_type(memdb_integer parent, int32 type);

extern memdb_integer libwrap_get_listener_id_by_dest(int8 *dest, memdb_integer parent, int32 type);

extern result_t libwrap_get_listener_links(int32 reg, memdb_integer nid, rf_link_t *links);

extern result_t libwrap_connect_asset_module();

extern int get_db_info_string(uint8 db_name, memdb_integer node_id, int8* name, int8* output, uint32 len);

extern result_t libwrap_pack_pzone_coll_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_pzone_to_json(json_t *output, pzone_member_t *pzone_member, rest_uri_param_t *param);

extern void add_loc_info(json_t *loc, int32 input, const int8 *name);

extern result_t libwrap_update_put_pzone_info(json_t *req, put_pzone_t *put_info);

extern result_t libwrap_update_put_tzone_info(json_t *req, put_tzone_t *put_info);

extern result_t libwrap_pack_psu_coll_to_json(json_t *output, uint32 pzone_idx, rest_uri_param_t *param);

extern result_t libwrap_update_put_psu_info(json_t *req, put_psu_t *put_info, int32 pzone_idx, int32 psu_idx);

extern result_t libwrap_pack_tzone_to_json(json_t *output, tzone_member_t *tzone_member, rest_uri_param_t *param);

extern result_t libwrap_pack_tzone_coll_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_fan_coll_to_json(json_t *output, uint32 tzone_idx, rest_uri_param_t *param);

extern result_t libwrap_pack_mbp_coll_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_drawer_coll_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_drawer_to_json(json_t *output, drawer_member_t *drawer_member, rest_uri_param_t *param);

extern result_t libwrap_pack_fan_to_json(json_t *output, fan_member_t *fan_member, const rest_uri_param_t *param);

extern result_t libwrap_pack_mbp_to_json(json_t *output, mbp_member_t *mbp_member, rest_uri_param_t *param);

extern result_t libwrap_update_put_mbp_info(json_t *req, put_mbp_t *put_info);

extern result_t libwrap_update_put_drawer_info(json_t *req, put_drawer_t *put_info, int32 dzone_idx, int32 drawer_idx);

extern result_t libwrap_pack_psu_to_json(json_t *output, psu_member_t *psu_member, const rest_uri_param_t *param);

extern result_t libwrap_pack_service_root_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_chassis_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_manager_coll_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_rmc_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_pack_mbpc_to_json(json_t *output, rest_uri_param_t *param);

extern result_t libwrap_manager_eth_inf_init(mngr_eth_inf_t *eth_inf, int32 inf_id);

extern result_t libwrap_pack_manager_eth_inf_json(json_t *result, mngr_eth_inf_t *eth_inf);

extern result_t libwrap_manager_network_srv_init(mngr_network_srv_t *network_srv);

extern result_t libwrap_pack_manager_network_srv_json(json_t *result, mngr_network_srv_t *network_srv);

extern result_t libwrap_pack_manager_vlans_json(json_t *result, mngr_vlans_t *vlans);

extern result_t libwrap_manager_vlans_init(mngr_vlans_t *vlans, int32 inf_id);

extern result_t libwrap_pack_manager_vlan_json(json_t *result, mngr_vlan_t *vlan);

extern result_t libwrap_manager_vlan_init(mngr_vlan_t *vlan, int32 inf_id, int32 vlan_idx);

extern result_t libwrap_pack_manager_eth_inf_coll_json(json_t *result, mngr_eth_inf_coll_t *eth_inf_coll);

extern result_t libwrap_manager_eth_inf_coll_init(mngr_eth_inf_coll_t *eth_inf_coll);

extern void update_response_info(rest_uri_param_t *param, int32 status);

extern result_t get_tzone_node_id(int32 tzone_idx, memdb_integer *tzone_node_id);

extern int32 get_rf_evt_type(int8 *input);

extern json_t *libwrap_get_attr_json(input_attr_t *input_attrs, uint32 size, int8 *name);

extern int32 libwrap_check_input_attrs(input_attr_t *input_attrs, uint32 size, json_t *req, int8 *prefix);

#endif
