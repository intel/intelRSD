/*!
 * @brief Volume builder class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file volume_builder.cpp
 */

#include "agent-framework/discovery/builders/volume_builder.hpp"

using namespace agent_framework;
using namespace agent_framework::discovery;


model::Volume VolumeBuilder::build_default(const Uuid& parent_uuid) {
    model::Volume volume{parent_uuid};
    return volume;
}

void VolumeBuilder::add_replica_info(model::Volume& volume,
                                     const model::enums::ReplicaType& type,
                                     const model::enums::ReplicaRole& role,
                                     const OptionalField<Uuid>& replica_volume_uuid,
                                     const OptionalField<model::enums::ReplicaReadOnlyAccess>& access) {

    model::attribute::ReplicaInfo replica{};

    replica.set_replica(replica_volume_uuid);
    replica.set_replica_role(role);
    replica.set_replica_type(type);
    replica.set_replica_read_only_access(access);

    volume.add_replica_info(std::move(replica));
}

void VolumeBuilder::update_replica_info(agent_framework::model::Volume& volume,
                                        const Uuid& old_uuid, const Uuid& new_uuid) {
    auto replica_infos = volume.get_replica_infos();
    for (auto& replica_info : replica_infos) {
        if (replica_info.get_replica() == old_uuid) {
            replica_info.set_replica(new_uuid);
        }
    }
    // Replace collection
    volume.set_replica_infos(replica_infos);
}

void VolumeBuilder::set_storage_pool_capacity_source(agent_framework::model::Volume& volume,
                                                     const Uuid& providing_pool) {
    model::attribute::CapacitySource capacity_source{};
    capacity_source.set_allocated_bytes(volume.get_capacity().get_allocated_bytes());

    if (!providing_pool.empty()) {
        capacity_source.set_providing_pools({providing_pool});
    }

    volume.set_capacity_sources({capacity_source});
}