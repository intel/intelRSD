/*!
 * @brief Ethernet interface discoverer declaration.
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
 * @file ethernet_interface_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/model/network_interface.hpp"



namespace agent_framework {
namespace discovery {

class EthernetInterfaceDiscoverer {
public:

    /*!
     * @brief Constructor
     * @param interface_names List of interfaces to parse
     */
    explicit EthernetInterfaceDiscoverer(const std::vector<std::string>& interface_names) :
        m_interface_names(interface_names) {}


    /*! Virtual destructor */
    virtual ~EthernetInterfaceDiscoverer();


    /*!
     * @brief Perform discovery of ethernet interface objects.
     * @param parent_uuid Parent UUID.
     * @return Ethernet interface collection filled by the method with discovered data.
     * */
    virtual std::vector<model::NetworkInterface> discover(const Uuid& parent_uuid);


protected:
    std::vector<std::string> m_interface_names{};

};

}
}
