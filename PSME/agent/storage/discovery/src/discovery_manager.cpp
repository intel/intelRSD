/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file storage/src/discovery/discovery_manager.cpp
 */

#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/storage_fabric_discoverer.hpp"
#include "discovery/discoverers/storage_zone_discoverer.hpp"
#include "discovery/discoverers/storage_endpoint_discoverer.hpp"
#include "discovery/discoverers/storage_drive_discoverer.hpp"
#include "discovery/discoverers/lvm_volume_discoverer.hpp"
#include "discovery/discoverers/lvm_storage_pool_discoverer.hpp"
#include "discovery/builders/lvm_volume_builder.hpp"

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/discovery/discoverers/manager_discoverer.hpp"
#include "agent-framework/discovery/discoverers/chassis_discoverer.hpp"
#include "agent-framework/discovery/discoverers/storage_service_discoverer.hpp"
#include "agent-framework/discovery/discoverers/system_discoverer.hpp"
#include "agent-framework/discovery/discoverers/ethernet_interface_discoverer.hpp"

#include "database/persistent_attributes.hpp"

#include "agent/utils/utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;

using namespace agent::storage::discovery;

using namespace database;


DiscoveryManager::~DiscoveryManager() {}


void DiscoveryManager::update_volume_capacity_sources() {
    for (const auto& volume_uuid : get_manager<Volume>().get_keys()) {
        Uuid providing_pool{};
        auto volume = get_manager<Volume>().get_entry_reference(volume_uuid);

        const auto& parent_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume->get_uuid());
        if (parent_pools.size() == 1) {
            providing_pool = parent_pools.front();
        }
        else {
            log_error("storage-discovery", "Volume " << volume->get_uuid() << " has 0 or more than 1 parent pool!");
        }

        LvmVolumeBuilder::set_storage_pool_capacity_source(volume.get_raw_ref(), providing_pool);
    }
}


Uuid DiscoveryManager::discover() {
    log_info("storage-discovery", "Starting discovery...");

    auto manager = discover_manager();
    auto storage_service = discover_storage_service(manager);
    auto chassis = discover_chassis(manager);
    auto system = discover_system(manager, chassis);

    try {
        auto interfaces = discover_ethernet_interfaces(system);
        for (const auto& interface : interfaces) {
            get_manager<NetworkInterface>().add_or_update_entry(interface);
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto drives = discover_drives(chassis);
        for (const auto& drive : drives) {
            get_manager<Drive>().add_or_update_entry(drive);
            get_m2m_manager<StorageService, Drive>().add_entry(storage_service.get_uuid(), drive.get_uuid());
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto volumes = discover_volumes(storage_service);
        for (const auto& volume : volumes) {
            get_manager<Volume>().add_or_update_entry(volume);
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto storage_pools = discover_storage_pools(storage_service);
        for (auto& pool : storage_pools) {
            get_manager<StoragePool>().add_or_update_entry(pool);
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    // After discovery of Volumes, Storage Pools and Drives update its relations
    update_volume_capacity_sources();

    auto fabric = discover_fabric(manager);
    get_manager<Fabric>().add_or_update_entry(fabric);

    try {
        // It is important to do Endpoints discovery before Zones.
        auto endpoints = discover_endpoints(fabric);
        for (auto& endpoint : endpoints) {
            get_manager<Endpoint>().add_or_update_entry(endpoint);
        }

        auto zones = discover_zones(fabric);
        for (auto& zone : zones) {
            get_manager<Zone>().add_or_update_entry(zone);
        }

        restore(fabric.get_uuid());
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    get_m2m_manager<System, StorageService>().add_entry(system.get_uuid(), storage_service.get_uuid());
    get_manager<StorageService>().add_or_update_entry(storage_service);
    get_manager<System>().add_or_update_entry(system);
    get_manager<Chassis>().add_or_update_entry(chassis);
    get_manager<Manager>().add_or_update_entry(manager);

    log_info("storage-discovery", "Discovery for Storage Agent finished.");
    return manager.get_uuid();
}


Manager DiscoveryManager::discover_manager() {
    log_debug("storage-discovery", "Starting discovery of a manager.");
    auto manager = agent_framework::discovery::ManagerDiscoverer{}.discover();
    m_stabilizer.stabilize(manager);
    log_debug("storage-discovery", "Finished discovery of a manager.");
    return manager;
}


StorageService DiscoveryManager::discover_storage_service(const Manager& manager) {
    log_debug("storage-discovery", "Starting discovery of a storage service.");
    auto storage_service = agent_framework::discovery::StorageServiceDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(storage_service);
    log_debug("storage-discovery", "Finished discovery of a storage service.");
    return storage_service;
}


System DiscoveryManager::discover_system(const agent_framework::model::Manager& manager,
                                         const agent_framework::model::Chassis& chassis) {

    log_debug("storage-discovery", "Starting discovery of a system.");
    auto system = agent_framework::discovery::SystemDiscoverer{m_context->sysfs_interface}
        .discover(manager.get_uuid(), chassis.get_uuid());
    m_stabilizer.stabilize(system);
    log_debug("storage-discovery", "Finished discovery of a system.");
    return system;
}


std::vector<NetworkInterface> DiscoveryManager::discover_ethernet_interfaces(const System& system) {
    log_debug("storage-discovery", "Starting discovery of interfaces.");
    auto interface_names = m_context->interface_reader->get_interfaces();
    auto interfaces = agent_framework::discovery::EthernetInterfaceDiscoverer{interface_names}
        .discover(system.get_uuid());
    for (auto& interface : interfaces) {
        m_stabilizer.stabilize(interface);
    }
    log_debug("storage-discovery", "Finished discovery of interfaces.");
    return interfaces;
}


std::vector<StoragePool> DiscoveryManager::discover_storage_pools(const StorageService& storage_service) {
    log_debug("storage-discovery", "Starting discovery of storage pools.");
    auto pools = LvmStoragePoolDiscoverer{m_context}.discover(storage_service.get_uuid());
    for (auto& pool : pools) {
        Uuid old_uuid = pool.get_uuid();
        m_stabilizer.stabilize(pool);
        utils::update_storage_pool_relations(old_uuid, pool.get_uuid());
    }

    log_debug("storage-discovery", "Finished discovery of storage pools.");
    return pools;
}


std::vector<Volume> DiscoveryManager::discover_volumes(const StorageService& parent_storage_service) {
    log_debug("storage-discovery", "Starting discovery of volumes.");
    auto volumes = LvmVolumeDiscoverer{m_context}.discover(parent_storage_service.get_uuid());
    for (auto& volume : volumes) {
        Uuid old_uuid = volume.get_uuid();
        m_stabilizer.stabilize(volume);

        // Update all volumes with new UUID of currently processing volume
        for (auto& v : volumes) {
            LvmVolumeBuilder::update_replica_info(v, old_uuid, volume.get_uuid());
        }
    }

    log_debug("storage-discovery", "Finished discovery of volumes.");
    return volumes;
}


Chassis DiscoveryManager::discover_chassis(const Manager& manager) {
    log_debug("storage-discovery", "Starting discovery of a chassis.");
    auto chassis = agent_framework::discovery::ChassisDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(chassis);
    agent_framework::discovery::ChassisDiscoverer{}.discover_database(chassis);
    log_debug("storage-discovery", "Finished discovery of a chassis.");
    return chassis;
}


std::vector<Drive> DiscoveryManager::discover_drives(const Chassis& parent_chassis) {
    log_debug("storage-discovery", "Starting discovery of drives.");
    auto drives = StorageDriveDiscoverer{m_context}.discover(parent_chassis.get_uuid());
    for (auto& drive : drives) {
        m_stabilizer.stabilize(drive);
    }
    log_debug("storage-discovery", "Finished discovery of drives (count: " << drives.size() << ").");
    return drives;
}


Fabric DiscoveryManager::discover_fabric(const Manager& manager) {
    log_debug("storage-discovery", "Starting discovery of a fabric.");
    auto fabric = StorageFabricDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(fabric);
    log_debug("storage-discovery", "Finished discovery of a fabric.");
    return fabric;
}


std::vector<Zone> DiscoveryManager::discover_zones(const Fabric& fabric) {
    std::vector<Zone> zones{};
    log_debug("storage-discovery", "Starting discovery of zones.");
    zones = StorageZoneDiscoverer{}.discover(fabric.get_uuid());
    // Zones are stored in database so stabilization is not needed.
    log_debug("storage-discovery", "Finished discovery of zones.");
    return zones;
}


std::vector<Endpoint> DiscoveryManager::discover_endpoints(const Fabric& fabric) {
    log_debug("storage-discovery", "Starting discovery of endpoints.");
    auto endpoints = StorageEndpointDiscoverer{m_context}.discover(fabric.get_uuid());
    // Endpoints are stored in database so stabilization is not needed.
    log_debug("storage-discovery", "Finished discovery of endpoints (count: " << endpoints.size() << ").");
    return endpoints;
}


void DiscoveryManager::restore(const Uuid&) {
    log_debug("storage-discovery", "Starting restoration of Endpoints and Zones from database.");
    StorageEndpointDiscoverer{m_context}.restore();
    log_debug("storage-discovery", "Finished restoration of Endpoints and Zones from database.");
}
