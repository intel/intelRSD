/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file storage/src/discovery/discovery_manager.cpp
 * */

#include "agent-framework/module/storage_components.hpp"

#include "discovery/discovery_manager.hpp"
#include "discovery/builders/manager_builder.hpp"
#include "discovery/builders/chassis_builder.hpp"
#include "discovery/builders/system_builder.hpp"
#include "discovery/builders/fabric_builder.hpp"
#include "discovery/builders/volume_builder.hpp"
#include "discovery/builders/drive_builder.hpp"
#include "discovery/builders/storage_service_builder.hpp"
#include "discovery/builders/storage_pool_builder.hpp"

#include "lvm/lvm_api.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"
#include "iscsi/utils.hpp"

#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;

using namespace agent::storage::discovery;
using namespace agent::storage::lvm;
using namespace agent::storage::iscsi;

using namespace database;

namespace {

bool starts_with(const std::string& source, const std::string& target) {
    return source.substr(0, target.length()) == target;
}

}

namespace {

bool match_target_iqn(const IscsiTarget& iscsi_target, const Endpoint& endpoint) {
    for (const auto& identifier : endpoint.get_identifiers()) {
        if (identifier.get_durable_name_format() == enums::IdentifierType::iQN) {
            if (identifier.get_durable_name() == iscsi_target.get_target_iqn()) {
                return true;
            }
        }
    }
    return false;
}


bool match_target_volumes(const IscsiTarget::TargetLuns& luns, const Endpoint::ConnectedEntities& entities) {
    if (luns.get_array().size() != entities.get_array().size()) {
        log_debug("storage-discovery", "The number of LUNs and Connected Entities differ");
        return false;
    }
    uint32_t entities_matched = 0;
    for (const auto& entity : entities.get_array()) {
        for (const auto& lun : luns.get_array()) {
            std::string entity_lun{};
            try {
                entity_lun = attribute::Identifier::get_lun(entity);
            }
            catch (const std::logic_error&) {
                continue;
            }
            if (std::to_string(lun.get_lun()) == entity_lun) {
                if (lun.get_logical_drive() == entity.get_entity()) {
                    entities_matched++;
                }
                else {
                    log_debug("storage-discovery",
                              "Could not match Endpoint's Entity: '" << entity.get_entity()
                                                                     << "', with iSCSI Target backing store for LUN: "
                                                                     << lun.get_lun());
                    return false;
                }
            }
        }
    }

    if (entities_matched != entities.get_array().size()) {
        log_debug("storage-discovery", "Could not match all LUNs to Endpoint's Entities");
        return false;
    }
    return true;
}


bool match_target_credentials(const IscsiTarget& iscsi_target, const Endpoint& target_endpoint) {
    bool check_oneway = false;
    bool check_mutual = false;
    bool oneway_match = false;
    bool mutual_match = false;
    if (iscsi_target.get_chap_username().has_value()) {
        check_oneway = true;
    }
    if (iscsi_target.get_mutual_chap_username().has_value()) {
        check_mutual = true;
    }

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    auto& endpoint_manager = get_manager<Endpoint>();

    if (zone_endpoint_manager.get_parents(target_endpoint.get_uuid()).empty()) {
        log_debug("storage-discovery", "Endpoint does not belong to any Zone: " << target_endpoint.get_uuid());
        return false;
    }

    if (check_mutual) {
        if (iscsi_target.get_mutual_chap_username() == target_endpoint.get_username()) {
            mutual_match = true;
        }
        else {
            log_debug("storage-discovery", "Could not match Mutual CHAP username");
            return false;
        }
    }
    else {
        if (target_endpoint.get_username().has_value()) {
            log_debug("storage-discovery", "Mutual CHAP credentials missing in iSCSI Target from Endpoint: "
                << target_endpoint.get_uuid());
            return false;
        }
    }

    const auto zone_uuid = zone_endpoint_manager.get_parents(target_endpoint.get_uuid()).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto& child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                if (check_oneway) {
                    if (iscsi_target.get_chap_username() == child_endpoint.get_username()) {
                        oneway_match = true;
                    }
                    else {
                        log_debug("storage-discovery", "Could not match One-way CHAP username");
                        return false;
                    }
                }
                else {
                    if (child_endpoint.get_username().has_value()) {
                        log_debug("storage-discovery",
                                  "One-way CHAP credentials missing in iSCSI Target from Endpoint: "
                                      << target_endpoint.get_uuid());
                        return false;
                    }
                }
                break; // In Zone can only be one initiator
            }
        }
    }

    if (check_oneway != oneway_match) {
        log_debug("storage-discovery", "Could not match One-way CHAP username");
        return false;
    }
    if (check_mutual != mutual_match) {
        log_debug("storage-discovery", "Could not match Mutual CHAP username");
        return false;
    }

    return true;
}


bool match_target_initiator(const IscsiTarget& iscsi_target, const Endpoint& endpoint) {

    bool initiator_found = false; // Zone must have one Initiator Endpoint to match iSCSI Target

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    auto& endpoint_manager = get_manager<Endpoint>();

    if (zone_endpoint_manager.get_parents(endpoint.get_uuid()).empty()) {
        log_debug("storage-discovery", "Endpoint does not belong to any Zone: " << endpoint.get_uuid());
        return false;
    }
    // Endpoint can be only in one Zone
    const auto zone_uuid = zone_endpoint_manager.get_parents(endpoint.get_uuid()).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto& child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                initiator_found = true;
                for (const auto& identifier : child_endpoint.get_identifiers()) {
                    if (identifier.get_durable_name_format() == enums::IdentifierType::iQN) {
                        std::string initiator_iqn{};
                        // Empty string for Initiator IQN is a valid value
                        if (iscsi_target.get_initiator_iqn().has_value()) {
                            initiator_iqn = iscsi_target.get_initiator_iqn();
                        }
                        // Limited to one initiator IQN per Initiator Endpoint - if IQN exists it must match
                        if (identifier.get_durable_name() != initiator_iqn) {
                            log_debug("storage-discovery", "Endpoint IQN: '" << identifier.get_durable_name()
                                                                             << "' and iSCSI Target initiator: '"
                                                                             << initiator_iqn
                                                                             << "' do not match");
                            return false;
                        }
                    }
                }
            }
            break; // Zone can only have one Initiator Endpoint, so when it is found the loop can be broken
        }
    }

    if (!initiator_found) {
        log_debug("storage-discovery", "Could not find Initiator Endpoint in the Zone");
        return false;
    }
    return true;
}


Uuid get_volume_by_device_path(const Uuid& device_path) {
    for (const auto& volume : get_manager<Volume>().get_entries()) {
        if (device_path == Identifier::get_system_path(volume)) {
            return volume.get_uuid();
        }
    }
    return {};
}


void remove_duplicates(std::vector<std::string>& to_have_matching_values_removed, std::vector<std::string>& values) {
    to_have_matching_values_removed.erase(
        remove_if(begin(to_have_matching_values_removed), end(to_have_matching_values_removed),
                  [&](std::string x) {
                      return find(begin(values), end(values), x) !=
                             end(values);
                  }), end(to_have_matching_values_removed));
}


void clean_chap_from_connected_endpoints(const Uuid& endpoint_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    auto endpoint = endpoint_manager.get_entry_reference(endpoint_uuid);
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    endpoint->clean_username();
    agent::storage::iscsi::tgt::clean_chap_from_database(endpoint_uuid);

    const auto zone_uuid = zone_endpoint_manager.get_parents(endpoint_uuid).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        auto child_endpoint = endpoint_manager.get_entry_reference(child_endpoint_uuid);
        for (const auto& entity : child_endpoint->get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                child_endpoint->clean_username();
                agent::storage::iscsi::tgt::clean_chap_from_database(child_endpoint_uuid);
            }
        }
    }
}

}


DiscoveryManager::~DiscoveryManager() {}


void DiscoveryManager::discovery_iscsi_targets(const Uuid& uuid) const {
    tgt::Manager manager{};
    auto response = manager.show_targets();
    if (!response.is_valid()) {
        throw std::runtime_error("Invalid iSCSI show target response: " +
                                 tgt::Errors::get_error_message(response.get_error()));
    }
    tgt::TargetParser parser{};
    if (!response.get_extra_data().size()) {
        log_info("storage-discovery", "No iSCSI Targets returned from tgt daemon");
        return;
    }
    std::string iscsi_text(response.get_extra_data().data());
    if (response.get_extra_data().size() && (iscsi_text.size() + 1) < response.get_extra_data().size()) {
        log_warning("storage-discovery",
                    "show targets response size: " << response.get_extra_data().size()
                                                   << " truncated to: " << (iscsi_text.size() + 1));
    }
    const auto targets = parser.parse(iscsi_text);
    auto& target_manager = get_manager<IscsiTarget, IscsiTargetManager>();
    for (const auto& target : targets) {
        IscsiTarget iscsi_target{uuid};
        const auto& iscsi_data = target_manager.get_iscsi_data();
        iscsi_target.set_status({State::Enabled, Health::OK});
        iscsi_target.set_initiator_iqn(
            target->get_target_initiator().empty() ?
            OptionalField<std::string>() :
            OptionalField<std::string>(target->get_target_initiator())
        );
        iscsi_target.set_target_address(iscsi_data.get_portal_ip());
        iscsi_target.set_target_port(std::to_string(iscsi_data.get_portal_port()));
        iscsi_target.set_target_iqn(target->get_target_iqn());
        iscsi_target.set_target_id(target->get_target_id());
        if (target->get_authentication_method().has_value()) {
            iscsi_target.set_authentication_method(target->get_authentication_method().value().to_string());
        }
        iscsi_target.set_chap_username(
            target->get_chap_username().empty() ?
            OptionalField<std::string>() :
            OptionalField<std::string>(target->get_chap_username())
        );
        iscsi_target.set_mutual_chap_username(
            target->get_mutual_chap_username().empty() ?
            OptionalField<std::string>() :
            OptionalField<std::string>(target->get_mutual_chap_username())
        );
        /* add target LUNs */
        for (const auto& lun : target->get_luns()) {
            auto drive_uuid = get_volume_by_device_path(lun->get_device_path());
            TargetLun target_lun{uint32_t(lun->get_lun()), drive_uuid};
            iscsi_target.add_target_lun(target_lun);
        }
        /* add target id for ISCSI model */
        target_manager.add_target_id(
            uint32_t(target->get_target_id()), iscsi_target.get_uuid());
        /* add ISCSI model to the manager */
        target_manager.add_entry(iscsi_target);
    }
}


void DiscoveryManager::restore_iscsi_targets(const Uuid& uuid) const {
    // Storage Agent deletes from tgt iSCSI Targets which do not match any existing Zone/Endpoint,
    // and adds to tgt iSCSI Targets based on existing Zones/Endpoints.
    discovery_iscsi_targets(uuid);
    auto& target_manager = get_manager<IscsiTarget, IscsiTargetManager>();
    auto& endpoint_manager = get_manager<Endpoint>();
    std::vector<std::string> iscsi_targets_for_removal{};
    std::vector<std::string> endpoints_matched{};

    for (const auto& iscsi_target : target_manager.get_entries()) {
        bool iscsi_target_remove = true;
        for (const auto& endpoint : endpoint_manager.get_entries()) {
            if (match_target_iqn(iscsi_target, endpoint)) {
                log_debug("storage-discovery",
                          "iSCSI Target name matched with Endpoint IQN: " << iscsi_target.get_target_iqn());
                if (match_target_volumes(iscsi_target.get_target_lun(), endpoint.get_connected_entities())) {
                    if (match_target_initiator(iscsi_target, endpoint)) {
                        if (match_target_credentials(iscsi_target, endpoint)) {
                            log_debug("storage-discovery",
                                      "Succesfully matched existing iSCSI Target: '" << iscsi_target.get_target_iqn()
                                                                                     << "' and Target Endpoint: "
                                                                                     << endpoint.get_uuid());
                            iscsi_target_remove = false;
                            endpoints_matched.emplace_back(endpoint.get_uuid());
                        }
                        else {
                            log_debug("storage-discovery",
                                      "Partially matched existing iSCSI Target: '" << iscsi_target.get_target_iqn()
                                                                                   << "' and Target Endpoint: "
                                                                                   << endpoint.get_uuid());
                        }
                    }
                    else {
                        log_debug("storage-discovery",
                                  "Could not match iSCSI Target initiator IQN with Endpoint: "
                                      << iscsi_target.get_target_iqn());
                    }
                }
                else {
                    log_debug("storage-discovery",
                              "Could not match iSCSI Target LUNs with any Endpoint's Entities: "
                                  << iscsi_target.get_target_iqn());
                }
                break;
            }
        }
        if (iscsi_target_remove) {
            iscsi_targets_for_removal.emplace_back(iscsi_target.get_uuid());
        }
    }

    for (const auto& iscsi_target_uuid : iscsi_targets_for_removal) {
        log_debug("storage-discovery", "iSCSI Target which will be removed from tgt: " << iscsi_target_uuid);
        try {
            agent::storage::iscsi::tgt::remove_iscsi_target(iscsi_target_uuid);
        }
        catch (const agent_framework::exceptions::IscsiError& e) {
            log_debug("storage-discovery", "Error while removing iSCSI Target: " << e.get_message());
        }
    }

    std::vector<std::string> unmatched_endpoints = endpoint_manager.get_keys();
    // Removes Endpoints matched with existing iSCSI Targets from the list of all Endpoints from the database
    remove_duplicates(unmatched_endpoints, endpoints_matched);

    for (const auto& endpoint_uuid : unmatched_endpoints) {
        if (agent::storage::iscsi::tgt::is_endpoint_valid_for_iscsi(endpoint_uuid)) {
            if (agent::storage::iscsi::tgt::is_endpoint_valid_for_chap(endpoint_uuid)) {
                log_debug("storage-discovery", "iSCSI Target which will be added to tgt: " << endpoint_uuid);
                agent::storage::iscsi::tgt::add_iscsi_target(endpoint_uuid, true);
            }
            else {
                log_debug("storage-discovery",
                          "iSCSI Target which will be added to tgt without CHAP: " << endpoint_uuid);
                clean_chap_from_connected_endpoints(endpoint_uuid);
                agent::storage::iscsi::tgt::add_iscsi_target(endpoint_uuid, true);
            }
        }
        else {
            log_debug("storage-discovery",
                      "Endpoint is not valid to create an iSCSI Target: " << endpoint_uuid);
            // If Endpoint's username is not in use in TGT, then credentials cannot be restored
            if (!agent::storage::iscsi::tgt::endpoint_represents_iscsi_target(endpoint_uuid)) {
                agent::storage::iscsi::tgt::clean_chap_from_endpoint(endpoint_uuid);
            }
        }
    }
}


void DiscoveryManager::update_volume_relations(std::vector<Volume>& volumes_to_update,
                                               Volume& volume_to_update,
                                               const Uuid& old_volume_uuid) {

    VolumeBuilder::update_identifier_with_uuid(volume_to_update);

    // Update all volumes with new UUID of currently processing volume
    for (auto& volume : volumes_to_update) {
        VolumeBuilder::update_replica_info(volume, old_volume_uuid, volume_to_update.get_uuid());
    }
}


void DiscoveryManager::update_drive_relations(Drive& drive_to_update) {
    DriveBuilder::update_identifier_with_uuid(drive_to_update);
}


void DiscoveryManager::add_weak_pool_volume_collection() {
    get_m2m_manager<StoragePool, Volume>().clear_entries();
    for (const auto& pool : get_manager<StoragePool>().get_entries()) {
        for (const auto& volume : get_manager<Volume>().get_entries()) {
            auto volume_device_path = Identifier::get_system_path(volume);
            auto pool_device_path = Identifier::get_system_path(pool);
            if (::starts_with(volume_device_path, pool_device_path)) {
                log_info("storage-discovery",
                         "Pool " << pool_device_path << " is parent for Volume " << volume_device_path);

                get_m2m_manager<StoragePool, Volume>().add_entry(pool.get_uuid(), volume.get_uuid());
            }
        }
    }
}


void DiscoveryManager::update_volume_capacity_sources() {
    for (const auto& volume_uuid : get_manager<Volume>().get_keys()) {
        Uuid providing_pool{};
        auto volume = get_manager<Volume>().get_entry_reference(volume_uuid);

        // TODO: move it to discoverer
        const auto& parent_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume->get_uuid());
        if (parent_pools.size() == 1) {
            providing_pool = parent_pools.front();
        }
        else {
            log_error("storage-discovery", "Volume " << volume->get_uuid() << " has more than 1 parent pool!");
        }

        VolumeBuilder::add_capacity_source(volume.get_raw_ref(), providing_pool);
    }
}


Uuid DiscoveryManager::discover() {
    log_info("storage-discovery", "Starting discovery...");

    auto manager = discover_manager();
    m_stabilizer.stabilize(manager);

    auto chassis = discover_chassis(manager.get_uuid());
    m_stabilizer.stabilize(chassis, manager);

    auto storage_service = discover_storage_service(manager.get_uuid());
    m_stabilizer.stabilize(storage_service, manager);

    auto system = discover_system(manager.get_uuid(), chassis.get_uuid());
    m_stabilizer.stabilize(system, manager);

    try {
        auto interfaces = discover_ethernet_interfaces(system.get_uuid());
        for (auto& interface : interfaces) {
            m_stabilizer.stabilize(interface, system);
            get_manager<NetworkInterface>().add_or_update_entry(interface);
        }
    } catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto drives = discover_drives(chassis.get_uuid());
        for (auto& drive : drives) {
            m_stabilizer.stabilize(drive);
            update_drive_relations(drive);
            get_manager<Drive>().add_or_update_entry(drive);
            get_m2m_manager<StorageService, Drive>().add_entry(storage_service.get_uuid(), drive.get_uuid());
        }
    } catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto volumes = discover_volumes(storage_service.get_uuid());
        for (auto& volume : volumes) {
            Uuid old_uuid = volume.get_uuid();
            m_stabilizer.stabilize(volume);
            update_volume_relations(volumes, volume, old_uuid);
        }

        // Add all volumes after full discovery
        for (const auto& volume : volumes) {
            get_manager<Volume>().add_or_update_entry(volume);
        }
    } catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    try {
        auto storage_pools = discover_storage_pools(storage_service.get_uuid());
        for (auto& pool : storage_pools) {
            m_discoverer->discover_capacity_sources(pool);
            m_stabilizer.stabilize(pool);
            StoragePoolBuilder::update_identifier_with_uuid(pool);

            get_manager<StoragePool>().add_or_update_entry(pool);
        }
    } catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("storage-discovery", ex.get_message());
    }

    // After discovery of Volumes, Storage Pools and Drives update its relations
    add_weak_pool_volume_collection();
    update_volume_capacity_sources();

    auto fabric = discover_fabric(manager.get_uuid());
    m_stabilizer.stabilize(fabric, manager);
    get_manager<Fabric>().add_or_update_entry(fabric);

    try {
        // It is important to do Endpoints discovery before Zones.
        auto endpoints = discover_endpoints(fabric.get_uuid());
        for (auto& endpoint : endpoints) {
            // Endpoints are stored in database so stabilization is not needed.
            get_manager<Endpoint>().add_or_update_entry(endpoint);
        }

        auto zones = discover_zones(fabric.get_uuid());
        for (auto& zone : zones) {
            // Zones are stored in database so stabilization is not needed.
            get_manager<Zone>().add_or_update_entry(zone);
        }

        restore_iscsi_targets(manager.get_uuid());
    } catch (const agent_framework::exceptions::GamiException& ex) {
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
    auto manager = ManagerBuilder::build_default();
    log_debug("storage-discovery", "Starting discovery of a manager.");

    m_discoverer->discover_manager(manager);

    log_debug("storage-discovery", "Finished discovery of a manager.");
    return manager;
}


StorageService DiscoveryManager::discover_storage_service(const Uuid& parent_uuid) {
    auto storage_service = StorageServiceBuilder::build_default(parent_uuid);
    log_debug("storage-discovery", "Starting discovery of a storage service.");

    m_discoverer->discover_storage_service(storage_service);

    log_debug("storage-discovery", "Finished discovery of a storage service.");
    return storage_service;
}

System DiscoveryManager::discover_system(const Uuid& parent_uuid, const Uuid& chassis_uuid) {
    auto system = SystemBuilder::build_default(parent_uuid, chassis_uuid);

    log_debug("storage-discovery", "Starting discovery of a system.");

    m_discoverer->discover_system(system);

    log_debug("storage-discovery", "Finished discovery of a system.");
    return system;
}

std::vector<NetworkInterface> DiscoveryManager::discover_ethernet_interfaces(const Uuid& parent_uuid) {
    std::vector<NetworkInterface> interfaces{};
    log_debug("storage-discovery", "Starting discovery of interfaces.");

    m_discoverer->discover_ethernet_interfaces(interfaces, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of interfaces.");
    return interfaces;
}

std::vector<StoragePool> DiscoveryManager::discover_storage_pools(const Uuid& parent_uuid) {
    std::vector<StoragePool> pools{};
    log_debug("storage-discovery", "Starting discovery of storage pools.");

    m_discoverer->discover_storage_pools(pools, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of storage pools.");
    return pools;
}


std::vector<Volume> DiscoveryManager::discover_volumes(const Uuid& parent_uuid) {
    std::vector<Volume> volumes{};
    log_debug("storage-discovery", "Starting discovery of volumes.");

    m_discoverer->discover_volumes(volumes, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of volumes.");
    return volumes;
}


Chassis DiscoveryManager::discover_chassis(const Uuid& parent_uuid) {
    auto chassis = ChassisBuilder::build_default(parent_uuid);
    log_debug("storage-discovery", "Starting discovery of a chassis.");

    m_discoverer->discover_chassis(chassis);

    log_debug("storage-discovery", "Finished discovery of a chassis.");
    return chassis;
}


std::vector<Drive> DiscoveryManager::discover_drives(const Uuid& parent_uuid) {
    std::vector<Drive> drives{};
    log_debug("storage-discovery", "Starting discovery of drives.");

    m_discoverer->discover_drives(drives, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of drives.");
    return drives;
}


Fabric DiscoveryManager::discover_fabric(const Uuid& parent_uuid) {
    auto fabric = FabricBuilder::build_default(parent_uuid);
    log_debug("storage-discovery", "Starting discovery of a fabric.");

    m_discoverer->discover_fabric(fabric);

    log_debug("storage-discovery", "Finished discovery of a fabric.");

    return fabric;
}


std::vector<Zone> DiscoveryManager::discover_zones(const Uuid& parent_uuid) {
    std::vector<Zone> zones{};
    log_debug("storage-discovery", "Starting discovery of zones.");

    m_discoverer->discover_zones(zones, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of zones.");

    return zones;
}


std::vector<Endpoint> DiscoveryManager::discover_endpoints(const Uuid& parent_uuid) {
    std::vector<Endpoint> endpoints{};
    log_debug("storage-discovery", "Starting discovery of endpoints.");

    m_discoverer->discover_endpoints(endpoints, parent_uuid);

    log_debug("storage-discovery", "Finished discovery of endpoints.");

    return endpoints;
}
