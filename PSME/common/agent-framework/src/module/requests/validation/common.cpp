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
 * */

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/validators/procedure_validator.hpp"



using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void CommonValidator::validate_set_drive_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Drive::ASSET_TAG == name) {
            check_nullable_string(value, name, "agent-framework");
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
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_processor_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Fpga::ERASED == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::Fpga::SECURELY_ERASE == name) {
            // value does not matter
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_chassis_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Chassis::ASSET_TAG == name) {
            check_nullable_string(value, name, "agent-framework");
        }
        else if (literals::Chassis::RESET == name) {
            check_enum<enums::ResetType>(value, name, "agent-framework");
        }
        else if (literals::Chassis::GEO_TAG == name) {
            check_nullable_string(value, name, "agent-framework");
        }
        else if (literals::Chassis::LOCATION_ID == name) {
            check_string(value, name, "agent-framework");
        }
        else if (literals::Chassis::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_system_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::System::BOOT_OVERRIDE == name) {
            check_enum<enums::BootOverride>(value, name, "agent-framework");
        }
        else if (literals::System::BOOT_OVERRIDE_MODE == name) {
            check_enum<enums::BootOverrideMode>(value, name, "agent-framework");
        }
        else if (literals::System::BOOT_OVERRIDE_TARGET == name) {
            check_enum<enums::BootOverrideTarget>(value, name, "agent-framework");
        }
        else if (literals::System::RESET == name) {
            check_enum<enums::ResetType>(value, name, "agent-framework");
        }
        else if (literals::System::ASSET_TAG == name) {
            check_nullable_string(value, name, "agent-framework");
        }
        else if (literals::System::USER_MODE_ENABLED == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::System::RESET_CONFIGURATION == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::System::ERASE_CONFIGURATION_KEYS == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::System::OEM == name) {
            Oem::from_json(value);
        }
        else if (literals::System::CURRENT_PERFORMANCE_CONFIGURATION == name) {
            check_number(value, name, "agent-framework");
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_manager_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);

        if (literals::Manager::RESET == name) {
            check_enum<enums::ResetType>(value, name, "agent-framework");
        }
        else if (literals::Manager::FACTORY_DEFAULTS == name) {
            check_boolean(value, name, "agent-framework");
        }
        else if (literals::Manager::PACKAGE_URL == name) {
            check_string(value, name, "agent-framework");
        }
        else if (literals::Manager::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "agent-framework", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_pcie_device_attributes(const Attributes& attributes) {
    jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::PcieDevice::ASSET_TAG, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        literals::PcieDevice::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    );

    validator.validate(attributes.to_json());
    log_debug("agent-framework", "Request validation passed.");
}


void CommonValidator::validate_set_endpoint_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::Endpoint::USERNAME, VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING)),
        literals::Endpoint::PASSWORD, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        nullptr
    );
    validator.validate(attributes.to_json());

    log_debug("agent-framework", "Request validation passed.");
}

}
}
}
}
