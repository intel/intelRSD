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

#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/storage_components.hpp"

using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {


void StorageValidator::validate_set_iscsi_target_attributes(const Attributes& attributes) {
    for(const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);

        if (literals::IscsiTarget::INITIATOR_IQN == name) {
            check_string_no_whitespace(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::AUTHENTICATION_METHOD == name) {
            check_nullable_enum<enums::TargetAuthenticationMethod>(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::CHAP_USERNAME == name) {
            check_nullable_string_no_whitespace(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::CHAP_SECRET == name) {
            check_nullable_string_no_whitespace(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::MUTUAL_CHAP_USERNAME == name) {
            check_nullable_string_no_whitespace(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::MUTUAL_CHAP_SECRET == name) {
            check_nullable_string_no_whitespace(value, name, "storage-agent");
        }
        else if (literals::IscsiTarget::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "storage-agent", "Unrecognized attribute.", name, value);
        }
    }

    log_debug(GET_LOGGER("storage-agent"), "Request validation passed.");
}


void StorageValidator::validate_set_logical_drive_attributes(const Attributes& attributes) {
    for(const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);

        if(literals::LogicalDrive::BOOTABLE == name) {
            check_boolean(value, name, "storage-agent");
        }
        else if (literals::LogicalDrive::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "storage-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug(GET_LOGGER("storage-agent"), "Request validation passed.");
}

}
}
}
}
