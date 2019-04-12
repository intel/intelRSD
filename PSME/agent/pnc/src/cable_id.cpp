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
 * @file cable_id.cpp
 * @brief Cable ID reading implementation
 * */
#include "cable_id.hpp"
#include "agent-framework/module/enum/chassis.hpp"


using namespace agent::pnc;
using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;

CableId::~CableId(){}

void CableId::read(const i2c::I2cAccessInterfacePtr i2c, PM85X6TwiPort port,
        PCA9548TwiExpanderChannel channel) {
    select_channel(i2c, port, channel);
    read_cable_id(i2c, port);
    log_cable_eeprom(port, channel);
}

void CableId::select_channel(const i2c::I2cAccessInterfacePtr i2c, PM85X6TwiPort port,
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
    log_notice("pnc-twi", "Reading Cable Id of connector connected to TWI Port: "
        << std::uint32_t(static_cast<uint8_t>(port))
        << " TWI Channel: " << std::uint32_t(data));
}

void CableId::read_cable_id(const i2c::I2cAccessInterfacePtr i2c, PM85X6TwiPort port) {

    i2c->read(static_cast<uint8_t>(port),
              static_cast<uint16_t>(PM85X6TwiDeviceAddress::HOST_CABLE),
              0,
              reinterpret_cast<uint8_t*>(&fields),
              HOST_CABLE_EEPROM_DATA_SIZE_BYTES);
}

void CableId::log_cable_eeprom(PM85X6TwiPort port, PCA9548TwiExpanderChannel channel) {
    std::string serial_number(reinterpret_cast<const char*>(fields.serial_number),
                              HOST_CABLE_EEPROM_SERIAL_NUMBER_SIZE_BYTES);

    log_debug("pnc-twi", "Cable Id of connector connected to TWI Port: " << std::uint32_t(port)
                                   << " TWI Channel: " << std::uint32_t(channel)
                                   << " Cable Id (serial number): " << serial_number);
}
