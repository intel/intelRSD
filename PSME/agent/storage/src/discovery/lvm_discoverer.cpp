/*!
 * @brief Storage discoverer implementation using LVM.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
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
 * @file lvm_discoverer.cpp
 */

#include "discovery/lvm_discoverer.hpp"
#include "discovery/builders/storage_pool_builder.hpp"
#include "discovery/builders/volume_builder.hpp"

#include "lvm/lvm_api.hpp"


using namespace agent_framework;

namespace {

using Volumes = std::vector<agent_framework::model::Volume>;

Volumes::iterator get_volume_by_uuid(Volumes& volumes, const Uuid& uuid) {
    return std::find_if(volumes.begin(), volumes.end(), [&uuid](const model::Volume& volume) {
        return volume.get_uuid() == uuid;
    });
}

}

namespace agent {
namespace storage {
namespace discovery {

LvmDiscoverer::~LvmDiscoverer() {}


bool LvmDiscoverer::discover_capacity_sources(model::StoragePool& storage_pool) {
    lvm::LvmApi lvm_api{};
    auto volume_groups = lvm_api.discover_volume_groups(lvm::DiscoveryType::VG_PV);
    for (const auto& volume_group : volume_groups) {
        const auto& volume_group_path = volume_group.get_name();
        const auto& pool_device_path = model::attribute::Identifier::get_system_path(storage_pool);
        if (("/dev/" + volume_group_path) != pool_device_path) {
            continue; // Another volume group
        }

        // Clear current capacity sources
        storage_pool.set_capacity_sources(model::attribute::Array<model::attribute::CapacitySource>());

        for (const auto& physical_volume : volume_group.get_physical_volumes()) {
            log_debug("discovery", physical_volume.to_string());
            const auto& device_path = physical_volume.get_name();
            const auto& drive_uuids = module::get_manager<model::Drive>().get_keys(
                [&device_path](const model::Drive& drive) {
                    return model::attribute::Identifier::get_system_path(drive) == device_path;
                });

            if (drive_uuids.empty()) {
                log_error("lvm-discovery", "Did not find drive representing physical volume '"
                    << physical_volume.get_name() << "'.");
                continue;
            }

            StoragePoolBuilder::add_capacity_source(storage_pool,
                                                    physical_volume.get_capacity_b(),
                                                    physical_volume.get_capacity_b() - physical_volume.get_free_b(),
                                                    drive_uuids.front());

            log_debug("lvm-discovery", "Added new capacity source ["
                << device_path << "] for storage pool [" << volume_group_path << "].");
        }
    }
    return true;
}


bool LvmDiscoverer::discover_storage_pools(std::vector<model::StoragePool>& storage_pools,
                                           const Uuid& parent_uuid) {

    lvm::LvmApi lvm_api{};
    auto volume_groups = lvm_api.discover_volume_groups(lvm::DiscoveryType::VG);
    for (const auto& volume_group : volume_groups) {
        log_debug("discovery", volume_group.to_string());
        auto pool = StoragePoolBuilder::build_default(parent_uuid);
        StoragePoolBuilder::add_identifier_with_system_path(pool, volume_group);
        StoragePoolBuilder::add_identifier_with_uuid(pool);
        StoragePoolBuilder::update_with_lvm(pool, volume_group);

        storage_pools.emplace_back(std::move(pool));
    }
    return true;
}


bool LvmDiscoverer::discover_volumes(std::vector<agent_framework::model::Volume>& volumes, const Uuid& parent_uuid) {

    std::map<std::string, Uuid> name_to_uuid{};
    std::map<Uuid, std::string> uuid_to_master{};

    lvm::LvmApi lvm_api{};
    auto volume_groups = lvm_api.discover_volume_groups(lvm::DiscoveryType::VG_LV);
    for (const auto& volume_group : volume_groups) {
        for (const auto& logical_volume : volume_group.get_logical_volumes()) {
            log_debug("discovery", logical_volume.to_string());
            auto volume = VolumeBuilder::build_default(parent_uuid);
            VolumeBuilder::add_identifier_with_uuid(volume);
            VolumeBuilder::add_identifier_with_system_path(volume, volume_group, logical_volume);
            VolumeBuilder::update_with_lvm(volume, logical_volume);

            /* Add entry to the map [ LV name <-> LV UUID] */
            name_to_uuid[logical_volume.get_name()] = volume.get_uuid();

            /* If LV is snapshot, store the master [LV UUID <-> LV master name] */
            if (logical_volume.is_snapshot()) {
                uuid_to_master[volume.get_uuid()] = logical_volume.get_master_name();
            }

            volumes.emplace_back(std::move(volume));
        }
    }

    /* Resolve LV master UUIDs for each snapshot logical volume */
    for (const auto& item : uuid_to_master) {
        const auto& volume_uuid = item.first;
        const auto& master_name = item.second;

        auto it = ::get_volume_by_uuid(volumes, volume_uuid);
        if (it != volumes.end()) {
            const auto& master_uuid = name_to_uuid[master_name];
            VolumeBuilder::add_replica_info(*it, master_uuid,
                                            model::enums::ReplicaType::Snapshot,
                                            model::enums::ReplicaRole::Target);

            auto master_it = ::get_volume_by_uuid(volumes, master_uuid);
            if (master_it != volumes.end()) {
                VolumeBuilder::add_replica_info(*master_it, it->get_uuid(),
                                                model::enums::ReplicaType::Snapshot,
                                                model::enums::ReplicaRole::Source);
            }
            else {
                log_critical("lvm-discovery", "Master volume's UUID from newly created vector is missing!");
            }
        }
        else {
            log_critical("lvm-discovery", "Volume's UUID from newly created vector is missing!");
        }
    }

    return true;
}

bool LvmDiscoverer::rediscover_pool_capacity(model::StoragePool& storage_pool) {
    using model::attribute::Identifier;

    /* Find and update pool's capacity */
    std::vector<model::StoragePool> pools{};
    discover_storage_pools(pools, storage_pool.get_parent_uuid());
    auto it = std::find_if(pools.begin(), pools.end(), [&storage_pool](const model::StoragePool& pool) {
        return Identifier::get_system_path(pool) == Identifier::get_system_path(storage_pool);
    });
    if (it != pools.end()) {
        storage_pool.set_capacity(it->get_capacity());
    }

    /* Update only capacity sources */
    discover_capacity_sources(storage_pool);
    return true;
}


}
}
}
