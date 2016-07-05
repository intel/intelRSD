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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libutils/dump.h"

#include "libutils/sock.h"
#include "libutils/rmm.h"
#include "libutils/curl_ref.h"
#include "libutils/string.h"
#include "libutils/rmm_json.h"
#include "libredfish/rf_event.h"
#include "libredfish/mr_def.h"
#include "libredfish/msg_reg.h"
#include "libcfg/cfg.h"
#include "rf_memdb.h"
#include "rf_log.h"
#include "libinit/libinit.h"
#include "libsecurec/safe_lib.h"



static int32 rf_msg_idx = 1;

static void record_rmm_log(int32 level, int8 *module_name, int8 *func_name, int8 *msg)
{
	rmm_log_ex(module_name, func_name, level, "%s", msg);
}

static void record_rf_log(int32 level, int32 msg_num, int8 *str_args)
{
	RF_RECORD_EVT(level, "%d %s", msg_num, str_args);
}

static void rf_get_event_type(int8 *msg_id_str, int8 *output, int32 len)
{
	sscanf(msg_id_str, "RMM1.0.0MSG%s", output);
}

static int32 get_general_msg_sn(int32 msg_sn)
{
	switch (msg_sn) {
	case MSGRackStatusChange:
	case MSGPSUStatusChange:
	case MSGDrawerStatusChange:
	case MSGMBPStatusChange:
	case MSGFanStatusChange:
		return MSGStatusChange;
	case MSGPZoneResourceAdded:
	case MSGPSUResourceAdded:
	case MSGTZoneResourceAdded:
	case MSGFanResourceAdded:
	case MSGDrawerResourceAdded:
	case MSGMBPResourceAdded:
		return MSGResourceAdded;
	case MSGPZoneResourceRemoved:
	case MSGPSUResourceRemoved:
	case MSGTZoneResourceRemoved:
	case MSGFanResourceRemoved:
	case MSGDrawerResourceRemoved:
	case MSGMBPResourceRemoved:
		return MSGResourceRemoved;
	case MSGTZoneResourceUpdated:
	case MSGRackResourceUpdated:
		return MSGResourceUpdated;
	case MSGDrawerAlert:
		return MSGAlert;
	default:
		return 0;
	}

	return 0;
}


static int32 get_msg_origin(int32 msg_sn, int8 *output, int32 len, int8 *str_args)
{
	int8 *args[10];
	int8 prefix[PREFIX_LEN] = {0};
	int32 count = rf_get_args_count(str_args, args);

	if (rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN) == -1) {
		rmm_log(ERROR, "Fail to get rest prefix from rmm cfg and using the default value: /v1\n");
		strncpy_s(prefix, sizeof(prefix), "/v1", strnlen_s("/v1", RSIZE_MAX_STR));
	}

	switch (msg_sn) {
	case MSGRackStatusChange:
	case MSGRackResourceUpdated:
		snprintf_s_s(output, len, RF_EVENT_RACK_FMT, prefix);
		return 0;
	case MSGPZoneResourceAdded:
	case MSGPZoneResourceRemoved:
	case MSGPZoneResourceUpdated:
		snprintf_s_ss(output, len, RF_EVENT_PZONE_FMT, prefix, args[0]);
		return 0;
	case MSGTZoneResourceRemoved:
	case MSGTZoneResourceAdded:
	case MSGTZoneResourceUpdated:
		snprintf_s_ss(output, len, RF_EVENT_TZONE_FMT, prefix, args[0]);
		return 0;
	case MSGPSUStatusChange:
	case MSGPSUResourceRemoved:
	case MSGPSUResourceUpdated:
	case MSGPSUResourceAdded:
		snprintf_s_sss(output, len, RF_EVENT_PSU_FMT, prefix, args[0], args[1]);
		return 0;
	case MSGFanResourceAdded:
	case MSGFanResourceRemoved:
	case MSGFanStatusChange:
		snprintf_s_sss(output, len, RF_EVENT_FAN_FMT, prefix, args[0], args[1]);
		return 0;
	case MSGDrawerResourceRemoved:
	case MSGDrawerResourceAdded:
	case MSGDrawerStatusChange:
	case MSGDrawerAlert:
		snprintf_s_ss(output, len, RF_EVENT_DRAWER_FMT, prefix, args[0]);
		return 0;
	case MSGMBPStatusChange:
	case MSGMBPResourceRemoved:
	case MSGMBPResourceAdded:
	case MSGMBPResourceUpdated:
		snprintf_s_ss(output, len, RF_EVENT_MBP_FMT, prefix, args[0]);
		return 0;
	default:
		return -1;
	}

	return 0;
}


static int32 rf_pack_json_msg(int8 *output, int32 len, int8 *str_args, int8 *msg_id_str, int8 *msg_origin, int8 *context)
{
	int8 *args[10];
	int8 evt_type[32] = {};
	int8 odata_id[128] = {};
	int32 count = rf_get_args_count(str_args, args);
	int32 i = 0;
	json_t *result = NULL;
	json_t *args_array = NULL;
	json_t *evt = NULL;
	json_t *evt_array = NULL;
	json_t *origin = NULL;


	result = json_object();
	if (result == NULL) {
		rmm_log(ERROR, "json object request error\n");
		return -1;
	}

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, "/redfish/v1/$metadata#EventService/Members/Events/Members/$entity");
	snprintf_s_i(odata_id, sizeof(odata_id), "/redfish/v1/EventService/Events/%d", rf_msg_idx);
	add_json_string(result, RMM_JSON_ODATA_ID, odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, "#EventService.1.0.0.Event");
	add_json_integer(result, RMM_JSON_ID, rf_msg_idx);
	rf_msg_idx++;

	evt_array = json_array();
	if (evt_array == NULL) {
		rmm_log(ERROR, "msg json array request error\n");
		return -1;
	}

	evt =  json_object();
	if (evt == NULL) {
		rmm_log(ERROR, "msg json object request error\n");
		return -1;
	}

	rf_get_event_type(msg_id_str, evt_type, sizeof(evt_type));
	json_object_add(evt, RMM_JSON_MSG_EVT_TYPE, json_string(evt_type));
	json_object_add(evt, RMM_JSON_MSG_ID, json_string(msg_id_str));


	args_array = json_array();
	if (args_array == NULL) {
		rmm_log(ERROR, "args json array request error\n");
		return -1;
	}

	for (i = 0; i < count; i++) {
		json_array_add(args_array, json_string(args[i]));
	}
	json_object_add(evt, RMM_JSON_MSG_ARGS, args_array);

	json_object_add(evt, RMM_JSON_MSG_CONTEXT, json_string(context));

	origin = json_object();
	if (evt == NULL) {
		rmm_log(ERROR, "origin json object request error\n");
		return -1;
	}
	json_object_add(origin, RMM_JSON_ODATA_ID, json_string(msg_origin));
	json_object_add(evt, RMM_JSON_MSG_ORIGIN, origin);

	json_array_add(evt_array, evt);
	json_object_add(result, RMM_JSON_MSG_EVENTS, evt_array);

	json_format(result, output, len);

	return 0;
}

static void post_rf_evt(int32 msg_sn, int8 *msg_origin, struct rf_log_req_info *req_info)
{
	int8 msg[2048] = {0};
	struct dest_info *listener = NULL;
	struct dest_info *tmp;
	int8 msg_id_str[256] = {0};
	int b_https = 0;

	msg_reg_get_msg_id_str(msg_id_str, 256, msg_sn);
	rf_memdb_get_listeners(msg_id_str, &listener);

	while (listener) {
		b_https = 0;
		if (strstr(listener->dest, "http") != NULL) {
			if (strstr(listener->dest, "https") != NULL) {
				b_https = 1;
			}
			memset(msg, 0, sizeof(msg));
			rf_pack_json_msg(msg, sizeof(msg), req_info->data.fmt1.str_args, msg_id_str, msg_origin, listener->context);
			curl_post_msg(listener->dest, msg, b_https);
		}
		tmp = listener->pnext;
		free(listener);
		listener = tmp;
	}
}


/**
*
* Asset index could be queried via offset and msg args. Msg entity would be queried via msg sn.
* Redfish daemon maintain a map of msg unique string and memdb node id, we can find redfish event node via
* msg unique string which could be queried via msg sn.
*
* For redfish log recording:
* 1. All redfish msg are required to be recorded in rmm log, like other modules. It requires to query
*    rmm msg format via msg_sn and msg args
* 2. Redfish msg is also recorded in redfish log for redfish log query.
* 3. Redfish event msg will be recorded in step 1 & 2 and posted to listeners
*
* Diagram for step 1 and 2:
* .--------------------.   .----------------.   .------------.     .-------------.
* |query location index|-->|query msg entity|-->| record log |--> < process event >
* `--------------------'   `----------------'   `------------'     `-------------"
*
* Diagram for step 3:
* .------------------.   .-----------------------.   .------------------.
* |query msg identity|-->|query listeners via map|-->|post redfish event|
* `------------------'   `-----------------------'   `------------------'
*/
static int32 rf_msg_handler(struct rf_log_req_info *req_info)
{
	int32 location_idx = 0;
	int8 unified_id_str[256] = {0};
	int8 msg[2048] = {0};
	int8 msg_origin[256] = {0};

	int32 msg_sn = req_info->data.fmt1.msg_sn;
	int32 msg_sn_general = get_general_msg_sn(msg_sn);
	int32 level = req_info->data.fmt1.level;

	if (msg_reg_get_msg_str(msg, msg_sn, req_info->data.fmt1.str_args) == RF_ERR)
		return -1;

	printf("msg:		%s\n", msg);

	/*record log and event.*/
	record_rf_log(level, msg_sn, req_info->data.fmt1.str_args);

	record_rmm_log(level,
			req_info->data.fmt1.module_name,
			req_info->data.fmt1.func_name,
			msg);

	/*If the request is event, post event to destination*/
	if (req_info->data.fmt1.is_event == RF_EVENT) {
		get_msg_origin(msg_sn, msg_origin, sizeof(msg_origin), req_info->data.fmt1.str_args);
		post_rf_evt(msg_sn, msg_origin, req_info);
		post_rf_evt(msg_sn_general, msg_origin, req_info);
	}

	return 0;
}

static void set_socket_addr(struct sockaddr_in *addr, int32 port)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

static void parser_log(int8 *data, int8 *out)
{
	int32 line_num = 0;
	int8 date[16] = {0};
	int8 ct[32] = {0};
	int8 level[16] = {0};
	int8 func_name[32] = {0};
	int32 msg_sn = 0;
	int8 str_args[128] = {0};
	int len = 0;

	int8 msg[RF_MSG_MAX_LEN] = {0};

	if (sscanf(data, "%d %s %s %s %s %d %s", &line_num, date, ct, level, func_name, &msg_sn, str_args) == -1)
		rmm_log(ERROR, "%s", "sscanf return error.\n");
	msg_reg_get_msg_str(msg, msg_sn, str_args);
	//snprintf(out, RF_MSG_MAX_LEN, "%d %s %s %s %s %s", line_num, date, ct, level, func_name, msg);

	len = snprintf_s_i(out, RF_MSG_MAX_LEN, "%d", line_num);
	len += snprintf_s_s(out+len, RF_MSG_MAX_LEN-len, " %s", date);
	len += snprintf_s_ssss(out+len, RF_MSG_MAX_LEN-len, "%s %s %s %s", ct, level, func_name, msg);
}

static void parser_logs(int8 *resp_data, int32 read_lines_count)
{
	int32 index = 0;
	int8 out[RF_MSG_MAX_LEN] = {0};

	for (index = 0; index < read_lines_count; index++) {
		memset(out, 0, RF_MSG_MAX_LEN);
		parser_log(resp_data + RF_MSG_MAX_LEN * index, out);
		strncpy_safe(resp_data + RF_MSG_MAX_LEN * index, out, RF_MSG_MAX_LEN, RF_MSG_MAX_LEN - 1);
	}
}

static void rf_get_log_handler(int32 fd, struct sockaddr_in addr_from, int32 msg_req_count)
{
	struct rf_log_desc	resp_disc;
	int8 *resp_data = NULL;
	int32 val = 0;
	int32 read_lines_count = 0;
	int32 msg_count = 0;
	int32 index = 0;
	int32 msg_length = 0;
	int32 addrlen = sizeof(addr_from);

	if (msg_req_count > 0) {
		resp_data = (int8 *)malloc(abs(msg_req_count) * RF_MSG_MAX_LEN);
		read_lines_count = rf_log_get(msg_req_count, resp_data);
		parser_logs(resp_data, read_lines_count);
		resp_disc.count = read_lines_count;
		resp_disc.length = read_lines_count * RF_MSG_MAX_LEN;
	} else {
		resp_disc.count = 0;
		resp_disc.length = 0;
	}

	val = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, val & ~O_NONBLOCK) < 0)
		return;

	sendto(fd, (uint8 *)(&resp_disc), sizeof(struct rf_log_desc), 0, (struct sockaddr *)&addr_from, addrlen);
	usleep(100);
	while (index * BUF_SIZE < resp_disc.length) {
		if ((index  + 1) * BUF_SIZE > resp_disc.length)
			msg_length = resp_disc.length - index * BUF_SIZE;
		else
			msg_length = BUF_SIZE;
		sendto(fd, (uint8 *)resp_data + index * BUF_SIZE, msg_length, 0, (struct sockaddr *)&addr_from, addrlen);
		usleep(100);
		index++;
	}

	if (fcntl(fd, F_SETFL, val & O_NONBLOCK) < 0)
		return;
	if (resp_data)
		free(resp_data);
	resp_data = NULL;
}

static void sigterm_handler(int32 signum)
{
	/* do cleanup jobs here */
	exit(0);
}

int32 main(int32 argc, int8 **argv)
{
	int32 rc;
	int32 fd;
	fd_set fds;
	int32 port;
	socklen_t addrlen;
	struct sockaddr_in addr_from;
	struct rf_log_req_info req_info;

	reg_sigterm_handler(sigterm_handler);

	if(rmm_modules_init(MODULEINIT_COREDUMP | MODULEINIT_LOG, 0,0))
		exit(-1);

	port = rmm_cfg_get_port(REDFISHD_PORT);
	if (port == 0) {
		rmm_log(ERROR, "%s", "Fail to call rmm_cfg_get_redfishd_port.\n");
		exit(-1);
	}
	/*Prepare to listening information.*/

	fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
	if (fd < 0)
		return -1;

	printf("redfish daemon is Running ...\n");

	/*redfish log init.*/
	rf_log_init();

	/*Redfish MessageRegistry init, load default config file.*/
	msg_reg_init(NULL);
	rf_memdb_event_node_init();
	curl_init();

	set_socket_addr(&addr_from, port);
	addrlen = sizeof(addr_from);
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0)
			continue;

		rc = recvfrom(fd, &req_info, sizeof(struct rf_log_req_info), 0, (struct sockaddr *)&addr_from, &addrlen);
		if (rc <= 0)
			continue;
		switch (req_info.type) {
		case RF_MSG_REQ:
			rf_msg_handler(&req_info);
			break;
		case RF_GET_LOG_BY_LIMIT:
			rf_get_log_handler(fd, addr_from, req_info.data.fmt2.count);
			break;
		default:
			printf("Error message type: %d", req_info.type);
			break;
		}
	}

	close(fd);
	curl_uninit();
	return 0;
}

