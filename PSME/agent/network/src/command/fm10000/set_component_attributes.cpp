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
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "agent-framework/module/requests/validation/network.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "utils/lag.hpp"



using namespace agent_framework;
using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;
using namespace agent::network::api::netlink;
using namespace agent::network::utils;

namespace {
using PortAttributeType = SwitchPortInfo::PortAttributeType;
using PortAttributeValue = SwitchPortInfo::PortAttributeValue;


void check_for_empty_lag(const EthernetSwitchPort& port) {
    if (PortClass::Logical == port.get_port_class()) {
        /* it's not possible to set any of attributes on an empty LAG */
        if (is_logical_port_empty(port.get_port_identifier())) {
            THROW(InvalidValue, "network-agent", "LAG port is empty!");
        }
    }
}


void set_port_attributes(const EthernetSwitchPort& port, const Attributes& attributes,
                         SetComponentAttributes::Response& response) {

    /* Make sure port is not an empty LAG */
    check_for_empty_lag(port);
    NetworkValidator::validate_set_port_attributes(attributes);

    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    SwitchPortInfo port_info(port.get_port_identifier());
    for (const auto& attribute_name : attributes.get_names()) {
        const auto& field_value = attributes.get_value(attribute_name);
        try {
            PortAttributeValue value{};
            auto type = port_info.get_port_attribute_type(attribute_name, field_value);
            const auto& attribute_value = attributes.get_value(attribute_name);

            value.set_value_from_json(attribute_value);

            /* Convert default VLAN UUID into VLAN id */
            if (PortAttributeType::DEFAULTVID == type) {
                const auto& vlan_uuid = attribute_value.asString();
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


void set_chassis_attributes(const std::string& uuid, const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    CommonValidator::validate_set_chassis_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("network-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        try {
            if (literals::Drive::ASSET_TAG == name) {
                CommonComponents::get_instance()->get_chassis_manager().get_entry_reference(uuid)
                    ->set_asset_tag(value.asString());
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "network-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_no_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        try {
            // The response must have a message for every attribute that could not be set.
            const auto& field_value = attributes.get_value(name);
            THROW(agent_framework::exceptions::UnsupportedField, "network-agent",
                  "Setting attribute is not supported.", name, field_value);
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_port_vlan_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    NetworkValidator::validate_set_port_vlan_attributes(attributes);
    set_no_attributes(attributes, response);
}


void set_static_mac_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    NetworkValidator::validate_set_static_mac_attributes(attributes);
    set_no_attributes(attributes, response);
}


void set_acl_rule_attributes(const attribute::Attributes& attributes, SetComponentAttributes::Response& response) {
    NetworkValidator::validate_set_acl_rule_attributes(attributes);
    set_no_attributes(attributes, response);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    auto& rule_manager = network_components->get_acl_rule_manager();
    auto& static_mac_manager = network_components->get_static_mac_manager();

    /* check if this is switch port attributes */
    if (port_manager.entry_exists(uuid)) {
        auto port = port_manager.get_entry(uuid);
        set_port_attributes(port, attributes, response);
        return;
    }
        /* check if this is chassis attributes */
    else if (CommonComponents::get_instance()->get_chassis_manager().entry_exists(uuid)) {
        log_debug(GET_LOGGER("fm10000"), "Setting Chassis attributes [ uuid=" << uuid << " ]");
        set_chassis_attributes(uuid, attributes, response);
    }
        /* check if this is VLAN component */
    else if (port_vlan_manager.entry_exists(uuid)) {
        const auto& vlan_port = port_vlan_manager.get_entry(uuid);
        log_debug(GET_LOGGER("network-agent"), "Setting vlan port attributes isn't supported "
            << "[ vlan=" << vlan_port.get_vlan_id() << " uuid=" << uuid << " ]");
        set_port_vlan_attributes(attributes,response);
    }
        /* check if this is ACL rule */
    else if (rule_manager.entry_exists(uuid)) {
        log_debug(GET_LOGGER("fm10000"), "Setting ACL rule attributes isn't supported [ uuid=" << uuid << " ]");
        set_acl_rule_attributes(attributes,response);
    }
        /* check if this is Static MAC */
    else if (static_mac_manager.entry_exists(uuid)) {
        log_debug(GET_LOGGER("fm10000"), "Setting Static MAC attributes isn't supported [ uuid=" << uuid << " ]");
        set_static_mac_attributes(attributes,response);
    }
        /* exists in other components */
    else if (network_components->get_acl_manager().entry_exists(uuid) ||
        network_components->get_remote_switch_manager().entry_exists(uuid) ||
        network_components->get_switch_manager().entry_exists(uuid) ||
        network_components->get_vlan_manager().entry_exists(uuid)) {
        THROW(InvalidValue, "fm10000", "Operation not available for this component.");
    }
    else {
        /* no component found */
        THROW(InvalidUuid, "network-agent", "Component not found, UUID = '" + uuid + "'.");
    }
}
}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
