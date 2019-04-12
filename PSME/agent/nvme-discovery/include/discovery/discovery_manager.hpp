/*!
 * @brief Definition of discovery manager.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file discovery_manager.hpp
 */

#pragma once

#include "agent-framework/discovery/discovery_manager.hpp"
#include "nvme_agent_context.hpp"

#include <string>

namespace agent {
namespace nvme {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class DiscoveryManager final {
public:

    /*! @brief Default constructor. */
    DiscoveryManager();

    /*! @brief Copy constructor */
    DiscoveryManager(const DiscoveryManager&) = default;

    /*! @brief Assignment operator */
    DiscoveryManager& operator=(const DiscoveryManager&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();

    Uuid discover(std::shared_ptr<NvmeAgentContext> context);
};

}
}
}
