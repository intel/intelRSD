/*!
 * @brief Ethernet interface builder class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file ethernet_interface_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/network_interface.hpp"

/* Forward declaration */
namespace net {
class NetworkInterface;
}

namespace agent_framework {
namespace discovery {

class EthernetInterfaceBuilder {
public:

    /*!
     * @brief Build default ethernet interface object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default ethernet interface object.
     */
    static agent_framework::model::NetworkInterface build_default(const Uuid& parent_uuid);


    /*!
     * @brief Update existing interface with data from net interface structure
     * @param interface Ethernet interface to update
     * @param net_interface Interface structure from net library
     */
    static void update_with_net(agent_framework::model::NetworkInterface& interface,
                                const net::NetworkInterface& net_interface);

};

}
}
