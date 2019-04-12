/*!
 * @brief SPDK Endpoint  builder class interface.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_endpoint_builder.hpp
 */

#pragma once



#include "agent-framework/discovery/builders/endpoint_builder.hpp"
#include "spdk/model/nvmf_subsystem.hpp"



namespace agent {
namespace spdk {
namespace discovery {

class SpdkEndpointBuilder : public agent_framework::discovery::EndpointBuilder {
public:

    /*!
     * @brief Build default SPDK endpoint.
     * @param parent_uuid Parent UUID.
     * @return Endpoint object.
     */
    static agent_framework::model::Endpoint build_default(const Uuid& parent_uuid);

    /*!
     * @brief Updates target endpoint model with SPDK model data.
     * @param[in,out] endpoint The endpoint to be updated.
     * @param[in] nvmf_subsystem SPDK NVMf subsystem data.
     */
    static void update_target_with_spdk(agent_framework::model::Endpoint& endpoint,
                                        const ::spdk::model::NvmfSubsystem& nvmf_subsystem);

    /*!
     * @brief Add IP transport details based on SPDK listen address and interface UUID.
     * @param endpoint The endpoint to be updated.
     * @param listen_address SPDK listen address data.
     * @param system_interfaces Collection of system interfaces.
     */
    static void add_ip_transport_details(agent_framework::model::Endpoint& endpoint,
                                         const ::spdk::model::ListenAddress& listen_address,
                                         const std::vector<Uuid>& system_interfaces);

};

}
}
}