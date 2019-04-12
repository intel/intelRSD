/*!
 * @brief Definition of discovery worker virtual class.
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
 * @file discovery_worker.hpp
 */

#pragma once

#include "agent-framework/module/model/resource.hpp"

namespace agent {
namespace nvme {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class DiscoveryWorker {
public:

    /*! @brief Default constructor. */
    DiscoveryWorker() = default;

    /*! @brief Copy constructor */
    DiscoveryWorker(const DiscoveryWorker&) = default;

    /*! @brief Assignment operator */
    DiscoveryWorker& operator=(const DiscoveryWorker&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryWorker();

    virtual void discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) = 0;
};

}
}
}

