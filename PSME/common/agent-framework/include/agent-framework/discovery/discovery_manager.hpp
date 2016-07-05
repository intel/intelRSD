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
 * @brief Contains interface used to perform initial discovery on modules.
 * */

#ifndef AGENT_FRAMEWORK_DISCOVERY_DISCOVERY_MANAGER_HPP
#define	AGENT_FRAMEWORK_DISCOVERY_DISCOVERY_MANAGER_HPP

#include "agent-framework/module/module.hpp"

namespace agent_framework {
namespace generic {

/*!
 * @brief Handles initial discovery procedure which is responsible for
 * gathering basic information about modules.
 */
class DiscoveryManager {
public:

    /*!
     * @brief Default constructor.
     */
    DiscoveryManager() = default;

    /*!
     * @brief Copy constructor.
     * @param orig reference to object which will be copied.
     */
    DiscoveryManager(const DiscoveryManager& orig) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~DiscoveryManager();

    /*!
     * @brief Gather information about Module and its submodules.
     *
     * Uses component id to get IP and PORT number.
     * After that performs discovery.
     *
     * @param module component id/uuid used to map Module to its IP and PORT.
     */
    virtual void discover(Module& module) const = 0;
};

}
}
#endif	/* AGENT_FRAMEWORK_DISCOVERY_DISCOVERY_MANAGER_HPP */
