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
#include "agent-framework/command/network/get_remote_ethernet_switch_info.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::command;

/*! Network command GetRemoteEthernetSwitchInfo */
class GetRemoteEthernetSwitchInfo final : public network::GetRemoteEthernetSwitchInfo {
public:
    /*! Command constructor */
    GetRemoteEthernetSwitchInfo() { }

    using agent_framework::command::network::GetRemoteEthernetSwitchInfo::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto& manager = agent_framework::module::NetworkManager::
            get_instance()->get_remote_switch_manager();
        std::string uuid = request.get_switch();
        RemoteSwitch rem_switch{};

        if (manager.entry_exists(uuid)) {
            rem_switch = manager.get_entry(uuid);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid,
                "network-agent", "Remote Switch not found, uuid = " + uuid);
        }
        response.set_switch(std::move(rem_switch));
    }

    /*! Command destructor */
    ~GetRemoteEthernetSwitchInfo();
};

GetRemoteEthernetSwitchInfo::~GetRemoteEthernetSwitchInfo() { }

static Command::Register<GetRemoteEthernetSwitchInfo> g("Stubs");
