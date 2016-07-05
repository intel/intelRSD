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

#ifndef AGENT_STORAGE_DISCOVERY_DISCOVERYMANAGER_HPP
#define AGENT_STORAGE_DISCOVERY_DISCOVERYMANAGER_HPP

#include "agent-framework/discovery/discovery.hpp"

namespace agent {
namespace storage {
namespace discoveries {

/*!
 * @brief Implementation of initial discovery
 */
class DiscoveryManager final : public agent_framework::discovery::Discovery {
public:

    /*! @brief Default constructor */
    DiscoveryManager() = default;

    /*!
     * @brief Gather information about storage and its submodules.
     * @param module component uuid.
     */
    void discovery(const std::string& module) override;

    /*! @brief Default destructor */
     ~DiscoveryManager();
};

}
}
}

#endif /* AGENT_STORAGE_DISCOVERY_DISCOVERYMANAGER_HPP */
