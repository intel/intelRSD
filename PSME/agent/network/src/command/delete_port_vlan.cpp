/*!
 * @brief Implementation of DeletePortVlan command.
 *
 * File contains all implementations of methods for DeletePortVlan command.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file delete_port_vlan.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "hal/switch_info_impl.hpp"
#include "hal/switch_port_vlan.hpp"

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::network;
using namespace agent::network::hal;

namespace {
void delete_port_vlan(const DeletePortVlan::Request& request, DeletePortVlan::Response&) {

    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    const auto& port_manager = network_components->get_port_manager();
    auto port_vlan = port_vlan_manager.get_entry(request.get_port_vlan());
    auto port = port_manager.get_entry(port_vlan.get_parent_uuid());
    auto switch_module = get_manager<EthernetSwitch>().get_entry(port.get_parent_uuid());
    auto vlan_id = port_vlan.get_vlan_id();

    /* delete a vlan on the switch port */
    SwitchPortVlan switch_port_vlan(SwitchPortVlan::VlanId(port_vlan.get_vlan_id()));
    switch_port_vlan.remove_port(port.get_port_identifier(), port_vlan.get_tagged());

    /* remove port-vlan from network manager */
    port_vlan_manager.remove_entry(port_vlan.get_uuid());

    // delete VLAN id from switch if it doesn't exist on any other port
    SwitchInfoImpl switch_info(switch_module.get_switch_identifier());
    if (!switch_info.switch_vlan_exists(vlan_id)) {
        switch_info.delete_switch_vlan(vlan_id);
    }
}
}

REGISTER_COMMAND(DeletePortVlan, ::delete_port_vlan);
