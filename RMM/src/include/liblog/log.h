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


#ifndef __LIBLOG_LOG_H__
#define __LIBLOG_LOG_H__

#include <sys/prctl.h>
#include "libutils/types.h"
#include "libutils/log_level.h"

extern const char *__progname;

#define rmm_log(level, fmt, args...) rmm_log_request(level, __func__, fmt, ##args)

#define rmm_log_ex(module_name, func_name, level, fmt, args...) rmm_log_request(level, func_name, fmt, ##args)

/**
 * @brief rmm log initialize.
 *
 */
int rmm_log_init(void);

/**
 * @brief write log to rmm log file.
 *
 * @param  level 			log level.
 * @param  func 			function name of this log.
 */
int rmm_log_request(int level, const char *func, const char *fmt, ...);

#endif
