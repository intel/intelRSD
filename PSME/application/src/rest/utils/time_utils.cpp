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
 * */
#include "psme/rest/utils/time_utils.hpp"
#include "bits/basic_string.h"
#include "logger/logger.hpp"

#include <stdexcept>
#include <locale>

using namespace psme::rest::utils;

namespace {
    constexpr const size_t WITHOUT_TIME_ZONE_LENGTH = 19;
    constexpr size_t TIME_BUFFER_SIZE = 26;
}

std::string TimeUtils::get_time(const steady_clock::time_point& t) {
    return get_time("%FT%H:%M", t);
}

std::string TimeUtils::get_time_with_zone(const steady_clock::time_point& t) {
    std::string time{get_time("%FT%T%z", t)};
    if (WITHOUT_TIME_ZONE_LENGTH < time.length())
        return time.insert(time.length()-2, ":");
    else
        return time;
}

std::string TimeUtils::get_time(const char* format,
                                    const steady_clock::time_point& tp) {
    char time_buffer[TIME_BUFFER_SIZE];
    auto t = system_clock::to_time_t(system_clock::now()
                                     + (tp - steady_clock::now()));
    struct tm local_tm;
    tzset();
    if(localtime_r(&t, &local_tm)) {
        std::strftime(time_buffer, TIME_BUFFER_SIZE, format, &local_tm);
    } else {
        log_error("rest", " Cannot get local time: ");
        throw std::runtime_error("Cannot get local time");
    }
    return time_buffer;
}
