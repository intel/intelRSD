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


#include "stdlib.h"
#include "libjson/json.h"
#include "libcfg/cfg.h"
#include "libutils/file.h"
#include "libutils/string.h"
#include "libencrypter/encrypter.hpp"

#define RMM_CFG_FILE_PATH	"/etc/rmm/rmm.cfg"
#define RMM_DECRYPT_KEYFILE	"/etc/rmm/keyfile"

static int64 get_json_int_value(json_t *jobj, char *name)
{
	json_t *mjobj = json_object_get(jobj, name);
	if (mjobj == NULL)
		return 0;

	int64 id = json_integer_value(mjobj);
	if (-1 == id)
		return 0;

	return id;
}

static int get_log_modules(json_t *obj, struct rmm_log_module *modules, int *count)
{
	json_pair_t *pair;
	int index = 0;
	int max_lines;
	int level;
	int handler;

	json_t *rmm_logd = json_object_get(obj, PROC_RMM_LOGD);

	if (rmm_logd == NULL) {
		printf("Can't find the %s attribute.\n", PROC_RMM_LOGD);
		return -1;
	}

	json_t *log_modules = json_object_get(rmm_logd, ATTR_LOG_MODULES);

	if (log_modules == NULL)
		return -1;

	json_object_t *jobj_log_modules = json_to_object(log_modules);

	if (jobj_log_modules == NULL)
		return -1;

	for (pair = jobj_log_modules->next; pair != NULL; pair = pair->next) {
		strncpy_safe(modules[index].name, pair->name, RMM_LOG_MODULE_NAME_LEN, RMM_LOG_MODULE_NAME_LEN - 1);
		modules[index].max_lines = get_json_int_value(pair->value,  ATTR_MAX_LINES);
		modules[index].level = get_json_int_value(pair->value, ATTR_LOG_LEVEL);
		index++;
	}
	*count = index;
	return 0;
}

static int get_tty_infos(json_t *obj, char *infos, int *count)
{
	int index = 0;
	char *data;

	json_t *rmm_logd = json_object_get(obj, PROC_ASSETD);
	if (rmm_logd == NULL) {
		printf("Can't find the %s attribute.\n", PROC_ASSETD);
		return -1;
	}

	json_t *tty_infos = json_object_get(rmm_logd, ATTR_TTY_INFOS);
	if (tty_infos == NULL) {
		printf("Can't find the %s attribute.\n", ATTR_TTY_INFOS);
		return -1;
	}

	int array_size = json_array_size(tty_infos);
	for (index = 0; index < array_size; index++) {
		data = json_string_value(json_array_get(tty_infos, index));
		if(data)
			strncpy_safe(infos + RMM_TTY_INFO_LEN * index, data, RMM_TTY_INFO_LEN, RMM_TTY_INFO_LEN - 1);
	}

	*count = index;
	return 0;
}

static json_t *load_rmm_cfg()
{
	char *buffer;
	buffer = read_file(RMM_CFG_FILE_PATH);	
	if (buffer == NULL) {
		printf("load default rmm config file fail...\n");
		return NULL;
	}

	json_t *obj = json_parse(buffer);
	if (!obj) {
		printf("Json parse error...\n");
	}

	free(buffer);
	return obj;
}

static json_t *get_json_attr(json_t *conf, char *proc_name, char *attr)
{
	char *data;

	json_t *proc = json_object_get(conf, proc_name);
	if (proc == NULL) {
		printf("Can't find the %s attribute.\n", proc_name);
		return NULL;
	}

	json_t *mjobj = json_object_get(proc, attr);
	return mjobj;
}

static int get_str_attr(char *proc_name, char *attr, char *data, int max_len)
{
	char *tmp = NULL;
	json_t *conf = load_rmm_cfg();
	if (conf == 0) {
		printf("Load rmm config file fail...\n");
		return -1;
	}

	json_t *jattr = get_json_attr(conf, proc_name, attr);
	if (jattr == NULL) {
		return -1;
	}
	tmp = json_string_value(jattr);
	if (tmp)
		strncpy_safe(data, tmp, max_len, max_len - 1);

	json_free(conf);
	return 0;
}

static int get_int_attr(char *proc_name, char *attr)
{
	json_t *conf = load_rmm_cfg();
	int ret = -1;
	if (conf == 0) {
		printf("Load rmm config file fail...\n");
		return -1;
	}
	json_t *jattr = get_json_attr(conf, proc_name, attr);
	if (jattr == NULL) {
		printf("Can't find the %s attribute.\n", attr);
		return -1;
	}

	ret = json_integer_value(jattr);
	json_free(conf);
	return ret;
}

int rmm_cfg_get_tty_infos(char *infos, int *count)
{
	json_t *conf;
	*count = 0;
	int ret = 0;

	conf = load_rmm_cfg();
	if (conf == 0)
		return -1;
	ret = get_tty_infos(conf, infos, count);
	json_free(conf);
	return ret;
}

int rmm_cfg_get_log_modules(struct rmm_log_module *modules, int *count)
{
	json_t *conf;
	*count = 0;
	int ret = 0;

	conf = load_rmm_cfg();
	if (conf == 0)
		return -1;
	ret = get_log_modules(conf, modules, count);
	json_free(conf);
	return ret;
}


int rmm_cfg_get_port(int port_attr)
{
	switch (port_attr) {
        case ASSETD_PORT:
			return get_int_attr(PROC_ASSETD, ATTR_PORT);
        case RESTD_PORT:
			return get_int_attr(PROC_RESTD, ATTR_PORT);
        case MEMDBD_PORT:
            return get_int_attr(PROC_MEMDBD, ATTR_PORT);
        case IPMIMODULE_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_PORT);
        case REDFISHD_PORT:
            return get_int_attr(PROC_REDFISHD, ATTR_PORT);
		case SNMPAGENTD_PORT:
			return get_int_attr(PROC_SNMP_SUBAGENTD, ATTR_PORT);
		case SENSORD_PORT:
			return get_int_attr(PROC_SENSORD, ATTR_PORT);
		case UPGRADED_PORT:
			return get_int_attr(PROC_UPGRADED, ATTR_PORT);
		case LOGD_PORT:
			return get_int_attr(PROC_RMM_LOGD, ATTR_PORT);
		case DBSYNCD_PORT:
			return get_int_attr(PROC_DB_SYNCD, ATTR_PORT);
		case RMMKEEPERD_PORT:
			return get_int_attr(PROC_RMM_KEEPERD, ATTR_PORT);
		case SYNCDB_VER_PORT:
			return get_int_attr(PROC_RMM_KEEPERD, ATTR_SYNC_DB_VER_PORT);
		case KEEPERDB_PORT:
			return get_int_attr(PROC_RMM_KEEPERD, ATTR_KEEPER_DDB_PORT);
		case SNAPSHOT_SYNC_PORT:
			return get_int_attr(PROC_DB_SYNCD, ATTR_SNAPSHOT_SYNC_PORT);
		case IPMIRMCPCLIENT_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_RMCP_CLIENT_PORT);
		case IPMIIPMB_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_IPMB_UDP_PORT);
		case IPMII2CD_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_I2CD_UDP_PORT);
		case IPMI_LISTEN_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_LISTEN_PORT);
		case IPMICOOLINGCTRL_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_COOLING_CTRL_PORT);
		case IPMIPOWERMGMT_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_POWER_MGMT_PORT);
		case IPMIASSETMODULE_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_ASSET_MODULE_PORT);
		case IPMISENSORD_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_SENSOR_MONITOR_PORT);
		case IPMITEST_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_TEST_PORT);
		case IPMIUARTTGTCFG_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_UART_TGT_CFG_PORT);
		case IPMIWEBSOCKET_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_WEBSOCKET_PORT);
		case IPMIJSONRPC_SERVER_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_JSON_RPC_SERVER_PORT);
		case IPMIJSONRPC_IPMID_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_JSON_RPC_IPMID_PORT);
		case ASSETMGR_JSONRPC_PORT:
			return get_int_attr(PROC_ASSETD, ATTR_JRPC_PORT);
		case IPMIJSONRPC_SNMP_SUBAGENT_PORT:
			return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMI_JSON_RPC_SNMP_SUBAGNETD_PORT);
		case ASSET_MODULE_PORT:
			return get_int_attr(PROC_ASSET_MODULE, ATTR_PORT);
		case REGISTERD_PORT:
			return get_int_attr(PROC_REGISTERD, ATTR_PORT);
		case REGISTERD_JSONRPC_PORT:
			return get_int_attr(PROC_REGISTERD, ATTR_JRPC_PORT);
        default:
            break;
    }

	return -1;
}

int rmm_cfg_get_rest_prefix(char *prefix, int max_len)
{
    if (prefix == NULL)
        return -1;
    else
        return get_str_attr(PROC_RESTD, ATTR_REST_PREFIX, prefix, max_len/*MAX_URL*/);
}

int rmm_cfg_get_rmcp_username(char *username, int max_len)
{
	int rc = 0;
	char ciphertxt[CIPHER_LENGTH_MAX] = {};
	char plaintxt[PLAIN_LENGTH_MAX] = {};

	rc = get_str_attr(PROC_IPMI_MODULE, ATTR_RMCP_USERNAME, ciphertxt, CIPHER_LENGTH_MAX);
	if (rc == 0) {
		rc = rmm_decrypt(ciphertxt, RMM_DECRYPT_KEYFILE, plaintxt);
		if (rc == 0) {
			strncpy_safe(username, plaintxt, max_len, max_len - 1);
		}
	}
	return rc;
}

int rmm_cfg_get_rmcp_password(char *password, int max_len)
{
	int rc = 0;
	char ciphertxt[CIPHER_LENGTH_MAX] = {};
	char plaintxt[PLAIN_LENGTH_MAX] = {};

	rc = get_str_attr(PROC_IPMI_MODULE, ATTR_RMCP_PASSWORD, ciphertxt, CIPHER_LENGTH_MAX);
	if (rc == 0) {
		rc = rmm_decrypt(ciphertxt, RMM_DECRYPT_KEYFILE, plaintxt);
		if (rc == 0) {
			strncpy_safe(password, plaintxt, max_len, max_len - 1);
		}
	}
	return rc;
}

int rmm_cfg_get_baudrate(void)
{
	return get_int_attr(PROC_IPMI_MODULE, ATTR_IPMID_SERIAL_BAUDRATE);
}

unsigned char rmm_cfg_get_databits(void)
{
	char data[2] = {};

	if (get_str_attr(PROC_IPMI_MODULE, ATTR_IPMID_SERIAL_DATABITS, data, sizeof(data)) != 0)
		return 0;

	return data[0];
}

unsigned char rmm_cfg_get_stopbits(void)
{
	char data[2] = {};

	if (get_str_attr(PROC_IPMI_MODULE, ATTR_IPMID_SERIAL_STOPBITS, data, sizeof(data)) != 0)
		return 0;

	return data[0];
}

unsigned char rmm_cfg_get_parity(void)
{
	char data[2] = {};

	if (get_str_attr(PROC_IPMI_MODULE, ATTR_IPMID_SERIAL_PARITY, data, sizeof(data)) != 0)
		return 0;

	return data[0];
}

unsigned char rmm_cfg_get_flow_control()
{
	char data[2] = {};

	if (get_str_attr(PROC_IPMI_MODULE, ATTR_IPMID_SERIAL_FLOWCONTROL, data, sizeof(data)) != 0)
		return 0;

	return data[0];
}

int rmm_cfg_get_vm_root_password(char *password, int max_len, int vm_idx)
{
	json_t *jvm;
	char attr[16] = {0};
	snprintf_s_si(attr, sizeof(attr),"%s%d", ATTR_VM_HEADER, vm_idx);

	json_t *conf = load_rmm_cfg();
	if (conf == 0) {
		printf("Load rmm config file fail...\n");
		return -1;
	}

	jvm = get_json_attr(conf, PROC_AUTO_TEST, attr);
	if (jvm == NULL)
		return -1;

	json_t *jpw = json_object_get(jvm, ATTR_VM_ROOT_PASSWORD);
	if (jpw == NULL) {
		printf("Can't find the %s attribute.\n", ATTR_VM_ROOT_PASSWORD);
		return -1;
	}
	char *data = json_string_value(jpw);
	strncpy_safe(password, data, max_len, max_len - 1);
	json_free(conf);
	return 0;
}

int rmm_cfg_get_platform(char *platform, int max_len)
{
	return get_str_attr(PROC_RMM, ATTR_RACK_PLATFORM, platform, max_len);
}

int is_platform_bdcr(void)
{
	char platform[16];
	if (rmm_cfg_get_platform(platform, sizeof(platform)) != 0) {
		fprintf(stderr, "fail to get platform string, suppose it's BDC-A.\n");
		return 0;
	}

	if (strncmp(platform, ATTR_RACK_PLATFORM_BDCR, strnlen_s(ATTR_RACK_PLATFORM_BDCR, RSIZE_MAX_STR)) == 0) {
		return 1;
	} else {
		return 0;
	}
}
