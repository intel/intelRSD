/*!
 * @brief Grantley network interface builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @file gantley_network_interface_builder.hpp
 */

#pragma once



#include "discovery/builders/network_interface_builder.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"



namespace agent {
namespace compute {
namespace discovery {

class GrantleyNetworkInterfaceBuilder : public NetworkInterfaceBuilder {
public:
    /*!
     * @brief Update network interface object with data retrieved via IPMI.
     * @param network_interface Network interface object to be updated with the discovered data.
     * @param get_mac_address_response GetSystemMacAddressHaswell IPMI command response object.
     */
    static void update_mac_address_info(agent_framework::model::NetworkInterface& network_interface,
                                        const ipmi::command::sdv::response::GetSystemMacAddressHaswell& get_mac_address_response);
};

}
}
}
