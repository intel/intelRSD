/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/netlink/switch_vlan.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "tree_stability/network_tree_stabilizer.hpp"



using namespace agent_framework::model;
using namespace agent_framework::command_ref;
using namespace agent::network::api::netlink;
using namespace agent::network;

namespace {
void add_port_vlan(const AddPortVlan::Request& request, AddPortVlan::Response& response) {
    auto network_components = agent_framework::module::NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    const auto& port_manager = network_components->get_port_manager();
    auto port = port_manager.get_entry(request.get_port());

    /* check if the VLAN already exists */
    for (const auto& uuid : port_vlan_manager.get_keys(port.get_uuid())) {
        auto port_vlan_module = port_vlan_manager.get_entry(uuid);
        if (request.get_vlan_id() == port_vlan_module.get_vlan_id()) {
            THROW(agent_framework::exceptions::InvalidValue, "fm10000",
                  "VLAN " + std::to_string(request.get_vlan_id()) +
                  " already exists on port " + port.get_port_identifier() + ".");
        }
    }

    /* add new vlan on the switch port thru Netlink API */
    SwitchVlan vlan_info{SwitchVlan::VlanId(request.get_vlan_id())};
    vlan_info.add_port(port.get_port_identifier(), request.get_tagged());
    /* add new vlan to CPU port if needed as required by DCRP */
    if (enums::PortType::Downstream == port.get_port_type()) {
        SwitchPortInfo port_info(SwitchPortInfo::CPU_PORT);
        if (!port_info.vlan_exists(vlan_info.get_vlan_id())) {
            /* add new VLAN to CPU port as tagged */
            vlan_info.add_port(SwitchPortInfo::CPU_PORT, true);
        }
    }

    /* add port-vlan into network manager */
    EthernetSwitchPortVlan port_vlan{port.get_uuid()};
    const std::string port_vlan_uuid = port_vlan.get_uuid();

    port_vlan.set_vlan_id(request.get_vlan_id());
    port_vlan.set_tagged(request.get_tagged());
    port_vlan.set_vlan_enable(true);
    port_vlan.set_status({
                             enums::State::Enabled,
                             enums::Health::OK
                         });
    port_vlan_manager.add_entry(std::move(port_vlan));

    const std::string port_vlan_persistent_uuid = NetworkTreeStabilizer().stabilize_port_vlan(port_vlan_uuid);

    response.set_port_vlan(port_vlan_persistent_uuid);
}
}

REGISTER_COMMAND(AddPortVlan, ::add_port_vlan);
