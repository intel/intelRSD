/*!
 * @brief Options set for a logger
 *
 * All logger settings container
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file logger_options.cpp
 */

#include "logger/logger_options.hpp"
#include "logger/logger.h"

namespace logger_cpp {

/*
 * level = LOG_INFO,
 * time_format = LOG_TIME_DATE_NS,
 * color = false,
 * tagging = true,
 * more_debug = true,
 * output_enable = true
 * This default options store in int value give number 0x3a6
 * */
static constexpr unsigned int OPTION_DEFAULT = 0x3a6;


Options::Options() :
    m_raw(OPTION_DEFAULT) {

    enable_more_debug(false);
    enable_color(true);
}


void Options::set_level(Level level) {
    union logger_options options;
    options.raw = m_raw;
    options.option.level = LOG_LEVEL_MASK & static_cast<unsigned int>(level);
    options.option.def_level = options.option.level;
    m_raw = options.raw;
}


Level Options::get_default_level() {
    union logger_options options;
    options.raw = m_raw;
    return static_cast<enum Level>(options.option.def_level);
}


void Options::set_time_format(TimeFormat time_format) {
    union logger_options options;
    options.raw = m_raw;
    options.option.time_format =
        LOG_TIME_MASK & static_cast<unsigned int>(time_format);
    m_raw = options.raw;
}


void Options::enable_color(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.color = enable;
    m_raw = options.raw;
}


void Options::enable_output(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.output_enable = enable;
    m_raw = options.raw;
}


void Options::enable_tagging(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.tagging = enable;
    m_raw = options.raw;
}


void Options::enable_more_debug(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.more_debug = enable;
    m_raw = options.raw;
}

}
