/*!
 * @brief Opaepp network interface discoverer interface.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file fpgaof_opaeproxy_interface_discoverer.hpp
 */

#pragma once


#include "agent-framework/module/model/network_interface.hpp"
#include "agent-framework/module/model/system.hpp"
#include "fpgaof_agent_context.hpp"

namespace agent {
namespace fpgaof {
namespace discovery {

class OpaeproxyNetworkInterfaceDiscoverer {
public:

    /*!
     * @brief Constructor
     * @param context FPGA-oF agent context pointer.
     */
    explicit OpaeproxyNetworkInterfaceDiscoverer(AgentContext::SPtr context) : m_context(context) {}

    /*! Virtual destructor */
    virtual ~OpaeproxyNetworkInterfaceDiscoverer() = default;


    /*!
     * @brief Perform discovery of Network Interface object.
     * @param[in] system_uuid UUID of parent System
     * @return Network Interface object filled with discovered data.
     * */
    virtual std::vector<agent_framework::model::NetworkInterface> discover(const Uuid& system_uuid);

private:
    AgentContext::SPtr m_context{};
};

}
}
}
