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


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "libutils/log_level.h"
#include "libutils/sock.h"
#include "log_manager.h"
#include "libcfg/cfg.h"
#include "libutils/dump.h"

int main(int argc, char **argv)
{
	int rc;
	int fd;
	fd_set fds;
	socklen_t addrlen;
	int rmm_logd_port = 0;

	struct sockaddr_in addr;
	struct log_evt evt;

	printf("RMM log daemon is Running ...\n");
	enable_core_dump();
	addrlen = sizeof(addr);

	rmm_logd_port = rmm_cfg_get_port(LOGD_PORT);
	if (rmm_logd_port == 0) {
		printf("load rmm logd port fail...\n");
		exit(-1);
	}

	fd = create_udp_listen(INADDR_LOOPBACK, rmm_logd_port, 0, 0);
	if (fd < 0)
		return -1;

	log_mgr_init();

	for (;;) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0)
			continue;

		rc = recvfrom(fd, &evt, sizeof(struct log_evt), 0, (struct sockaddr *)&addr, &addrlen);
		if (rc <= 0)
			continue;

		if (evt.type == EVT_WRITE_LOG)
			log_mgr_put(evt.module_name, evt.level, evt.fn_name, evt.msg);
	}

	close(fd);
	return 0;
}
