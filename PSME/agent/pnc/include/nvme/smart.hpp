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
 * @file smart.hpp
 * @brief NVMe drive Smart structure
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! NVMe namespace */
namespace nvme {

/*! NVMe drive Smart structure */
class Smart {

    static constexpr uint8_t FLAG_RESET_NOT_REQUIRED_MASK = 0x10;
    static constexpr uint8_t FLAG_DRIVE_FUNCTIONAL_MASK = 0x20;
    static constexpr uint8_t FLAG_DRIVE_NOT_READY_MASK = 0x40;

    static constexpr uint8_t FLAG_SPACE_TEMPERATURE_WARNINGS_MASK = 0x03;
    static constexpr uint8_t FLAG_OTHER_WARNINGS_MASK = 0x1C;

    agent_framework::model::enums::PlatformType m_platform{agent_framework::model::enums::PlatformType::EDK};

    void select_channel(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);
    void read_smart(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port);
public:

    /*! Default constructor */
    Smart(agent_framework::model::enums::PlatformType platform) : m_platform(platform) {}

    /*! Copy constructor */
    Smart(const Smart&) = default;

    /*! Assignment operator */
    Smart& operator=(const Smart&) = default;

    /*!
     * @brief Checks if either spare space or temperature warning is set
     * @return True if any of the flags is set
     * */
    bool is_space_or_temperature_warning_flag_set() const {
        return ((~fields.critical_warnings) & FLAG_SPACE_TEMPERATURE_WARNINGS_MASK) != 0x00;
    }

    /*!
     * @brief Checks if any warning flag is set (excluding spare space or temperature warnings
     * @return True if any of the warning flags are set (excluding spare space and temperature warnings)
     * */
    bool is_any_other_warning_flag_set() const {
        return ((~fields.critical_warnings) & FLAG_OTHER_WARNINGS_MASK) != 0x00;
    }

    /*!
     * @brief Checks if drive is functional
     * @return True if drive is functional
     * */
    bool is_drive_functional() const {
        return (FLAG_DRIVE_FUNCTIONAL_MASK & fields.status_flags) != 0x00;
    }

    /*!
     * @brief Checks if drive is ready
     * @return True if drive is ready
     * */
    bool is_drive_ready() const {
        return !((FLAG_DRIVE_NOT_READY_MASK & fields.status_flags) != 0x00);
    }

    /*!
     * @brief Checks if drive requires reset
     * @return True if drive requires reset
     * */
    bool is_reset_required() const {
        return !((FLAG_RESET_NOT_REQUIRED_MASK & fields.status_flags) != 0x00);
    }

#pragma pack(push, 1)
    /*! Smart */
    struct {
        uint8_t status_length;
        uint8_t status_flags;
        uint8_t critical_warnings;
        uint8_t composite_temperature;
        uint8_t percentage_drive_life_used;
    } fields{};
#pragma pack(pop)

    /*!
     * @brief Reads Smart of drive connected to given port and channel.
     *
     * @param i2c Interface used to read vpd
     * @param port TWI Port number
     * @param channel TWI Expander channel number
     * */
    void read(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);

    virtual ~Smart();
};

}
}
}
