/*!
 * @brief Implementation of DeleteEthernetSwitchPortMembers command.
 *
 * File contains all implementations of methods for DeleteEthernetSwitchPortMembers command.
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
 * @file delete_ethernet_switch_port_members.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "utils/lag.hpp"
#include "utils/port.hpp"

#include <stdexcept>



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent::network::api::netlink;
using namespace agent::network::utils;
using namespace agent_framework::exceptions;
using namespace agent::network;

namespace {
/*!
 * @brief Check if port is logical
 *
 * @param[in] port EthernetSwitchPort model
 * */
void check_if_logical(const EthernetSwitchPort& port) {
    if (enums::PortClass::Logical != port.get_port_class()) {
        THROW(agent_framework::exceptions::InvalidValue, "fm10000", "Port is not LAG.");
    }
}


/*!
 * @brief Check if member ports are present
 *
 * @param[in] members EthernetSwitchPort members
 * */
void check_if_members_are_present(const vector<string>& members) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();

    /* check that member ports are present */
    for (const auto& member_uuid : members) {
        if (!port_manager.entry_exists(member_uuid)) {
            THROW(agent_framework::exceptions::InvalidUuid, "fm10000",
                  std::string("Member port not found ") + member_uuid);
        }
    }
}


/*!
 * @brief Check member size
 *
 * @param[in] port EthernetSwitchPort model (LAG)
 * @param[in] members EthernetSwitchPort members
 * */
void check_members_size(const EthernetSwitchPort& port, const vector<string>& members) {
    auto port_members = get_logical_port_members(port.get_port_identifier());

    /* According to the requirements, empty Lag is not
     * supported. Execution of DeleteSwitchPort will cause
     * removing of Lag and member ports */
    if (port_members.size() <= members.size()) {
        THROW(agent_framework::exceptions::UnsupportedValue, "fm10000",
              "Removing all members from Lag is not permitted.");
    }
}


void delete_ethernet_switch_port_members(const DeleteEthernetSwitchPortMembers::Request& request,
                                         DeleteEthernetSwitchPortMembers::Response&) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& port_members_manager = network_components->get_port_members_manager();
    auto port = port_manager.get_entry(request.get_port());
    const auto& members = request.get_members();
    SwitchPortInfo port_info(port.get_port_identifier());

    /* Port must be a logical port */
    check_if_logical(port);

    check_if_members_are_present(members);

    check_members_size(port, members);

    /* remove port members from the LAG port */
    for (const auto& member_uuid : members) {
        const auto& member_port = port_manager.get_entry(member_uuid);

        /* remove member from the LAG port */
        SwitchPortInfo::remove_member(member_port.get_port_identifier());

        /* update VLAN UUID on port */
        init_switch_vlan_port(member_port.get_port_identifier());

        /* Remove member from data model */
        port_members_manager.remove_entry(port.get_uuid(), member_uuid);
    }
}
}

REGISTER_COMMAND(DeleteEthernetSwitchPortMembers, ::delete_ethernet_switch_port_members);
