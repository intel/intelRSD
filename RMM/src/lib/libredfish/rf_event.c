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


#include <stdlib.h>
#include <stdarg.h>

#include "libutils/rack.h"
#include "libutils/sock.h"
#include "libutils/string.h"
#include "libredfish/rf_types.h"
#include "libredfish/msg_reg.h"
#include "libredfish/snmp_event.h"
#include "libsecurec/safe_lib.h"


#define _GNU_SOURCE
extern const int8 *__progname;

static int32 redfish_fd	= -1;
static int32 snmp_subagentd_fd = -1;

const id_evt_type_t rf_evt_msg[MAX_EVT_ACTION_NUM] = { 
	{MC_REDFISH_RACK_CHANGE, RF_EVENT_TYPE_RACK_CHANGE_STR},
	{MC_REDFISH_RACK_UPDATE, RF_EVENT_TYPE_RACK_UPDATE_STR},

	{MC_REDFISH_PZONE_ADD, RF_EVENT_TYPE_PZONE_ADD_STR},
	{MC_REDFISH_PZONE_REMOVE,    RF_EVENT_TYPE_PZONE_REMOVE_STR},
	{MC_REDFISH_PSU_CHANGE, RF_EVENT_TYPE_PSU_CHANGE_STR},
	{MC_REDFISH_PSU_ADD,  RF_EVENT_TYPE_PSU_ADD_STR},
	{MC_REDFISH_PSU_REMOVE, RF_EVENT_TYPE_PSU_REMOVE_STR},

	{MC_REDFISH_TZONE_ADD, RF_EVENT_TYPE_TZONE_ADD_STR},
	{MC_REDFISH_TZONE_REMOVE, RF_EVENT_TYPE_TZONE_REMOVE_STR},
	{MC_REDFISH_TZONE_UPDATE, RF_EVENT_TYPE_TZONE_UPDATE_STR},
	{MC_REDFISH_FAN_ADD,  RF_EVENT_TYPE_FAN_ADD_STR},
	{MC_REDFISH_FAN_REMOVE, RF_EVENT_TYPE_FAN_REMOVE_STR},
	{MC_REDFISH_FAN_CHANGE, RF_EVENT_TYPE_FAN_CHANGE_STR},

	{MC_REDFISH_DRAWER_CHANGE,    RF_EVENT_TYPE_DRAWER_CHANGE_STR},
	{MC_REDFISH_DRAWER_ADD, RF_EVENT_TYPE_DRAWER_ADD_STR},
	{MC_REDFISH_DRAWER_REMOVE,  RF_EVENT_TYPE_DRAWER_REMOVE_STR},
	{MC_REDFISH_DRAWER_ALERT, RF_EVENT_TYPE_DRAWER_ALERT_STR},

	{MC_REDFISH_MBP_CHANGE,    RF_EVENT_TYPE_MBP_CHANGE_STR},
	{MC_REDFISH_MBP_ADD, RF_EVENT_TYPE_MBP_ADD_STR},
	{MC_REDFISH_MBP_REMOVE,  RF_EVENT_TYPE_MBP_REMOVE_STR},

	{MC_REDFISH_RESOURCE_CHANGE, RF_EVENT_TYPE_CHANGE_STR},
	{MC_REDFISH_RESOURCE_UPDATE, RF_EVENT_TYPE_UPDATE_STR},
	{MC_REDFISH_RESOURCE_ADD,    RF_EVENT_TYPE_ADD_STR},
	{MC_REDFISH_RESOURCE_REMOVE, RF_EVENT_TYPE_DELETE_STR},
	{MC_REDFISH_RESOURCE_ALERT,  RF_EVENT_TYPE_ALERT_STR},
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};


int32 rf_connect(uint32 host, int32 port)
{
	redfish_fd = udp_connect(host, port);
	if (redfish_fd < 0) {
		return -1;
	}

	msg_reg_init(NULL);

	return 0;
}

int32 snmp_subagentd_connect(uint32 host, int32 port)
{
	snmp_subagentd_fd = udp_connect(host, port);
	if (snmp_subagentd_fd < 0) {
		return -1;
	}

	msg_reg_init(NULL);

	return 0;
}

static int32 _msg_sent(int32 fd, const int8 *func_name, int32 level, int32 is_evt, int32 msg_sn, int8 *args)
{
	struct rf_log_req_info req_info;

	req_info.type = RF_MSG_REQ;
	req_info.data.fmt1.is_event = is_evt;
	req_info.data.fmt1.msg_sn = msg_sn;
	req_info.data.fmt1.level = level;
	strncpy_safe(req_info.data.fmt1.func_name, func_name, MAX_FUNC_NAME_LEN, MAX_FUNC_NAME_LEN - 1);
	strncpy_safe(req_info.data.fmt1.module_name, __progname, MAX_MODULE_NAME_LEN, MAX_MODULE_NAME_LEN - 1);
	strncpy_safe(req_info.data.fmt1.str_args, args, RF_MSG_MAX_LEN, RF_MSG_MAX_LEN - 1);

	return socket_send(fd,	&req_info, sizeof(struct rf_log_req_info));
}

int32 rf_log_get_by_limit(int32 count, struct rf_log_desc *desc_rsp, int8 *data)
{
	int32 rc = -1;
	int32 val;
	int32 index = 0;
	int32 msg_length = 0;
	struct rf_log_req_info req_info;

	req_info.type = RF_GET_LOG_BY_LIMIT;
	req_info.data.fmt2.count = count;
	val = fcntl(redfish_fd, F_GETFL, 0);
	if (fcntl(redfish_fd, F_SETFL, val & ~O_NONBLOCK) < 0)
		return -1;

	rc = send(redfish_fd, &req_info, sizeof(struct rf_log_req_info), 0);
	if (rc < 0)
		goto expt_end;

	usleep(10);
	rc = socket_recv(redfish_fd, desc_rsp, sizeof(struct rf_log_desc));
	if (rc <= 0) {
		goto expt_end;
	}
	printf("info length: %d", desc_rsp->length);
	if (desc_rsp->length) {
		while (index * BUF_SIZE < desc_rsp->length) {
			if ((index  + 1) * BUF_SIZE > desc_rsp->length)
				msg_length = desc_rsp->length - index * BUF_SIZE;
			else
				msg_length = BUF_SIZE;
			rc = socket_recv(redfish_fd, data + index * BUF_SIZE, msg_length);
			if (rc <= 0) {
				goto expt_end;
			}
			index++;
		}
	}
	rc = 0;
expt_end:
	fcntl(redfish_fd, F_SETFL, val & O_NONBLOCK);
	return rc;
}


/**
* Before sending redfish msg, need to transform msg into compact redfish msg format
*
*/
int32 rf_msg_send(const int8 *func_name, int32 level, int32 is_evt, int32 msg_sn, ...)
{
	int32 ret = 0;
	int8 msg_args_fmt[RF_MSG_MAX_LEN] = {0};
	int8 str_args[RF_MSG_MAX_LEN] = {0};
	va_list args;

	/*get msg args types, such as "%s,%d,%d,%s" to express the args format*/
	ret = msg_reg_get_args_format(msg_args_fmt, RF_MSG_MAX_LEN, msg_sn);
	if (ret) {
		printf("get msg type failed, ret is %d .......\n", ret);
		return RF_ERR;
	}

	va_start(args, msg_sn);
	vsnprintf(str_args, RF_MSG_MAX_LEN, msg_args_fmt, args);
	va_end(args);

	_msg_sent(redfish_fd, func_name, level, is_evt, msg_sn, str_args);

	return RF_SUCCESS;
}

int32 snmp_subagentd_msg_send(const int8 *func_name, int32 level, int32 is_evt, int32 msg_sn, ...)
{
	int32 ret = 0;
	int8 msg_args_fmt[RF_MSG_MAX_LEN] = {0};
	int8 str_args[RF_MSG_MAX_LEN] = {0};
	va_list args;

	/*get msg args types, such as "%s,%d,%d,%s" to express the args format*/
	ret = msg_reg_get_args_format(msg_args_fmt, RF_MSG_MAX_LEN, msg_sn);
	if (ret) {
		printf("get msg type failed, ret is %d .......\n", ret);
		return RF_ERR;
	}

	va_start(args, msg_sn);
	vsnprintf(str_args, RF_MSG_MAX_LEN, msg_args_fmt, args);
	va_end(args);

	_msg_sent(snmp_subagentd_fd, func_name, level, is_evt, msg_sn, str_args);

	return RF_SUCCESS;
}

int32 snmp_subagentd_set(int32 is_remove, int8 *ip, int32 port)
{
	struct rf_log_req_info req_info;
	memset(&req_info, 0, sizeof(struct rf_log_req_info));

	req_info.type = RF_SET_TRAP_IP;
	req_info.data.fmt3.is_remove = is_remove;
	memcpy_s(req_info.data.fmt3.ip, sizeof(req_info.data.fmt3.ip), ip, strnlen_s(ip, RSIZE_MAX_STR));
	req_info.data.fmt3.port = port;
	return socket_send(snmp_subagentd_fd, &req_info, sizeof(struct rf_log_req_info));
}

int32 rf_get_fixed_info(rf_fixed_t *fixed)
{
	snprintf_s_s(fixed->copy_right, sizeof(fixed->copy_right), "%s", RF_COPY_RIGHT);
	snprintf_s_s(fixed->id, sizeof(fixed->id), "%s", RF_EVENT_SERVICE_STR);
	snprintf_s_s(fixed->name, sizeof(fixed->name), "%s", RF_EVENT_SERVICE_NAME);
	snprintf_s_s(fixed->odata_type, sizeof(fixed->odata_type), "%s", RF_EVENT_ODATA_TYPE_EVT);
	snprintf_s_s(fixed->status.health, sizeof(fixed->status.health), "%s", RF_HEALTH_STATUS_OK);
	snprintf_s_s(fixed->status.state, sizeof(fixed->status.state), "%s", RF_ENABLE_STATUS_EN);
	fixed->service_enabled = true;
	fixed->retry = 0;
	fixed->retry_interval = 0;

	return 0;
}

