/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @brief Set Component Attributes JSONRPC command implementation.
 * */

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"
#include "asset_configuration/asset_configuration.hpp"

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using agent::AssetConfiguration;
using agent_framework::exceptions;

namespace {

    void set_boot_override(xmlpp::Node* blade,
                           const std::string& boot_override) {

        AssetConfiguration::set_content(blade,
                literals::Blade::BOOT_OVERRIDE,
                boot_override);
    }

    void set_boot_override_target(xmlpp::Node* blade,
                                  const std::string& boot_override_target) {

        AssetConfiguration::set_content(blade,
                literals::Blade::BOOT_OVERRIDE_TARGET,
                boot_override_target);
    }

    void set_power_state(xmlpp::Node* blade,
                         const std::string& power_state) {

        AssetConfiguration::set_content(blade,
                literals::Blade::POWER_STATE,
                power_state);
    }

    void set_component_attributes(const SetComponentAttributes::Request& request,
                                  SetComponentAttributes::Response& response) {

        auto& asset = agent::AssetConfiguration::get_instance();
        xmlpp::Node* parent = asset.get_drawer();
        xmlpp::Node* blade = find_element(parent, request);

        if (!blade) {
            THROW(InvalidUuid, "compute-agent", "Blade not found!");
        }

        const auto& attributes = request.get_attributes();
        ComputeValidator::validate_set_system_attributes(attributes);
        const auto& attribute_names = attributes.get_names();
        if (attribute_names.empty()) {
            log_info("compute-agent", "Nothing has been changed (empty request).");
            return;
        }

        const auto& attributes_names = attributes.get_names();
        for (const auto& attribute_name : attributes_names) {
            try {
                const auto& value = attributes.get_value(attribute_name);
                if (attribute_name.compare(literals::Blade::BOOT_OVERRIDE) == 0) {
                    set_boot_override(blade, value.asString());
                }
                else if (literals::Blade::BOOT_OVERRIDE_TARGET == attribute_name) {
                    set_boot_override_target(blade, value.asString());
                }
                else if (literals::Blade::POWER_STATE == attribute_name) {
                    set_power_state(blade, value.asString());
                }
                else if (literals::OEMData::OEM == attribute_name) {
                    set_oem(Oem::from_json(value));
                    log_debug("compute-agent", "Changed " + name + " attribute.");
                }
                else {
                    THROW(UnsupportedField, "compute-agent", "Unsupported attribute: " + attribute_name + ".");
                }
                response.set_result(attribute_name);
            }
            catch (const exceptions::ExceptionBase& ex) {
                response.set_result(attribute_name, {ex.get_error_code(), ex.get_message()});
            }
        }
    }
}

REGISTER_COMMAND(SetComponentAttributes, set_component_attributes);
