/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file dimm_module_type.cpp
 * */

#include "agent-framework/module/enum/memory_module_type.hpp"

#include <array>
#include <cstring>

using namespace std;

namespace {
    array<const char*, 11> names = {{
        "RDIMM", "UDIMM", "SO_DIMM", "LRDIMM", "Mini_RDIMM", "Mini_UDIMM",
        "72b_SO_RDIMM", "72b_SO_UDIMM", "16b_SO_DIMM", "32b_SO_DIMM",
        "Unknown"
    }};
}

using namespace agent_framework::model::enums;

const char* MemoryModuleType::to_string() const{
    return names[m_value];
}

MemoryModuleType MemoryModuleType::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return MemoryModuleTypeEnum(static_cast<MemoryModuleTypeEnum>(i));
        }
    }
    return MemoryModuleType(MemoryModuleTypeEnum::Unknown);
}

std::vector<std::string> MemoryModuleType::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}

bool MemoryModuleType::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
