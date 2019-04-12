/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file sensor_type.cpp
 */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module/enum/sensor_type.hpp"
#include <array>



namespace {
static std::array<const char*, 43> names = {
    {
        "Platform Security Violation Attempt", "Temperature", "Voltage", "Current", "Fan", "Physical Chassis Security",
        "Processor", "Power Supply / Converter", "PowerUnit", "CoolingDevice", "Other Units-based Sensor", "Memory",
        "Drive Slot/Bay", "POST Memory Resize", "System Firmware Progress", "Event Logging Disabled", "System Event",
        "Critical Interrupt", "Button/Switch", "Module/Board", "Microcontroller/Coprocessor", "Add-in Card", "Chassis",
        "ChipSet", "Other FRU", "Cable/Interconnect", "Terminator", "SystemBoot/Restart", "Boot Error",
        "BaseOSBoot/InstallationStatus", "OS Stop/Shutdown", "Slot/Connector", "System ACPI PowerState", "Watchdog",
        "Platform Alert", "Entity Presence", "Monitor ASIC/IC", "LAN", "Management Subsystem Health", "Battery",
        "Session Audit", "Version Change", "FRUState"
    }
};
}

using namespace agent_framework::model::enums;


const char* SensorType::to_string() const {
    return names[m_value];
}


SensorType SensorType::from_string(const std::string& name) {
    for (size_t i = 0; i < names.size(); i++) {
        if (name == names[i]) {
            return SensorTypeEnum(static_cast<SensorTypeEnum>(i));
        }
    }
    THROW(agent_framework::exceptions::InvalidValue, "agent-framework",
            std::string("SensorType enum value not found: '") + name + "'.");
}


std::vector<std::string> SensorType::get_values() {
    return std::vector<std::string>(names.cbegin(), names.cend());
}


bool SensorType::is_allowable_value(const std::string& string) {
    for (size_t i = 0; i < names.size(); i++) {
        if (string == names[i]) {
            return true;
        }
    }
    return false;
}
