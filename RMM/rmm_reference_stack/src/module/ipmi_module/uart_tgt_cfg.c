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
#include "libcfg/cfg.h"
#include "ipmi_log.h"
#include "libsecurec/safe_lib.h"


static int serial_index;

static void switch_uart(int fd);

/**
  *  @brief handle open serial port command
  *
  *  @param
  *  @return
  */
static int serial_open_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;
	int serial_fd = 0;

	printf("SERIAL OPEN RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	if (rsp_len == 5)
		memcpy_s(&serial_fd, sizeof(serial_fd), rsp + 1, 4);
	IPMI_LOG_DEBUG("serial fd: %d\n", serial_fd);

	switch_uart(serial_fd);

	return 0;
}

/**
  *  @brief handle switch UART response command
  *
  *  @param
  *  @return
  */
static int switch_uart_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	int i;

	printf("SERIAL OPEN RSP RCV'D: ");
	for (i = 0; i < rsp_len; i++)
		printf("%02X ", rsp[i]);
	printf("\n");

	return 0;
}

/**
  *  @brief open serial port
  *
  *  @param
  *  @return
  */
static int serial_open(char *dev)
{
	struct ipmi_msg req = {0};

	req.union_app_req.serial.serial_flag = IPMI_SERIAL_OPEN_DEV;
	req.netfn = OEM_IPMI_NETFN;
	req.cmd   = OEM_SERIAL_OPEN_CMD;

	memcpy_s(req.data, sizeof(req.data), dev, strnlen_s(dev, RSIZE_MAX_STR));

	req.data_len = strnlen_s(dev, RSIZE_MAX_STR);

	libipmi_serial_cmd(&req, serial_open_resp_handler, NULL);

	return 0;
}

/**
  *  @brief switch UART
  *
  *  @param
  *  @return
  */
static void switch_uart(int fd)
{
	struct ipmi_msg req = {0};

	req.netfn = 0x38;
	req.cmd   = 0x09;
	req.union_app_req.serial.serial_flag = IPMI_SERIAL_OPERATION;
	req.union_app_req.serial.serial_fd = fd;
	req.data[0] = serial_index;
	req.data_len = 1;

	libipmi_serial_cmd(&req, switch_uart_resp_handler, NULL);
}

int main(int argc, char **argv)
{
	int rc;
	int max_fd;
	fd_set rfds;
	int port;

	if (argc != 3) {
		printf("usage: %s <uart_dev> <uart_index>\n", argv[0]);
		printf("exampel:\n");
		printf("	uart_tgt_cfg /dev/ttyCm1Serial 2\n");
		exit(0);
	}
	port = rmm_cfg_get_port(IPMIUARTTGTCFG_PORT);
	if (libipmi_init(port) < 0)
		return -1;

	serial_index = atoi(argv[2]);

	serial_open(argv[1]);

	for (;;) {
		max_fd = -1;
		FD_ZERO(&rfds);

		libipmi_callback_selectfds(&rfds, &max_fd);

		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0)
			continue;

		libipmi_callback_processfds(&rfds);
	}

	return 0;
}
