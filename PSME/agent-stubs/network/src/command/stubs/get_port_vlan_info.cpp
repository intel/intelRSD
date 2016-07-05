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
#include "agent-framework/command/network/get_port_vlan_info.hpp"

using namespace agent_framework::model;
using namespace agent_framework::command;

class GetPortVlanInfo : public agent_framework::command::network::GetPortVlanInfo {
public:
    GetPortVlanInfo() { }

    using agent_framework::command::network::GetPortVlanInfo::execute;

    void execute(const Request& request, Response& response) {
        auto& manager = agent_framework::module::NetworkManager::
            get_instance()->get_port_vlan_manager();
        std::string uuid = request.get_port_vlan();
        PortVlan vlan{};

        if (manager.entry_exists(uuid)) {
            vlan = manager.get_entry(uuid);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "network-agent",
                "Port VLAN not found, uuid = " + uuid);
        }
        response.set_port_vlan(std::move(vlan));
    }

    ~GetPortVlanInfo();
};

GetPortVlanInfo::~GetPortVlanInfo() { }

static Command::Register<GetPortVlanInfo> g("Stubs");
