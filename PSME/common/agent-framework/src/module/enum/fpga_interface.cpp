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
 * @file fpga_interface.cpp
 * */

#include "agent-framework/module/enum/fpga_interface.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <array>



using namespace agent_framework::model::enums;

namespace {
std::array<const char*, 26> names = {{
                                         "QPI",
                                         "UPI",
                                         "2xPCIe-4",
                                         "4xPCIe-4",
                                         "8xPCIe-4",
                                         "16xPCIe-4",
                                         "2xPCIe-5",
                                         "4xPCIe-5",
                                         "8xPCIe-5",
                                         "16xPCIe-5",
                                         "1x10G",
                                         "2x10G",
                                         "4x10G",
                                         "10x10G",
                                         "1x25G",
                                         "2x25G",
                                         "4x25G",
                                         "1x40G",
                                         "2x40G",
                                         "1x50G",
                                         "2x50G",
                                         "1x100G",
                                         "SPI",
                                         "SMBus",
                                         "I2C",
                                         "OEM"
                                     }};
}


const char* FpgaInterface::to_string() const {
    return names[m_value];
}


FpgaInterface FpgaInterface::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return FpgaInterfaceEnum(static_cast<FpgaInterfaceEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
          std::string("FpgaInterface enum value not found: '") + name + "'.");
}


std::vector<std::string> FpgaInterface::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}


bool FpgaInterface::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
