/*!
 * @brief Implementation of Tools class.
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
 * @file tools.cpp
 */

#include "tools/tools.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/storage_components.hpp"



using namespace agent::nvme::tools;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;

namespace {

static inline bool is_in_role(const Endpoint& endpoint, const enums::EntityRole& role) {
    for (const auto& ce : endpoint.get_connected_entities()) {
        if (role == ce.get_entity_role()) {
            return true;
        }
    }
    return false;
}


Uuid get_endpoint_uuid(const Uuid& volume_uuid) {
    for (const auto& endpoint : get_manager<Endpoint>().get_entries()) {
        for (const auto& entity : endpoint.get_connected_entities()) {
            if (volume_uuid == entity.get_entity()) {
                return endpoint.get_uuid();
            }
        }
    }

    return {};
}

}


bool agent::nvme::tools::is_target(const Endpoint& endpoint) {
    return ::is_in_role(endpoint, enums::EntityRole::Target);
}


bool agent::nvme::tools::is_initiator(const Endpoint& endpoint) {
    return ::is_in_role(endpoint, enums::EntityRole::Initiator);
}


bool agent::nvme::tools::is_volume_in_endpoint(const Uuid& uuid) {
    return !get_endpoint_uuid(uuid).empty();
}


bool agent::nvme::tools::is_volume_in_endpoint(const Volume& volume) {
    return is_volume_in_endpoint(volume.get_uuid());
}


bool agent::nvme::tools::is_volume_shared(const Uuid& uuid) {
    auto endpoint_uuid = get_endpoint_uuid(uuid);
    if (endpoint_uuid.empty()) {
        return false;
    }

    // check if the endpoint is in a zone and if that zone has an initiator endpoint
    for (const auto& zone : get_manager<Zone>().get_keys()) {
        auto endpoints = get_m2m_manager<Zone, Endpoint>().get_children(zone);
        if (find(endpoints.cbegin(), endpoints.cend(), endpoint_uuid) != endpoints.cend()) {
            // local lamba function to check for an initiator
            auto is_initiator_endpoint = [](const Uuid& endpoint) {
                // convert to model Endpoint and call helper
                auto& manager = get_manager<Endpoint>();
                return is_initiator(manager.get_entry(endpoint));
            };

            return (find_if(endpoints.cbegin(), endpoints.cend(), is_initiator_endpoint) != endpoints.cend());
        }
    }

    return false;
}


bool agent::nvme::tools::is_volume_shared(const Volume& volume) {
    return is_volume_shared(volume.get_uuid());
}


void agent::nvme::tools::convert_to_subnqn(std::string& nqn) {
    // NQN format: nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
    transform(nqn.begin(), nqn.end(), nqn.begin(), ::tolower);
    size_t found = nqn.find("uuid");
    if (std::string::npos != found) {
        // passing "uuid:" in nqn
        static constexpr short UUID_INFIX_LENGTH = 5;
        found += UUID_INFIX_LENGTH;
        nqn = nqn.substr(found);
    }
    else {
        THROW(exceptions::InvalidValue, "nvme-agent", "Wrong NQN format");
    }
}


void agent::nvme::tools::update_storage_pool_consumed_capacity(const Uuid& storage_pool_uuid) {
    auto volume_uuids = get_m2m_manager<StoragePool, Volume>().get_children(storage_pool_uuid);
    int64_t consumed_capacity_bytes{};
    for (const auto& volume_uuid : volume_uuids) {
        auto volume = get_manager<Volume>().get_entry(volume_uuid);
        if (volume.get_capacity().get_allocated_bytes().has_value()) {
            // check for overlap
            if (consumed_capacity_bytes + volume.get_capacity().get_allocated_bytes() < 0) {
                consumed_capacity_bytes = std::numeric_limits<int64_t>::max();
                log_error("nvme-agent", "Total capacity consumed by volumes overlaps max value");
                break;
            }
            consumed_capacity_bytes += volume.get_capacity().get_allocated_bytes();
        }
    }
    auto storage_pool_ref = get_manager<StoragePool>().get_entry_reference(storage_pool_uuid);
    auto cap = storage_pool_ref->get_capacity();
    cap.set_consumed_bytes(consumed_capacity_bytes);
    storage_pool_ref->set_capacity(cap);
}


template<>
void agent::nvme::tools::clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx, const Uuid& zone) {
    attribute::Array<Uuid> endpoints = get_m2m_manager<Zone, Endpoint>().get_children(zone);
    clear_initiator_filter(ctx, endpoints);
}


template<>
void agent::nvme::tools::clear_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                                                const attribute::Array<Uuid>& endpoints) {
    set_initiator_filter(ctx, endpoints, true);
}


template<>
void agent::nvme::tools::set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                                              const attribute::Array<Uuid>& endpoints,
                                              bool clear_filter) {
    std::vector<Endpoint> endpoints_vector{};
    for (const auto& endpoint_uuid : endpoints) {
        endpoints_vector.push_back(get_manager<Endpoint>().get_entry(endpoint_uuid));
    }
    set_initiator_filter(ctx, endpoints_vector, clear_filter);
}


template<>
void agent::nvme::tools::set_initiator_filter(std::shared_ptr<NvmeAgentContext> ctx,
                                              const std::vector<Uuid>& endpoints, bool clear_filter) {
    set_initiator_filter(ctx, attribute::Array<Uuid>{endpoints}, clear_filter);
}
