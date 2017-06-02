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
#include <stdio.h>
#include <netinet/in.h>

#include "libutils/sock.h"
#include "libcfg/cfg.h"

int main(int argc, char **argv)
{
	int rc;
	int fd = -1;
	fd_set fds;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	char buff[1024] = {0};
	int port; 

	port = rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT);
	printf("port is %d\n", port);
	if (port == 0) {
		printf("Failed to call rmm_cfg_get_ipmi_json_rpc_server_port!\n");
		exit(-1);
	}
	fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 1);
	if(fd == -1) {
        printf("Failed to init socket to app!\n");
		return -1;
	}

	for (;;) {
	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	rc = select(fd + 1, &fds, NULL, NULL, NULL);
	if (rc < 0)
		continue;

	if (FD_ISSET(fd, &fds))/*receive from app*/
		rc = recvfrom(fd, buff, 1024, 0, (struct sockaddr *)&addr, &addr_len);

	if (rc <= 0)
		continue;
	printf("test json ipmid: rcv msg is %s\n", buff);
	}

	close(fd);
	return 0;
}
