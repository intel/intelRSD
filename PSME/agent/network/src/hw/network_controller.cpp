/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file hw/network_controller.cpp
 *
 * @brief Interface for Network Controller. Implement this interface to
 * handle connections and sending/receiving commands.
 *
 * */

#include "hw/network_controller.hpp"
#include "agent-framework/module-ref/network_manager.hpp"

#include <cstdlib>
#include <sstream>

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent::network::hw;
using std::string;

NetworkController::NetworkController() {
    // read mesh ports from environment variable
    static constexpr char MESH_PORT_ENV_NAME[] = "MESH_PORTS";
    const auto ports = std::getenv(MESH_PORT_ENV_NAME);
    if (ports) {
        std::stringstream ss{ports};
        string port;
        while (std::getline(ss, port, ' ')) {
            m_mesh_ports.push_back(port);
        }
    }
}

NetworkController::~NetworkController() {}

bool NetworkController::get_port_uuid_by_identifier (
    const string& port_identifier, string& port_uuid) const {
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    for (const auto& uuid : port_manager.get_keys()) {
        auto port = port_manager.get_entry(uuid);
        if (port_identifier == port.get_port_identifier()) {
            port_uuid = uuid;
            return true;
        }
    }
    return false;
}

void NetworkController::show_network_manager_info() {
    auto network_manager = NetworkManager::get_instance();
    auto& module_manager = network_manager->get_module_manager();
    auto& switch_manager = network_manager->get_switch_manager();
    auto& port_manager = network_manager->get_port_manager();
    auto& port_vlan_manager = network_manager->get_port_vlan_manager();
    for (const auto& uuid : module_manager.get_keys()) {
        log_debug(GET_LOGGER("network-agent"), "Manager UUID: " + uuid);
        for (const auto& switch_uuid : switch_manager.get_keys(uuid)) {
            log_debug(GET_LOGGER("network-agent"), "Switch UUID: " + switch_uuid);
            for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
                log_debug(GET_LOGGER("network-agent"), "Port UUID: " + port_uuid);
                for (const auto& port_vlan_uuid :
                                    port_vlan_manager.get_keys(port_uuid)) {
                    log_debug(GET_LOGGER("network-agent"), "Port-vlan UUID: "
                                                            + port_vlan_uuid);
                }
            }
        }
    }
}

void NetworkController::add_physical_port(const std::string& switch_uuid,
                    const std::string& port_identifier) {
    /* add port into network manager */
    bool in_mesh = std::find(m_mesh_ports.begin(), m_mesh_ports.end(),
                             port_identifier) != m_mesh_ports.end();
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    SwitchPort port_model{switch_uuid};
    port_model.add_collection({CollectionName::Vlans,
                              CollectionType::PortVlans, ""});
    port_model.set_link_technology(LinkTechnology::Ethernet);
    port_model.set_port_class(PortClass::Physical);
    port_model.set_port_type(in_mesh ? PortType::MeshPort : PortType::Upstream);
    port_model.set_port_mode(PortMode::Unknown);
    port_model.set_port_identifier(port_identifier);
    port_model.set_vlan_enable(true);
    port_model.set_status({
        enums::State::Enabled,
        enums::Health::OK
    });
    port_manager.add_entry(port_model);
}
