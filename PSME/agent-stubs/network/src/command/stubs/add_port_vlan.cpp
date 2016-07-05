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
 * */
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"

using namespace agent_framework::model;
using namespace agent_framework::command_ref;

namespace {
    void add_port_vlan(const AddPortVlan::Request& request, AddPortVlan::Response& response) {
        using NetworkManager = agent_framework::module::NetworkManager;
        PortVlan port_vlan = PortVlan{request.get_port()};
        port_vlan.set_vlan_id(request.get_vlan_id());
        port_vlan.set_vlan_name(request.get_vlan_name());
        port_vlan.set_tagged(request.get_tagged());
        port_vlan.set_oem(request.get_oem());
        port_vlan.set_status(attribute::Status(
            enums::State::Enabled,
            enums::Health::OK));
        log_debug(GET_LOGGER("network-agent"), "Adding port VLAN: " << port_vlan.get_uuid());
        response.set_port_vlan(port_vlan.get_uuid());
        NetworkManager::get_instance()->get_port_vlan_manager().add_entry(std::move(port_vlan));
    }
}

REGISTER_COMMAND(AddPortVlan, ::add_port_vlan);
