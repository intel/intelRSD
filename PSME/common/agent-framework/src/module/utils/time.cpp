/*!
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
 *
 * @brief Collection of functions for dealing with time
 * @file time.cpp
 * */

#include "agent-framework/module/utils/time.hpp"

#include <sstream>
#include <stdexcept>
#include <iomanip>

// System header included to ensure compatibility with ARM architecture
// and alleviate bugs in GCC and clang
extern "C" {
#include <time.h>
}



namespace agent_framework {
namespace utils {

const std::string make_iso_8601_timestamp() {
    time_t raw_time = time(nullptr);
    // ISO 8601 time string contains 25 characters, + 1 for terminating character
    constexpr const size_t ISO_TIME_STRING_LENGTH = 26;
    char time_string_buffer[ISO_TIME_STRING_LENGTH] = {};

    auto l_time = localtime(&raw_time);
    if (!l_time)
        throw std::runtime_error("Error while reading time");

    const size_t written_chars = strftime(time_string_buffer, ISO_TIME_STRING_LENGTH, "%Y-%m-%dT%H:%M:%S%z", l_time);

    if (0 != written_chars) {
        std::string time_string{time_string_buffer};
        // ISO 8601 specifies that time zone information should be in format (+/)HH:MM
        // the colon is placed before character 22 in the time string extracted from
        // the time_stream stringstream
        time_string.insert(22, ":");
        return time_string;
    }
    else {
        throw std::runtime_error("Error while parsing time stream");
    }
}

}
}
