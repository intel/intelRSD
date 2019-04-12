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
 * @file get_fan_tachs.hpp
 *
 * @brief GetFanTachs IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetFanTachs request command.
 */
class GetFanTachs : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetFanTachs();

    /*!
     * @brief destructor.
     */
    virtual ~GetFanTachs();

    virtual const char* get_command_name() const override {
        return "GetFanTachs";
    }

protected:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetFanTachs response command.
 */
class GetFanTachs : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetFanTachs();

    /*!
     * @brief Get number of measured fans
     * @return number of fans
     */
    std::uint8_t get_slot_count() const;

    /*!
     * @brief Tachometer reading getter.
     * @param fan_index Fan index
     * @return Tachometer reading for given fan.
     */
    std::uint16_t get_tach_reading(std::uint8_t fan_index) const;

    virtual const char* get_command_name() const override {
        return "GetFanTachs";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 13;
    static constexpr const std::size_t NUM_READINGS = (RESPONSE_SIZE-1)/2;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    std::uint16_t m_tach_readings[NUM_READINGS] = {};
};

}

}
}
}
