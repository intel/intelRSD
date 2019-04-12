/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file hotswap_manager.cpp
 * @brief Implementation of Hotswap manager
 */

#include "agent/utils/utils.hpp"
#include "agent/watcher/hotswap_manager.hpp"
#include "tree_stability/storage_stabilizer.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/storage_drive_discoverer.hpp"
#include "discovery/discoverers/lvm_storage_pool_discoverer.hpp"

#include "agent-framework/module/storage_components.hpp"

#include "sysfs/construct_dev_path.hpp"



using namespace agent_framework;
using namespace agent::storage::watcher;
using namespace agent::storage::discovery;
using Drives = std::vector<agent_framework::model::Drive>;

namespace {

bool compare_drives(const model::Drive& lhs, const model::Drive& rhs) {
    using namespace agent_framework::model::attribute;

    if (lhs.get_name().has_value() && rhs.get_name().has_value()) {
        return (
            lhs.get_fru_info().get_serial_number() == rhs.get_fru_info().get_serial_number()
            && (lhs.get_fru_info().get_model_number() == rhs.get_fru_info().get_model_number())
            && (lhs.get_name().value() == rhs.get_name().value())
        );
    }
    else {
        log_debug("hot-swap", "One of compared drives has no name.");
        return false;
    }
}


template<typename T>
bool mark_as_critical(T& model) {
    const auto& status = model.get_status();
    if (model::enums::Health::Critical == status.get_health()) {
        // Resource is already marked as critical.
        return false;
    }

    /* Change status of resource to critical */
    model.set_status({status.get_state(), model::enums::Health::Critical});
    log_debug("hot-swap", "Changed health to critical for: " << model.get_uuid());
    return true;
}


bool remove_drive_relation(model::StoragePool& pool, const model::Drive& drive) {
    auto capacity_sources = pool.get_capacity_sources().get_array();

    auto point_to_drive = [&drive](const model::attribute::CapacitySource& capacity_source) {
        // We assume that providing drive is only one
        auto& providing_drives = capacity_source.get_providing_drives();
        auto it = std::find_if(providing_drives.begin(), providing_drives.end(), [&drive](const Uuid& uuid) {
            return uuid == drive.get_uuid();
        });
        return it != providing_drives.end();
    };

    // Remove all capacity sources using removed drive
    auto end_it = std::remove_if(capacity_sources.begin(), capacity_sources.end(), point_to_drive);
    if (end_it != capacity_sources.end()) {
        capacity_sources.erase(end_it);
        pool.set_capacity_sources(capacity_sources);
        return true;
    }
    return false;
}


void update_related_zones(const model::Endpoint& endpoint) {
    auto& zone_endpoints_manager = module::get_m2m_manager<model::Zone, model::Endpoint>();
    for (const auto& zone_uuid : zone_endpoints_manager.get_parents(endpoint.get_uuid())) {
        auto zone = module::get_manager<model::Zone>().get_entry_reference(zone_uuid);
        if (mark_as_critical(zone.get_raw_ref())) {
            eventing::send_update(zone.get_raw_ref());
        }
    }
}


void update_related_endpoints(const model::Volume& volume) {
    for (const auto& endpoint_uuid : module::get_manager<model::Endpoint>().get_keys()) {
        auto endpoint = module::get_manager<model::Endpoint>().get_entry_reference(endpoint_uuid);
        for (const auto& entity : endpoint->get_connected_entities()) {
            // Only endpoint which points to critical volume
            if (entity.get_entity() == volume.get_uuid()) {
                if (mark_as_critical(endpoint.get_raw_ref())) {
                    eventing::send_update(endpoint.get_raw_ref());
                }
                update_related_zones(endpoint.get_raw_ref());
            }
        }
    }
}


void update_related_volumes(const model::StoragePool& pool) {
    const auto& pool_volume_manager = module::get_m2m_manager<model::StoragePool, model::Volume>();
    for (const auto& volume_uuid : pool_volume_manager.get_children(pool.get_uuid())) {
        auto volume = module::get_manager<model::Volume>().get_entry_reference(volume_uuid);
        if (mark_as_critical(volume.get_raw_ref())) {
            eventing::send_update(volume.get_raw_ref());
        }
        update_related_endpoints(volume.get_raw_ref());
    }
}


void update_related_storage_pools(const model::Drive& removed_drive) {
    const auto& pool_uuids = module::get_manager<model::StoragePool>().get_keys();
    for (const auto& pool_uuid : pool_uuids) {
        auto pool = module::get_manager<model::StoragePool>().get_entry_reference(pool_uuid);
        if (remove_drive_relation(pool.get_raw_ref(), removed_drive)) {
            if (mark_as_critical(pool.get_raw_ref())) {
                eventing::send_update(pool.get_raw_ref());
            }
            update_related_volumes(pool.get_raw_ref());
        }
    }
}

}


void HotswapManager::hotswap_discover_hard_drives() {
    if (module::get_manager<model::StorageService>().get_keys().empty()) {
        log_critical("hot-swap", "No Storage Services found!");
        return;
    }
    if (module::get_manager<model::Chassis>().get_keys().empty()) {
        log_critical("hot-swap", "No Chassis found!");
        return;
    }
    auto storage_service_uuid = module::get_manager<model::StorageService>().get_keys().front();
    auto chassis_uuid = module::get_manager<model::Chassis>().get_keys().front();

    /* Get list of hard drives on a system */
    Drives drives_detected = StorageDriveDiscoverer{m_context}.discover(chassis_uuid);

    /* Make copy of discovered drives to compare later */
    Drives hard_drives{drives_detected};
    auto hard_drives_end_it = hard_drives.end();

    auto drive_uuids = module::get_manager<model::Drive>().get_keys();
    auto drive_uuids_end_it = drive_uuids.end();

    /* Find new/removed drives */
    for (const auto& drive : module::get_manager<model::Drive>().get_entries()) {
        for (const auto& drive_detected : drives_detected) {
            if (::compare_drives(drive_detected, drive)) {
                /* Move the same instance from both vectors to the end of the vector */
                drive_uuids_end_it = std::remove(drive_uuids.begin(), drive_uuids_end_it, drive.get_uuid());
                hard_drives_end_it = std::remove_if(hard_drives.begin(), hard_drives_end_it,
                                                    [&drive_detected](const model::Drive& d) {
                                                        return ::compare_drives(d, drive_detected);
                                                    });
            }
        }
    }

    // Erase removed items from vector
    drive_uuids.erase(drive_uuids_end_it, drive_uuids.end());
    hard_drives.erase(hard_drives_end_it, hard_drives.end());

    /*
     * If Drive UUIDs list isn't empty then some physical drives have been removed from the system.
     * Remove drives from the storage manager.
     * */
    for (const auto& drive_uuid : drive_uuids) {
        auto drive = module::get_manager<model::Drive>().get_entry(drive_uuid);
        remove_drive(drive);
    }

    /*
     * If hard drives list is not empty, it means new hard (physical) drives have been attached/added.
     * Add these new drives to the storage manager.
     * */
    for (auto& drive : hard_drives) {
        add_drive(drive, storage_service_uuid);
    }
}


void HotswapManager::add_drive(model::Drive& detected_drive, const Uuid& storage_service_uuid) {
    StorageStabilizer stabilizer{};
    /* Stabilize drive first */
    stabilizer.stabilize(detected_drive);

    /* Add new drive to the storage manager */
    module::get_manager<model::Drive>().add_entry(detected_drive);
    module::get_m2m_manager<model::StorageService, model::Drive>()
        .add_entry(storage_service_uuid, detected_drive.get_uuid());

    /* Send event notification about adding new drive */
    eventing::send_event(detected_drive.get_uuid(), model::enums::Component::Drive,
                         model::enums::Notification::Add, detected_drive.get_parent_uuid());

    /* Perform discovery of pools to detect if inserted drive is assigned to previously created pool */
    DiscoveryManager(m_context).rediscover<LvmStoragePoolDiscoverer, model::StoragePool>(
            storage_service_uuid, utils::update_storage_pool_relations);

    /* Log successful adding */
    if (detected_drive.get_name().has_value()) {
        log_info("hot-swap", "Added new drive: " << sysfs::construct_dev_path(detected_drive.get_name()));
    }
    else {
        log_error("hot-swap", "Added drive with uuid '" << detected_drive.get_uuid() << "' has no name.");
    }

}


void HotswapManager::remove_drive(const model::Drive& removed_drive) {
    /* Send event notification about removing drive */
    eventing::send_event(removed_drive.get_uuid(), model::enums::Component::Drive,
                         model::enums::Notification::Remove, removed_drive.get_parent_uuid());


    /* Resolve dependencies */
    ::update_related_storage_pools(removed_drive);

    /* Remove physical drive from storage manager */
    module::get_manager<model::Drive>().remove_entry(removed_drive.get_uuid());
    module::get_m2m_manager<model::StorageService, model::Drive>().remove_child(removed_drive.get_uuid());

    /* Log about successful removal */
    if (removed_drive.get_name().has_value()) {
        log_info("hot-swap", "Removed drive [" << removed_drive.get_uuid() << "]: "
                             << sysfs::construct_dev_path(removed_drive.get_name()));
    }
    else {
        log_error("hot-swap", "Removed drive with uuid '" << removed_drive.get_uuid() << "' has no name.");
    }

}
