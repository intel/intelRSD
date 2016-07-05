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
 * @file discovery/components/haswell.hpp
 *
 * @brief Interface discovery haswell
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_COMPONENTS_DISCOVERYHASWELL_HPP
#define	AGENT_COMPUTE_DISCOVERY_COMPONENTS_DISCOVERYHASWELL_HPP

#include <string>

namespace ipmi {
    class ManagementController;
}

namespace agent {
namespace compute {

namespace discovery {

class DiscoveryHaswell {
public:
    /*!
     * @brief Discovery Haswell modules
     *
     * @param mc IPMI management controller
     * @param module Compute module uuid
     */
    void discovery(ipmi::ManagementController& mc,
                                            const std::string& module) const;
};

}
}
}

#endif	/* AGENT_COMPUTE_DISCOVERY_COMPONENTS_DISCOVERYHASWELL_HPP */

