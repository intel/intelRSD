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


#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

/**
* @brief Initilize log mgr.
*
* Load log info from /var/rmm/rmm.conf file. and initialize those log modules.
*/
int log_mgr_init(void);

/**
 * @brief write log to the log file.
 *
 * @param  module_name 	log mudule name(as the same as process name)
 * @param  level		log level
 * @param  func_name	func name
 * @param  msg			the log message
 */

void log_mgr_put(char *module_name, int level, const char *func_name, char *msg);

/**
 * @brief write log to the log file.
 *
 * @param  module_name 	log mudule name(as the same as process name)
 * @param  count		the count of lines.
 * @param  data			the pointer of data.
 */
void log_mgr_get(char *module_name, int count, char *data);

#endif