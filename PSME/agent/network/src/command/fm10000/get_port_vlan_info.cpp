/*!
 * @brief Implementation of GetPortVlanInfo command.
 *
 * File contains all implementations of methods for GetPortVlanInfo command.
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
 * @file get_port_vlan_info.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"



using namespace agent_framework::command_ref;
using namespace agent_framework::module;

namespace {
void get_port_vlan_info(const GetPortVlanInfo::Request& request,
                        GetPortVlanInfo::Response& response) {

    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();

    response = port_vlan_manager.get_entry(request.get_uuid());
}
}

REGISTER_COMMAND(GetPortVlanInfo, ::get_port_vlan_info);
