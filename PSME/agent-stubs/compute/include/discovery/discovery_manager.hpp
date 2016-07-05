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

#ifndef DISCOVERYMANAGER_HPP
#define	DISCOVERYMANAGER_HPP

#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/logger_ext.hpp"

using agent_framework::generic::Module;
using agent_framework::generic::Submodule;
using agent_framework::generic::ModuleManager;

namespace agent {
namespace compute {
namespace discovery {

/*!
 * @brief Implementation of initial discovery.
 *
 * Gets component id, translates it to IP and port number. After that
 * performs discovery of the module.
 */
class DiscoveryManager: public agent_framework::generic::DiscoveryManager {
public:

    /*! @brief Default constructor. */
    DiscoveryManager();

    /*! @brief Copy constructor. */
    DiscoveryManager(const DiscoveryManager&) = default;

    /*! @brief Assignment operator */
    DiscoveryManager& operator=(const DiscoveryManager&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();

    /*!
     * @brief Gather information about Module and its submodules.
     *
     * Uses component id to get IP and PORT number. After that performs discovery.
     *
     * @param module component id/uuid used to map Module to its IP and PORT.
     */
    virtual void discover(Module& module) const;
};

}
}
}
#endif	/* DISCOVERYMANAGER_HPP */

