/*!
 * @brief Definition of endpoint reader class.
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
 * @file endpoint_reader.hpp
 */

#pragma once

#include "tools/base_endpoint_reader.hpp"

#include <string>

namespace agent {
namespace nvme {
namespace tools {

/*! @brief Implementation of initial discovery. */
class EndpointReader : public BaseEndpointReader {
public:

    static const std::string DATABASE_NVME_PORT;
    static const std::string DATABASE_RDMA_PORT;
    static const std::string DATABASE_NQN;
    static const std::string DATABASE_IPV4;
    static const std::string DATABASE_ROLE;
    static const std::string DATABASE_NVME_DEVICE_UUID;

    /*! @brief Default constructor. */
    EndpointReader() = default;

    /*! @brief Copy constructor */
    EndpointReader(const EndpointReader&) = default;

    /*! @brief Assignment operator */
    EndpointReader& operator=(const EndpointReader&) = default;

    /*! @brief Default destructor. */
    virtual ~EndpointReader();

    BaseEndpointReader::EndpointContainer read_endpoints(std::shared_ptr<NvmeAgentContext> ctx, const Uuid& fabric) override;
};

}
}
}


