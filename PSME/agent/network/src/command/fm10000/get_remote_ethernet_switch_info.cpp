/*!
 * @brief Implementation of GetRemoteEthernetSwitchInfo command.
 *
 * File contains all implementations of methods for GetRemoteEthernetSwitchInfo command.
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
 * @file get_remote_ethernet_switch_info.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/dcrp/remote_switch_info.hpp"



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::network;

using std::runtime_error;

namespace {
void get_remote_ethernet_switch_info(const GetRemoteEthernetSwitchInfo::Request& request,
                                     GetRemoteEthernetSwitchInfo::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    auto& neighbor_manager = network_components->get_remote_switch_manager();
    auto neighbor = neighbor_manager.get_entry(request.get_uuid());
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
    response = neighbor;
}
}

REGISTER_COMMAND(GetRemoteEthernetSwitchInfo, ::get_remote_ethernet_switch_info);
