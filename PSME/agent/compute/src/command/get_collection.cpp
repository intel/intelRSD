/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/compute_commands.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace {

using KeysVec = std::vector<std::string>;
using ComputeComponents = agent_framework::module::ComputeComponents;
using CommonComponents = agent_framework::module::CommonComponents;
using Collections = Array<Collection>;


void response_add_subcomponents(const KeysVec& keys,
                                GetCollection::Response& response) {

    for (const auto& key : keys) {
        SubcomponentEntry s;
        s.set_subcomponent(key);
        response.add_entry(s);
    }
}


const Collection find_collection(const Collections& collections,
                                 const std::string& name) {
    for (const auto& collection : collections) {
        if (name == collection.get_name()) {
            return collection;
        }
    }
    // Returning a non-existent collection
    Collection c;
    c.set_type(enums::CollectionType::None);
    return c;
}


void process_system(const Collection& collection, const std::string& uuid,
                    GetCollection::Response& response, const std::string& name) {

    // Seven possible collection types of a system
    if (enums::CollectionType::Processors == collection.get_type()) {
        response_add_subcomponents(get_manager<Processor>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::Memory == collection.get_type()) {
        response_add_subcomponents(get_manager<Memory>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::MemoryDomains == collection.get_type()) {
        response_add_subcomponents(get_manager<MemoryDomain>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::StorageSubsystems == collection.get_type()) {
        response_add_subcomponents(get_manager<StorageSubsystem>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::NetworkInterfaces == collection.get_type()) {
        response_add_subcomponents(get_manager<NetworkInterface>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::NetworkDevices == collection.get_type()) {
        response_add_subcomponents(get_manager<NetworkDevice>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::TrustedModules == collection.get_type()) {
        response_add_subcomponents(get_manager<TrustedModule>().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_manager(const Collection& collection, const std::string& uuid,
                     GetCollection::Response& response, const std::string& name) {

    // Two possible collection types of a manager
    if (enums::CollectionType::Chassis == collection.get_type()) {
        response_add_subcomponents(get_manager<Chassis>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::Systems == collection.get_type()) {
        response_add_subcomponents(get_manager<System>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::PCIeDevices == collection.get_type()) {
        response_add_subcomponents(get_manager<PcieDevice>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::LogServices == collection.get_type()) {
        response_add_subcomponents(get_manager<LogService>().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection, "compute-agent",
              "Collection not found: \'" + name + "\'.");
    }
}


void process_chassis(const Collection& collection, const std::string& uuid,
                     GetCollection::Response& response, const std::string& name) {

    // Three possible collection type for a chassis
    if (enums::CollectionType::Drives == collection.get_type()) {
        response_add_subcomponents(get_manager<Drive>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::PowerZones == collection.get_type()) {
        response_add_subcomponents(get_manager<PowerZone>().get_keys(uuid), response);
    }
    else if (enums::CollectionType::ThermalZones == collection.get_type()) {
        response_add_subcomponents(get_manager<ThermalZone>().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_storage_subsystem(const Collection& collection, const std::string& uuid,
                               GetCollection::Response& response, const std::string& name) {
    // Two possible collection type of a storage subsystem
    if (enums::CollectionType::Drives == collection.get_type()) {
        // StorageSubsystem has the same drives as Chassis
        // So, find the StorageSubsystem's parent System and grandparent Manager.
        // Then, find the chassis under the manager.
        // and find it's Drives.
        auto system_uuid = get_manager<StorageSubsystem>().get_entry_reference(uuid)->get_parent_uuid();
        auto manager_uuid = get_manager<System>().get_entry_reference(system_uuid)->get_parent_uuid();

        // In ComputeLoader, one chassis is created under the manager - no extra checks are required:
        auto chassis_uuid = get_manager<Chassis>().get_keys(manager_uuid).front();

        response_add_subcomponents(get_manager<Drive>().get_keys(chassis_uuid), response);
    }
    else if (enums::CollectionType::StorageControllers ==
             collection.get_type()) {
        response_add_subcomponents(get_manager<StorageController>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_network_interface(const Collection& collection, const
std::string&, GetCollection::Response&, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::EthernetSwitchPortVlans == collection.get_type()) {
        // Always empty
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_network_device(const Collection& collection, const std::string& uuid,
                     GetCollection::Response& response, const std::string& name) {

    // One possible collection type for a chassis
    if (enums::CollectionType::NetworkDeviceFunctions == collection.get_type()) {
        response_add_subcomponents(get_manager<NetworkDeviceFunction>().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_thermal_zone(const Collection& collection, const std::string& uuid,
                          GetCollection::Response& response, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::Fans == collection.get_type()) {
        response_add_subcomponents(get_manager<Fan>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_pcie_device(const Collection& collection, const std::string& uuid,
                          GetCollection::Response& response, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::PCIeFunctions == collection.get_type()) {
        response_add_subcomponents(get_manager<PcieFunction>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}

void process_memory_domain(const Collection& collection, const std::string& uuid,
                         GetCollection::Response& response, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::MemoryChunks == collection.get_type()) {
        response_add_subcomponents(get_manager<MemoryChunks>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}

void process_processor(const Collection& collection, const std::string& uuid,
                           GetCollection::Response& response, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::Processors == collection.get_type()) {
        response_add_subcomponents(get_manager<Processor>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_log_service(const Collection& collection, const std::string& uuid,
                         GetCollection::Response& response, const std::string& name) {
    // Only one possible collection
    if (enums::CollectionType::LogEntries == collection.get_type()) {
        response_add_subcomponents(get_manager<LogEntry>().get_keys(uuid), response);
    }
    else {
        THROW(::agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void do_get_collection(const GetCollection::Request& request, GetCollection::Response& response) {

    auto uuid = request.get_uuid();
    auto name = request.get_name();
    log_debug("compute-agent", "Getting collection of " + name + '.');

    if (get_manager<Manager>().entry_exists(uuid)) {

        const Manager& manager = get_manager<Manager>().get_entry(uuid);
        const Collection& collection =
            find_collection(manager.get_collections(), name);

        process_manager(collection, uuid, response, name);
    }
    else if (get_manager<System>().entry_exists(uuid)) {

        const System& sys = get_manager<System>().get_entry(uuid);
        const Collection& collection =
            find_collection(sys.get_collections(), name);

        process_system(collection, uuid, response, name);
    }
    else if (get_manager<StorageSubsystem>().entry_exists(uuid)) {

        const StorageSubsystem& subsystem =
            get_manager<StorageSubsystem>().get_entry(uuid);
        const Collection& collection =
            find_collection(subsystem.get_collections(), name);

        process_storage_subsystem(collection, uuid, response, name);
    }
    else if (get_manager<NetworkInterface>().entry_exists(uuid)) {

        const NetworkInterface& interface =
            get_manager<NetworkInterface>().get_entry(uuid);
        const Collection& collection =
            find_collection(interface.get_collections(), name);

        process_network_interface(collection, uuid, response, name);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {

        const Chassis& chassis =
            get_manager<Chassis>().get_entry(uuid);
        const Collection& collection =
            find_collection(chassis.get_collections(), name);

        process_chassis(collection, uuid, response, name);
    }
    else if (get_manager<NetworkDevice>().entry_exists(uuid)) {

        const NetworkDevice& device =
            get_manager<NetworkDevice>().get_entry(uuid);
        const Collection& collection =
            find_collection(device.get_collections(), name);

        process_network_device(collection, uuid, response, name);
    }
    else if (get_manager<ThermalZone>().entry_exists(uuid)) {

        const ThermalZone zone = get_manager<ThermalZone>().get_entry(uuid);
        const Collection& collection = find_collection(zone.get_collections(), name);

        process_thermal_zone(collection, uuid, response, name);
    }
    else if (get_manager<PcieDevice>().entry_exists(uuid)) {

        const PcieDevice device = get_manager<PcieDevice>().get_entry(uuid);
        const Collection& collection = find_collection(device.get_collections(), name);

        process_pcie_device(collection, uuid, response, name);
    }
    else if (get_manager<MemoryDomain>().entry_exists(uuid)) {

        const MemoryDomain memory_domain = get_manager<MemoryDomain>().get_entry(uuid);
        const Collection& collection = find_collection(memory_domain.get_collections(), name);

        process_memory_domain(collection, uuid, response, name);
    }
    else if (get_manager<Processor>().entry_exists(uuid)) {

        const Processor processor = get_manager<Processor>().get_entry(uuid);
        const Collection& collection = find_collection(processor.get_collections(), name);

        process_processor(collection, uuid, response, name);
    }
    else if (get_manager<LogService>().entry_exists(uuid)) {

        const LogService log_service = get_manager<LogService>().get_entry(uuid);
        const Collection& collection = find_collection(log_service.get_collections(), name);

        process_log_service(collection, uuid, response, name);
    }
    else if (get_manager<Memory>().entry_exists(uuid)
             || get_manager<NetworkDeviceFunction>().entry_exists(uuid)
             || get_manager<Drive>().entry_exists(uuid)
             || get_manager<PowerZone>().entry_exists(uuid)
             || get_manager<Fan>().entry_exists(uuid)
             || get_manager<PcieFunction>().entry_exists(uuid)) {

        THROW(agent_framework::exceptions::InvalidUuid, "compute-agent",
              "This component type has no collections.");
    }
    else {
        THROW(agent_framework::exceptions::InvalidUuid, "compute-agent",
              "Component not found, invalid uuid: " + uuid);
    }

}
}

REGISTER_COMMAND(GetCollection, ::do_get_collection);
