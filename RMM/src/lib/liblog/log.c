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
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <errno.h>

#include "libutils/sock.h"
#include "libutils/string.h"
#include "liblog/log.h"
#include "libutils/log_level.h"
#include "libutils/sock.h"
#include "libcfg/cfg.h"

#define _GNU_SOURCE

static int rmm_logd_fd = -1;	/* used for sending log to rmmlog daemon. */

static int send_rmm_log(const char *module_name, int level, const char *func, char *msg)
{
	struct log_evt evt;

	if (rmm_logd_fd < 0)
		exit(0);
	evt.type = EVT_WRITE_LOG;
	evt.level = level;
	strncpy_safe(evt.module_name, module_name, EVT_MODULE_LEN, EVT_MODULE_LEN - 1);
	strncpy_safe(evt.fn_name, func, EVT_FN_LEN, EVT_FN_LEN - 1);
	strncpy_safe(evt.msg, msg, EVT_MSG_LEN, EVT_MSG_LEN - 1);
	return socket_send(rmm_logd_fd, &evt, sizeof(struct log_evt));
}

int rmm_log_request(int level, const char *func, const char *fmt, ...)
{
	char module_name[32];
	va_list args;
	char msg[EVT_MSG_LEN] = {0};

	memset(module_name, 0, 32);
	prctl(PR_GET_NAME, (unsigned long)module_name);

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	return send_rmm_log(module_name, level, func, msg);
}

int rmm_log_init(void)
{
	int rmm_logd_port = 0;

	rmm_logd_port = rmm_cfg_get_port(LOGD_PORT);
	if (rmm_logd_port == 0) {
		printf("load rmm logd port fail...");
		return -1;
	}

	rmm_logd_fd = udp_connect(INADDR_LOOPBACK, rmm_logd_port);
	if (rmm_logd_fd < 0) {
		fprintf(stderr, "%s failed to connect rmmlog daemon.", __progname);
		return -1;
	}
	return 0;
}
