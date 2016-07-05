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
 * @file discovery_manager.hpp
 *
 * @brief Initial discovery implementation.
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_DISCOVERYMANAGER_HPP
#define	AGENT_COMPUTE_DISCOVERY_DISCOVERYMANAGER_HPP

#include "agent-framework/discovery/discovery.hpp"

#include <memory>

namespace ipmi {
    class ManagementController;
}

namespace agent {
namespace compute {

namespace discovery {

/*!
 * @brief Implementation of initial discovery. Gets component id, translates it
 * to IP and port number. After that performs discovery of the module.
 */
class DiscoveryManager final : public ::agent_framework::discovery::Discovery {
private:
    std::unique_ptr<ipmi::ManagementController> m_mc;

public:

    /*!
     *  @brief Default constructor.
     *  Initializes ipmitool based implementation of ManagementController
     */
    DiscoveryManager();

    /*!
     * @brief Sets the ipmi::ManagementController implementation.
     */
    DiscoveryManager(std::unique_ptr<ipmi::ManagementController>& mc);


    /*!
     * @brief Default destructor.
     */
     ~DiscoveryManager();

    void discovery(const std::string& module) override;
};

}
}
}
#endif	/* DISCOVERYMANAGER_HPP */
