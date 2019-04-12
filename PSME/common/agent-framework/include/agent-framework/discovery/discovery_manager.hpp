/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#pragma once



#include "agent-framework/module/utils/uuid.hpp"



namespace agent_framework {
namespace discovery {

/*!
 * @brief Handles initial discovery procedure.
 *
 * Discovery manager is responsible for gathering basic information about modules.
 * */
class DiscoveryManager {
public:

    /*! @brief Default constructor */
    DiscoveryManager() = default;


    /*! @brief Enable copy */
    DiscoveryManager(const DiscoveryManager&) = default;


    /*! @brief Enable copy */
    DiscoveryManager& operator=(const DiscoveryManager&) = default;


    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();


    /*!
     * @brief Gather information about Module and its submodules.
     * @return UUID of the discovered Manager component.
     * */
    virtual Uuid discover() = 0;
};

}
}
