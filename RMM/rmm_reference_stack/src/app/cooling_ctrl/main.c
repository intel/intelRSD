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


#include <stdio.h>
#include <stdlib.h>

#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libjipmi/common.h"
#include "cooling_ctrl.h"
#include "libcfg/cfg.h"
#include "libinit/libinit.h"

int main()
{
	int rc;
	int max_fd;
	fd_set rfds;
	struct cooling_ctrl *ctrl;
	int port;

	if (daemon(1, 1) < 0)
		exit(-1);

	if (rmm_modules_init(MODULEINIT_LOG, IPMICOOLINGCTRL_PORT, JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI) == -1)
		exit(-1);

	ctrl = cooling_allocate();

	if (ctrl == NULL)
		exit(-1);

	if (libdb_init_subscription(NOTIFY_BY_SELECT, cooling_event_ops, ctrl) < 0) {
		rmm_log(ERROR, "Failed to init select subscirbe mode!\n");
		return -1;
	}

	cooling_init(ctrl);

	rmm_log(INFO, "cooling ctrl default init success!\n");

	for (;;) {
		max_fd = -1;
		FD_ZERO(&rfds);

		libjsonrpcapi_callback_selectfds(&rfds, &max_fd);

		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0)
			continue;

		libjsonrpcapi_callback_processfds(&rfds);
	}

}


