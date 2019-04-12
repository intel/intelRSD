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
 * @file get_drawer_power.hpp
 *
 * @brief GetDrawerPower IPMI command request and response.
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
class GetDrawerPower : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetDrawerPower();

    virtual const char* get_command_name() const override {
        return "GetDrawerPower";
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetDrawerPower response command.
 */
class GetDrawerPower : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetDrawerPower();

    /*!
     * @brief Returns power reading on slot
     * @param slot_id Id of the slot
     * @returns Power on slot
     */
    uint16_t get_power_on_slot(uint8_t slot_id) const {
        return slot_id < MAX_SLOTS && slot_id < m_slots ? m_power_on_slot[slot_id] : INVALID_VALUE;
    }

    /*!
     * @brief checks if power value is correct
     * @param slot_id Id of the queried slot
     * @return True if value on the slot is correct
     */
    bool is_valid_value_on_slot(uint8_t slot_id) const {
        return get_power_on_slot(slot_id) != INVALID_VALUE;
    }

    /*!
     * @brief Returns total number of slots
     * @returns Total number of slots returned
     */
    int get_slot_count() const {
        return m_slots;
    }

    virtual const char* get_command_name() const override {
        return "GetDrawerPower";
    }

private:
    static constexpr const std::size_t MIN_RESPONSE_SIZE = 2;
    static constexpr const std::uint8_t DATA_OFFSET = 2;
    static constexpr const std::uint8_t POWER_LSB_OFFSET = 1;
    static constexpr const std::uint8_t POWER_MSB_OFFSET = 0;
    static constexpr const std::uint8_t ENTRY_SIZE = 2;
    static constexpr const std::uint16_t INVALID_VALUE = 0xFFFF;
    static constexpr const std::uint8_t MAX_SLOTS = 4;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    uint16_t m_power_on_slot[MAX_SLOTS];
    uint8_t m_slots{0};
};

}

}
}
}
