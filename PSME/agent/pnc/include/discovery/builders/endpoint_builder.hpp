/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file endpoint_builder.hpp
 *
 * @brief EndpointBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create endpoint objects
 */
class EndpointBuilder : public AbstractBuilder<agent_framework::model::Endpoint> {

public:

    using ReturnType = EndpointBuilder&;

    EndpointBuilder() = default;
    ~EndpointBuilder() = default;

    /*!
     * @brief Adds a host connected entity
     * @return Pointer to the builder
     * */
    virtual ReturnType add_host_entity();

    /*!
     * @brief Adds a PCIe device connected entity
     * @param[in] device_uuid Processor's uuid
     * @return Pointer to the builder
     * */
    virtual ReturnType add_device_entity(const Uuid& device_uuid);

    /*!
     * @brief Adds a unknown target connected entity
     * @return Pointer to the builder
     * */
    virtual ReturnType add_unknown_target_entity();

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;
};

}
}
}
}
