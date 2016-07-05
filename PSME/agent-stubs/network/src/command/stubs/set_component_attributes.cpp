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
 * @file command/stubs/set_component_attributes.cpp
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/command/network/set_component_attributes.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/logger_ext.hpp"

#include <string>
using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;


/*! @brief SetComponentAttributes implementation */
class SetComponentAttributes : public network::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using network::SetComponentAttributes::execute;
    using NetworkManager = agent_framework::module::NetworkManager;

    void execute(const Request& request, Response&) {

        const auto& uuid = request.get_component();
        const auto& attributes = request.get_attributes();
        std::string message{};

        if (NetworkManager::get_instance()->get_port_manager().entry_exists(uuid)) {
            message += "For ethernet switch port with uuid: " + uuid;
            process_port(uuid, attributes, message);
        }
        else if (NetworkManager::get_instance()->get_vlan_manager().entry_exists(uuid)) {
            message += "For vlan with uuid: " + uuid;
            process_vlan(uuid, attributes, message);
        }
        else if (NetworkManager::get_instance()->get_port_vlan_manager().entry_exists(uuid)) {
            message += "For port vlan with uuid: " + uuid;
            process_port_vlan(uuid, attributes, message);
        }
        else if (NetworkManager::get_instance()->get_acl_rule_manager().entry_exists(uuid)) {
            message += "Operation not supported by switch";
            THROW(exceptions::UnsupportedParameter, "network-agent", message);
        }
        else if (NetworkManager::get_instance()->get_static_mac_manager().entry_exists(uuid)) {
            message += "For static MAC with uuid: " + uuid;
            process_static_mac(uuid, attributes, message);
        }
        else if (NetworkManager::get_instance()->get_switch_manager().entry_exists(uuid)
                || NetworkManager::get_instance()->get_remote_switch_manager().entry_exists(uuid)
                || NetworkManager::get_instance()->get_module_manager().entry_exists(uuid)) {
            message =
                "SetComponentAttributes command is not available for this component";
            THROW(exceptions::InvalidParameters, "network-agent", message);
        }
        else {
            message = "No component with uuid: " + uuid;
            THROW(exceptions::InvalidUuid, "network-agent", message);
        }

        message += "setComponentAttributes finished succesfully";
        log_info(GET_LOGGER("network-agent"), message);
    }

    ~SetComponentAttributes();

private:
    /* message_short is being appended to message only if the attribute value
    is printable */
    void process_port(const std::string& uuid, const Attributes& attributes, std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto port = NetworkManager::get_instance()->
                get_port_manager().get_entry_reference(uuid);

        for (const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if (literals::SwitchPort::LINK_SPEED_MBPS == name) {
                port->set_link_speed_mbps(value.asUInt());
                message_short += std::to_string(value.asUInt());
            }
            else if (literals::SwitchPort::ADMINISTRATIVE_STATE == name) {
                try {
                    port->set_administrative_state(enums::
                        AdministrativeState::from_string(value.asString()));
                    message_short += value.asString();
                }
                catch(const exceptions::InvalidParameters&) {
                    message = "Did not change  " + name + " attribute to: " +
                        value.asString() + " - invalid value.";
                    THROW(exceptions::InvalidParameters,
                            "network-agent", message);
                }
            }
            else if (literals::SwitchPort::FRAME_SIZE == name) {
                port->set_frame_size(value.asUInt());
                message_short += std::to_string(value.asUInt());
            }
            else if (literals::SwitchPort::AUTO_SENSE == name) {
                port->set_auto_sense(value.asBool());
                message_short += value.asBool() ? "true" : "false";
            }
            else if (literals::SwitchPort::MODE == name) {
                try {
                    port->set_port_mode(enums::PortMode::from_string(
                        value.asString()));
                    message_short += value.asString();
                }
                catch(const exceptions::InvalidParameters&) {
                    message = "Did not change  " + name + " attribute to: " +
                        value.asString() + " - invalid value.";
                    THROW(exceptions::InvalidParameters,
                            "network-agent", message);
                }
            }
            else if (literals::SwitchPort::VLAN_ENABLE == name) {
                port->set_vlan_enable(value.asBool());
                message_short += value.asBool() ? "true" : "false";
            }
            else if (literals::SwitchPort::DEFAULT_VLAN == name) {
                port->set_default_vlan(value.asString());
                message_short += value.asString();
            }
            else if (literals::SwitchPort::OEM == name) {
                port->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message = "Unknown or unsettable attribute name: " + name;
                THROW(exceptions::UnsupportedParameter,
                        "network-agent", message);
            }
            message += message_short + "\n";
        }
    }

    void process_vlan(const std::string& uuid, const Attributes& attributes, std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto vlan = NetworkManager::get_instance()->
                get_vlan_manager().get_entry_reference(uuid);

        for (const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if (literals::Vlan::VLAN_NAME == name) {
                vlan->set_vlan_name(value.asString());
                message_short += value.asString();
            }
            else if (literals::Vlan::VLAN_ENABLE == name) {
                vlan->set_vlan_enable(value.asBool());
                message_short += value.asBool() ? "true" : "false";
            }
            else if (literals::Vlan::OEM == name) {
                vlan->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message = "Unknown or unsettable attribute name: " + name;
                THROW(exceptions::UnsupportedParameter,
                        "network-agent", message);
            }
            message += message_short + "\n";
        }
    }

    void process_port_vlan(const std::string& uuid, const Attributes& attributes, std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto portvlan = NetworkManager::get_instance()->
                get_port_vlan_manager().get_entry_reference(uuid);

        for (const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if (literals::PortVlan::VLAN_NAME == name) {
                portvlan->set_vlan_name(value.asString());
                message_short += value.asString();
            }
            else if (literals::PortVlan::VLAN_ENABLE == name) {
                portvlan->set_vlan_enable(value.asBool());
                message_short += value.asBool() ? "true" : "false";
            }
            else if (literals::PortVlan::OEM == name) {
                portvlan->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message = "Unknown or unsettable attribute name: " + name;
                THROW(exceptions::UnsupportedParameter,
                        "network-agent", message);
            }
            message += message_short + "\n";
        }
    }

    void process_static_mac(const std::string& uuid, const Attributes& attributes, std::string& message) {
        const auto& attribute_names = attributes.get_names();
        auto static_mac = NetworkManager::get_instance()->
                get_static_mac_manager().get_entry_reference(uuid);

        for (const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if (literals::StaticMac::ADDRESS == name) {
                static_mac->set_address(value.asString());
                message_short += value.asString();
            }
            else if (literals::StaticMac::VLAN_ID == name) {
                static_mac->set_vlan_id(value.asUInt());
                message_short += value.asUInt() ? "true" : "false";
            }
            else if (literals::StaticMac::OEM == name) {
                static_mac->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message = "Unknown or unsettable attribute name: " + name;
                THROW(exceptions::UnsupportedParameter,
                      "network-agent", message);
            }
            message += message_short + "\n";
        }
    }
};

SetComponentAttributes::~SetComponentAttributes() {}

static Command::Register<SetComponentAttributes> g("Stubs");
