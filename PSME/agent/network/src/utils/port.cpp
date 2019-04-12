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
 * @file port.cpp
 *
 * @brief Switch port utils
 * */

#include "utils/port.hpp"

#include "agent-framework/module/network_components.hpp"
#include "hal/switch_port_info_impl.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"




using namespace agent::network::hal;

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

using std::string;

void agent::network::utils::init_switch_vlan_port(const string& port_identifier) {
    SwitchPortInfoImpl port_info{port_identifier};
    std::string port_uuid{};
    if (!get_port_uuid_by_identifier(port_identifier, port_uuid)) {
        throw std::runtime_error("UUID not found for port identifier " +
                                 port_identifier);
    }
    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    for (const auto& vlan : port_info.get_vlans()) {
        EthernetSwitchPortVlan portvlan_model{port_uuid};
        portvlan_model.set_vlan_id(vlan.get_vlan_id());
        portvlan_model.set_tagged(vlan.is_tagged());
        portvlan_model.set_vlan_enable(true);
        portvlan_model.set_status({State::Enabled, Health::OK});
        port_vlan_manager.add_entry(std::move(portvlan_model));
        log_debug("network-agent", "Adding port-vlan ["
                  << "port=" << port_identifier
                  << " vlan=" << vlan.get_vlan_id()
                  << " tag=" << vlan.is_tagged()
                  << "]");
    }
}

bool agent::network::utils::get_port_uuid_by_identifier(
        const string& port_identifier, string& port_uuid) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    for (const auto& uuid : port_manager.get_keys()) {
        auto port = port_manager.get_entry(uuid);
        if (port_identifier == port.get_port_identifier()) {
            port_uuid = uuid;
            return true;
        }
    }
    return false;
}

void agent::network::utils::send_update_event(const std::string& parent_uuid,
                                              const std::string& uuid) {
    agent_framework::model::attribute::EventData edat;
    edat.set_component(uuid);
    edat.set_type(::agent_framework::model::enums::Component::EthernetSwitchPort);
    edat.set_notification(::agent_framework::model::enums::Notification::Update);
    edat.set_parent(parent_uuid);
    agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
}


void agent::network::utils::set_port_neighbor_mac(const std::string& port_name,
                                                  const std::string& neighbor_mac) {
    auto& port_manager = get_manager<EthernetSwitchPort>();
    for (const auto& port_uuid : port_manager.get_keys()) {
        auto port_model = port_manager.get_entry_reference(port_uuid);
        auto switch_uuid = port_model->get_parent_uuid();

        if (PortType::Downstream == port_model->get_port_type()) {
            if (port_model->get_port_identifier() == port_name) {
                port_model->set_neighbor_mac(neighbor_mac);
                log_debug("network-agent", "Adding neighbor MAC address " + neighbor_mac +
                                                       " to interface " + port_name);
                send_update_event(switch_uuid, port_uuid);

            } else {
                if (port_model->get_neighbor_mac() == neighbor_mac) {
                    log_debug("network-agent", "Deleting neighbor MAC address " + neighbor_mac +
                                                           " from interface " + port_name);
                    port_model->set_neighbor_mac({});
                    send_update_event(switch_uuid, port_uuid);
                }
            }
        }
    }
}


bool agent::network::utils::is_port_physical_or_up(const std::string& port_identifier, const json::Json& switch_config) {
    if (switch_config.count("interface " + port_identifier) == 1) {
        return true;
    }
    return false;
}