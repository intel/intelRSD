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
 * @file logger_level.c
 *
 * @brief Logger level implementation
 * */

#include "logger_level.h"

static const char *logger_level_table[] = {
    "EMERG",
    "ALERT",
    "CRIT ",
    "ERROR",
    "WARN ",
    "NOTE ",
    "INFO ",
    "DEBUG"
};

const char *logger_level_get_string(unsigned int options) {
    return logger_level_table[LOG_LEVEL_MASK & options];
}

unsigned logger_level_get_number_of_levels(void)
{
    return sizeof(logger_level_table) / sizeof(logger_level_table[0]);
}

#define toupper(c) \
    ((('a' <= c) && (c <= 'z')) ? c - ('a' - 'A') : c)

int logger_level_get_level(const char* level)
{
    int id;
    int i;

    if (!level) {
        return -1;
    }
    for (id = (int) logger_level_get_number_of_levels(); id--; ) {
        for (i = 0; /* no exit cond */; i++) {
            char c = logger_level_table[id][i];
            if ((c == ' ') || (c == '\0')) {
                if (level[i] == '\0') {
                    return id;
                }
                break;
            }
            if (toupper(level[i]) != c) {
                break;
            }
        }
    }
    return -1;
}
