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
 * @file get_aggregated_thermal_sensor.hpp
 *
 * @brief GetAggregatedThermalSensor IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetAggregatedThermalSensor request command.
 */
class GetAggregatedThermalSensor : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetAggregatedThermalSensor();

    virtual const char* get_command_name() const override {
        return "GetAggregatedThermalSensor";
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetAggregatedThermalSensor response command.
 */
class GetAggregatedThermalSensor : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetAggregatedThermalSensor();

    /*!
     * @brief Returns thermal reading on slot
     * @param slot_id Id of the slot
     * @returns Temperature on slot
     */
    uint8_t get_thermal_reading_on_slot(uint8_t slot_id) const {
        return slot_id < MAX_SLOTS && slot_id < m_slots ? m_temperature_on_slot[slot_id] : INVALID_TEMPERATURE_VALUE;
    }

    /*!
     * @brief checks if temperature value is correct
     * @param slot_id Id of the queried slot
     * @return True if value on the slot is correct
     */
    bool is_valid_thermal_reading_value_on_slot(uint8_t slot_id) const {
        return get_thermal_reading_on_slot(slot_id) != INVALID_TEMPERATURE_VALUE;
    }

    /*!
     * @brief Returns total number of slots
     * @returns Total number of slots returned
     */
    int get_slot_count() const {
        return m_slots;
    }

    virtual const char* get_command_name() const override {
        return "GetAggregatedThermalSensor";
    }

private:
    static constexpr const std::size_t MIN_RESPONSE_SIZE = 2;
    static constexpr const std::uint8_t DATA_OFFSET = 2;
    static constexpr const std::uint8_t TEMPERATURE_OFFSET = 2;
    static constexpr const std::uint8_t ENTRY_SIZE = 3;
    static constexpr const std::uint8_t MAX_SLOTS = 4;
    static constexpr const std::uint8_t INVALID_TEMPERATURE_VALUE = 0xFF;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    uint8_t m_temperature_on_slot[MAX_SLOTS];
    uint8_t m_slots{0};
};

}

}
}
}
