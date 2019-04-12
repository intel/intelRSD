/*!
 * @brief Definition of discovery target.
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
 * @file discovery_target.hpp
 */

#pragma once

#include "discovery/discovery_worker.hpp"
#include "tools/base_endpoint_reader.hpp"
#include "tools/base_endpoint_creator.hpp"
#include "nvme_agent_context.hpp"

namespace agent {
namespace nvme {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class DiscoveryTarget final : public DiscoveryWorker {
public:

    /*! @brief Constructor. */
    DiscoveryTarget(std::shared_ptr<NvmeAgentContext> context, tools::BaseEndpointReader& endpoint_reader,
        tools::BaseEndpointCreator& endpoint_creator);

    /*! @brief Copy constructor */
    DiscoveryTarget(const DiscoveryTarget&) = default;

    /*! @brief Assignment operator */
    DiscoveryTarget& operator=(const DiscoveryTarget&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryTarget();

    void discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) override;

private:
    tools::BaseEndpointReader& m_endpoint_reader;
    tools::BaseEndpointCreator& m_endpoint_creator;
    std::shared_ptr<NvmeAgentContext> m_context;
};

}
}
}
