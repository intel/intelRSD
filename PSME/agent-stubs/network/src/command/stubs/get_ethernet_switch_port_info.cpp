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
#include "agent-framework/command/network/get_ethernet_switch_port_info.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::command;

/*! Network command GetEthernetSwitchPortInfo */
class GetEthernetSwitchPortInfo final : public network::GetEthernetSwitchPortInfo {
public:
    /*! Command constructor */
    GetEthernetSwitchPortInfo() { }

    using agent_framework::command::network::GetEthernetSwitchPortInfo::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto& manager = agent_framework::module::NetworkManager::
            get_instance()->get_port_manager();
        std::string uuid = request.get_port();
        SwitchPort port{};

        if (manager.entry_exists(uuid)) {
            port = manager.get_entry(uuid);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "network-agent",
                "Port not found, uuid = " + uuid);
        }
        response.set_port(std::move(port));
    }

    /*! Command destructor */
    ~GetEthernetSwitchPortInfo();
};

GetEthernetSwitchPortInfo::~GetEthernetSwitchPortInfo() { }

static Command::Register<GetEthernetSwitchPortInfo> g("Stubs");
