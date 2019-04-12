/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file responses/psme/heart_beat.hpp
 * @brief Definition of PSME heart_beat response
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace responses {

/*! HeartBeat response */
class HeartBeat {
public:

    /*!
     * @brief Sets time stamp
     * @param time_stamp New time stamp
     * */
    void set_time_stamp(std::uint64_t time_stamp) {
        m_time_stamp = time_stamp;
    }

    /*!
     * @brief Sets min delay
     * @param min_delay New delay
     * */
    void set_min_delay(std::uint64_t min_delay) {
        m_min_delay = min_delay;
    }

    /*!
     * @brief Gets time stamp
     * @return Time stamp value
     * */
    std::uint64_t get_time_stamp() const {
        return m_time_stamp;
    }

    /*!
     * @brief Gets min delay
     * @return Min delay value
     * */
    std::uint64_t get_min_delay() const {
        return m_min_delay;
    }

    /*!
     * @brief Transform response to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create HeartBeat from Json
     * @param[in] json the input argument
     * @return new HeartBeat
     */
    static HeartBeat from_json(const json::Json& json);

private:

    std::uint64_t m_time_stamp{0};
    std::uint64_t m_min_delay{0};

};

}
}
}
