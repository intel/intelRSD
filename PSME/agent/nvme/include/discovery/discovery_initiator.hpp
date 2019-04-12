/*!
 * @brief Definition of discovery initiator.
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
 * @file discovery_initiator.hpp
 */

#pragma once

#include "discovery/discovery_worker.hpp"

namespace agent {
namespace nvme {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class DiscoveryInitiator final : public DiscoveryWorker {
public:

    /*! @brief Default constructor. */
    DiscoveryInitiator();

    /*! @brief Copy constructor */
    DiscoveryInitiator(const DiscoveryInitiator&) = default;

    /*! @brief Assignment operator */
    DiscoveryInitiator& operator=(const DiscoveryInitiator&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryInitiator();

    void discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) override;
};

}
}
}

