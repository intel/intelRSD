/*!
 * @brief Volume builder class implementation.
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
 * @file volume_builder.cpp
 */

#include "discovery/builders/volume_builder.hpp"
#include "discovery/builders/identifier_builder.hpp"

#include "lvm/lvm_api.hpp"

#include "agent-framework/module/requests/storage/add_volume.hpp"

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


model::Volume VolumeBuilder::build_default(const Uuid& parent_uuid) {
    model::Volume volume{parent_uuid};
    return volume;
}


void VolumeBuilder::update_with_lvm(model::Volume& volume, const lvm::model::LogicalVolume& logical_volume) {

    volume.set_status(model::attribute::Status{
        model::enums::State::from_string(logical_volume.get_status()),
        model::enums::Health::from_string(logical_volume.get_health())
    });

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(logical_volume.get_capacity_b());
    volume.set_capacity(capacity);

    volume.set_bootable(logical_volume.is_bootable());
    if (logical_volume.get_protection_status()) {
        volume.set_access_capabilities(model::Volume::AccessCapabilities{
            model::enums::AccessCapability::Read
        });
    }
    else {
        volume.set_access_capabilities(model::Volume::AccessCapabilities{
            model::enums::AccessCapability::Read,
            model::enums::AccessCapability::Write,
        });
    }
}


void VolumeBuilder::update_with_request(model::Volume& volume, const model::requests::AddVolume& request) {
    volume.set_status(true);
    volume.set_oem(request.get_oem());
    volume.set_bootable(request.get_bootable());

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(request.get_capacity_bytes());
    volume.set_capacity(std::move(capacity));
}


void VolumeBuilder::add_replica_info(model::Volume& volume,
                                     const Uuid& replica_volume,
                                     const model::enums::ReplicaType& type,
                                     const model::enums::ReplicaRole& role) {

    model::attribute::ReplicaInfo replica{};

    replica.set_replica(replica_volume);
    replica.set_replica_role(role);
    replica.set_replica_type(type);

    if (type == model::enums::ReplicaType::Snapshot) {
        replica.set_replica_read_only_access(model::enums::ReplicaReadOnlyAccess::SourceElement);
    }

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


void VolumeBuilder::add_capacity_source(agent_framework::model::Volume& volume, const Uuid& providing_pool) {
    model::attribute::CapacitySource capacity_source{};
    capacity_source.set_allocated_bytes(volume.get_capacity().get_allocated_bytes());

    if (!providing_pool.empty()) {
        capacity_source.set_providing_pools({providing_pool});
    }

    volume.add_capacity_source(capacity_source);
}


void VolumeBuilder::add_identifier_with_system_path(agent_framework::model::Volume& volume,
                                                    const lvm::model::VolumeGroup& volume_group,
                                                    const lvm::model::LogicalVolume& logical_volume) {

    IdentifierBuilder::add_system_path_identifier(volume, "/dev/" + volume_group.get_name()
                                                          + "/" + logical_volume.get_name());
}


void VolumeBuilder::add_identifier_with_uuid(agent_framework::model::Volume& volume) {
    IdentifierBuilder::add_uuid_identifier(volume, volume.get_uuid());
}


void VolumeBuilder::update_identifier_with_uuid(agent_framework::model::Volume& volume) {
    IdentifierBuilder::update_uuid_identifier(volume, volume.get_uuid());
}
