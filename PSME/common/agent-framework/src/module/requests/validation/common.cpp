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
 * */

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void CommonValidator::validate_set_drive_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Drive::ASSET_TAG == name) {
            check_string(value, name, "agent-framework");
        }
        else if (literals::Drive::ERASED == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::Drive::SECURELY_ERASE == name) {
            // value does not matter
        }
        else if (literals::Drive::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value) ;
        }
    }
    log_debug(GET_LOGGER("agent-framework"), "Request validation passed.");
}


void CommonValidator::validate_set_chassis_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Chassis::ASSET_TAG == name) {
            check_string(value, name, "agent-framework");
        }
        else if (literals::Chassis::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug(GET_LOGGER("agent-framework"), "Request validation passed.");
}


void CommonValidator::validate_set_system_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::System::BOOT_OVERRIDE == name) {
            check_enum<enums::BootOverride>(value, name, "agent-framework");
        }
        else if (literals::System::BOOT_OVERRIDE_MODE == name) {
            check_enum<enums::BootOverrideMode>(
                value, name, "agent-framework");
        }
        else if (literals::System::BOOT_OVERRIDE_TARGET == name) {
            check_enum<enums::BootOverrideTarget>(
                value, name, "agent-framework");
        }
        else if (literals::System::POWER_STATE == name) {
            check_enum<enums::ResetType>(value, name, "agent-framework");
        }
        else if (literals::System::ASSET_TAG == name) {
            check_string(value, name, "agent-framework");
        }
        else if (literals::System::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug(GET_LOGGER("agent-framework"), "Request validation passed.");
}

}
}
}
}
