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
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/command/chassis_commands.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/requests/validation/common.hpp"

using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;


namespace {

using agent_framework::module::CommonComponents;

void process_chassis(const std::string& uuid, const attribute::Attributes& attributes,
                     SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_chassis_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("chassis-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Drive::ASSET_TAG == name) {
                agent_framework::module::get_manager<Chassis>().get_entry_reference(uuid)->set_asset_tag(value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "chassis-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}



void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    log_info("chassis-agent", "Executing setComponentAttributes.");

    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (agent_framework::module::get_manager<Chassis>().entry_exists(uuid)) {
        process_chassis(uuid, attributes, response);
    }
    else {
        THROW(InvalidUuid, "chassis-agent", "No component with UUID = '" + uuid + "'.");
    }

    log_info("chassis-agent", "setComponentAttributes finished successfully.");
}

}


REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
