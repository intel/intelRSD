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


#include "libutils/log.h"
#include "rf_log.h"
#include "libsecurec/safe_lib.h"


#define RF_EVT_MODULE_NAME		"rfevent"
#define RF_EVT_LOG_MAX_LINES	10000
#define RF_EVT_LOG_LEVEL		6

int32 rf_evt_log_handler = 0;

void rf_log_init(void)
{
	rf_evt_log_handler = log_init(RF_EVT_MODULE_NAME);
	log_set_level(rf_evt_log_handler, RF_EVT_LOG_LEVEL);
	log_set_max_lines(rf_evt_log_handler, RF_EVT_LOG_MAX_LINES);
}

int32 rf_log_put(int32 level, const int8 *func, const int8 *fmt, ...)
{
	va_list args;
	int8 buff[MAX_LOG_MSG_SIZE - 1] = {0};
	int8 msg[MAX_LOG_MSG_SIZE] = {0};

	va_start(args, fmt);
	vsnprintf(buff, MAX_LOG_MSG_SIZE - 1, fmt, args);
	va_end(args);
	snprintf_s_s(msg, MAX_LOG_MSG_SIZE, "%s\n", buff);
	log_put(rf_evt_log_handler, level, func, msg);

	return 0;
}

int32 rf_log_get(int32 last_count, int8 *data)
{
	return log_get(rf_evt_log_handler, last_count, data);
}
