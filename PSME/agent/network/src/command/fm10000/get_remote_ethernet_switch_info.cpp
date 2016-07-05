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
#include "agent-framework/module-ref/model/remote_switch.hpp"
#include "agent-framework/module-ref/model/attributes/status.hpp"
#include "agent-framework/command/network/get_remote_ethernet_switch_info.hpp"
#include "api/dcrp/remote_switch_info.hpp"

using std::runtime_error;

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::command;
using namespace agent::network;

namespace fm10000 {

using namespace agent_framework;

/*! Network command GetRemoteEthernetSwitchInfo */
class GetRemoteEthernetSwitchInfo final : public network::GetRemoteEthernetSwitchInfo {

public:
    /*! Command constructor */
    GetRemoteEthernetSwitchInfo() { }

    using network::GetRemoteEthernetSwitchInfo::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& neighbor_manager = network_manager->get_remote_switch_manager();
        auto neighbor = neighbor_manager.get_entry(request.get_switch());
        api::dcrp::RemoteSwitchInfo switch_info;
        try {
            switch_info.fill_response_with_switch_info(neighbor.get_switch_identifier(),
                                                        neighbor);
            neighbor.set_status({enums::State::Enabled, enums::Health::OK});
        }
        catch (runtime_error& dcrp_error) {
            log_error(GET_LOGGER("fm10000"), "Get neighbor info failed");
            log_debug(GET_LOGGER("fm10000"), dcrp_error.what());
            neighbor.set_status({enums::State::Disabled,
                                enums::Health::Warning});
        }
        response.set_switch(neighbor);
    }

    /*! Command destructor */
    ~GetRemoteEthernetSwitchInfo();
};

GetRemoteEthernetSwitchInfo::~GetRemoteEthernetSwitchInfo() { }

}

static Command::Register<fm10000::GetRemoteEthernetSwitchInfo> g("fm10000");
