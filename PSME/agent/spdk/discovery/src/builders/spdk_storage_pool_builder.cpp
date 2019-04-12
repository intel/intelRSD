/*!
 * @brief SPDK storage pool builder class implementation.
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
 * @file spdk_storage_pool_builder.cpp
 */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "discovery/builders/spdk_storage_pool_builder.hpp"

using namespace agent_framework;
using namespace agent_framework::discovery;
using namespace agent::spdk::discovery;

namespace {

static constexpr const char STORAGE_POOL_DESCRIPTION[] = "SPDK Logical Volume Store (LVOL Store)";

}

void SpdkStoragePoolBuilder::update_with_spdk(model::StoragePool& storage_pool,
                                              const ::spdk::model::LvolStore& lvol_store) {

    storage_pool.set_name(lvol_store.get_name());
    storage_pool.set_description(::STORAGE_POOL_DESCRIPTION);
    storage_pool.set_parent_type(model::enums::Component::StorageService);

    storage_pool.set_block_size_bytes(lvol_store.get_block_size());

    std::uint64_t allocated_bytes = lvol_store.get_cluster_size() * lvol_store.get_total_data_clusters();
    std::uint64_t free_bytes = lvol_store.get_free_clusters() * lvol_store.get_cluster_size();
    std::uint64_t consumed_bytes = allocated_bytes - free_bytes;

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(allocated_bytes);
    capacity.set_consumed_bytes(consumed_bytes);
    capacity.set_guaranteed_bytes(free_bytes);
    storage_pool.set_capacity(capacity);

    agent_framework::discovery::IdentifierBuilder::set_uuid(storage_pool, lvol_store.get_uuid());

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    storage_pool.set_status(std::move(status));
}
