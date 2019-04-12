/*!
 * @brief Definition of endpoint creator base virtual class.
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
 * @file base_endpoint_creator.hpp
 */

#pragma once

#include <string>
#include <memory>
#include "nvme_agent_context.hpp"

namespace agent {
namespace nvme {
namespace tools {

class BaseEndpointCreator {
public:
    /*!
     * @brief Create a target endpoint.
     *
     * @param ip_address IPv4 address on which the endpoint will be accessed.
     * @param port RDMA port of the endpoint.
     * @param nqn NQN string for the endpoint.
     * @param device_path System path to the drive/partition device.
     *
     * @return NVME port that was assigned to this endpoint
     */
    virtual uint16_t create_target_endpoint(std::shared_ptr<NvmeAgentContext> context, const std::string& ip_address,
                                            uint16_t port, const std::string& nqn,
                                            const std::string& device_path) const = 0;

    /*!
     * @brief Create an initiator endpoint.
     *
     * @param nqn NQN string for the initiator.
     */
    virtual void create_initiator_endpoint(std::shared_ptr<NvmeAgentContext> context, const std::string& nqn) const = 0;

    /*!
     * @brief Delete target endpoint from system.
     *
     * @param nqn NQN of the target
     * @param nvme_port NVME port of the target
     */
    virtual void delete_target_endpoint(std::shared_ptr<NvmeAgentContext> context, const std::string& nqn,
                                        const std::string& nvme_port) const = 0;
};

}
}
}

