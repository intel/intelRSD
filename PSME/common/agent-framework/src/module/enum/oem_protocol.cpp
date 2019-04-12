/*!
 * @brief OEM protocol enum implementation
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file oem_protocol.cpp
 */


#include "agent-framework/module/enum/oem_protocol.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <array>



using namespace agent_framework::model::enums;

namespace {
std::array<const char*, 1> names = {{
                                        "FPGA-oF",
                                    }};
}


const char* OemProtocol::to_string() const {
    return names[m_value];
}


OemProtocol OemProtocol::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return OemProtocolEnum(static_cast<OemProtocolEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
          std::string("OemProtocol enum value not found: '") + name + "'.");
}


std::vector<std::string> OemProtocol::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}


bool OemProtocol::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
