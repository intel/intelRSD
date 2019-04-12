/*!
 * @brief Storage pool builder class implementation.
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation.
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
 * @file storage_pool_builder.cpp
 */

#include "agent-framework/discovery/builders/storage_pool_builder.hpp"



using namespace agent_framework::discovery;
using namespace agent_framework;


model::StoragePool StoragePoolBuilder::build_default(const Uuid& parent_uuid) {
    model::StoragePool pool{parent_uuid};

    pool.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Volumes,
        model::enums::CollectionType::Volumes
    ));

    return pool;
}


void StoragePoolBuilder::add_drive_capacity_source(model::StoragePool& storage_pool,
                                                   std::int64_t allocated_bytes,
                                                   std::int64_t consumed_bytes,
                                                   const Uuid& drive_uuid) {

    model::attribute::CapacitySource capacity_source{};
    capacity_source.set_allocated_bytes(allocated_bytes);
    capacity_source.set_consumed_bytes(consumed_bytes);
    capacity_source.set_providing_drives({drive_uuid});
    storage_pool.add_capacity_source(capacity_source);
}

