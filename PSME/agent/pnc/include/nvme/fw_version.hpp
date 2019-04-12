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
 * @file fw_version.hpp
 * @brief NVMe drive FirmwareVersion structure
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

/*! NVMe drive FirmwareVersion structure */
class FirmwareVersion {

    agent_framework::model::enums::PlatformType m_platform{agent_framework::model::enums::PlatformType::MF3};

    void select_channel(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);
    void read_firmware_version(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port);
    void log_fw_version(gas::PM85X6TwiPort port, gas::PCA9548TwiExpanderChannel channel);
public:

    /*! Size of fields struct (in bytes) */
    static constexpr uint8_t FW_VERSION_SIZE_BYTES = 0x08;

    /*! Firmware version offset (in bytes) */
    static constexpr uint8_t FW_VERSION_OFFSET_BYTES = 0x27;

    /*! Default constructor */
    FirmwareVersion(agent_framework::model::enums::PlatformType platform) : m_platform(platform) {}

    /*! Copy constructor */
    FirmwareVersion(const FirmwareVersion&) = default;

    /*! Assignment operator */
    FirmwareVersion& operator=(const FirmwareVersion&) = default;


#pragma pack(push, 1)
    /*! FirmwareVersion */
    struct {
        uint8_t firmware_version[FW_VERSION_SIZE_BYTES];
    } fields{};
#pragma pack(pop)

    /*!
     * @brief Reads FirmwareVersion of drive connected to given port and channel.
     *
     * @param i2c Interface used to read firmware version
     * @param port TWI Port number
     * @param channel TWI Expander channel number
     * */
    void read(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);

    virtual ~FirmwareVersion();
};

}
}
}
