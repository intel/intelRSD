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


#ifndef __RF_LOG_H__
#define __RF_LOG_H__
#include "liblog/log.h"

#define RF_RECORD_EVT(level, fmt, args...) rf_log_put(level, __func__, fmt, ##args)
/**
 * @brief init redfish event log
 */
void rf_log_init(void);

/**
 * @brief put redfish log.
 *
 * @param  level 		log level
 * @param  func			func name 
 * @param  fmt 			format of log information
 *cd
 * @return 0			success.
 * 		   -1			fail
 *
 */
int32 rf_log_put(int32 level, const int8 *func, const int8 *fmt, ...);

/**
 * @brief get redfish log.
 *
 * @param  last_count   the count of the log
 * @param  data			the data of log
 *
 * @return 0			success.
 * 		   -1			fail
 *
 */
int32 rf_log_get(int32 last_count, int8 *data);

#endif
