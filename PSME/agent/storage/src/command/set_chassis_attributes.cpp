/*!
 * @copyright
 * Copyright (c) 2018 Intel Corporation
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
 * */

#include "agent-framework/module/requests/validation/common.hpp"
#include "command/set_chassis_attributes.hpp"

using namespace agent::storage;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::command;
using namespace agent_framework::model::requests::validation;

namespace agent {
namespace storage {

void process_chassis_attributes(const Uuid& uuid, const attribute::Attributes& attributes,
                                SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_chassis_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("storage-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Chassis::ASSET_TAG == name) {
                get_manager<Chassis>().get_entry_reference(uuid)->set_asset_tag(value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "storage-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}

}
}
