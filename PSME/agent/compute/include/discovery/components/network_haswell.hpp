/*!
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
 *
 * @file discovery/components/network_haswell.hpp
 *
 * @brief Interface discovery network haswell
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_DISCOVERYNETWORKHASWELL_HPP
#define	AGENT_COMPUTE_DISCOVERY_DISCOVERYNETWORKHASWELL_HPP

#include <string>

namespace ipmi {
    class ManagementController;
}

namespace agent {
namespace compute {

namespace discovery {

class DiscoveryNetworkHaswell {
public:
    /*!
     * @brief Discovery Avoton modules
     *
     * @param mc IPMI management controller
     * @param module Compute module uuid
     */
    void discovery(ipmi::ManagementController& mc,
                                            const std::string& module) const;

private:
    static constexpr std::uint16_t HASWELL_ETHERNET_INTERFACE_1 = 0x0001;
    static constexpr std::uint16_t HASWELL_ETHERNET_INTERFACE_2 = 0x0004;
};

}
}
}

#endif	/* AGENT_COMPUTE_DISCOVERY_DISCOVERYNETWORKHASWELL_HPP */

