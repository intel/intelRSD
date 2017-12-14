/*!
 * @brief Implementation of SetComponentAttributes command.
 *
 * File contains all implementations of methods for SetComponentAttributes command.
 *
 * @copyright Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file set_component_attributes.cpp
 */


#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "agent-framework/module/requests/validation/network.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "hal/switch_port_info_impl.hpp"
#include "hal/switch_vlan.hpp"
#include "utils/lag.hpp"



using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;
using namespace agent::network::hal;
using namespace agent::network::utils;

namespace {
using PortAttributeType = SwitchPortInfo::PortAttributeType;
using PortAttributeValue = SwitchPortInfo::PortAttributeValue;


void set_port_attributes(const EthernetSwitchPort& port, const Attributes& attributes,
                         SetComponentAttributes::Response& response) {

    /* Make sure port is not an empty LAG */
    NetworkValidator::validate_set_port_attributes(attributes);

    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    SwitchPortInfoImpl port_info(port.get_port_identifier());
    for (const auto& attribute_name : attributes.get_names()) {
        const auto& field_value = attributes.get_value(attribute_name);
        try {
            PortAttributeValue value{};
            auto type = port_info.get_port_attribute_type(attribute_name, field_value);
            const auto& attribute_value = attributes.get_value(attribute_name);

            value.set_value_from_json(attribute_value);

            /* Convert default VLAN UUID into VLAN id */
            if (PortAttributeType::DEFAULTVID == type) {
                const auto& vlan_uuid = attribute_value.get<std::string>();
                const auto& vlan_port = port_vlan_manager.get_entry(vlan_uuid);
                value.set(vlan_port.get_vlan_id());
            }

            log_debug(GET_LOGGER("network-agent"), "Setting port attributes [ attribute="
                << attribute_name << " port="
                << port.get_port_identifier() << " ]");

            port_info.set_switch_port_attribute(type, value);
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_port_vlan_attributes(const EthernetSwitchPortVlan& port_vlan,
                              const attribute::Attributes& attributes,
                              SetComponentAttributes::Response&) {
    NetworkValidator::validate_set_port_vlan_attributes(attributes);
    SwitchVlan switch_vlan{};
    for (const auto& attribute_name : attributes.get_names()) {
        switch_vlan.set_attribute(port_vlan.get_uuid(), attribute_name,
                                  attributes.get_value(attribute_name));
    }
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();

    /* check if this is switch port attributes */
    if (port_manager.entry_exists(uuid)) {
        auto port = port_manager.get_entry(uuid);
        set_port_attributes(port, attributes, response);
        return;
    }
        /* check if this is VLAN component */
    else if (port_vlan_manager.entry_exists(uuid)) {
        const auto& vlan_port = port_vlan_manager.get_entry(uuid);
        log_debug(GET_LOGGER("network-agent"), "Setting vlan port attributes "
            << "[ vlan=" << vlan_port.get_vlan_id() << " uuid=" << uuid << " ]");
        set_port_vlan_attributes(vlan_port, attributes, response);
    }
    else {
        /* no component found */
        THROW(InvalidUuid, "network-agent", "Component not found, UUID = '" + uuid + "'.");
    }
}
}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
