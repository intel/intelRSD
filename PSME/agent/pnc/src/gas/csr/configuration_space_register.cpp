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
 * @file configuration_space_register.cpp
 * @brief Configuration Space Register
 * */

#include "logger/logger_factory.hpp"
#include "gas/csr/configuration_space_register.hpp"

using namespace agent::pnc::gas::csr;

ConfigurationSpaceRegister::~ConfigurationSpaceRegister(){}

void ConfigurationSpaceRegister::read(AccessInterface* iface) {
    iface->read(output.pci_caps.raw, CSR_REG_SIZE_USED, CSR_REG_OFFSET
                                     + SINGLE_PCIE_FUN_CSR_REG_SIZE * input.fields.instance_id
                                     + PCI_HEADER_SIZE);
}

double ConfigurationSpaceRegister::speed_bit_to_double(const uint32_t speed_bit) {
    switch (speed_bit) {
        case 1: return 2.5;
        case 2: return 5.0;
        case 3: return 8.0;
        default:
            throw std::runtime_error(std::string{"Unrecognized speed bit value: "}
                                     + std::to_string(speed_bit));
    }
}
