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


#include "libjson/json.h"
#include "libutils/log.h"
#include "libutils/string.h"
#include "libcfg/cfg.h"
#include "log_manager.h"

#define LOG_MODULES_MAX_NUM		(12)

struct log_module {
	long handler;
	char name[128];
};

static struct log_module log_modules[LOG_MODULES_MAX_NUM];

int log_mgr_init(void)
{
	struct rmm_log_module rmm_log_modules[MAX_LOG_MODULES_CNT];
	int module_cnt = 0;
	int index = 0;
	long handler  = 0;

	rmm_cfg_get_log_modules(rmm_log_modules, &module_cnt);
	if (module_cnt == 0) {
		printf("Load log module config fail...\n");
		exit(-1);
	}

	for (index = 0; index < module_cnt; index++) {
		handler = log_init(rmm_log_modules[index].name);
		log_set_level(handler, rmm_log_modules[index].level);
		log_set_max_lines(handler, rmm_log_modules[index].max_lines);

		log_modules[index].handler = handler;
		strncpy_s(log_modules[index].name,
					sizeof(log_modules[index].name),
					rmm_log_modules[index].name,
					//RMM_LOG_MODULE_NAME_LEN,
					RMM_LOG_MODULE_NAME_LEN - 1);
	}

	return 0;
}

void log_mgr_put(char *module_name, int level, const char *func_name, char *msg)
{
	int index = 0;

	for (index = 0; index < LOG_MODULES_MAX_NUM; index++) {
		if (strstr(log_modules[index].name, module_name) != 0) {
				log_put(log_modules[index].handler, level, func_name, msg);
			break;
		}
	}
}

void log_mgr_get(char *module_name, int count, char *data)
{
	int index = 0;

	for (index = 0; index < LOG_MODULES_MAX_NUM; index++) {
		if (strstr(log_modules[index].name, module_name) != 0) {
			log_get(log_modules[index].handler, count, data);
			break;
		}
	}
}
