/*!
 * @brief Definition of Tools class.
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
 * @file tools.hpp
 */

#pragma once



#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/model/volume.hpp"
#include "agent-framework/module/model/zone.hpp"
#include "agent-framework/module/model/storage_pool.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "nvme_agent_context.hpp"

#include <string>
#include <type_traits>



namespace agent {
namespace nvme {
namespace tools {

/*!
 * @brief Checks if given endpoint is in target role.
 * @param endpoint Endpoint to check
 * @return true if endpoint is in target role
 */
bool is_target(const agent_framework::model::Endpoint& endpoint);


/*!
 * @brief Checks if given endpoint is in initiator role.
 * @param endpoint Endpoint to check
 * @return true if endpoint is in initiator role
 */
bool is_initiator(const agent_framework::model::Endpoint& endpoint);


/*!
 * @brief Converts nqn to subnqn.
 * @param nqn proper format nqn ex: nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
 */
void convert_to_subnqn(std::string& nqn);


/*!
 * @brief Updates storage pool consumed capacity
 * @param storage_pool_uuid Uuid of the storage pool to be updated
 */
void update_storage_pool_consumed_capacity(const Uuid& storage_pool_uuid);


/*!
 * @brief Check if volume is in use by an Endpoint.
 * @param uuid Uuid of the volume.
 * @return True if volume is in use, false otherwise.
 */
bool is_volume_in_endpoint(const Uuid& uuid);


/*!
 * @brief Check if volume is in use by an Endpoint.
 * @param volume The volume.
 * @return True if volume is in use, false otherwise.
 */
bool is_volume_in_endpoint(const agent_framework::model::Volume& volume);


/*!
 * @brief Check if volume is shared over fabrics.
 * @param uuid Uuid of the volume.
 * @return True if volume is shared over fabrics, false otherwise.
 */
bool is_volume_shared(const Uuid& uuid);


/*!
 * @brief Check if volume is shared over fabrics.
 * @param volume The volume.
 * @return True if volume is shared over fabrics, false otherwise.
 */
bool is_volume_shared(const agent_framework::model::Volume& volume);


/*!
 * @brief Set the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param initiator_endpoint An initiator endpoint.
 * @param endpoints A container of endpoints. It must contain only target endpoints.
 */
template<typename EndpointContainer>
void set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                          const typename EndpointContainer::value_type& initiator_endpoint,
                          const EndpointContainer& endpoints, bool clear_filter = false) {
    assert(is_initiator(initiator_endpoint));
    auto initiator_nqn = agent_framework::model::attribute::Identifier::get_nqn(initiator_endpoint);
    convert_to_subnqn(initiator_nqn);
    for (const auto& endpoint : endpoints) {
        assert(is_target(endpoint));
        auto target_nqn = agent_framework::model::attribute::Identifier::get_nqn(endpoint);
        convert_to_subnqn(target_nqn);
        static const std::string SYSFS_NAMESPACE_NUMBER = "1";
        if (clear_filter) {
            ctx->nvme_target_handler->set_subsystem_namespace_enable(target_nqn, SYSFS_NAMESPACE_NUMBER, false);
            ctx->nvme_target_handler->remove_subsystem_host(target_nqn, initiator_nqn);
        }
        else {
            ctx->nvme_target_handler->add_subsystem_host(target_nqn, initiator_nqn);
            ctx->nvme_target_handler->set_subsystem_namespace_enable(target_nqn, SYSFS_NAMESPACE_NUMBER, true);
        }
    }
}


/*!
 * @brief Set the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param endpoints A container of endpoints. At least one initiator and one target endpoint is necessary.
 * @param clear_filter A flag to switch to clearing the filter. This is a *private* parameter, please use
 * clear_initiator_filter instead.
 */
template<typename EndpointContainer>
void set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx, const EndpointContainer& endpoints,
                          bool clear_filter = false) {
    EndpointContainer initiators{};
    EndpointContainer targets{};

    // separate initiator and target endpoints
    for (const auto& endpoint : endpoints) {
        if (is_target(endpoint)) {
            targets.push_back(endpoint);
        }
        else if (is_initiator(endpoint)) {
            initiators.push_back(endpoint);
        }
        else {
            throw std::runtime_error("Invalid endpoint role");
        }
    }

    if (initiators.size() == 1) {
        set_initiator_filter(ctx, initiators.front(), targets, clear_filter);
    }
    else {
        log_warning("nvme-agent", "No initiator specified for setting/clearing filter");
    }
}


/*!
 * @brief Clear the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param endpoints A container of target endpoints.
 */
template<typename EndpointContainer>
void clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx, const EndpointContainer& endpoints) {
    set_initiator_filter(ctx, endpoints, true);
}


/*!
 * @brief Clear the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param initiator_endpoint An initiator endpoint.
 * @param endpoints A container of endpoints. It must contain only target endpoints.
 */
template<typename EndpointContainer>
void clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                            const typename EndpointContainer::value_type& initiator_endpoint,
                            const EndpointContainer& endpoints) {
    set_initiator_filter(ctx, initiator_endpoint, endpoints, true);
}


/*!
 * @brief Clear the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param endpoints An array object of endpoint UUIDs.
 * @param clear_filter A flag to switch to clearing the filter. This is a *private* parameter, please use
 * clear_initiator_filter instead.
 */
template<>
void set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                          const agent_framework::model::attribute::Array<Uuid>& endpoints, bool clear_filter);


/*!
 * @brief Clear the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param endpoints A vector of endpoint UUIDs.
 * @param clear_filter A flag to switch to clearing the filter. This is a *private* parameter, please use
 * clear_initiator_filter instead.
 */
template<>
void set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                          const std::vector<Uuid>& endpoints, bool clear_filter);


/*!
 * @brief Clear the allowed host filter to all target endpoints.
 * @param ctx The NVMe agent context.
 * @param endpoints A container of endpoints. It must contain at least one initiator endpoint.
 */
template<>
void clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                            const agent_framework::model::attribute::Array<Uuid>& endpoints);


/*!
 * @brief Clear the allowed host filter to all target endpoints in the specified zone.
 * @param ctx The NVMe agent context.
 * @param zone the zone for which all filters are to be cleared.
 */
template<>
void clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx, const Uuid& zone);

}
}
}
