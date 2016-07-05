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


#include <stdlib.h>
#include <stdio.h>
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libwrap/wrap.h"
#include "libmemdb/memdb.h"
#include "liblog/log.h"
#include "libcfg/cfg.h"
#include "libjsonrpc/jsonrpc.h"
#include "libredfish/mr_def.h"
#include "libutils/rack.h"
#include "libutils/rmm.h"
#include "libutils/utils.h"
#include "libutils/rmm_json.h"
#include "libsecurec/safe_lib.h"
#include "handler.h"
#include "attribute.h"
#include "map.h"

struct reset_param {
	int host;
	int cm_idx;
};

struct _mbp_hw_info {
	unsigned char hw_addr[8];
};

static struct _mbp_hw_info mbp_hw_info[MAX_CM_NUM] = {
	{"usb:2-3"},
	{"usb:2-4"},
};

typedef int (*cmd_handle_fn) (jrpc_req_pkg_t *req, json_t *resp);
typedef int (*handle_fn_add) (char *uuid, char *parent_uuid, json_t *req, json_t *resp);
typedef int (*handle_fn_change) (char *uuid, json_t *req, json_t *resp);

static int on_add(jrpc_req_pkg_t *req, json_t *resp);
static int on_remove(jrpc_req_pkg_t *req, json_t *resp);
static int on_change(jrpc_req_pkg_t *req, json_t *resp);

#if 0
static int set_id_field(jrpc_req_pkg_t *req, json_t *resp);
static int get_id_field(jrpc_req_pkg_t *req, json_t *resp);
static int get_thermal_sensor(jrpc_req_pkg_t *req, json_t *resp);
static int get_health_sensor(jrpc_req_pkg_t *req, json_t *resp);
static int get_tray_power(jrpc_req_pkg_t *req, json_t *resp);

static int get_fan_pwm_by_id(jrpc_req_pkg_t *req, json_t *resp);
static int set_fan_pwm(jrpc_req_pkg_t *req, json_t *resp);
static int post_psu_by_node_id(jrpc_req_pkg_t *req, json_t *resp);
static int peripheral_hard_reset(jrpc_req_pkg_t *req, json_t *resp);
#endif
static int rmm_factory_reset(jrpc_req_pkg_t *req, json_t *resp);

static cmd_handle_fn cmd_handles[MAX_EVT] = {
	[ADD_EVT]				= on_add,
	[REMOVE_EVT]			= on_remove,
	[CHANGE_EVT]			= on_change,
	#if 0
	[SET_ID_FIELD]			= set_id_field,
	[GET_ID_FIELD]			= get_id_field,
	[GET_THERMAL_SENSOR]	= get_thermal_sensor,
	[GET_HEALTH_SENSOR]		= get_health_sensor,
	[GET_TRAY_POWER]		= get_tray_power,
	[GET_FAN_PWM_BY_ID]		= get_fan_pwm_by_id,
	[SET_FAN_PWM]			= set_fan_pwm,
	[RESET_HARD_RESET]		= peripheral_hard_reset,
	[POST_PSU_BY_NODE_ID]	= post_psu_by_node_id,
	#endif
	[RMM_FACTORY_RESET]		= rmm_factory_reset
};

typedef struct cmd_func_map {
	int type;
	handle_fn_add		on_add;
	handle_fn_change	on_change;
	/* data */
} cmd_func_map_t;

static int32 get_global_idx(memdb_integer zone_nid, int32 zone_lid, int32 max_zone_num, int8 *output, int32 len)
{
	int32 rc = 0;
	int8 lid_str[128] = {};
	int32 cm_lid = 0;
	int32 global_lid = 0;
	struct node_info *zone_node = NULL;

	zone_node = libdb_get_node_by_node_id(DB_RMM, zone_nid, LOCK_ID_NULL);
	if (zone_node == NULL) {
		rmm_log(ERROR, "get zone node fail\n");
		return -1;
	}

	memset(lid_str, 0, sizeof(lid_str));
	rc = libdb_attr_get_string(DB_RMM, zone_node->parent, WRAP_LOC_ID_STR, lid_str, sizeof(lid_str), LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "get cm lid fail\n");
		return -1;
	}
	cm_lid = str2int(lid_str);

	/* get zone loc id from memdb if not provided */
	if (zone_lid == 0) {
		rc = libdb_attr_get_string(DB_RMM, zone_nid, WRAP_LOC_ID_STR, lid_str, sizeof(lid_str), LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "get zone lid fail\n");
			return -1;
		}
		zone_lid = str2int(lid_str);
	}

	global_lid = (cm_lid - 1) * max_zone_num + zone_lid;
	memset(output, 0, len);
	snprintf_s_i(output, sizeof(len), "%d", global_lid);

	return 0;
}

static void remove_node(memdb_integer node_id, memdb_integer parent, memdb_integer type)
{
	int32 msg_sn = 0;
	int32 resource_sn = 0;
	int32 rc = 0;
	int8 uuid[64] = {0};
	int8 lid_str[64] = {0};
	int32 max_zone_num = 0;
	int8 zone_lid[64] = {0};

	switch (type) {
	case MC_TYPE_CM:
		msg_sn = MSGMBPResourceRemoved;
		break;
	case MC_TYPE_PZONE:
		msg_sn = MSGPZoneResourceRemoved;
		max_zone_num = MAX_PZONE_NUM;
		break;
	case MC_TYPE_PSU:
		msg_sn = MSGPSUResourceRemoved;
		max_zone_num = MAX_PZONE_NUM;
		break;
	case MC_TYPE_TZONE:
		msg_sn = MSGTZoneResourceRemoved;
		max_zone_num = MAX_TZONE_NUM;
		break;
	case MC_TYPE_FAN:
		msg_sn = MSGFanResourceRemoved;
		max_zone_num = MAX_TZONE_NUM;
		break;
	case MC_TYPE_DRAWER:
		msg_sn = MSGDrawerResourceRemoved;
		max_zone_num = MAX_DZONE_NUM;
		break;
	default:
		break;
	}

	rc = libdb_attr_get_string(DB_RMM, node_id, WRAP_UUID_STR, uuid, sizeof(uuid), LOCK_ID_NULL);
	if (rc == -1 || strnlen_s(uuid, sizeof(uuid)-1) == 0) {
		rmm_log(ERROR, "memdb get uuid fail\n");
		return;
	}
	nmap_remove_by_node_id(node_id);

	if (msg_sn != 0) {
		if (msg_sn == MSGPSUResourceRemoved|| msg_sn == MSGFanResourceRemoved) {
			get_global_idx(parent, 0, max_zone_num, zone_lid, sizeof(zone_lid));

			rc = libdb_attr_get_string(DB_RMM, node_id, WRAP_LOC_ID_STR, lid_str, sizeof(lid_str), LOCK_ID_NULL);
			if (rc == -1 || strnlen_s(lid_str, sizeof(lid_str)-1) == 0) {
				rmm_log(ERROR, "memdb get location id fail\n");
				return;
			}

			rf_snmp_evt(INFO, msg_sn, zone_lid, lid_str);
		} else if (msg_sn == MSGTZoneResourceRemoved|| msg_sn == MSGPZoneResourceRemoved) {
			get_global_idx(node_id, 0, max_zone_num, zone_lid, sizeof(zone_lid));
			rf_snmp_evt(INFO, msg_sn, zone_lid);
		} else if (msg_sn == MSGDrawerResourceRemoved) {
			int32 lid_num = 0;
			get_global_idx(parent, 0, max_zone_num, zone_lid, sizeof(zone_lid));
			rc = libdb_attr_get_string(DB_RMM, node_id, WRAP_LOC_ID_STR, lid_str, sizeof(lid_str), LOCK_ID_NULL);
			if (rc == -1 || strnlen_s(lid_str, sizeof(lid_str)-1) == 0) {
				rmm_log(ERROR, "memdb get location id fail\n");
				return;
			}

			lid_num = (str2int(zone_lid) - 1) * MAX_DRAWER_NUM + str2int(lid_str);
			snprintf_s_i(lid_str, sizeof(lid_str), "%d", lid_num);
			rf_snmp_evt(INFO, msg_sn, lid_str);
		} else {
			rc = libdb_attr_get_string(DB_RMM, node_id, WRAP_LOC_ID_STR, lid_str, sizeof(lid_str), LOCK_ID_NULL);
			if (rc == -1 || strnlen_s(lid_str, sizeof(lid_str)-1) == 0) {
				rmm_log(ERROR, "memdb get location id fail\n");
				return;
			}

			rf_snmp_evt(INFO, msg_sn, lid_str);
		}
	}
}

static void ad_remove_node(char *uuid)
{
	int i = 0;
	struct node_info *subnode;
	struct node_info *sub_node;
	struct node_info *cnode;
	int nodenum = 0;
	int8 subnode_uuid[UUID_LEN] = {};
	memdb_integer rc = 0;
	memdb_integer node_id = 0;
	memdb_integer parent_id = 0;
	memdb_integer type = 0;

	if (nmap_get_node_id_by_uuid(&node_id, uuid) == -1)
		return;

	cnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
	if (cnode == NULL)
		return;

	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return;
	}
	node_id = cnode->node_id;
	parent_id = cnode->parent;
	type = cnode->type;

	subnode = libdb_list_subnode(DB_RMM, node_id, &nodenum, NULL, LOCK_ID_NULL);
	if (subnode != NULL) {
		memcpy_s(sub_node, sizeof(struct node_info) * nodenum, subnode, sizeof(struct node_info) * nodenum);
		for (i = 0; i < nodenum; i++) {
			rc = libdb_attr_get_string(DB_RMM, sub_node[i].node_id, WRAP_UUID_STR, subnode_uuid, sizeof(subnode_uuid), LOCK_ID_NULL);
			if (rc == 0) {
				ad_remove_node(subnode_uuid);
			}
		}
	}

	remove_node(node_id, parent_id, type);
	libdb_destroy_node(DB_RMM, node_id, LOCK_ID_NULL);
	libdb_free_node(sub_node);
}

void set_chassis_fw_ver(uint8 ver_low, uint8 ver_hi, memdb_integer node_id)
{
	char tmp[8] = {0};
	char ver_l[8] = {0};
	char ver_h[8] = {0};
	char buff[32] = {0};
	int ret;

	snprintf_s_i(tmp, sizeof(tmp), "%02X", ver_low);
	ret = sscanf(tmp, "%[0-9a-fA-F]", ver_l);
	if (ret != 1)
		return;

	snprintf_s_i(tmp, sizeof(tmp), "%02X", ver_hi);
	ret = sscanf(tmp, "%[0-9a-fA-F]", ver_h);
	if (ret != 1)
		return;

	snprintf_s_ss(buff, sizeof(buff), "%s.%s", ver_h, ver_l);

	if (libdb_attr_set_string(DB_RMM, node_id, MBP_FW_VER_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL) == -1) {
		rmm_log(ERROR, "memdb set mbp fw ver fail\n");
	}
}

result_t get_mbp_hw_addr(unsigned int pos, unsigned char *hw_addr)
{
	if ((1 == pos) || (2 == pos)) {
		memcpy_s((char *)hw_addr, strnlen_s((char *)(mbp_hw_info[pos - 1].hw_addr), 8-1), (char *)mbp_hw_info[pos - 1].hw_addr,
			   strnlen_s((char *)(mbp_hw_info[pos - 1].hw_addr), 8-1));
	}

	return RESULT_OK;
}

int on_cm_add(char *uuid, char *parent_uuid, json_t *req, json_t *resp)
{
	char buff[16] = {0};
	memdb_integer node_id = 0;
	int64 cm_lid = 0;
	int64 v1_nid = 0;
	int32 node_num = 0;
	struct node_info *pnode = NULL;

	if (jrpc_get_named_param_value(req, JRPC_CM_LID, JSON_INTEGER, &cm_lid) != JSONRPC_SUCCESS)
		return -1;

	node_id = get_subnode_id_by_lid(cm_lid, MC_NODE_ROOT, MC_TYPE_CM);
	if (node_id == INVALID_NODE_ID) {
		/* cm node not exist, creating cm node and cooling zone node, set all attribute */
		node_id = libdb_create_node(DB_RMM, MC_NODE_ROOT,
								 MC_TYPE_CM, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "new node created, id:%lu\n", node_id);
		char lid_str[64] = {0};
		snprintf_s_ll(lid_str, sizeof(lid_str), "%lld", cm_lid);
		rf_snmp_evt(INFO, MSGMBPResourceAdded, lid_str);
	}

	if (libdb_attr_set_string(DB_RMM, node_id, MBP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL) == -1) {
		rmm_log(ERROR, "memdb set mbp uuid attr fail\n");
		return -1;
	}
	nmap_add(uuid, node_id);

	pnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_V1, &node_num, NULL, LOCK_ID_NULL);
	if (pnode != NULL) {
		v1_nid = pnode[0].node_id;
		node_id = get_subnode_id_by_lid(cm_lid, v1_nid, MC_TYPE_MBPC);
		if (node_id == INVALID_NODE_ID) {
			node_id = libdb_create_node(DB_RMM, v1_nid,
								 MC_TYPE_MBPC, SNAPSHOT_NEED, LOCK_ID_NULL);
			init_mbpc_attr(&node_id, cm_lid, SNAPSHOT_NEED_ALL);
		}
		init_mbpc_attr(&node_id, cm_lid, SNAPSHOT_NEED_NOT);
	}
	return 0;
}

int on_cm_change(char* uuid, json_t *req, json_t *resp)
{
	int64 cm_lid = 0;
	int64 cm_uloc = 0;
	char buff[16] = {0};
	int64 ip_address = 0;
	int64 ver_high;
	int64 ver_low;
	int rc = 0;
	memdb_integer node_id = 0;

	nmap_get_node_id_by_uuid(&node_id, uuid);

	if (jrpc_get_named_param_value(req, JRPC_CM_LID, JSON_INTEGER, &cm_lid) == JSONRPC_SUCCESS) {
		if (cm_lid != 0) {
			unsigned char hw_addr[8] = {0};
			snprintf_s_i(buff, sizeof(buff), "%d", (int)cm_lid);
			rc = libdb_attr_set_string(DB_RMM, node_id, MBP_LOC_ID_STR, 0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set mbp loc id fail\n");
				return -1;
			}
			get_mbp_hw_addr(cm_lid, hw_addr);
			rc = libdb_attr_set_string(DB_RMM, node_id, MBP_HW_ADDR_STR,
				0x0, (char *)hw_addr, SNAPSHOT_NEED, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set hw addr fail\n");
				return -1;
			}
			rc = libdb_attr_set_string(DB_RMM, node_id, MBP_MBPID_STR, 0x0, buff, SNAPSHOT_NEED, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set mbp id fail\n");
				return -1;
			}
			init_mbp_attr(&node_id, cm_lid, PERSISTENT_ALL);
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_CM_ULOC, JSON_INTEGER, &cm_uloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_ll(buff, sizeof(buff), "%lld", cm_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, MBP_U_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc != 0) {
			rmm_log(ERROR, "memdb set cm ulocation fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_IPADDRESS, JSON_INTEGER, &ip_address) == JSONRPC_SUCCESS) {
		if (ip_address != 0) {
			/* save ip address */
			rc = libdb_attr_set_int(DB_RMM, node_id, MBP_IP_ADDR_STR, 0x0, ip_address, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set mbp ip addr fail\n");
				return -1;
			}
		}
	}

	if((jrpc_get_named_param_value(req, JRPC_VER_HIGH, JSON_INTEGER, &ver_high) == JSONRPC_SUCCESS) &&
			(jrpc_get_named_param_value(req, JRPC_VER_LOW, JSON_INTEGER, &ver_low) == JSONRPC_SUCCESS)) {
		set_chassis_fw_ver((uint8)ver_low, (uint8)ver_high, node_id);
	}

	return 0;
}

int on_pzone_add(char *uuid, char *parent_uuid, json_t *req, json_t *resp)
{
	memdb_integer cm_nid = 0;
	memdb_integer pz_nid = 0;
	int cm_lid = 0;
	int64 pz_lid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&cm_nid, parent_uuid);
	if (cm_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get cm node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_PZ_LID, JSON_INTEGER, &pz_lid) != JSONRPC_SUCCESS)
		return -1;

	rc = libdb_attr_get_int(DB_RMM, cm_nid, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
	if (rc != 0) {
		rmm_log(ERROR, "get cm loc id fail\n");
		return rc;
	}

	pz_nid = get_subnode_id_by_lid(pz_lid, cm_nid, MC_TYPE_PZONE);
	if (pz_nid == INVALID_NODE_ID) {
		pz_nid = libdb_create_node(DB_RMM, cm_nid,
										 MC_TYPE_PZONE, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "pzone created, id:%lu\n", pz_nid);
		char lid_str[64] = {0};
		get_global_idx(pz_nid, pz_lid, MAX_PZONE_NUM, lid_str, sizeof(lid_str));
		rf_snmp_evt(INFO, MSGPZoneResourceAdded, lid_str);
	}

	json_object_add(resp, JRPC_NODE_ID, json_integer(pz_nid));
	rc = libdb_attr_set_string(DB_RMM, pz_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}
	nmap_add(uuid, pz_nid);

	init_pzone_attr(&pz_nid, pz_lid, cm_lid, PERSISTENT_ALL);

	return 0;
}

static void update_pzone_power_cap(memdb_integer node_id, int64 input)
{
	#if 0
	int32 total = 0;
	memdb_integer rc = -1;

	if (node_id == 0) {
		rmm_log(ERROR, "invalid node id\n");
		return;
	}

	rc = libdb_attr_get_string(DB_RMM, node_id, PZONE_TT_PWR_CAP_STR, buff, sizeof(buff), LOCK_ID_NULL);
	if (rc != 0) {
		rmm_log(ERROR, "get total power cap fail\n");
		return;
	}

	total = str2int(buff) + input;
	#endif
	int8 buff[16] = {};
	
	snprintf_s_i(buff, sizeof(buff), "%d", input);
	libdb_attr_set_string(DB_RMM, node_id, PZONE_TT_PWR_CAP_STR, 0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
}

int on_pzone_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	memdb_integer cm_nid = 0;
	memdb_integer pz_nid = 0;
	int64 pz_lid = 0;
	int64 rc = 0;
	int64 pz_uloc = 0;
	int64 pcap = 0;
	char buff[16] = {};

	nmap_get_node_id_by_uuid(&pz_nid, uuid);
	if (pz_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get pzone node id fail, uuid is %d\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_TOTAL_POWER_CAP, JSON_INTEGER, &pcap) == JSONRPC_SUCCESS) {
		update_pzone_power_cap(pz_nid, pcap);
		return 0;
	}
	
	if (jrpc_get_named_param_value(req, JRPC_PZ_LID, JSON_INTEGER, &pz_lid)!= JSONRPC_SUCCESS) {
		rmm_log(ERROR, "get pzone loc id fail, uuid is %d\n", uuid);
		return -1;
	}

	pnode = libdb_get_node_by_node_id(DB_RMM, pz_nid, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get pzone node id fail\n");
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_PZ_ULOC, JSON_INTEGER, &pz_uloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_ll(buff, sizeof(buff), "%lld", pz_uloc);
		rc = libdb_attr_set_string(DB_RMM, pz_nid, PZONE_U_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc != 0) {
			rmm_log(ERROR, "memdb set tzone ulocation fail\n");
			return -1;
		}
	}

	return 0;
}

int on_tzone_add(char *uuid, char *parent_uuid, json_t *req, json_t *resp)
{
	memdb_integer cm_nid = 0;
	memdb_integer tz_nid = 0;
	int64 tz_lid = 0;
	int rc = 0;
	int32 cm_lid = 0;

	nmap_get_node_id_by_uuid(&cm_nid, parent_uuid);
	if (cm_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get cm node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_LID, JSON_INTEGER, &tz_lid) != JSONRPC_SUCCESS)
		return -1;

	rc = libdb_attr_get_int(DB_RMM, cm_nid, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
	if (rc != 0) {
		rmm_log(ERROR, "get cm loc id fail\n");
		return -1;
	}

	tz_nid = get_subnode_id_by_lid(tz_lid, cm_nid, MC_TYPE_TZONE);
	if (tz_nid == INVALID_NODE_ID) {
		tz_nid = libdb_create_node(DB_RMM, cm_nid,
										 MC_TYPE_TZONE, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "tzone created, id:%lu\n", tz_nid);
		char lid_str[64] = {0};
		get_global_idx(tz_nid, tz_lid, MAX_TZONE_NUM, lid_str, sizeof(lid_str));
		rf_snmp_evt(INFO, MSGTZoneResourceAdded, lid_str);
	}

	rc = libdb_attr_set_string(DB_RMM, tz_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc != 0) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}

	nmap_add(uuid, tz_nid);

	init_tzone_attr(&tz_nid, tz_lid, cm_lid, PERSISTENT_ALL);

	return 0;
}


int on_tzone_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	memdb_integer tz_nid = 0;
	int64 tz_lid = 0;
	int64 tz_uloc = 0;
	int64 tz_rpm = 0;
	int64 rc = 0;
	int8 buff[16] = {};
	int32 pwm = 0;
	int64 temp = 0;
	int64 airflow = 0;
	int8 pwm_str[64] = {};

	nmap_get_node_id_by_uuid(&tz_nid, uuid);
	if (tz_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get tzone node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_RPM, JSON_INTEGER, &tz_rpm) == JSONRPC_SUCCESS) {
		int8 speed_rpm[64] = {};

		snprintf_s_i(speed_rpm, sizeof(speed_rpm), "%d", (int32)tz_rpm);
		if (libdb_attr_set_string(DB_RMM, tz_nid,
			FAN_DESIRED_SPD_RPM_STR,0x0, speed_rpm,
			SNAPSHOT_NEED_NOT, LOCK_ID_NULL) == -1) {
			return -1;
		}

		return 0;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_TEMP, JSON_INTEGER, &temp) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_int(DB_RMM, tz_nid, TZ_PRESENT_TEMP, 0x0, (int32)temp, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set tzone inlet temp fail\n");
			return -1;
		}

		return 0;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_AIRFLOW, JSON_INTEGER, &airflow) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_int(DB_RMM, tz_nid, TZ_VOLUM_AIRFLOW, 0x0, (int32)airflow, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set tzone airflow fail\n");
			return -1;
		}

		return 0;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_OUTLET_TEMP, JSON_INTEGER, &temp) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_int(DB_RMM, tz_nid, TZ_OUTLET_TEMP, 0x0, (int32)temp, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set tzone outlet temp fail\n");
			return -1;
		}

		return 0;
	}

	if (jrpc_get_named_param_value(req, JRPC_TZ_LID, JSON_INTEGER, &tz_lid) != JSONRPC_SUCCESS) {
		rmm_log(ERROR, "get tzone node loc id fail, uuid is %s\n", uuid);
		return -1;
	}

	pnode = libdb_get_node_by_node_id(DB_RMM, tz_nid, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get tzone node id fail\n");
		return -1;
	}

	rc = libdb_attr_get_string(DB_RMM, tz_nid, FAN_DESIRED_SPD_PWM_STR, pwm_str, sizeof(pwm_str), LOCK_ID_NULL);
	if (rc != 0) {
		rmm_log(ERROR, "get tz pwm fail\n");
		return -1;
	}

	if (sscanf(pwm_str, "%d", &pwm) == 1)
		libwrap_set_tzone_pwm(tz_nid, pwm);

	if (jrpc_get_named_param_value(req, JRPC_TZ_ULOC, JSON_INTEGER, &tz_uloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_ll(buff, sizeof(buff), "%lld", tz_uloc);
		rc = libdb_attr_set_string(DB_RMM, tz_nid, TZONE_U_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set tzone ulocation fail\n");
			return -1;
		}
	}

	return 0;
}

int on_dzone_add(char *uuid, char *parent_uuid, json_t *req, json_t *resp)
{
	memdb_integer cm_nid = 0;
	memdb_integer dz_nid = 0;
	int64 dz_lid = 0;
	int cm_lid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&cm_nid, parent_uuid);
	if (cm_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get cm node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_DZ_LID, JSON_INTEGER, &dz_lid) != JSONRPC_SUCCESS)
		return -1;

	dz_nid = get_subnode_id_by_lid(dz_lid, cm_nid, MC_TYPE_DZONE);
	if (dz_nid == INVALID_NODE_ID) {
		dz_nid = libdb_create_node(DB_RMM, cm_nid,
										 MC_TYPE_DZONE, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "dzone created, id:%lu\n", dz_nid);
	}

	rc = libdb_attr_set_string(DB_RMM, dz_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}
	nmap_add(uuid, dz_nid);

	libdb_attr_get_int(DB_RMM, cm_nid, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);

	init_dzone_attr(&dz_nid, dz_lid, cm_lid, PERSISTENT_ALL);

	libwrap_set_memdb_rack_puid();

	return 0;
}

int on_dzone_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	int64 present = 0;
	int64 alert = 0;
	memdb_integer node_id = 0;
	int64 dz_lid = 0;
	int64 dz_uloc = 0;
	int rc = 0;
	char buff[128] = {0};

	nmap_get_node_id_by_uuid(&node_id, uuid);
	if (jrpc_get_named_param_value(req, JRPC_PRESENT_INFO, JSON_INTEGER, &present) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_char(DB_RMM, node_id, DRAWER_PRESENT_STR, 0x0, (char)present, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer present fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_DRAWER_ALERT, JSON_INTEGER, &alert) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_char(DB_RMM, node_id, ALERT_STR, 0x0, (char)present, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer alert fail\n");
			return -1;
		}
		memset(buff, 0, sizeof(buff));
		snprintf_s_ll(buff, sizeof(buff), "%lld", (alert & 0xFF));
//		rf_snmp_evt(INFO, MSGDrawerAlert, buff);
	}

	if (jrpc_get_named_param_value(req, JRPC_DZ_LID, JSON_INTEGER, &dz_lid) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get dzone node id fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_DZ_ULOC, JSON_INTEGER, &dz_uloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_ll(buff, sizeof(buff), "%lld", dz_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, DZONE_U_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer ulocation fail\n");
			return -1;
		}
	}

	return 0;
}



int on_psu_add(char *uuid, char *parent_uuid, json_t *req, json_t *resp)
{
	int64 psu_lid = 0;
	int32 pz_lid = 0;
	int32 cm_lid = 0;
	struct node_info *pz_node = NULL;
	memdb_integer pz_nid = 0;
	memdb_integer psu_nid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&pz_nid, parent_uuid);
	if (pz_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get pzone node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_LID, JSON_INTEGER, &psu_lid) != JSONRPC_SUCCESS)
		return -1;

	psu_nid = get_subnode_id_by_lid(psu_lid, pz_nid, MC_TYPE_PSU);
	if (psu_nid == INVALID_NODE_ID) {
		psu_nid = libdb_create_node(DB_RMM, pz_nid,
										 MC_TYPE_PSU, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "psu created, id:%lu\n", psu_nid);
		char lid_str[64] = {0};
		char zone_lid[64] = {0};
		snprintf_s_ll(lid_str, sizeof(lid_str), "%lld", psu_lid);
		get_global_idx(pz_nid, 0, MAX_PZONE_NUM, zone_lid, sizeof(zone_lid));
		rf_snmp_evt(INFO, MSGPSUResourceAdded, zone_lid, lid_str);
	}

	rc = libdb_attr_set_string(DB_RMM, psu_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}
	nmap_add(uuid, psu_nid);

	pz_node = libdb_get_node_by_node_id(DB_RMM, pz_nid, LOCK_ID_NULL);
	if (pz_node == NULL) {
		rmm_log(ERROR, "get pzone node id fail\n");
		return -1;
	}

	libdb_attr_get_int(DB_RMM, pz_node->parent, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
	libdb_attr_get_int(DB_RMM, pz_node->node_id, PZONE_LOC_ID_STR, &pz_lid, LOCK_ID_NULL);
	rmm_log(INFO, "initial psu attr, psu loc is %lld at cm %d\n", psu_lid, cm_lid);
	init_psu_attr(&psu_nid, psu_lid, pz_lid, cm_lid, PERSISTENT_ALL);

	return 0;
}

int on_psu_change(char *uuid, json_t *req, json_t *resp)
{
	int64 pin;
	int64 pcap;
	int64 pout;
	char buff[16] = {};
	char output[16] = {};
	char *str_ptr;
	memdb_integer node_id = 0;
	struct node_info *pnode = NULL;
	int64 total_pin = 0;
	int64 total_po = 0;
	int64 health_status = 0;
	int64 psu_xloc = 0;
	int64 psu_uloc = 0;
	int pz_lid = 0;
	int psu_lid = 0;
	int cm_lid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&node_id, uuid);

	if (jrpc_get_named_param_value(req, JRPC_PSU_SERIAL, JSON_STRING, &str_ptr) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_SER_NUM_STR, 0x0, str_ptr, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu serial num fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_MANUFACTURE, JSON_STRING, &str_ptr) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_MANUFACT_STR, 0x0, str_ptr, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu manufacture fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_MODEL, JSON_STRING, &str_ptr) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_MODEL_STR, 0x0, str_ptr, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu model fail\n");
			return -1;
		}
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_PART_NUM_STR, 0x0, str_ptr, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu part number fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_FW_VER, JSON_STRING, &str_ptr) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_FW_VER_STR, 0x0, str_ptr, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu fw ver fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_POWER_IN, JSON_INTEGER, &pin) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)pin);
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_TT_PWR_IN_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set ps tt pwr in fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_POWER_OUT, JSON_INTEGER, &pout) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)pout);
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_TT_PWR_OUT_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu current out fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_POWER_CAP, JSON_INTEGER, &pcap) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)pcap);
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_TT_PWR_CAP_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu current out fail\n");
			return -1;
		}

		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get psu node fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}
		//update_pzone_power_cap(pnode->parent, pcap);
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_TOTAL_POWER_IN, JSON_INTEGER, &total_pin) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get psu node fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)total_pin);
		libdb_attr_set_string(DB_RMM, pnode->parent, PZONE_TT_PWR_PROD_STR, 0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_TOTAL_POWER_OUT, JSON_INTEGER, &total_po) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get psu node fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)total_po);
		libdb_attr_set_string(DB_RMM, pnode->parent, PZONE_TT_PWR_CONSUM_STR, 0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_HEALTH_STATUS, JSON_INTEGER, &health_status) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get psu node fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}

		rc = libdb_attr_get_string(DB_RMM, pnode->node_id, PSU_HEALTH_ST_STR, output, sizeof(output), LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "get psu health status fail\n");
			return -1;
		}

		rc = libdb_attr_get_int(DB_RMM, pnode->node_id, PSU_LOC_ID_STR, &psu_lid, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "get psu location id fail\n");
			return -1;
		}

		rc = libdb_attr_get_int(DB_RMM, pnode->parent, PZONE_LOC_ID_STR, &pz_lid, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "get powerzone location id fail\n");
			return -1;
		}

		if ((atoi(output) & RMM_JSON_HEALTH_MASK) != (health_status & RMM_JSON_HEALTH_MASK)) {
			rmm_log(INFO, "Previous health status is %s, current health status is %d\n", output, health_status);
			int8 pz_lid_str[16] = {};
			int8 psu_lid_str[16] = {};
			snprintf_s_i(pz_lid_str, sizeof(pz_lid_str), "%d", pz_lid);
			snprintf_s_i(psu_lid_str, sizeof(psu_lid_str), "%d", psu_lid);
			rf_snmp_evt(INFO, MSGPSUStatusChange, pz_lid_str, psu_lid_str, "Health Status Change");
		}
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)health_status);
		libdb_attr_set_string(DB_RMM, pnode->node_id, PSU_HEALTH_ST_STR, 0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_XLOC, JSON_INTEGER, &psu_xloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)psu_xloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_X_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu xlocation fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_PSU_ULOC, JSON_INTEGER, &psu_uloc) == JSONRPC_SUCCESS) {
		memset(buff, 0, sizeof(buff));
		snprintf_s_i(buff, sizeof(buff), "%d", (int)psu_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, PSU_U_LOC_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set psu ulocation fail\n");
			return -1;
		}
	}

	return 0;
}

int on_drawer_add(char *uuid, char *present_uuid, json_t *req, json_t *resp)
{
	memdb_integer dz_nid = 0;
	memdb_integer drawer_nid = 0;
	int64 drawer_lid = 0;
	int32 drawer_lid_global = 0;
	int cm_lid = 0;
	int64 dz_lid = 0;
	int rc = 0;
	struct node_info *pnode = NULL;

	nmap_get_node_id_by_uuid(&dz_nid, present_uuid);
	if (dz_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get dzone node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_DRAWER_LID, JSON_INTEGER, &drawer_lid) != JSONRPC_SUCCESS)
		return -1;

	drawer_nid = get_subnode_id_by_lid(drawer_lid, dz_nid, MC_TYPE_DRAWER);
	if (drawer_nid == INVALID_NODE_ID) {
		drawer_nid = libdb_create_node(DB_RMM, dz_nid,
										 MC_TYPE_DRAWER, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "drawer created, id:%lu\n", drawer_nid);
		char lid_str[64] = {0};
		char zone_lid[64] = {0};
		get_global_idx(dz_nid, 0, MAX_DZONE_NUM, zone_lid, sizeof(zone_lid));
		drawer_lid_global = (str2int(zone_lid) - 1) * MAX_DRAWER_NUM + drawer_lid;
		snprintf_s_i(lid_str, sizeof(lid_str), "%d", drawer_lid_global);
		rf_snmp_evt(INFO, MSGDrawerResourceAdded, lid_str);
	}

	rc = libdb_attr_set_string(DB_RMM, drawer_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}
	nmap_add(uuid, drawer_nid);

	pnode = libdb_get_node_by_node_id(DB_RMM, dz_nid, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get dzone node id fail\n");
		return -1;
	}

	rc = libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "get cm location id fail\n");
		return -1;
	}

	rmm_log(INFO, "initial drawer attr, drawer loc is %lld at cm %d\n", drawer_lid, cm_lid);
	init_drawer_attr(&drawer_nid, drawer_lid, dz_lid, cm_lid, PERSISTENT_ALL);
	return 0;
}

int on_drawer_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	int64 temp = 0;
	int64 drawer_lid = 0;
	int64 present = 0;
	int64 health_st = 0;
	int dz_lid = 0;
	char temp_str[16] = {};
	char health_str[16] = {};
	jrpc_data_string ip;
	memdb_integer node_id = 0;
	int rc = 0;
	int64 drawer_uloc = 0;
	int64 drawer_xloc = 0;

	int64 ip_address = 0;
	int64 ip_src = 0;
	int64 subnet_mask = 0;
	int64 power = 0;

	nmap_get_node_id_by_uuid(&node_id, uuid);

	if (jrpc_get_named_param_value(req, JRPC_PRESENT_INFO, JSON_INTEGER, &present) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_char(DB_RMM, node_id, SLED_PRESENT_STR, 0x0, (char)present, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set sled present fail\n");
			return -1;
		}
	}
	
	if (jrpc_get_named_param_value(req, JRPC_INFO, JSON_INTEGER, &temp) == JSONRPC_SUCCESS) {
		snprintf_s_i(temp_str, sizeof(temp_str), "%d", (int)temp);
		rc = libdb_attr_set_string(DB_RMM, node_id, CHASSIS_TEMP_STR, 0x0, temp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set chassis temp fail\n");
			return -1;
		}

		rc = libdb_attr_set_string(DB_RMM, node_id, DRAWER_PRES_TEMP, 0x0, temp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer pres temp fail\n");
			return -1;
		}
	}

	#if 0
	if (jrpc_get_named_param_value(req, JRPC_IPADDRESS, JSON_STRING, &ip) == JSONRPC_SUCCESS) {
		rc = libdb_attr_set_string(DB_RMM, node_id, DRAWER_TMC_IP_STR, 0x0, ip, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set tmc ip fail\n");
			return -1;
		}
	}
	#endif

	if (jrpc_get_named_param_value(req, JRPC_HEALTH_STATUS, JSON_INTEGER, &health_st) == JSONRPC_SUCCESS) {
		snprintf_s_i(health_str, sizeof(health_str), "%d", (uint8)health_st);
		rc = libdb_attr_set_string(DB_RMM, node_id, DRAWER_HEALTH_STR, 0x0, health_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set health status fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_DRAWER_LID, JSON_INTEGER, &drawer_lid) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get drawer node id fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}
		libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &dz_lid, LOCK_ID_NULL);
	}

	if (jrpc_get_named_param_value(req, JRPC_DRAWER_ULOC, JSON_INTEGER, &drawer_uloc) == JSONRPC_SUCCESS) {
		memset(temp_str, 0, sizeof(temp_str));
		snprintf_s_ll(temp_str, sizeof(temp_str), "%lld", drawer_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, DRAWER_U_LOC_STR, 0x0, temp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer ulocation fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_DRAWER_POWER, JSON_INTEGER, &power) == JSONRPC_SUCCESS) {
		if (power != 0) {
			/* save power */
			rc = libdb_attr_set_int(DB_RMM, node_id, DRAWER_PWR_CONSUM_STR, 0x0, power, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set drawer power fail\n");
				return -1;
			}
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_IPADDRESS, JSON_INTEGER, &ip_address) == JSONRPC_SUCCESS) {
		if (ip_address != 0) {
			/* save ip address */
			rc = libdb_attr_set_int(DB_RMM, node_id, DRAWER_TMC_IP_STR, 0x0, ip_address, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set drawer tmc ip addr fail\n");
				return -1;
			}
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_IP_SOURCE, JSON_INTEGER, &ip_src) == JSONRPC_SUCCESS) {
		if (ip_src != 0) {
			/* save ip source */
			rc = libdb_attr_set_int(DB_RMM, node_id, DRAWER_TMC_IP_SRC, 0x0, ip_src, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set drawer tmc ip source fail\n");
				return -1;
			}
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_SUBNET_MASK, JSON_INTEGER, &subnet_mask) == JSONRPC_SUCCESS) {
		if (subnet_mask != 0) {
			/* save subnet mask */
			rc = libdb_attr_set_int(DB_RMM, node_id, DRAWER_TMC_SUBNET_MASK, 0x0, subnet_mask, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set drawer tmc subnet mask fail\n");
				return -1;
			}
		}
	}

	return 0;
}


int on_sled_add(char *uuid, char *present_uuid, json_t *req, json_t *resp)
{
	//memdb_integer dz_nid = 0;
	//memdb_integer drawer_nid = 0;
	struct node_info *pnode = NULL;
	memdb_integer drawer_nid = 0;
	memdb_integer sled_nid = 0;
	
	//int64 drawer_lid = 0;
	int dz_lid = 0;
	int64 sled_lid = 0;
	int64 drawer_lid = 0;
	int cm_lid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&drawer_nid, present_uuid);
	if (drawer_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get drawer node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_SLED_LID, JSON_INTEGER, &sled_lid) != JSONRPC_SUCCESS)
		return -1;

	sled_nid = get_subnode_id_by_lid(sled_lid, drawer_nid, MC_TYPE_SLED);
	if (sled_nid == INVALID_NODE_ID) {
		sled_nid = libdb_create_node(DB_RMM, drawer_nid,
										 MC_TYPE_SLED, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		rmm_log(INFO, "sled created, id:%lu\n", sled_nid);
		char lid_str[64] = {0};
		char drawer_lid[64] = {0};
		get_global_idx(drawer_nid, 0, MAX_DRAWER_NUM, drawer_lid, sizeof(drawer_lid));
		sled_lid = (str2int(drawer_lid) - 1) * MAX_SLED_NUM + sled_lid;
		snprintf_s_ll(lid_str, sizeof(lid_str), "%lld", sled_lid);
		//rf_snmp_evt(INFO, MSGDrawerResourceAdded, lid_str);
		//rf_snmp_evt(INFO, MSGResourceAdded);
	}

	rc = libdb_attr_set_string(DB_RMM, sled_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}
	nmap_add(uuid, sled_nid);

	pnode = libdb_get_node_by_node_id(DB_RMM, drawer_nid, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get cm node id fail\n");
		return -1;
	}
	libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &dz_lid, LOCK_ID_NULL);

	pnode = libdb_get_node_by_node_id(DB_RMM, pnode->parent, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get cm node id fail\n");
		return -1;
	}
	libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);

	rmm_log(INFO, "initial sled attr, sled loc is %lld at drawer %d, dz %d, cm %d\n", sled_lid, drawer_lid, dz_lid, cm_lid);
	init_sled_attr(&sled_nid, sled_lid, drawer_lid, dz_lid, cm_lid, PERSISTENT_ALL);
	return 0;
}

int on_sled_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	int64 temp = 0;
	int64 type = 0;
	int64 pwm = 0;
	int64 power = 0;
	int drawer_lid = 0;
	int dz_lid = 0;
	int sled_lid = 0;
	int cm_lid = 0;
	char tmp_str[16] = {};
	//jrpc_data_string ip;
	memdb_integer node_id = 0;
	memdb_integer tz_node_id = 0;
	int32 i = 0;
	int rc = 0;
	//int64 drawer_uloc = 0;
	//int64 drawer_xloc = 0;

	nmap_get_node_id_by_uuid(&node_id, uuid);
	
	if (jrpc_get_named_param_value(req, JRPC_SLED_TYPE, JSON_INTEGER, &type) == JSONRPC_SUCCESS) {
		snprintf_s_i(tmp_str, sizeof(tmp_str), "%d", (uint8)type);
		rc = libdb_attr_set_string(DB_RMM, node_id, SLED_TYPE_STR, 0x0, tmp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set sled type fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_SLED_TEMP, JSON_INTEGER, &temp) == JSONRPC_SUCCESS) {
		snprintf_s_i(tmp_str, sizeof(tmp_str), "%d", (uint8)temp);
		rc = libdb_attr_set_string(DB_RMM, node_id, SLED_INLETTEMP_STR, 0x0, tmp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set sled inlet temp fail\n");
			return -1;
		}
	}

#if 0
	if (jrpc_get_named_param_value(req, JRPC_SLED_TEMP_AVG, JSON_INTEGER, &temp) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get sled node id fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}

		pnode = libdb_get_node_by_node_id(DB_RMM, pnode->parent, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get drawer node id fail\n");
			return -1;
		}

		pnode = libdb_get_node_by_node_id(DB_RMM, pnode->parent, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get drawer zone node id fail\n");
			return -1;
		}

		for (i = 0; i < MAX_TZONE_NUM; i++) {
			tz_node_id = get_subnode_id_by_lid(i + 1, pnode->parent, MC_TYPE_TZONE);
			if (tz_node_id == 0)
				continue;

			rc = libdb_attr_set_int(DB_RMM, tz_node_id, TZ_PRESENT_TEMP, 0x0, (int32)temp, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
			if (rc == -1) {
				rmm_log(ERROR, "memdb set tzone inlet temp fail\n");
				return -1;
			}
		}
	}
#endif

	if (jrpc_get_named_param_value(req, JRPC_SLED_PWM, JSON_INTEGER, &pwm) == JSONRPC_SUCCESS) {
		snprintf_s_i(tmp_str, sizeof(tmp_str), "%d", (uint8)pwm);
		rc = libdb_attr_set_string(DB_RMM, node_id, SLED_DESIRED_PWM_STR, 0x0, tmp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set sled pwm fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_SLED_POWER, JSON_INTEGER, &power) == JSONRPC_SUCCESS) {
		snprintf_s_i(tmp_str, sizeof(tmp_str), "%d", (uint16)power);
		rc = libdb_attr_set_string(DB_RMM, node_id, SLED_POWER_STR, 0x0, tmp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set sled power fail\n");
			return -1;
		}
	}

#if 0
	if (jrpc_get_named_param_value(req, JRPC_DRAWER_ULOC, JSON_INTEGER, &drawer_uloc) == JSONRPC_SUCCESS) {
		memset(temp_str, 0, sizeof(temp_str));
		snprintf(temp_str, sizeof(temp_str), "%lld", drawer_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, DRAWER_U_LOC_STR, 0x0, temp_str, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set drawer ulocation fail\n");
			return -1;
		}
	}
#endif

	return 0;
}


int on_fan_add(char *uuid, char *present_uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	memdb_integer tz_nid = 0;
	memdb_integer fan_nid = 0;
	int64 fan_lid = 0;
	int64 tz_lid = 0;
	int rc = 0;
	int cm_lid = 0;

	nmap_get_node_id_by_uuid(&tz_nid, present_uuid);
	if (tz_nid == INVALID_NODE_ID) {
		rmm_log(ERROR, "get tz node id fail, uuid is %s\n", uuid);
		return -1;
	}

	if (jrpc_get_named_param_value(req, JRPC_FAN_LID, JSON_INTEGER, &fan_lid) != JSONRPC_SUCCESS)
		return -1;

	fan_nid = get_subnode_id_by_lid(fan_lid, tz_nid, MC_TYPE_FAN);
	if (fan_nid == INVALID_NODE_ID) {
		fan_nid = libdb_create_node(DB_RMM, tz_nid,
										 MC_TYPE_FAN, SNAPSHOT_NEED, LOCK_ID_NULL);
		rmm_log(INFO, "fan created, id:%lu\n", fan_nid);
		char lid_str[64] = {0};
		char zone_lid[64] = {0};
		snprintf_s_ll(lid_str, sizeof(lid_str), "%lld", fan_lid);
		snprintf_s_ll(zone_lid, sizeof(zone_lid), "%lld", tz_lid);
		get_global_idx(tz_nid, 0, MAX_TZONE_NUM, zone_lid, sizeof(zone_lid));
		rf_snmp_evt(INFO, MSGFanResourceAdded, zone_lid, lid_str);
	}

	rc = libdb_attr_set_string(DB_RMM, fan_nid, WRAP_UUID_STR, 0x0, uuid, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set uuid fail\n");
		return -1;
	}

	nmap_add(uuid, fan_nid);

	pnode = libdb_get_node_by_node_id(DB_RMM, tz_nid, LOCK_ID_NULL);
	if (pnode == NULL) {
		rmm_log(ERROR, "get tzone node id fail\n");
		return -1;
	}
	libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &cm_lid, LOCK_ID_NULL);
	rmm_log(INFO, "initial fan attr, fan loc is %lld at cm %d\n", fan_lid, cm_lid);
	init_fan_attr(&fan_nid, fan_lid, tz_lid, cm_lid, PERSISTENT_ALL);
	return 0;
}

int on_fan_change(char *uuid, json_t *req, json_t *resp)
{
	struct node_info *pnode = NULL;
	int64 speed = 0;
	char string[64] = {0};
	memdb_integer node_id = 0;
	int64 fan_lid = 0;
	int64 fan_uloc = 0;
	int64 fan_xloc = 0;
	int tz_lid = 0;
	int cm_lid = 0;
	int rc = 0;

	nmap_get_node_id_by_uuid(&node_id, uuid);
	if (jrpc_get_named_param_value(req, JRPC_INFO, JSON_INTEGER, &speed) == JSONRPC_SUCCESS) {
		snprintf_s_i(string, sizeof(string), "%d", (int)speed);
		rc = libdb_attr_set_string(DB_RMM, node_id, FAN_TACH_READ_STR, 0x0, string, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set fan tach fail\n");
			return -1;
		}
		json_object_add(resp, JRPC_NODE_ID, json_integer(node_id));
	}

	if (jrpc_get_named_param_value(req, JRPC_FAN_LID, JSON_INTEGER, &fan_lid) == JSONRPC_SUCCESS) {
		pnode = libdb_get_node_by_node_id(DB_RMM, node_id, LOCK_ID_NULL);
		if (pnode == NULL) {
			rmm_log(ERROR, "get fan node id fail, node id is %llu, uuid is %s\n", node_id, uuid);
			return -1;
		}
		libdb_attr_get_int(DB_RMM, pnode->parent, MBP_LOC_ID_STR, &tz_lid, LOCK_ID_NULL);
	}

	if (jrpc_get_named_param_value(req, JRPC_FAN_ULOC, JSON_INTEGER, &fan_uloc) == JSONRPC_SUCCESS) {
		memset(string, 0, sizeof(string));
		snprintf_s_ll(string, sizeof(string), "%lld", fan_uloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, FAN_U_LOC_STR, 0x0, string, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc != 0) {
			rmm_log(ERROR, "memdb set fan ulocation fail\n");
			return -1;
		}
	}

	if (jrpc_get_named_param_value(req, JRPC_FAN_XLOC, JSON_INTEGER, &fan_xloc) == JSONRPC_SUCCESS) {
		memset(string, 0, sizeof(string));
		snprintf_s_ll(string, sizeof(string), "%lld", fan_xloc);
		rc = libdb_attr_set_string(DB_RMM, node_id, FAN_X_LOC_STR, 0x0, string, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc != 0) {
			rmm_log(ERROR, "memdb set fan xlocation fail\n");
			return -1;
		}
	}

	return 0;
}

cmd_func_map_t cmd_func_maps[] = {
	{ CM_TYPE,		on_cm_add,		on_cm_change },
	{ PZ_TYPE,		on_pzone_add,	on_pzone_change },
	{ TZ_TYPE,		on_tzone_add,	on_tzone_change },
	{ DZ_TYPE,		on_dzone_add,	on_dzone_change },
	{ PSU_TYPE,		on_psu_add,		on_psu_change},
	{ FAN_TYPE,		on_fan_add,		on_fan_change},
	{ DRAWER_TYPE,	on_drawer_add,	on_drawer_change},
	{ SLED_TYPE,	on_sled_add,	on_sled_change}
};


cmd_func_map_t *find_cmd_func(int type)
{
	int index = 0;

	for (index = 0; index < sizeof(cmd_func_maps) / sizeof(cmd_func_map_t); index++) {
		if (cmd_func_maps[index].type == type)
			return &cmd_func_maps[index];
	}

	return NULL;
}



int on_add(jrpc_req_pkg_t *req, json_t *resp)
{
	int64 type;
	int64 pos;
	char *uuid = NULL;
	char *parent_uuid = NULL;

	jrpc_get_named_param_value(req->json, JRPC_EVT_TYPE, JSON_INTEGER, &type);
	jrpc_get_named_param_value(req->json, JRPC_UUID, JSON_STRING, &uuid);
	if(jrpc_get_named_param_value(req->json, JRPC_PARENT_UUID, JSON_STRING, &parent_uuid) == JSONRPC_FAILED)
		 parent_uuid = NULL;

	cmd_func_map_t *cmd_func_entity = find_cmd_func(type);

	if (cmd_func_entity != NULL)
		cmd_func_entity->on_add(uuid, parent_uuid, req->json, resp);

	return 0;
}

int on_remove(jrpc_req_pkg_t *req, json_t *resp)
{
	char *uuid;

	jrpc_get_named_param_value(req->json, JRPC_UUID, JSON_STRING, &uuid);

	if (strnlen_s(uuid, JSONRPC_MAX_STRING_LEN-1) == 0)
		return -1;

	ad_remove_node(uuid);

	return 0;
}

int on_change(jrpc_req_pkg_t *req, json_t *resp)
{
	int64 type = 0;
	char * uuid = NULL;
	int cm_idx = 0;
	cmd_func_map_t *cmd_func_entity = NULL;

	jrpc_get_named_param_value(req->json, JRPC_UUID, JSON_STRING, &uuid);
	/*printf("%s node_id: %lld\n", __func__, node_id);*/
	jrpc_get_named_param_value(req->json, JRPC_EVT_TYPE, JSON_INTEGER, &type);
	/*printf("%s type: %lld\n", __func__, type);*/

	cmd_func_entity = find_cmd_func(type);
	if (cmd_func_entity != NULL)
		cmd_func_entity->on_change(uuid, req->json, resp);

	return 0;
}

int process_req(int func_id, jrpc_req_pkg_t *req, json_t *resp)
{
	return cmd_handles[func_id](req, resp);
}

static void reset_zone(memdb_integer node_id, int cm_lid, reset_zone_fn zone_fn, reset_item_fn item_fn, int sub_type)
{
	int i = 0;
	int lid = 0;
	struct node_info *subnode = NULL;
	struct node_info *sub_node = NULL;
	int num = 0;
	int sub_idx = 0;

	libdb_attr_get_int(DB_RMM, node_id, WRAP_LOC_ID_STR, &lid, LOCK_ID_NULL);
	zone_fn(&node_id, cm_lid, lid);
	subnode = libdb_list_subnode_by_type(DB_RMM, node_id, sub_type, &num, NULL, LOCK_ID_NULL);
	sub_node = (struct node_info *)malloc(CMDBUFSIZ);
	if (sub_node == NULL) {
		return;
	}
	memcpy_s(sub_node, sizeof(struct node_info) * num, subnode, sizeof(struct node_info) * num);

	for (i = 0; i < num; i ++) {
		sub_idx = 0;
		libdb_attr_get_int(DB_RMM, sub_node[i].node_id, WRAP_LOC_ID_STR, &sub_idx, LOCK_ID_NULL);
		item_fn(&(sub_node[i].node_id), cm_lid, sub_idx, lid);
	}

	libdb_free_node(sub_node);
}

static void reset_zones(memdb_integer node_id, reset_zone_fn zone_fn, int zone_type, reset_item_fn item_fn, int sub_type)
{
	int i = 0;
	int lid = 0;
	struct node_info *subnode = NULL;
	int num = 0;
	struct node_info *zone_node = NULL;

	subnode = libdb_list_subnode_by_type(DB_RMM, node_id, zone_type, &num, NULL,LOCK_ID_NULL);

	if (subnode) {
		zone_node = (struct node_info *)malloc(num * sizeof(struct node_info));
		if (zone_node == NULL)
			return;

		memcpy_s(zone_node, num * sizeof(struct node_info), subnode, num * sizeof(struct node_info));
		libdb_attr_get_int(DB_RMM, node_id, WRAP_LOC_ID_STR, &lid, LOCK_ID_NULL);
		for (i = 0; i < num; i++) {
			reset_zone(zone_node[i].node_id, lid, zone_fn, item_fn, sub_type);
			if (zone_type == MC_TYPE_TZONE)
				libwrap_set_tzone_pwm(zone_node[i].node_id, DEFAULT_FAN_PWM);
		}
		free(zone_node);
		zone_node = NULL;
	}
}

int rmm_factory_reset(jrpc_req_pkg_t *req, json_t *resp)
{
	int i = 0; 
	int j = 0;
	struct node_info *subnode = NULL;

	int mbp_num = 0;
	int mbp_lid = 0;
	struct node_info *mbp_nodes = NULL;

	reset_rack_attr();
	subnode = libdb_list_subnode_by_type(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM, &mbp_num, NULL, LOCK_ID_NULL);
	if(!subnode)
		return 0;

	mbp_nodes = (struct node_info *)malloc(sizeof(struct node_info) * mbp_num);
	if (mbp_nodes == NULL)
		return 0;
	memcpy_s(mbp_nodes, sizeof(struct node_info) * mbp_num, subnode, sizeof(struct node_info) * mbp_num);

	for (i = 0; i < mbp_num; i++) {
		libdb_attr_get_int(DB_RMM, mbp_nodes[i].node_id, WRAP_LOC_ID_STR, &mbp_lid, LOCK_ID_NULL);;
		reset_mbp_attr(&(mbp_nodes[i].node_id), mbp_lid);

		reset_zones(mbp_nodes[i].node_id, reset_dzone_attr, MC_TYPE_DZONE, reset_drawer_attr, MC_TYPE_DRAWER);
		reset_zones(mbp_nodes[i].node_id, reset_pzone_attr, MC_TYPE_PZONE, reset_psu_attr, MC_TYPE_PSU);
		reset_zones(mbp_nodes[i].node_id, reset_tzone_attr, MC_TYPE_TZONE, reset_fan_attr, MC_TYPE_FAN);
	}
	free(mbp_nodes);
	return 0;
}
