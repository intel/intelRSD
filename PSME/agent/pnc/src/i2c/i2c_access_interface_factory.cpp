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
 * @file i2c_interface_factory.cpp
 * @brief I2c interface factory implementation
 * */

#include "i2c/i2c_access_interface_factory.hpp"
#include "i2c/gas_i2c_access_interface.hpp"
#include "i2c/ipmi_i2c_access_interface.hpp"
#include "gas/global_address_space_registers.hpp"

using namespace agent::pnc::i2c;
using namespace agent::pnc::gas;

namespace {
    static I2cAccessInterfaceFactory i2c_interface_factory{};
}

I2cAccessInterfaceFactory::~I2cAccessInterfaceFactory() {}

I2cAccessInterfaceFactory& I2cAccessInterfaceFactory::get_instance() {
    return i2c_interface_factory;
}

I2cAccessInterfacePtr I2cAccessInterfaceFactory::get_interface() {

    switch (m_current_interface) {
        case I2cAccessInterfaceCode::Gas:
            if (!m_gas_initialized) {
                throw std::runtime_error("Gas interface was not initialized");
            }
            else {
                GasI2cAccessInterface* gas_iface =
                    new GasI2cAccessInterface(GlobalAddressSpaceRegisters::get_default(m_gas_path));
                return I2cAccessInterfacePtr(gas_iface);
            }
        case I2cAccessInterfaceCode::Ipmi:
            if (!m_ipmi_initialized) {
                throw std::runtime_error("Ipmi interface was not initialized");
            }
            else {
                IpmiI2cAccessInterface* ipmi_iface = new IpmiI2cAccessInterface(m_ip, m_port, m_user, m_pass);
                return I2cAccessInterfacePtr(ipmi_iface);
            }

        case I2cAccessInterfaceCode::None:
            throw std::runtime_error("I2c interface was not chosen!");
        default:
            throw std::runtime_error("I2c interface factory error");
    }
}
