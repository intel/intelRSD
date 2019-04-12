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
 * @file logger_color.c
 *
 * @brief Logger color implementation
 * */

#include "logger_color.h"

static const char *glogger_color_table[] = {
    COLOR_CYAN_BRIGHT,
    COLOR_MAGENTA_BRIGHT,
    COLOR_RED_NORMAL,
    COLOR_RED_BRIGHT,
    COLOR_YELLOW_BRIGHT,
    COLOR_BLUE_BRIGHT,
    COLOR_WHITE_BRIGHT,
    COLOR_GREEN_BRIGHT
};

const char *logger_color_by_level(unsigned int options) {
    return glogger_color_table[LOG_LEVEL_MASK & options];
}
