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
 * @file heartbeat_response.hpp
 * @brief Heartbeat response
 * */

#pragma once
#include <chrono>
#include <string>

namespace agent_framework {
namespace generic {

/*!
 * @brief HeartBeat response.
 */
class HeartBeatResponse final {
public:
    /*!
     * @brief Constructor
     * @param min_delay minimum delay in seconds between heartbeat requests.
     * @param timestamp Period in seconds the AMC application is up.
     */
    HeartBeatResponse(std::chrono::seconds min_delay, std::chrono::seconds timestamp)
        : m_min_delay(min_delay), m_timestamp(timestamp) {}

    /*!
     * @brief Gets minimum delay value between heartbeats requests.
     * @return Minimum delay in seconds between heartbeat requests.
     */
    const std::chrono::seconds& get_min_delay() const {
        return m_min_delay;
    }

    /*!
     * @brief Gets AMC application timestamp
     * @return Timestamp returned by th AMC application
     *
     * Timestamp indicates period in seconds the AMC application is up.
     * If the period from previous heartbeat is higher than the received one,
     * registration request is repeated.
     */
    const std::chrono::seconds& get_timestamp() const {
        return m_timestamp;
    }
private:
    std::chrono::seconds m_min_delay;
    std::chrono::seconds m_timestamp;
};

}
}

