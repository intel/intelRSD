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
 * */

#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "loader/network_loader.hpp"
#include "loader/network_config.hpp"
#include "logger/logger_factory.hpp"
#include "json/json.hpp"

using namespace agent::network::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
void check_required_fields(const json::Value& config) {
    if (!config.is_member("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }
    const auto& agent_prop = config["agent"];
    if (!agent_prop.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent_prop.is_member("vendor") ||
            !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.is_member("capabilities") ||
           !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.is_member("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.is_member("ipv4") ||
            !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.is_member("port") ||
            !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.is_member("interval") ||
            !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.is_member("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.is_member("port") ||
            !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.is_member("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }
    auto& managers_array = config["managers"].as_array();

    if (managers_array.empty()) {
        throw std::runtime_error("'managers' array should have at least one entry.");
    }

    for(const auto& manager: managers_array) {
        if (!manager.is_member("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.is_member("serialConsoleEnabled") || !manager["serialConsoleEnabled"].is_boolean()) {
            throw std::runtime_error("Each entry in manager must have 'serialConsoleEnabled' field.");
        }
    }
}

bool add_port_vlan_module(const std::string& port_identifier,
    uint32_t vlan_id, bool vlan_tag) {
    auto& port_manager = NetworkManager::get_instance()->get_port_manager();
    for (const auto& uuid : port_manager.get_keys()) {
        const auto port = port_manager.get_entry(uuid);
        if (port_identifier == port.get_port_identifier()) {
            PortVlan portvlan_model{port.get_uuid()};
            portvlan_model.set_vlan_id(vlan_id);
            portvlan_model.set_tagged(vlan_tag);
            portvlan_model.set_vlan_enable(true);
            NetworkManager::get_instance()->
                            get_port_vlan_manager().add_entry(portvlan_model);
            log_debug(GET_LOGGER("network-agent"), "Created PortVlan module [vlan="
                    << portvlan_model.get_vlan_id() << " port="
                    << port_identifier << " tag=" << vlan_tag << "]");
            return true;
        }
    }
    return false;
}

SwitchPort make_port_module(const json::Value& json, const std::string& switch_uuid) {
    if (json.is_member("id") && json["id"].is_string()) {
        SwitchPort port_model{switch_uuid};
        port_model.set_port_identifier(json["id"].as_string());

        /* read link state of the port */
        if (json.is_member("linkState") && json["linkState"].is_string()) {
            port_model.set_administrative_state(AdministrativeState::from_string(
                                                json["linkState"].as_string()));
        }

        /* read Ethernet mode of the port */
        if (json.is_member("ethmod") && json["ethmod"].is_string()) {
            NetworkConfig().get_instance()->
                add_port_ethmode(port_model.get_port_identifier(),
                    EthMode::from_string(json["ethmod"].as_string()));
        }

        /* read port link technology */
        if (json.is_member("linkTechnology") && json["linkTechnology"].is_string()) {
            port_model.set_link_technology(LinkTechnology::from_string(
                                            json["linkTechnology"].as_string()));
        }

        /* read port type */
        if (json.is_member("portType") && json["portType"].is_string()) {
            port_model.set_port_type(PortType::from_string(
                                            json["portType"].as_string()));
        }

        /* read autoneg of the port */
        if (json.is_member("autoneg") && json["autoneg"].is_string()) {
            NetworkConfig().get_instance()->
                add_port_autoneg(port_model.get_port_identifier(),
                    AutoNeg::from_string(json["autoneg"].as_string()));
        }

        log_debug(GET_LOGGER("network-agent"), "Created Port module [port="
                << port_model.get_port_identifier() << "]");

        return port_model;
    }
    throw std::runtime_error("'id' field for port is required.");
}

bool make_vlan_module(const json::Value& json) {
    if (json.is_member("id") && json["id"].is_number()) {
        /* read tagged ports */
        auto vlan_id = json["id"].as_uint();
        if (json.is_member("tagged_ports") && json["tagged_ports"].is_array()) {
            for (const auto& port : json["tagged_ports"].as_array()) {
                if (!add_port_vlan_module(port.as_string(), vlan_id, true)) {
                    log_error(GET_LOGGER("network-agent"), "Failed to add PortVlan module");
                }
            }
        }
        /* read untagged ports */
        if (json.is_member("untagged_ports") && json["untagged_ports"].is_array()) {
            for (const auto& port : json["untagged_ports"].as_array()) {
                if (!add_port_vlan_module(port.as_string(), vlan_id, false)) {
                    log_error(GET_LOGGER("network-agent"), "Failed to add PortVlan module");
                }
            }
        }
        return true;
    }
    return false;
}

Switch make_switch_module(const json::Value& json,
                          const std::string& manager_uuid) {
    Switch switch_model{manager_uuid};

    /* set switch identifier */
    auto switch_count = NetworkManager::get_instance()->
        get_switch_manager().get_entry_count();
    switch_model.set_switch_identifier(std::to_string(switch_count));

    /* read switch management port name */
    if (json.is_member("mgmt_port") && json["mgmt_port"].is_string()) {
        NetworkConfig().get_instance()->
            add_switch_mgmt_port(switch_model.get_switch_identifier(),
                                 json["mgmt_port"].as_string());
    }

    /* read port configurations */
    if (json.is_member("ports") && json["ports"].is_array()) {
        for (const auto& port_json : json["ports"].as_array()) {
            auto port = make_port_module(port_json, switch_model.get_uuid());
            NetworkManager::get_instance()->
                get_port_manager().add_entry(port);
        }
    }

    /* read vlan configurations */
    if (json.is_member("vlans") && json["vlans"].is_array()) {
        for (const auto& vlan_json : json["vlans"].as_array()) {
            make_vlan_module(vlan_json);
        }
    }

    /* read vlan public configurations */
    if (json.is_member("public_vlans") && json["public_vlans"].is_array()) {
        for (auto vlan_id : json["public_vlans"].as_array()) {
            NetworkConfig().get_instance()->add_public_vlan(vlan_id.as_uint());
        }
    }

    /* add chassis */
    Chassis chassis_model{manager_uuid};
    chassis_model.set_type(enums::ChassisType::Module);
    chassis_model.set_status(attribute::Status(true));
    switch_model.set_chassis(chassis_model.get_uuid());

    log_debug(GET_LOGGER("network-agent"), "Created Chassis module");
    NetworkManager::get_instance()->
        get_chassis_manager().add_entry(chassis_model);

    /* return new switch model object */
    log_debug(GET_LOGGER("network-agent"), "Created Switch module [switch="
        << switch_model.get_switch_identifier() << " mgmt="
        << NetworkConfig().get_instance()->get_switch_mgmt_port(
                    switch_model.get_switch_identifier()) << "]");
    return switch_model;
}

Manager make_manager(const json::Value& element) {
    Manager manager{};
    manager.set_status({enums::State::Enabled, enums::Health::OK});
    manager.set_ipv4_address(element["ipv4"].as_string());

    attribute::SerialConsole console{};
    console.set_enabled(element["serialConsoleEnabled"].as_bool());
    manager.set_serial_console(std::move(console));

    auto nm = NetworkManager::get_instance();
    if (element.is_member("switches") && element["switches"].is_array()) {
        for (const auto& switch_json : element["switches"].as_array()) {
            auto switch_module = make_switch_module(switch_json,
                                                    manager.get_uuid());
            nm->get_switch_manager().add_entry(switch_module);
        }
    }

    log_debug(GET_LOGGER("network-agent"), "Created Manager module [ipv4="
            << manager.get_ipv4_address() << "]");
    return manager;
}

}

void NetworkLoader::read_fabric_modules(const json::Value& config) {
    auto& managers_array = config["managers"].as_array();

    auto nm = NetworkManager::get_instance();
    for (const auto& element : managers_array) {
        auto manager = make_manager(element);
        nm->get_module_manager().add_entry(manager);
    }
}

bool NetworkLoader::load(const json::Value& json) {
    try {
        check_required_fields(json);
        read_fabric_modules(json);
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("network-agent"),
                "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}
