/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file logger_level.h
 *
 * @brief Logger level interface
 * */

#ifndef LOGGER_LEVEL_H
#define LOGGER_LEVEL_H

#include "logger/logger.h"

/*!
 * @brief Get log level string
 *
 * @param[in]   options     Log level value or logger options
 * @return      Return logger level tag string
 * */
const char *logger_level_get_string(unsigned int options);

/*!
 * @brief Get number of log levels
 * @return Number of log levels
 */
unsigned logger_level_get_number_of_levels(void);

/*!
 * @brief Get id of log level given as name
 * @param[in] level string with name of the level
 * @return logging level id or -1 if name not match
 */
int logger_level_get_level(const char* level);


#endif /* LOGGER_LEVEL_H */
