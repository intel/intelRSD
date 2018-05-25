/*!
 * @brief Grantley network interface builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file grantley_network_interface_builder.hpp
 */

#include "discovery/builders/platform_specific/grantley/grantley_network_interface_builder.hpp"

#include <bitset>
#include <algorithm>



using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;


void
GrantleyNetworkInterfaceBuilder::update_mac_address_info(agent_framework::model::NetworkInterface& network_interface,
                                                         const ipmi::command::sdv::response::GetSystemMacAddressHaswell& get_mac_address_response) {
    auto status = network_interface.get_status();
    status.set_state(enums::State::Enabled);

    network_interface.set_mac_address(get_mac_address_response.get_system_mac_address());
    network_interface.set_factory_mac_address(get_mac_address_response.get_system_mac_address());
    network_interface.set_status(status);
}
