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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "liblog/log.h"
#include "libutils/list.h"
#include "libutils/rmm.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libjipmi/jrpc_ipmi.h"
#include "libutils/sock.h"
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libcfg/cfg.h"

static int global_cm_addr = 0;

unsigned char ipmi_csum(char * d, int s)
{
	unsigned char c = 0;
	for (; s > 0; s--, d++)
		c += *d;
	return -c;
}
static int dev_id_resp_handler(int result, unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	if (result == -1) {
		printf("DEVID RSP FAIL\n");
	} else {
		printf("DEVID RSP RCV'D: ");
		for (i = 0; i < rsp_len; i++)
			printf("%02X ", rsp[i]);
		printf("\n");
	}

	return 0;
}

void request_dev_id(unsigned int host, char *username, char *passwd)
{
	jipmi_msg_t req = { {0} };

	FILL_STR(req.name,		username);
	FILL_STR(req.password,	passwd);
	FILL_STR(req.rsp_type,	"UDP");
	FILL_INT(req.netfn,		0x06);
	FILL_INT(req.cmd,		0x01);
	FILL_INT(req.data_len,	0x00);

	libjipmi_rmcp_cmd(host, IPMI_RMCP_PORT, &req, dev_id_resp_handler, NULL, JIPMI_SYNC);
}

void br_msg_req_via_lan(unsigned int host, char *username, char *passwd)
{
	jipmi_msg_t req = { {0} };
	int64 error_code = 0;
	int len = 0;
	FILL_STR(req.name,		username);
	FILL_STR(req.password,	passwd);
	FILL_STR(req.rsp_type,	"UDP");
	FILL_INT(req.netfn,		0x06);
	FILL_INT(req.cmd,		0x01);
	FILL_INT(req.data_len,	len);
	libjipmi_rmcp_cmd_send_msg(host, IPMI_RMCP_PORT, &req, dev_id_resp_handler, NULL,
		0x24,
		0x00,
		0x00,
		0x00,
		IPMI_BRIDGE_MSG_SINGLE,
		JIPMI_NON_SYNC);
}


void dual_br_msg_req_via_lan(unsigned int host, char *username, char *passwd)
{
	jipmi_msg_t req = { {0} };
	int64 error_code = 0;
	int len = 0;
	FILL_STR(req.name,		username);
	FILL_STR(req.password,	passwd);
	FILL_STR(req.rsp_type,	"UDP");
	FILL_INT(req.netfn,		0x06);
	FILL_INT(req.cmd,		0x01);
	FILL_INT(req.data_len,	len);
	libjipmi_rmcp_cmd_send_msg(host, IPMI_RMCP_PORT, &req, dev_id_resp_handler, NULL,
		0x10,
		0x02,
		0x24,
		0x00,
		IPMI_BRIDGE_MSG_DUAL,
		JIPMI_NON_SYNC);
}


int main(int argc, char **argv)
{
	int rc;
	int max_fd;
	fd_set rfds;
	int port = rmm_cfg_get_port(IPMITEST_PORT);
	char *username;
	char *passwd;

	if (port == 0) {
		printf("Failed to call rmm_cfg_get_ipmi_test_port!\n");
		exit(-1);
	}
	printf("port is %d\n", port);
	rmm_log_init();
	if (libjsonrpcapi_init(JSONRPCINIT_JIPMI, port) < 0) {
		printf("libjsonrpcapi init failed.\n");
		return -1;
	}
	username = argv[1];
	passwd = argv[2];
	global_cm_addr = inet_addr("1.1.1.1");
	request_dev_id(global_cm_addr, username, passwd);
	br_msg_req_via_lan(global_cm_addr, username, passwd);
	dual_br_msg_req_via_lan(global_cm_addr, username, passwd);
	for (;;) {
		max_fd = -1;
		FD_ZERO(&rfds);

		libjsonrpcapi_callback_selectfds(&rfds, &max_fd);

		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0)
			continue;

		libjsonrpcapi_callback_processfds(&rfds);
	}

	return 0;
}

