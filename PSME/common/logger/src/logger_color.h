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
 * @file logger_color.h
 *
 * @brief Logger color interface
 * */

#ifndef LOGGER_COLOR_H
#define LOGGER_COLOR_H

#include "logger/logger.h"

/*!
 * @def COLOR_BLACK_BRIGHT
 * Bright or bold black color
 *
 * @def COLOR_BLACK_NORMAL
 * Black color
 *
 * @def COLOR_BLUE_BRIGHT
 * Bright or bold blue color
 *
 * @def COLOR_BLUE_NORMAL
 * Blue color
 *
 * @def COLOR_CYAN_BRIGHT
 * Bright or bold cyan color
 *
 * @def COLOR_CYAN_NORMAL
 * Cyan color
 *
 * @def COLOR_GREEN_BRIGHT
 * Bright or bold creen color
 *
 * @def COLOR_GREEN_NORMAL
 * Green color
 *
 * @def COLOR_MAGENTA_BRIGHT
 * Bright or bold magnenta color
 *
 * @def COLOR_MAGENTA_NORMAL
 * Magnenta color
 *
 * @def COLOR_RED_BRIGHT
 * Bright or bold red color
 *
 * @def COLOR_RED_NORMAL
 * Red color
 *
 * @def COLOR_WHITE_BRIGHT
 * Bright or bold white color
 *
 * @def COLOR_WHITE_NORMAL
 * White color
 *
 * @def COLOR_YELLOW_BRIGHT
 * Bright or bold yellow color
 *
 * @def COLOR_YELLOW_NORMAL
 * Yellow color
 *
 * @def COLOR_DEFAULT
 * Reset colors to default
 * */

#define COLOR_BLACK_BRIGHT          "\x1B[1;30m"
#define COLOR_BLACK_NORMAL          "\x1B[0;30m"
#define COLOR_BLUE_BRIGHT           "\x1B[1;34m"
#define COLOR_BLUE_NORMAL           "\x1B[0;34m"
#define COLOR_CYAN_BRIGHT           "\x1B[1;36m"
#define COLOR_CYAN_NORMAL           "\x1B[0;36m"
#define COLOR_GREEN_BRIGHT          "\x1B[1;32m"
#define COLOR_GREEN_NORMAL          "\x1B[0;32m"
#define COLOR_MAGENTA_BRIGHT        "\x1B[1;35m"
#define COLOR_MAGENTA_NORMAL        "\x1B[0;35m"
#define COLOR_RED_BRIGHT            "\x1B[1;31m"
#define COLOR_RED_NORMAL            "\x1B[0;31m"
#define COLOR_WHITE_BRIGHT          "\x1B[1;37m"
#define COLOR_WHITE_NORMAL          "\x1B[0;37m"
#define COLOR_YELLOW_BRIGHT         "\x1B[1;33m"
#define COLOR_YELLOW_NORMAL         "\x1B[0;33m"

#define COLOR_DEFAULT               "\x1B[0m"

/*!
 * @brief Get color special string characters by given log level
 *
 * @param[in]   options   Log level value or logger options
 * @return      Return special color ANSI code
 * */
const char *logger_color_by_level(unsigned int options);

#endif /* LOGGER_COLOR_H */
