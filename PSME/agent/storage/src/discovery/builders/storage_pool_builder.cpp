/*!
 * @brief Storage pool builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file storage_pool_builder.cpp
 */

#include "discovery/builders/identifier_builder.hpp"
#include "discovery/builders/storage_pool_builder.hpp"



using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


model::StoragePool StoragePoolBuilder::build_default(const Uuid& parent_uuid) {
    model::StoragePool pool{parent_uuid};

    pool.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Volumes,
        model::enums::CollectionType::Volumes
    ));

    return pool;
}


void StoragePoolBuilder::update_with_lvm(model::StoragePool& storage_pool,
                                         const lvm::model::VolumeGroup& volume_group) {

    storage_pool.set_status(model::attribute::Status{
        model::enums::State::from_string(volume_group.get_status()),
        model::enums::Health::from_string(volume_group.get_health())
    });

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(volume_group.get_capacity_b());
    capacity.set_consumed_bytes(volume_group.get_capacity_b() - volume_group.get_free_b());
    capacity.set_guaranteed_bytes(volume_group.get_free_b());
    storage_pool.set_capacity(capacity);
}


void StoragePoolBuilder::add_capacity_source(agent_framework::model::StoragePool& storage_pool,
                                             std::uint64_t allocated_bytes,
                                             std::uint64_t consumed_bytes,
                                             const Uuid& drive_uuid) {

    model::attribute::CapacitySource capacity_source{};
    capacity_source.set_allocated_bytes(allocated_bytes);
    capacity_source.set_consumed_bytes(consumed_bytes);
    capacity_source.set_providing_drives({drive_uuid});
    storage_pool.add_capacity_source(capacity_source);
}

void StoragePoolBuilder::add_identifier_with_system_path(agent_framework::model::StoragePool& storage_pool,
                                                         const lvm::model::VolumeGroup& volume_group) {

    /* Device path is essential for matching Drives to StoragePools */
    IdentifierBuilder::add_system_path_identifier(storage_pool, "/dev/" + volume_group.get_name());
}

void StoragePoolBuilder::add_identifier_with_uuid(agent_framework::model::StoragePool& storage_pool) {
    IdentifierBuilder::add_uuid_identifier(storage_pool, storage_pool.get_uuid());
}


void StoragePoolBuilder::update_identifier_with_uuid(agent_framework::model::StoragePool& storage_pool) {
    IdentifierBuilder::update_uuid_identifier(storage_pool, storage_pool.get_uuid());
}
