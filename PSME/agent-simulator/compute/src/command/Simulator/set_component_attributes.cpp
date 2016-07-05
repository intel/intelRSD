/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/command/compute/set_component_attributes.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "asset_configuration/asset_configuration.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;

using agent::AssetConfiguration;
using agent_framework::exceptions;

namespace agent {
/*! Simulator SetComponentAttributes implementation */
class SetComponentAttributes : public compute::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using compute::SetComponentAttributes::execute;

    void execute(const Request& request, Response&) {
        auto& asset = agent::AssetConfiguration::get_instance();
        xmlpp::Node* parent = asset.get_drawer();
        xmlpp::Node* blade = find_element(parent, request);

        if (!blade) {
            THROW(InvalidUuid, "rpc", "Blade not found!");
        }

        const auto& attributes = request.get_attributes();
        const auto& attributes_names = attributes.get_names();
        for (const auto& attribute_name : attributes_names) {
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
                THROW(UnsupportedParameter, "rpc", "Unsupported attribute: oem.");
            }
            else {
                THROW(UnsupportedParameter, "rpc", "Unsupported attribute: " + attribute_name + ".");
            }
        }
    }
    ~SetComponentAttributes();
private:
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

};

SetComponentAttributes::~SetComponentAttributes() {}
}
static Command::Register<agent::SetComponentAttributes> g("Simulator");
