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


#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include "errno.h"

#include "libutils/types.h"
#include "libutils/sock.h"
#include "libutils/rmm.h"
#include "libjipmi/common.h"
#include "libjipmi/jrpc_ipmi.h"
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libcfg/cfg.h"
#include "libutils/base64.h"
#include "libsecurec/safe_lib.h"



static void send_udp_msg(int fd, unsigned int ip, unsigned short port, unsigned char *msg, unsigned short len)
{
	struct sockaddr_in dest;

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);/** htonl(ip) */
	sendto(fd, msg, len, 0, (struct sockaddr *)&dest, sizeof(dest));
}

static int jrpc_rmcp_req_string_build(unsigned char *method, int64 id,
			unsigned char *ipmi_data, unsigned short len,
			unsigned char *jrpc_data, unsigned int jrpc_data_len)
{
	jrpc_param_t param[9] = { { 0 } };
	json_t *req;
	int rs_len = 0;
	char buffer[IPMI_JSONRPC_MAX_STRING_LEN] = {0};
	jrpc_data_string str_buf_len = "1";
	jrpc_data_string cmd = "37";
	jrpc_data_string netfn = "56";
	jrpc_data_string password = "Admin";
	jrpc_data_string username = "Admin";
	jrpc_data_string target_ip = "127.0.0.1";
	jrpc_data_string rsp_addr = "8001";
	jrpc_data_string rsp_type = "UDP";

	base64_encode(buffer, (char *)ipmi_data, (int)len);

	param[0].name = "data_len";
	param[0].value = (void *)str_buf_len;
	param[0].value_type = JSON_STRING;

	param[1].name = "data";
	param[1].value = (void *)buffer;
	param[1].value_type = JSON_STRING;

	param[2].name = "rsp_type";
	param[2].value = (void *)rsp_type;
	param[2].value_type = JSON_STRING;

	param[3].name = "rsp_addr";
	param[3].value = (void *)rsp_addr;
	param[3].value_type = JSON_STRING;

	param[4].name = "target_ip";
	param[4].value = (void *)target_ip;
	param[4].value_type = JSON_STRING;

	param[5].name = "username";
	param[5].value = (void *)username;
	param[5].value_type = JSON_STRING;

	param[6].name = "password";
	param[6].value = (void *)password;
	param[6].value_type = JSON_STRING;

	param[7].name = "netfn";
	param[7].value = (void *)netfn;
	param[7].value_type = JSON_STRING;

	param[8].name = "cmd";
	param[8].value = (void *)cmd;
	param[8].value_type = JSON_STRING;

	req = jrpc_create_req(id, (char *)method, sizeof(param)/sizeof(jrpc_param_t), param);

	if ((NULL != req) && (NULL != jrpc_data)) {
		if (jrpc_format_string(req, (char *)jrpc_data, (int)jrpc_data_len) == JSONRPC_FAILED)
			printf("jrpc_data len failed!");
		else {
			printf("jrpc_data is : %s\n", jrpc_data);
			rs_len = strnlen_s((const char *)jrpc_data, JSONRPC_MAX_STRING_LEN);
		}
	}

	return rs_len;
}

static int jrpc_serial_req_string_build(unsigned char *method, int64 id,
					unsigned char *ipmi_data, unsigned short len,
					unsigned char *jrpc_data, unsigned int jrpc_data_len)
{
	jrpc_param_t param[10] = { { 0 } };
	json_t *req;
	int rs_len = 0;
	char buffer[IPMI_JSONRPC_MAX_STRING_LEN] = {0};
	jrpc_data_string str_buf_len = "0";
	jrpc_data_string serial_flag = "2";
	jrpc_data_string cmd = "1";
	jrpc_data_string netfn = "6";
	jrpc_data_string password = "Admin";
	jrpc_data_string username = "Admin";
	jrpc_data_string serial_fd = "7";
	jrpc_data_string rsp_addr = "8001";
	jrpc_data_string rsp_type = "UDP";

	if (len > 0)
		base64_encode(buffer, (char *)ipmi_data, (int)len);

	param[0].name = "data_len";
	param[0].value = (void *)str_buf_len;
	param[0].value_type = JSON_STRING;

	param[1].name = "data";
	param[1].value = (void *)buffer;
	param[1].value_type = JSON_STRING;

	param[2].name = "rsp_type";
	param[2].value = (void *)rsp_type;
	param[2].value_type = JSON_STRING;

	param[3].name = "rsp_addr";
	param[3].value = (void *)rsp_addr;
	param[3].value_type = JSON_STRING;

	param[4].name = "serial_flag";
	param[4].value = (void *)serial_flag;
	param[4].value_type = JSON_STRING;

	param[5].name = "username";
	param[5].value = (void *)username;
	param[5].value_type = JSON_STRING;

	param[6].name = "password";
	param[6].value = (void *)password;
	param[6].value_type = JSON_STRING;

	param[7].name = "netfn";
	param[7].value = (void *)netfn;
	param[7].value_type = JSON_STRING;

	param[8].name = "cmd";
	param[8].value = (void *)cmd;
	param[8].value_type = JSON_STRING;

	param[9].name = "serial_fd";
	param[9].value = (void *)serial_fd;
	param[9].value_type = JSON_STRING;

	req = jrpc_create_req(id, (char *)method, sizeof(param)/sizeof(jrpc_param_t), param);

	if ((NULL != req) && (NULL != jrpc_data)) {
		if (jrpc_format_string(req, (char *)jrpc_data, (int)jrpc_data_len) == JSONRPC_FAILED)
			printf("jrpc_data len failed!");
		else {
				printf("jrpc_data is : %s\n", jrpc_data);
			rs_len = strnlen_s((const char *)jrpc_data, JSONRPC_MAX_STRING_LEN);
		}
	}

	return rs_len;
}

static int jrpc_rmcp_req_send(int fd, ipmi_json_ipc_header_t header, unsigned char *ipmi_data, unsigned short len)
{
	unsigned char buffer[IPMI_JSONRPC_MAX_STRING_LEN] = {0};
	int str_len = 0;

	if ((str_len = jrpc_rmcp_req_string_build(header.method, header.json_ipc_id, ipmi_data, len, buffer, IPMI_JSONRPC_MAX_STRING_LEN)) > IPMI_JSONRPC_MAX_STRING_LEN) {
		printf("jrpc_string_build len is 0\n");
		return -1;
	}
	send_udp_msg(fd, header.ip, header.port, buffer, str_len);
	return 0;
}

static int jrpc_serial_req_send(int fd, ipmi_json_ipc_header_t header, unsigned char *ipmi_data, unsigned short len)
{
	unsigned char buffer[IPMI_JSONRPC_MAX_STRING_LEN] = {0};
	int str_len = 0;

	if ((str_len = jrpc_serial_req_string_build(header.method, header.json_ipc_id, ipmi_data, len, buffer, IPMI_JSONRPC_MAX_STRING_LEN)) > IPMI_JSONRPC_MAX_STRING_LEN) {
		printf("jrpc_string_build len is 0\n");
		return -1;
	}
	send_udp_msg(fd, header.ip, header.port, buffer, str_len);
	return 0;
}

int main(int argc, char **argv)
{
	int max_fd;
	fd_set rfds;
	struct timeval time_out;
	ipmi_json_ipc_header_t header;
	unsigned char ipmi_data[] = "1";
	int fd_client = -1;

	fd_client = create_udp_listen(INADDR_LOOPBACK, rmm_cfg_get_port(IPMIJSONRPC_IPMID_PORT)+2, 0, 1);
	if (fd_client == -1) {
		printf("Failed to init socket to app!\n");
		return -1;
	}

	if (0) {
		header.ip = inet_addr("127.0.0.1");
		header.json_ipc_id = 2;
		memcpy_s(header.method, sizeof(header.method), "rmcp_req", sizeof("rmcp_req"));
		header.port = (uint16)rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT);
		jrpc_rmcp_req_send(fd_client, header, ipmi_data, 5);
	} else {
		header.ip = inet_addr("127.0.0.1");
		header.json_ipc_id = 2;
		memcpy_s(header.method, sizeof(header.method), "serial_req", sizeof("serial_req"));
		header.port = (uint16)rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT);
		jrpc_serial_req_send(fd_client, header, ipmi_data, 5);
	}

	return 0;
}
