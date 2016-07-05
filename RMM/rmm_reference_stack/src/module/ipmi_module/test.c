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


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libutils/rmm.h"
#include "libipmi/ipmi.h"

#define TEST_NDISC_IPMI_NETFN	0x32
#define TEST_NDISC_IPMI_CMD		0x05
#define TEST_APP_IPMI_NETFN		0x06
#define TEST_GET_DEVID_CMD		0x01

#define OEM_NETFN				(0x30)

#define TEST_BRIDGE_IPMI_NETFN	0x06
#define TEST_BRIDGE_IPMI_CMD	0x34

#define TEST_PSU_IPMI_NETFN     0x38
#define TEST_SET_PSU_CMD        0x0e


static int global_cm_addr;

#if 0
static int dev_id_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("DEVID RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	usleep(100*1000);
	/*request_dev_id(global_cm_addr);*/

	return 0;
}


static void request_dev_id(unsigned int host)
{
	struct ipmi_msg req = {0};

	req.netfn = 0x6;/*TEST_APP_IPMI_NETFN;*/
	req.cmd   = 0x1;/*TEST_GET_DEVID_CMD;*/
	req.data_len = 0;

	libipmi_rmcp_cmd(host, &req, dev_id_resp_handler, NULL);
}
#endif

static int dev_id_resp_handler_br(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("DEVID BR RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	usleep(100*1000);
	/*ipmi_br_req_test(); */

	return 0;
}




static void ipmi_br_req_test(void)
{
	unsigned int host = global_cm_addr;
	struct ipmi_msg req;

	req.netfn = 0x06;/*TEST_APP_IPMI_NETFN; */
	req.cmd   = 0x01;/*TEST_GET_DEVID_CMD; */
	req.data_len = 0;

	libipmi_rmcp_cmd_send_msg(host, &req, dev_id_resp_handler_br, NULL, 0x20, 0x03, 0, 0, IPMI_BRIDGE_MSG_SINGLE);

}



#if 0
static int global_serial_fd = -1;
static int global_psu_id = 1;
static int global_cm_addr;

static void request_dev_id_via_serial(int fd);
static void ipmi_br_req_test(void);
static void request_dev_id(unsigned int host);
static void request_psu_current_out_via_lan(void);
static void request_psu_power_in_via_lan(void);



struct ipmi_test_br_info {
	unsigned int   host;
	unsigned long  my_addr;
	unsigned long  target_addr;
	unsigned long  target_channel;
	unsigned char  br_netfn;
	unsigned char  br_cmd;
	unsigned short data_len;
	unsigned char  data[IPMI_MAX_DATA_LENGTH];
};

static int dev_id_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("DEVID RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	usleep(100*1000);
	/*request_dev_id(global_cm_addr);*/

	return 0;
}


static int dev_id_resp_handler_br(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("DEVID BR RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	usleep(100*1000);
	/*ipmi_br_req_test();*/

	return 0;
}


static int serial_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("SERIAL RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	if (rsp_len == 5)
		memcpy(&global_serial_fd, rsp + 1, rsp_len);
	IPMI_LOG_DEBUG("serial fd: %d\n", global_serial_fd);

	return 0;
}

static void request_get_ip_via_serial(int fd)
{

	struct ipmi_msg req = {0};

	req.netfn = 0x0C;
	req.cmd   = 0x02;
	req.data[0] = 0x01;
	req.data[1] = 0x03;
	req.data[2] = 0x00;
	req.data[3] = 0x00;

	req.data_len = 4;

	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = fd;

	libipmi_serial_cmd(&req, dev_id_resp_handler, NULL);
}

static void request_dev_id(unsigned int host)
{
	struct ipmi_msg req = {0};

	req.netfn = 0x38;/*TEST_APP_IPMI_NETFN;*/
	req.cmd   = 0x02;/*TEST_GET_DEVID_CMD;*/
	req.data_len = 0;

	libipmi_rmcp_cmd(host, &req, dev_id_resp_handler, NULL);
}

static int serial_open(char *dev)
{
	struct ipmi_msg req = {0};

	req.serial_flag = IPMI_SERIAL_OPEN_DEV;
	req.netfn = OEM_NETFN;
	req.cmd   = OEM_SERIAL_OPEN_CMD;

	memcpy(req.data, dev, strlen(dev));

	req.data_len = strlen(dev);

	libipmi_serial_cmd(&req, serial_resp_handler, NULL);

	return 0;
}

static int serial_close(char *dev)
{
	struct ipmi_msg req = {0};

	req.serial_flag = IPMI_SERIAL_CLOSE_DEV;
	req.netfn = OEM_NETFN;
	req.cmd   = OEM_SERIAL_CLOSE_CMD;

	memcpy(req.data, dev, strlen(dev));

	req.data_len = strlen(dev);

	libipmi_serial_cmd(&req, serial_resp_handler, NULL);

	return 0;
}

static void request_reset_via_serial(int fd)
{
	struct ipmi_msg req = {0};

	req.netfn = 0x38;
	req.cmd   = 0x01;
	req.data[0] = 0x01;
	req.data_len = 1;
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = fd;

	libipmi_serial_cmd(&req, dev_id_resp_handler, NULL);
}


static void request_dev_id_via_serial(int fd)
{
	struct ipmi_msg req = {0};

	req.netfn = TEST_APP_IPMI_NETFN;
	req.cmd   = TEST_GET_DEVID_CMD;
	req.serial_flag = IPMI_SERIAL_OPERATION;
	req.serial_fd = fd;
	req.data_len = 0;

	libipmi_serial_cmd(&req, dev_id_resp_handler, NULL);
}

static void ipmi_br_req_test(void)
{
	unsigned int host = global_cm_addr;
	struct ipmi_msg req;

	req.netfn = 0x06;/*TEST_APP_IPMI_NETFN;*/
	req.cmd   = 0x01;/*TEST_GET_DEVID_CMD;*/
	req.data_len = 0;

	libipmi_rmcp_cmd_send_msg(host, &req, dev_id_resp_handler_br, NULL , 0x20, 0x04);

}

static int ipmi_ndisc_rsp_handler(unsigned char *rsp, int rsp_len,
		void *cb_data)
{
	int i;

	printf("NDISC RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	return 0;
}

static void send_ndisc_req(unsigned char sa)
{
	struct ipmi_msg req;

	req.netfn = TEST_NDISC_IPMI_NETFN;
	req.cmd   = TEST_NDISC_IPMI_CMD;
	req.data_len = 4;
	req.data[0] = 0x03;
	req.data[1] = 0x00;
	req.data[2] = 0x00;
	req.data[3] = IPMI_IPMB_ADDR;

	libipmi_ipmb_cmd(sa, &req, ipmi_ndisc_rsp_handler, NULL);
}

static int ipmi_ndisc_req_handler(unsigned char *req, int req_len,
		unsigned char *resp)
{
	int i;

	printf("NDISC REQ RCV'D: ");
	for (i = 0; i < req_len; i++)
		printf("%02X ", req[i]);
	printf("\n");

	resp[0] = IPMI_CC_OK;
	resp[1] = 0x03;
	resp[2] = 0x00;
	resp[3] = 0x00;
	resp[4] = IPMI_IPMB_ADDR;

	return 5;
}

static int psu_power_in_rsp_cb(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;
	char buff[1024];
	char hdr[64];
	char body[768] = {0};
	char end[64];
	char tmp[8];

	snprintf(hdr, sizeof(hdr), "PSU POWER IN RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%02X ", rsp[i]);
		strncat(body, tmp, 8);
	}
	snprintf(end, sizeof(end), "\n");
	snprintf(buff, sizeof(buff), "%s%s%s", hdr, body, end);

	log_ifo(buff);

	if (global_psu_id <= 6) {
		request_psu_current_out_via_lan();
		global_psu_id++;
	}

	return 0;
}


static void request_psu_power_in_via_lan(void)
{
	struct ipmi_msg req = {0};

	req.netfn = TEST_PSU_IPMI_NETFN;
	req.cmd   = TEST_SET_PSU_CMD;
	req.data[0] = global_psu_id;
	req.data[1] = 0x97;
	req.data_len = 2;

	libipmi_rmcp_cmd(global_cm_addr, &req, psu_power_in_rsp_cb, NULL);
}

static int psu_current_out_rsp_cb(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;
	char buff[1024];
	char hdr[64];
	char body[768] = {0};
	char end[64];
	char tmp[8];

	snprintf(hdr, sizeof(hdr), "PSU CURRENT OUT RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++) {
		snprintf(tmp, sizeof(tmp), "%02X ", rsp[i]);
		strncat(body, tmp, 8);
	}
	snprintf(end, sizeof(end), "\n");
	snprintf(buff, sizeof(buff), "%s%s%s", hdr, body, end);

	log_ifo(buff);

	if (global_psu_id <= 6) {
		request_psu_power_in_via_lan();
	}

	return 0;
}


static void request_psu_current_out_via_lan(void)
{
	struct ipmi_msg req = {0};

	req.netfn = TEST_PSU_IPMI_NETFN;
	req.cmd   = TEST_SET_PSU_CMD;
	req.data[0] = global_psu_id;
	req.data[1] = 0x8c;
	req.data_len = 2;

	libipmi_rmcp_cmd(global_cm_addr, &req, psu_current_out_rsp_cb, NULL);
}
#endif

int main(int argc, char **argv)
{
/*#if 0 */
	int rc;
	int max_fd;
	fd_set rfds;

	if (argc != 2) {
		printf("Too short arguments, the righ format is \"test_ipmi xx.xx.xx.xx\"\n");
		return -1;
	}

	global_cm_addr = inet_addr(argv[1]);

	/*log_init("test_ipmi"); */

	if (libipmi_init(IPMI_TEST_PORT) < 0)
		return -1;

/*	libipmi_cmd_subscribe(TEST_NDISC_IPMI_NETFN, TEST_NDISC_IPMI_CMD, */
/*				ipmi_ndisc_req_handler); */

/*	send_ndisc_req(IPMI_IPMB_BCAST_ADDR); */
/*	send_ndisc_req(0x04); */

/*	request_dev_id(inet_addr("192.168.0.8")); */
/*#if 0 */
	/*int tmp_fd = 0; */

#if 0
	int i = 0;

	for (i = 0; i < strlen(argv[1]); i++) {
		tmp_fd = tmp_fd * 10 + (argv[1][i] - '0');
	}
#endif

/*	serial_open(argv[1]); */
/*	request_dev_id_via_serial(tmp_fd); */
/*	serial_close(argv[1]); */
/*	request_get_ip_via_serial(tmp_fd); */
/*	request_reset_via_serial(tmp_fd); */
/*#endif */
	ipmi_br_req_test();
	/*request_dev_id(global_cm_addr); */
	/*request_psu_power_in_via_lan(); */

	for (;;) {
		/*ipmi_br_req_test();*/
		/*usleep(10*1000); */

		max_fd = -1;
		FD_ZERO(&rfds);

		libipmi_callback_selectfds(&rfds, &max_fd);

		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0)
			continue;

		libipmi_callback_processfds(&rfds);
	}
/*#endif */
	return 0;
}

