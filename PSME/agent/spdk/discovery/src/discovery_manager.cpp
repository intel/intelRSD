/*!
 * @brief Drive discoverer interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file discovery_manager.cpp
 * */


#include "discovery/builders/spdk_volume_builder.hpp"
#include "discovery/discoverers/spdk_drive_discoverer.hpp"
#include "discovery/discoverers/spdk_volume_discoverer.hpp"
#include "discovery/discoverers/spdk_storage_pool_discoverer.hpp"
#include "discovery/discoverers/spdk_fabric_discoverer.hpp"
#include "discovery/discoverers/spdk_endpoint_discoverer.hpp"
#include "discovery/discoverers/spdk_zone_discoverer.hpp"
#include "discovery/discovery_manager.hpp"

#include "agent-framework/discovery/discoverers/manager_discoverer.hpp"
#include "agent-framework/discovery/discoverers/chassis_discoverer.hpp"
#include "agent-framework/discovery/discoverers/ethernet_interface_discoverer.hpp"
#include "agent-framework/discovery/discoverers/system_discoverer.hpp"
#include "agent-framework/discovery/discoverers/storage_service_discoverer.hpp"
#include "agent-framework/module/storage_components.hpp"



using namespace agent::spdk::discovery;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::discovery;

namespace agent {
namespace spdk {
namespace discovery {

DiscoveryManager::~DiscoveryManager() { }


Uuid DiscoveryManager::discover() {

    auto manager = discover_manager();
    auto chassis = discover_chassis(manager);
    auto fabric = discover_fabric(manager);
    auto storage_service = discover_storage_service(manager);
    auto system = discover_system(manager, chassis);

    try {
        auto interfaces = discover_ethernet_interfaces(system);
        for (const auto& interface : interfaces) {
            get_manager<NetworkInterface>().add_entry(interface);
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("spdk-discovery", ex.get_message());
    }


    {
        // Set up mutex to prevent interruption from GAMI commands
        std::lock_guard<std::mutex> lock{m_context->spdk_mutex};

        auto drives = discover_drives(chassis);
        for (const auto& drive: drives) {
            get_manager<Drive>().add_entry(drive);
            get_m2m_manager<StorageService, Drive>().add_entry(storage_service.get_uuid(), drive.get_uuid());
        }

        auto storage_pools = discover_storage_pools(storage_service);
        for (auto& storage_pool: storage_pools) {
            get_manager<StoragePool>().add_entry(storage_pool);
        }

        auto volumes = discover_volumes(storage_service);
        for (auto& volume: volumes) {
            get_manager<Volume>().add_entry(volume);
        }

        auto endpoints = discover_endpoints(fabric);
        for (auto& endpoint: endpoints) {
            get_manager<Endpoint>().add_entry(endpoint);
        }

        auto zones = discover_zones(fabric);
        for (auto& zone: zones) {
            get_manager<Zone>().add_entry(zone);
        }
    }

    get_manager<System>().add_entry(system);
    get_manager<StorageService>().add_entry(storage_service);
    get_m2m_manager<System, StorageService>().add_entry(system.get_uuid(), storage_service.get_uuid());
    get_manager<Chassis>().add_entry(chassis);
    get_manager<Fabric>().add_entry(fabric);

    get_manager<Manager>().add_entry(manager);

    return manager.get_uuid();
}


Manager DiscoveryManager::discover_manager() {
    log_debug("spdk-discovery", "Starting discovery of a manager.");
    auto manager = ManagerDiscoverer{}.discover();
    m_stabilizer.stabilize(manager);
    log_debug("spdk-discovery", "Finished discovery of a manager.");
    return manager;
}

Chassis DiscoveryManager::discover_chassis(const Manager& manager) {
    log_debug("spdk-discovery", "Starting discovery of a chassis.");
    auto discoverer = ChassisDiscoverer{};
    auto chassis = discoverer.discover(manager.get_uuid());
    m_stabilizer.stabilize(chassis);
    discoverer.discover_database(chassis);
    log_debug("spdk-discovery", "Finished discovery of a chassis.");
    return chassis;
}


Fabric DiscoveryManager::discover_fabric(const Manager& manager) {
    log_debug("spdk-discovery", "Starting discovery of a fabric.");
    auto fabric = SpdkFabricDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(fabric);
    log_debug("spdk-discovery", "Finished discovery of a fabric.");
    return fabric;
}


StorageService DiscoveryManager::discover_storage_service(const Manager& manager) {
    log_debug("spdk-discovery", "Starting discovery of a storage service.");
    auto storage_service = StorageServiceDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(storage_service);
    log_debug("spdk-discovery", "Finished discovery of a storage service.");
    return storage_service;
}


System DiscoveryManager::discover_system(const Manager& manager, const Chassis& chassis) {
    log_debug("spdk-discovery", "Starting discovery of system.");
    auto system = SystemDiscoverer{m_context->sysfs_interface}.discover(manager.get_uuid(), chassis.get_uuid());
    m_stabilizer.stabilize(system);
    log_debug("spdk-discovery", "Finished discovery of system.");
    return system;
}


std::vector<NetworkInterface> DiscoveryManager::discover_ethernet_interfaces(const System& system) {
    log_debug("spdk-discovery", "Starting discovery of interfaces.");
    auto interface_names = m_context->interface_reader->get_interfaces();
    auto interfaces = EthernetInterfaceDiscoverer{interface_names}.discover(system.get_uuid());
    for (auto& interface : interfaces) {
        m_stabilizer.stabilize(interface);
    }
    log_debug("spdk-discovery", "Finished discovery of interfaces.");
    return interfaces;
}


std::vector<Drive> DiscoveryManager::discover_drives(const Chassis& chassis) {
    log_debug("spdk-discovery", "Starting discovery of drives.");
    auto drives = SpdkDriveDiscoverer{m_context}.discover(chassis.get_uuid());
    for (auto& drive : drives) {
        m_stabilizer.stabilize(drive);
    }
    log_debug("spdk-discovery", "Finished discovery of drives.");
    return drives;
}


std::vector<Volume> DiscoveryManager::discover_volumes(const StorageService& storage_service) {
    log_debug("spdk-discovery", "Starting discovery of volumes.");
    auto volumes = SpdkVolumeDiscoverer{m_context}.discover(storage_service.get_uuid());
    for (auto& volume : volumes) {
        Uuid old_volume_uuid = volume.get_uuid();
        m_stabilizer.stabilize(volume);
        get_m2m_manager<StoragePool, Volume>().update_child(old_volume_uuid, volume.get_uuid());

        // Update all replica UUIDs
        for (auto& v : volumes) {
            SpdkVolumeBuilder::update_replica_info(v, old_volume_uuid, volume.get_uuid());
        }
    }

    log_debug("spdk-discovery", "Finished discovery of volumes.");
    return volumes;
}


std::vector<StoragePool> DiscoveryManager::discover_storage_pools(const StorageService& storage_service) {
    log_debug("spdk-discovery", "Starting discovery of storage pools.");
    auto pools = SpdkStoragePoolDiscoverer{m_context}.discover(storage_service.get_uuid());
    std::for_each(pools.begin(), pools.end(), [this](StoragePool& pool) {
        m_stabilizer.stabilize(pool);
    });
    log_debug("spdk-discovery", "Finished discovery of storage pools.");
    return pools;
}


std::vector<Endpoint> DiscoveryManager::discover_endpoints(const Fabric& fabric) {
    log_debug("spdk-discovery", "Starting discovery of endpoints.");
    auto discoverer = SpdkEndpointDiscoverer{m_context};
    std::vector<Endpoint> endpoints{};

    auto hw_endpoints = discoverer.discover(fabric.get_uuid());
    std::for_each(hw_endpoints.begin(), hw_endpoints.end(), [this](Endpoint& endpoint) {
        m_stabilizer.stabilize(endpoint);
    });

    auto db_endpoints = discoverer.discovery_database(fabric.get_uuid());
    // No need to stabilize endpoints from DB

    // Join collections:
    endpoints.reserve(hw_endpoints.size() + db_endpoints.size());
    endpoints.insert(endpoints.end(), hw_endpoints.begin(), hw_endpoints.end());
    endpoints.insert(endpoints.end(), db_endpoints.begin(), db_endpoints.end());

    log_debug("spdk-discovery", "Finished discovery of endpoints.");
    return endpoints;
}


std::vector<Zone> DiscoveryManager::discover_zones(const Fabric& fabric) {
    log_debug("spdk-discovery", "Starting discovery of zones.");
    auto zones = SpdkZoneDiscoverer{m_context}.discover(fabric.get_uuid());
    log_debug("spdk-discovery", "Finished discovery of zones.");
    return zones;
}

}
}
}



