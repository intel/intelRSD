/*!
 * @brief Implementation of DeleteEthernetSwitchPort command.
 *
 * File contains all implementations of methods for DeleteEthernetSwitchPort command.
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
 * @file delete_ethernet_switch_port.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/netlink/switch_info.hpp"
#include "utils/lag.hpp"
#include "utils/port.hpp"

#include <stdexcept>



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;
using namespace agent::network::api::netlink;
using namespace agent::network::utils;
using namespace agent_framework::exceptions;

namespace {
void delete_ethernet_switch_port(const DeleteEthernetSwitchPort::Request& request,
                                 DeleteEthernetSwitchPort::Response&) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    const auto port = port_manager.get_entry(request.get_port());

    /* only LAG port can be removed */
    if (PortClass::Logical != port.get_port_class()) {
        THROW(InvalidValue, "fm10000", "Physical port can't be removed.");
    }

    /* get LAG members */
    auto members = get_logical_port_members(port.get_port_identifier());

    /* delete the port thru Netlink API */
    SwitchInfo info{{}};
    info.delete_switch_port(port.get_port_identifier());

    /* update VLAN UUID on members of the Lag */
    for (const auto& member : members) {
        init_switch_vlan_port(member);
    }

    /* remove VLAN UUID on the LAG */
    port_vlan_manager.remove_by_parent(port.get_uuid());

    /* delete the port form network manager */
    port_manager.remove_entry(port.get_uuid());
}
}

REGISTER_COMMAND(DeleteEthernetSwitchPort, ::delete_ethernet_switch_port);
