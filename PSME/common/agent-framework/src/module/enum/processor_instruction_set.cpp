/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file processor_instruction_set.cpp
 * */

#include "agent-framework/module/enum/processor_instruction_set.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <array>

using namespace agent_framework::model::enums;

namespace {
    std::array<const char*, 8> names = {{
        "x86-64", "x86", "IA-64", "ARM-A32", "ARM-A64", "MIPS32", "MIPS64", "OEM"}};
}

const char* ProcessorInstructionSet::to_string() const{
    return names[m_value];
}

ProcessorInstructionSet ProcessorInstructionSet::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return ProcessorInstructionSetEnum(static_cast<ProcessorInstructionSetEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
          std::string("ProcessorInstructionSet enum value not found: '") + name + "'.");}

std::vector<std::string> ProcessorInstructionSet::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}

bool ProcessorInstructionSet::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
