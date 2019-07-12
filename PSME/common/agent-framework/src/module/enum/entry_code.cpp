/*!
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file entry_code.cpp
 */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module/enum/entry_code.hpp"
#include <array>



namespace {
static std::array<const char*, 60> names = {
    {
        "Assert", "Deassert", "Lower Non-critical - going low", "Lower Non-critical - going high", "Lower Critical - going low",
        "Lower Critical - going high", "Lower Non-recoverable - going low", "Lower Non-recoverable - going high",
        "Upper Non-critical - going low", "Upper Non-critical - going high", "Upper Critical - going low",
        "Upper Critical - going high", "Upper Non-recoverable - going low", "Upper Non-recoverable - going high",
        "Transition to Idle", "Transition to Active", "Transition to Busy", "State Deasserted", "State Asserted",
        "Predictive Failure deasserted", "Predictive Failure asserted", "Limit Not Exceeded", "Limit Exceeded",
        "Performance Met", "Performance Lags", "Transition to OK", "Transition to Non-Critical from OK",
        "Transition to Critical from less severe", "Transition to Non-recoverable from less severe",
        "Transition to Non-Critical from more severe", "Transition to Critical from Non-recoverable",
        "Transition to Non-recoverable", "Monitor", "Informational", "Device Removed / Device Absent",
        "Device Inserted / Device Present", "Device Disabled", "DeviceEnabled", "Transition to Running", "Transition to In Test",
        "Transition to Power Off", "Transition to On Line", "Transition to Off Line", "Transition to Off Duty",
        "Transition to Degraded", "Transition to Power Save", "Install Error", "Fully Redundant", "Redundancy Lost",
        "Redundancy Degraded", "Non-redundant:Sufficient Resources from Redundant",
        "Non-redundant:Sufficient Resources from Insufficient Resources", "Non-redundant Insufficient Resources",
        "Redundancy Degraded from Fully Redundant", "Redundancy Degraded from Non-redundant", "D0 Power State", "D1 Power State",
        "D2 Power State", "D3 Power State", "OEM"
    }
};
}

using namespace agent_framework::model::enums;


const char* EntryCode::to_string() const {
    return names[m_value];
}


EntryCode EntryCode::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return EntryCodeEnum(static_cast<EntryCodeEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
          std::string("EntryCode enum value not found: '") + name + "'.");
}


std::vector<std::string> EntryCode::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}


bool EntryCode::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
