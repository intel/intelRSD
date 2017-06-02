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


#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>

#include "libmemdb/memdb.h"
#include "libredfish/msg_reg.h"
#include "libredfish/rf_event.h"
#include "libwrap/wrap.h"
#include "libcfg/cfg.h" 
#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libutils/types.h"

#if 0
static int get_drawer_ip_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}

	snprintf(buff, sizeof(buff), "%d.%d.%d.%d", rsp[1] ,rsp[2] ,rsp[3] ,rsp[4]);

	if(strlen(buff) != 0)
		libdb_attr_set_string(DB_RMM, *node_id, DRAWER_TMC_IP_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);


	return 0;
}

static int get_drawer_ip_by_idx(memdb_integer* node_id, int32 dz_idx, int32 drawer_idx)
{
	struct jipmi_msg req = {};
	int host;
	memdb_integer cm_node_id;
	int64 error_code = 0;

	req.data[0] = drawer_idx - 1;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		DRAWER_IP_CMD);
	FILL_INT(req.data_len,	1);

	cm_node_id = get_subnode_id_by_idx(dz_idx, MC_NODE_ROOT, MC_TYPE_CM);
	error_code = libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	if(error_code != 0) {
		return (int)error_code;
	}
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, get_drawer_ip_cb, node_id, JIPMI_NON_SYNC);

	return 0;	

}



result_t assetd_get_drawer_ip(memdb_integer* node_id, int32 dz_idx, int32 drawer_idx)
{
	return get_drawer_ip_by_idx(node_id, dz_idx, drawer_idx);
}


static int get_psu_serial_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}
	
	for (i = 1; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat(buff, tmp, strlen(tmp));
	}

	if(strlen(buff) != 0)
		libdb_attr_set_string(DB_RMM, *node_id, PSU_SER_NUM_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);


	return 0;
}


static int get_psu_serial(memdb_integer* node_id, int psu_idx, int pzone_idx)
{
	struct jipmi_msg req = {};
	int  host;
	memdb_integer cm_node_id;
	int64 error_code = 0;
	
	req.data[0] = psu_idx - 1;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_SERIAL;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_INT(req.data_len,	3);

	cm_node_id = get_subnode_id_by_idx(pzone_idx, MC_NODE_ROOT, MC_TYPE_CM);
	error_code = libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	if(error_code != 0) {
		return (int)error_code;
	}	
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, get_psu_serial_cb, node_id, JIPMI_NON_SYNC);

	return 0;
}


result_t assetd_get_psu_serial(memdb_integer *node_id, int psu_id, int pzone_id)
{
	return get_psu_serial(node_id, psu_id, pzone_id);
}


static int get_psu_manufacture_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}
	
	for (i = 1; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat(buff, tmp, strlen(tmp));
	}

	if(strlen(buff) != 0)
		libdb_attr_set_string(DB_RMM, *node_id, PSU_MANUFACT_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);


	return 0;
}


static int get_psu_manufacture(memdb_integer* node_id, int psu_idx, int pzone_idx)
{
	struct jipmi_msg req = {};
	int  host;
	memdb_integer cm_node_id;
	int64 error_code = 0;
	
	req.data[0] = psu_idx - 1;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_MANUFACTURE;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_INT(req.data_len,	3);

	cm_node_id = get_subnode_id_by_idx(pzone_idx, MC_NODE_ROOT, MC_TYPE_CM);
	error_code = libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	if(error_code != 0) {
		return (int)error_code;
	}	
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, get_psu_manufacture_cb, node_id, JIPMI_NON_SYNC);

	return 0;
}


result_t assetd_get_psu_manufacture(memdb_integer *node_id, int psu_id, int pzone_id)
{
	return get_psu_manufacture(node_id, psu_id, pzone_id);
}



static int get_psu_model_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}
	
	for (i = 1; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat(buff, tmp, strlen(tmp));
	}

	if(strlen(buff) != 0)
		libdb_attr_set_string(DB_RMM, *node_id, PSU_MODEL_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);


	return 0;
}


static int get_psu_model(memdb_integer* node_id, int psu_idx, int pzone_idx)
{
	struct jipmi_msg req = {};
	int host;
	memdb_integer cm_node_id;
	int64 error_code = 0;
	
	req.data[0] = psu_idx - 1;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_MODEL;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_INT(req.data_len,	3);

	cm_node_id = get_subnode_id_by_idx(pzone_idx, MC_NODE_ROOT, MC_TYPE_CM);
	error_code = libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	if(error_code != 0) {
		return (int)error_code;
	}	
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, get_psu_model_cb, node_id, JIPMI_NON_SYNC);

	return 0;
}


result_t assetd_get_psu_model(memdb_integer *node_id, int psu_id, int pzone_id)
{
	return get_psu_model(node_id, psu_id, pzone_id);
}


static int get_psu_fw_ver_cb(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer * node_id = (memdb_integer*)cb_data;
	char buff[128] = {0};
	char tmp[8] = {0};
	int i = 0;

	if(result == -1 || rsp[0] != IPMI_CC_OK) {
		return -1;
	}
	
	for (i = 1; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%c", rsp[i]);
		strncat(buff, tmp, strlen(tmp));
	}


	if(strlen(buff) != 0)
		libdb_attr_set_string(DB_RMM, *node_id, PSU_FW_VER_STR, 0x0, buff, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);


	return 0;
}


static int get_psu_fw_ver(memdb_integer* node_id, int psu_id, int pzone_id)
{
	struct jipmi_msg req = {};
	int  host;
	memdb_integer cm_node_id;
	int64 error_code = 0;
	
	req.data[0] = psu_id - 1;
	req.data[1] = PSU_READ_BLK;
	req.data[2] = PSU_OFFSET_REVISION;

	FILL_INT(req.netfn,		IPMI_CM_NETFN);
	FILL_INT(req.cmd,		SEND_PSU_CMD);
	FILL_INT(req.data_len,	3);

	cm_node_id = get_subnode_id_by_idx(pzone_id, MC_NODE_ROOT, MC_TYPE_CM);
	error_code = libdb_attr_get_int(DB_RMM, cm_node_id, MBP_IP_ADDR_STR, &host, LOCK_ID_NULL);
	if(error_code != 0) {
		return (int)error_code;
	}		
	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, get_psu_fw_ver_cb, node_id, JIPMI_NON_SYNC);

	return 0;
}


result_t assetd_get_psu_fw_ver(memdb_integer *node_id, int psu_id, int pzone_id)
{
	return get_psu_fw_ver(node_id, psu_id, pzone_id);
}
#endif
static int uuid_generate_t(char * uuid_str)
{
	FILE *fp;
	fp=fopen("/proc/sys/kernel/random/uuid","r");
	if (!(fp)) {
		printf("Error in open file!\n");
		return -1;
	}

	if (fscanf(fp,"%36s", uuid_str)) {
		fclose(fp);

		uuid_str[UUID_MAX_LEN-1] = '\0';

		printf("uuid is %s. str len is %d\n", uuid_str, (int)strnlen_s(uuid_str, RSIZE_MAX_STR));
		return 0;
	}
	fclose(fp);
	return -1;
}


result_t assetd_gen_uuid(char *key, char *uuid)
{
	char uuid_str[UUID_MAX_LEN] = {0};

	if (NULL == uuid)
		return -1;

	uuid_generate_t(uuid_str);

	memcpy_s(uuid, UUID_MAX_LEN, uuid_str, UUID_MAX_LEN);

	return RESULT_OK;
}


