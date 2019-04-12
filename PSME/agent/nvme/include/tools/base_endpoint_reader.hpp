/*!
 * @brief Definition of endpoint reader base virtual class.
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
 * @file base_endpoint_reader.hpp
 */

#pragma once

#include "nvme_agent_context.hpp"

#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/model/model_common.hpp"

#include <memory>

namespace agent {
namespace nvme {
namespace tools {

/*! @brief Implementation of initial discovery. */
class BaseEndpointReader {
public:

    using EndpointContainer = std::vector<agent_framework::model::Endpoint>;

    /*!
     * @brief Destructor
     */
    virtual ~BaseEndpointReader() {}

    /*!
     * @brief Read endpoints and propagate model under fabric.
     *
     * @param ctx Runtime context
     * @param fabric Fabric's UUID
     */
    virtual EndpointContainer read_endpoints(std::shared_ptr<NvmeAgentContext> ctx, const Uuid& fabric) = 0;
};

}
}
}

