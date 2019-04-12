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
 * @file logger_alloc.c
 *
 * @brief Logger allocation implementation
 * */

#include "logger_alloc.h"

#include "logger_assert.h"
#include "logger_memory.h"

#include <safe-string/safe_lib.h>

LOGGER_PRINTF_FORMAT(1, 0)
char *logger_alloc_fmt_string(const char *fmt, va_list args) {
    logger_assert(NULL != fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    char *buffer = NULL;

    int size = vsnprintf(NULL, 0, fmt, args);
    if (size > 0) {
        buffer = logger_memory_alloc((size_t)size + 1);
        if (NULL != buffer) {
            vsnprintf(buffer, (size_t)size, fmt, args_copy);
        }
    }

    va_end(args_copy);
    return buffer;
}

char *logger_alloc_copy_string(const char *str) {
    logger_assert(NULL != str);

    size_t size = strnlen_s(str, RSIZE_MAX_STR);
    char *buffer = logger_memory_alloc(size + 1);
    if (NULL != buffer) {
        strncpy_s(buffer, RSIZE_MAX_STR, str, size);
        buffer[size] = '\0';
    }

    return buffer;
}
