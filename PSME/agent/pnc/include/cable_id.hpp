/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file cable_id.hpp
 * @brief Cable ID reading implementation
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {

/*! Cable ID reader class */
class CableId {
public:
    static constexpr uint8_t HOST_CABLE_EEPROM_RESERVED0_SIZE_BYTES = 196;
    static constexpr uint8_t HOST_CABLE_EEPROM_SERIAL_NUMBER_SIZE_BYTES = 16;
    static constexpr uint8_t HOST_CABLE_EEPROM_RESERVED1_SIZE_BYTES = 44;

    agent_framework::model::enums::PlatformType m_platform{agent_framework::model::enums::PlatformType::EDK};

private:

    void select_channel(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);
    void read_cable_id(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port);
    void log_cable_eeprom(gas::PM85X6TwiPort port, gas::PCA9548TwiExpanderChannel channel);

public:
    /*! Default constructor */
    CableId(agent_framework::model::enums::PlatformType platform) : m_platform(platform) {}

    /*! Copy constructor */
    CableId(const CableId&) = default;

    /*! Assignment operator */
    CableId& operator=(const CableId&) = default;

#pragma pack(push, 1)
    /*! Host cable eeprom memory map */
    struct {
        uint8_t reserved0[HOST_CABLE_EEPROM_RESERVED0_SIZE_BYTES];
        uint8_t serial_number[HOST_CABLE_EEPROM_SERIAL_NUMBER_SIZE_BYTES];
        uint8_t reserved1[HOST_CABLE_EEPROM_RESERVED1_SIZE_BYTES];
    } fields{};
#pragma pack(pop)

    /*!
     * @brief Reads Cabled ID of connector connected to given port.
     *
     * @param i2c I2c interface used to read cable Id
     * @param channel TWI Expander channel number
     * */
    void read(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);

    virtual ~CableId();
};

}
}
