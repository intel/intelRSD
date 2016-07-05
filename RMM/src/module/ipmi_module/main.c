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


#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "ipmi.h"
#include "event.h"
#include "ipmi_log.h"
#include "libutils/dump.h"
#include "libutils/rmm.h"

bool ipmi_module_exit = false;
static void sigterm_handler(int signum)
{
	ipmi_module_exit = true;
	printf("catch sigterm and exit ipmi_module.\n");
	/* do cleanup jobs here */
}

int main(int argc, char **argv)
{
	reg_sigterm_handler(sigterm_handler);
	
	rmm_log_init();
	
	enable_core_dump();

	ipmi_subscribe_init();

	app_interface_init();

	rmcp_interface_init();

	IPMI_LOG_INFO("IPMI-Daemon is Running ...\n");

	fd_events_main_loop();
	return 0;
}
