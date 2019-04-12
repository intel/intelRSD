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
 * @file get_aggregated_health_sensor.hpp
 *
 * @brief GetAggregatedHealthSensor IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetAggregatedHealthSensor request command.
 */
class GetAggregatedHealthSensor : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetAggregatedHealthSensor();

    const char* get_command_name() const override {
        return "GetAggregatedHealthSensor";
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetAggregatedHealthSensor response command.
 */
class GetAggregatedHealthSensor : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetAggregatedHealthSensor();

    const char* get_command_name() const override {
        return "GetAggregatedHealthSensor";
    }

    /*!
     * @brief Checks if drawer is healthy
     * @return True if returned helath status is OK
     */
    bool is_healthy() const {
        return (HEALTH_OK == m_health_status);
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 2;
    static constexpr const std::uint8_t HEALTH_OFFSET = 1;
    static constexpr const std::uint8_t HEALTH_OK = 0;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    uint8_t m_health_status{};
};

}

}
}
}
