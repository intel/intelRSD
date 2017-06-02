/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"



using namespace agent_framework::command_ref;
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

    // Six possible collection types of a system
    if (enums::CollectionType::Processors == collection.get_type()) {
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_processor_manager().get_keys(uuid), response);
    }
    else if (enums::CollectionType::Memory == collection.get_type()) {
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_memory_manager().get_keys(uuid), response);
    }
    else if (enums::CollectionType::StorageSubsystems == collection.get_type()) {
        response_add_subcomponents(CommonComponents::get_instance()->
            get_storage_subsystem_manager().get_keys(uuid), response);
    }
    else if (enums::CollectionType::NetworkInterfaces == collection.get_type()) {
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_network_interface_manager().get_keys(uuid), response);
    }
    else if (enums::CollectionType::NetworkDevices == collection.get_type()) {
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_network_device_manager().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void process_manager(const Collection& collection, const std::string& uuid,
                     GetCollection::Response& response, const std::string& name) {

    // Add anything only if manager is present
    if (!CommonComponents::get_instance()->
        get_module_manager().get_entry_reference(uuid)->get_presence()) {
        return;
    }

    // Two possible collection types of a manager
    if (enums::CollectionType::Chassis == collection.get_type()) {
        response_add_subcomponents(CommonComponents::get_instance()->
            get_chassis_manager().get_keys(uuid), response);
    }
    else if (enums::CollectionType::Systems == collection.get_type()) {
        response_add_subcomponents(CommonComponents::get_instance()->
            get_system_manager().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection, "compute-agent",
              "Collection not found: \'" + name + "\'.");
    }
}


void process_chassis(const Collection& collection, const std::string& uuid,
                     GetCollection::Response& response, const std::string& name) {

    // One possible collection type for a chassis
    if (enums::CollectionType::Drives == collection.get_type()) {
        response_add_subcomponents(CommonComponents::get_instance()->
            get_drive_manager().get_keys(uuid), response);
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
        auto system_uuid = CommonComponents::get_instance()->
            get_storage_subsystem_manager().get_entry_reference(uuid)->get_parent_uuid();
        auto manager_uuid = CommonComponents::get_instance()->
            get_system_manager().get_entry_reference(system_uuid)->get_parent_uuid();

        // In ComputeLoader, one chassis is created under the manager - no extra checks are required:
        auto chassis_uuid = CommonComponents::get_instance()->
            get_chassis_manager().get_keys(manager_uuid).front();

        response_add_subcomponents(CommonComponents::get_instance()->
            get_drive_manager().get_keys(chassis_uuid), response);
    }
    else if (enums::CollectionType::StorageControllers ==
             collection.get_type()) {
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_storage_controller_manager().get_keys(uuid), response);
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
        response_add_subcomponents(ComputeComponents::get_instance()->
            get_network_device_function_manager().get_keys(uuid), response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection,
              "compute-agent", "Collection not found: \'" + name + "\'.");
    }
}


void do_get_collection(const GetCollection::Request& request, GetCollection::Response& response) {

    auto uuid = request.get_uuid();
    auto name = request.get_name();
    log_debug(GET_LOGGER("compute-agent"), "Getting collection of " + name + '.');

    if (CommonComponents::get_instance()->
        get_module_manager().entry_exists(uuid)) {

        const Manager& manager = CommonComponents::get_instance()->
            get_module_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(manager.get_collections(), name);

        process_manager(collection, uuid, response, name);
    }
    else if (CommonComponents::get_instance()->
        get_system_manager().entry_exists(uuid)) {

        const System& sys = CommonComponents::get_instance()->
            get_system_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(sys.get_collections(), name);

        process_system(collection, uuid, response, name);
    }
    else if (CommonComponents::get_instance()->
        get_storage_subsystem_manager().entry_exists(uuid)) {

        const StorageSubsystem& subsystem =
            CommonComponents::get_instance()->
                get_storage_subsystem_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(subsystem.get_collections(), name);

        process_storage_subsystem(collection, uuid, response, name);
    }
    else if (ComputeComponents::get_instance()->
        get_network_interface_manager().entry_exists(uuid)) {

        const NetworkInterface& interface =
            ComputeComponents::get_instance()->
                get_network_interface_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(interface.get_collections(), name);

        process_network_interface(collection, uuid, response, name);
    }
    else if (CommonComponents::get_instance()->
        get_chassis_manager().entry_exists(uuid)) {

        const Chassis& chassis =
            CommonComponents::get_instance()->
                get_chassis_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(chassis.get_collections(), name);

        process_chassis(collection, uuid, response, name);
    }
    else if (ComputeComponents::get_instance()->
        get_network_device_manager().entry_exists(uuid)) {

        const NetworkDevice& device =
            ComputeComponents::get_instance()->
                get_network_device_manager().get_entry(uuid);
        const Collection& collection =
            find_collection(device.get_collections(), name);

        process_network_device(collection, uuid, response, name);
    }
    else if (ComputeComponents::get_instance()->get_memory_manager().entry_exists(uuid)
             || ComputeComponents::get_instance()->get_processor_manager().entry_exists(uuid)
             || ComputeComponents::get_instance()->get_network_device_function_manager().entry_exists(uuid)
             || CommonComponents::get_instance()->get_drive_manager().entry_exists(uuid)) {

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
