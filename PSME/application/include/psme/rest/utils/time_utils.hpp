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
 * @file time_utils.hpp
 *
 * @brief Declaration of TimeUtils class
 * */
#pragma once
#include <string>
#include <chrono>

namespace psme {
namespace rest {
namespace utils {

using std::chrono::steady_clock;
using std::chrono::system_clock;

/*! @brief Provides methods to get time */
class TimeUtils {
public:
    /*!
     * @brief Local time getter
     * @return Local time with time zone
     * */
    static std::string get_time_with_zone(const steady_clock::time_point& t);

    /*!
     * @brief Local time getter
     * @return Local time
     * */
    static std::string get_time(const steady_clock::time_point& t);

private:
    static std::string get_time(const char* format,
                                const steady_clock::time_point& t);

    TimeUtils() = delete;
    TimeUtils(const TimeUtils& orig) = delete;
    TimeUtils& operator=(const TimeUtils&) = delete;
    virtual ~TimeUtils() = delete;
};

}
}
}


