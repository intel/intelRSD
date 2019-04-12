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
 * @file fw_version.cpp
 * @brief NVMe drive FirmwareVersion structure
 * */

#include "nvme/fw_version.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::nvme;
using namespace agent::pnc::i2c;


FirmwareVersion::~FirmwareVersion() {}


void FirmwareVersion::read(const I2cAccessInterfacePtr i2c, PM85X6TwiPort port,
                 PCA9548TwiExpanderChannel channel) {
    select_channel(i2c, port, channel);
    read_firmware_version(i2c, port);
    log_fw_version(port, channel);
}


void FirmwareVersion::select_channel(const I2cAccessInterfacePtr i2c, PM85X6TwiPort port,
                           PCA9548TwiExpanderChannel channel) {
    PM85X6TwiDeviceAddress address{};
    if (agent_framework::model::enums::PlatformType::EDK == m_platform) {
        address = PM85X6TwiDeviceAddress::EDK_TWI_EXPANDER;
    }
    else if (agent_framework::model::enums::PlatformType::MF3 == m_platform) {
        address = PM85X6TwiDeviceAddress::MF3_TWI_EXPANDER;
    }

    uint8_t data = uint8_t(channel);
    i2c->write(static_cast<uint8_t>(port),
               static_cast<uint16_t>(address),
               0,
               reinterpret_cast<uint8_t*>(&data),
               sizeof(data));
}


void FirmwareVersion::read_firmware_version(const I2cAccessInterfacePtr i2c, PM85X6TwiPort port) {
    i2c->read(static_cast<uint8_t>(port),
              static_cast<uint16_t>(PM85X6TwiDeviceAddress::SMART),
              FW_VERSION_OFFSET_BYTES,
              reinterpret_cast<uint8_t*>(&fields),
              sizeof(fields));

}

void FirmwareVersion::log_fw_version(PM85X6TwiPort port, PCA9548TwiExpanderChannel channel) {

    std::string fw_version(reinterpret_cast<const char*>(fields.firmware_version), FW_VERSION_SIZE_BYTES);

    log_debug("pnc-vpd", "NVMe firmware version for drive connected to TWI Port: "
        << std::uint32_t(port) << " TWI Channel: " << std::uint32_t(channel) << " - " << fw_version);
}
