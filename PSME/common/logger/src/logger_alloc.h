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
 * @file logger_alloc.h
 *
 * @brief Logger dynamic allocation interface
 * */

#ifndef LOGGER_ALLOC_H
#define LOGGER_ALLOC_H

#include "logger/logger.h"
#include <stdarg.h>

/*!
 * @brief Format string, dynamically allocate memory for that and copy
 * formatted string to that memory. User must provide to free memory after use
 * with logger_memory_free function
 *
 * @param[in]   fmt     Format string like printf. When parameter is NULL
 *                      return NULL
 * @param[in]   args    Variadic variables
 * @return              When success return copy of string otherwise
 *                      return NULL after failure
 * */
char *logger_alloc_fmt_string(const char *fmt, va_list args);

/*!
 * @brief Dynamically allocate memory for string and copy
 * string to that memory. User must provide to free memory after use
 * with logger_memory_free function
 *
 * @param[in]   str     String to copy. When parameter is NULL return NULL
 * @return              When success return copy of string otherwise
 *                      return NULL after failure
 * */
char *logger_alloc_copy_string(const char *str);

#endif /* LOGGER_ALLOC_H */
