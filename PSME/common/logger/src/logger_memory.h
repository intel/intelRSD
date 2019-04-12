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
 * @file logger_memory.h
 *
 * @brief Logger memory interaface
 * */

#ifndef LOGGER_MEMORY_H
#define LOGGER_MEMORY_H

#include "logger/logger.h"

#include <stdlib.h>

/*!
 * @brief Implemented by user. Allocate dynamically memory
 *
 * @param[in]   size    Memory size in bytes to allocate
 * @return              NULL when error otherwise allocation success
 * */
static inline void *logger_memory_alloc(size_t size) {
    return malloc(size);
}

/*!
 * @brief Implemented by user. Free allocate dynamically memory
 *
 * @param[in]   memory  Pointer to allocated dynamically memory that will be
 *                      free
 * */
static inline void logger_memory_free(void *memory) {
    free(memory);
}

#endif /* LOGGER_MEMORY_H */
