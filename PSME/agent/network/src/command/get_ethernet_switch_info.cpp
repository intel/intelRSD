/*!
 * @brief Implementation of GetEthernetSwitchInfo command.
 *
 * File contains all implementations of methods for GetEthernetSwitchInfo command.
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
 * @file get_switch_info.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "hal/switch_info_impl.hpp"
#include "loader/network_config.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::network::hal;
using namespace agent::network::loader;

namespace {
string get_unique_switch_identifier(const string& mac_address) {
    string ret{"sw"};
    std::string::size_type pos{};
    // generate unique identifier based on MAC address
    ret += mac_address;
    // remove unneeded characters
    while (std::string::npos != (pos = ret.find(':'))) {
        ret.erase(pos, 1);
    }
    return ret;
}


void get_switch_info(const GetEthernetSwitchInfo::Request& request,
                     GetEthernetSwitchInfo::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    const auto& switch_manager = network_components->get_switch_manager();
    auto switch_module = switch_manager.get_entry(request.get_uuid());
    const auto& mgmt_port = NetworkConfig().get_instance()->
        get_switch_mgmt_port(switch_module.get_switch_identifier());
    SwitchInfoImpl switch_info{mgmt_port};
    if (switch_info.get_is_enabled()) {
        switch_module.set_status({enums::State::Enabled,
                                  enums::Health::OK});
        /* read switch information thru Netlink */
        switch_info.read_switch_info();
        switch_module.set_mac_address(switch_info.get_mac_address());
        switch_module.set_switch_identifier(::get_unique_switch_identifier(switch_info.get_mac_address()));
    }
    else {
        switch_module.set_status({enums::State::Disabled,
                                  enums::Health::Warning});
    }

    response = switch_module;
}
}

REGISTER_COMMAND(GetEthernetSwitchInfo, ::get_switch_info);
