/*!
 * @brief Implementation of AddEthernetSwitchPortMembers command.
 *
 * File contains all implementations of methods for AddEthernetSwitchPortMembers command.
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
 * @file add_ethernet_switch_port_members.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/netlink/add_lag_port_message.hpp"
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
 * @brief Validate port members
 *
 * @param[in] port EthernetSwitchPort model (LAG)
 * @param[in] members EthernetSwitchPort members
 * @param[out] empty true if empty otherwise false
 * */
void validate_port_members_and_check_if_empty(const EthernetSwitchPort& port,
                                              const std::vector<string>& members,
                                              bool& empty) {
    auto port_members =
        get_logical_port_members(port.get_port_identifier());

    if ((port_members.size() + members.size()) > MAX_LAG_MEMBER_COUNT) {
        THROW(agent_framework::exceptions::InvalidValue, "fm10000",
              "Maximum LAG members count is " + std::to_string(MAX_LAG_MEMBER_COUNT) + ".");
    }

    empty = port_members.empty();
    if (!empty) {
        SwitchPortInfo::PortAttributeValue speed{};
        const auto& slaved_port = port_members.front();
        SwitchPortInfo slaved_port_info(slaved_port);
        slaved_port_info.get_switch_port_attribute(SwitchPortInfo::LINKSPEEDMBPS, speed);
        validate_port_members(members, int(speed));
    }
    else {
        validate_port_members(members);
    }
}


/*!
 * @brief Add member to the lag port
 *
 * @param[in] lag port model (LAG)
 * @param[in] member_port EthernetSwitchPort member
 * */
void add_member_to_lag(const EthernetSwitchPort& lag, const EthernetSwitchPort& member_port) {
    SwitchPortInfo member_port_info(member_port.get_port_identifier());

    /* force member port admin state "Down" */
    SwitchPortInfo::PortAttributeValue value{};
    value.set(string(enums::AdministrativeState(
        enums::AdministrativeState::Down).to_string()));
    member_port_info.set_switch_port_attribute(
        SwitchPortInfo::ADMINISTRATIVESTATE, value);

    /* add member into the LAG port */
    SwitchPortInfo::add_member(lag.get_port_identifier(),
                               member_port.get_port_identifier());
}


void add_ethernet_switch_port_members(const AddEthernetSwitchPortMembers::Request& request,
                                      AddEthernetSwitchPortMembers::Response&) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    auto port = port_manager.get_entry(request.get_port());
    const auto& members = request.get_members();
    bool empty{true};

    /* Port must be a logical port */
    check_if_logical(port);

    validate_port_members_and_check_if_empty(port, members,
                                             empty);

    /* add port members into the LAG port */
    for (const auto& member_uuid : members) {
        const auto& member_port = port_manager.get_entry(member_uuid);

        add_member_to_lag(port, member_port);

        /* remove vlan UUIDs */
        port_vlan_manager.remove_by_parent(member_port.get_uuid());
    }

    /* update VLAN on the LAG */
    if (empty) {
        init_switch_vlan_port(port.get_port_identifier());
    }
}
}

REGISTER_COMMAND(AddEthernetSwitchPortMembers, ::add_ethernet_switch_port_members);
