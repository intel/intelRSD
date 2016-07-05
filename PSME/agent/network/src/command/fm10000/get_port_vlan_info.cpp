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

#include "agent-framework/command/network/get_port_vlan_info.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "api/netlink/switch_vlan.hpp"
#include "network_config.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent::network::api::netlink;

namespace fm10000 {

class GetPortVlanInfo : public network::GetPortVlanInfo {

public:
    GetPortVlanInfo() { }

    using network::GetPortVlanInfo::execute;

    void execute(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        const auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        const auto& port_manager = network_manager->get_port_manager();
        auto port_vlan = port_vlan_manager.get_entry(request.get_port_vlan());
        auto port = port_manager.get_entry(port_vlan.get_parent_uuid());

        SwitchVlan::VlanPortInfo info{};
        SwitchVlan vlan{SwitchVlan::VlanId(port_vlan.get_vlan_id())};
        vlan.get_info(port.get_port_identifier(), info);
        port_vlan.set_tagged(info.get_tagged());

        response.set_port_vlan(std::move(port_vlan));
    }

    ~GetPortVlanInfo();
};

GetPortVlanInfo::~GetPortVlanInfo() { }

}

static
Command::Register<fm10000::GetPortVlanInfo> g("fm10000");
