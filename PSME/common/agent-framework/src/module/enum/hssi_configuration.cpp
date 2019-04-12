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
 * @file hssi_configuration.cpp
 * */

#include "agent-framework/module/enum/hssi_configuration.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <array>

using namespace agent_framework::model::enums;

namespace {
std::array<const char*, 4> names = {{"4x10G", "2x40G", "10x10G", "PCIe"}};
}

const char* HssiConfiguration::to_string() const{
    return names[m_value];
}

HssiConfiguration HssiConfiguration::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return HssiConfigurationEnum(static_cast<HssiConfigurationEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
          std::string("HssiConfiguration enum value not found: '") + name + "'.");
}

std::vector<std::string> HssiConfiguration::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}

bool HssiConfiguration::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
