/*!
 * @brief Definition of discovery.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file discovery.hpp
 */

#pragma once



#include "discovery/discovery_worker.hpp"



namespace agent {
namespace fpga_discovery {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class Discovery final : public DiscoveryWorker {
public:

    /*! @brief Constructor. */
    Discovery() = default;


    /*! @brief Copy constructor */
    Discovery(const Discovery&) = default;


    /*! @brief Assignment operator */
    Discovery& operator=(const Discovery&) = default;


    /*! @brief Default destructor. */
    virtual ~Discovery();


    /*!
     * @brief Discover all resources
     * @param manager_uuid uuid of the main manager
     * @param fabric_uuid uuid of the fabric
     */
    void discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) override;

};

}
}
}
