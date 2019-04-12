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
 * @file lag.cpp
 *
 * @brief LAG utils
 * */

#include "utils/lag.hpp"

#include "hal/switch_port_info_impl.hpp"

#include "agent-framework/module/network_components.hpp"

using namespace agent::network::hal;

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::exceptions;

using std::string;
using std::vector;

string agent::network::utils::add_logical_port(const string& lag_identifier,
        const string& switch_uuid) {
    /* add port into network manager */
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    EthernetSwitchPort port_model{switch_uuid};
    port_model.add_collection({CollectionName::Vlans,
                              CollectionType::EthernetSwitchPortVlans});
    port_model.add_collection({CollectionName::PortAcls,
                              CollectionType::Acls});
    port_model.add_collection({CollectionName::StaticMacs,
                              CollectionType::StaticMacs});
    port_model.set_link_technology(LinkTechnology::Ethernet);
    port_model.set_port_class(PortClass::Logical);
    port_model.set_port_type(PortType::Upstream);
    port_model.set_port_mode(PortMode::LinkAggregationStatic);
    port_model.set_port_identifier(lag_identifier);
    port_model.set_vlan_enable(true);
    port_model.set_status({State::Enabled, Health::OK});
    port_manager.add_entry(port_model);
    return port_model.get_uuid();
}

bool agent::network::utils::is_member_port(const string& port_identifier) {
    SwitchPortInfoImpl port_info{port_identifier};
    return port_info.is_member();
}

bool agent::network::utils::is_logical_port_empty(const string& lag_identifier) {
    SwitchPortInfoImpl port_info{lag_identifier};
    return port_info.get_port_members().size() == 0;
}

vector<string> agent::network::utils::get_logical_port_members(
        const string& lag_identifier) {
    SwitchPortInfoImpl port_info{lag_identifier};
    return port_info.get_port_members();
}

string agent::network::utils::get_lag_identifier(const string& port_identifier) {
    SwitchPortInfoImpl port_info{port_identifier};
    return port_info.get_master_port();
}

void agent::network::utils::validate_port_members(
        const vector<string>& ports, int lag_port_speed) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    int speed{lag_port_speed};

    for (const auto& port_uuid : ports) {
        /* check that member port is present */
        if (!port_manager.entry_exists(port_uuid)) {
            THROW(InvalidUuid, "network-agent",
                string("Member port ") + port_uuid + " not found");
        }

        /* get switch port instance */
        const auto port = port_manager.get_entry(port_uuid);

        /* check if port is physical */
        if (PortClass::Logical == port.get_port_class()) {
            THROW(InvalidValue, "network-agent", "Member port cannot be a Lag.");
        }

        /* check if port is not master of another network device. */
        if (is_member_port(port.get_port_identifier())) {
            THROW(InvalidValue, "network-agent", "Port is already added to another Lag.");
        }

        /* PCIe ports cannot participate in Link Aggregation. */
        if (LinkTechnology::PCIe == port.get_link_technology()) {
            THROW(UnsupportedValue, "network-agent", "PCIe port are not supported on Lag.");
        }

        /* DCRP mesh ports cannot be part of the LAG */
        if (PortType::MeshPort == port.get_port_type()) {
            THROW(InvalidValue, "network-agent", string("Mesh ports are not allowed on Lag: ") + port.get_port_identifier() + ".");
        }

        /* check speed for all ports to be the same */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfoImpl switch_port_info(port.get_port_identifier());
        switch_port_info.get_switch_port_attribute(SwitchPortInfo::LINKSPEEDMBPS, value);
        if (INVALID_PORT_SPEED == speed) {
            speed = int(value);
        }
        else if (speed != int(value)) {
            THROW(InvalidValue, "network-agent", "Speed should be same for all members of the Lag.");
        }
    }
}
