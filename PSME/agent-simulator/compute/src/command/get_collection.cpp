/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace {

    using KeysList = std::vector<std::string>;

    bool is_processor(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Processors;
    }

    bool is_memories(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Memory;
    }

    bool is_storage_controller(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::StorageControllers;
    }

    bool is_storage_subsystem(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::StorageSubsystems;
    }

    bool is_network(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::NetworkInterfaces;
    }

    bool is_drives(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Drives;
    }

    bool is_systems(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Systems;
    }

    bool is_chassis(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Chassis;
    }

    bool is_manager(const std::string& name) {
        return enums::CollectionName::from_string(name) == enums::CollectionName::Managers;
    }

    void response_add_submodules(const KeysList& keys, GetCollection::Response& response) {
        for (const auto& key : keys) {
            response.add_entry(SubcomponentEntry{key});
        }
    }

    void process_manager(const std::string& uuid, const std::string& name,
                         GetCollection::Response& response) {
        if (is_systems(name)) {
            response_add_submodules(CommonComponents::get_instance()->
                    get_system_manager().get_keys(uuid), response);
        }


        if (is_chassis(name)) {
            response_add_submodules(CommonComponents::get_instance()->
                    get_chassis_manager().get_keys(uuid), response);
        }

        if (is_manager(name)) {
            response_add_submodules(CommonComponents::get_instance()->
                    get_module_manager().get_keys(uuid), response);
        }
    }

    void process_storage_subsystem(const std::string& uuid, const std::string& name,
                                    GetCollection::Response& response) {
        if (is_drives(name)) {
            // StorageSubsystem has the same drives as Chassis
            // So, find the StorageSubsystem's parent System and grandparent Manager.
            // Then, find the chassis under the manager.
            // and find it's Drives.
            auto system_uuid = CommonComponents::get_instance()->
                get_storage_subsystem_manager().get_entry_reference(uuid)->get_parent_uuid();
            auto manager_uuid = CommonComponents::get_instance()->
                get_system_manager().get_entry_reference(system_uuid)->get_parent_uuid();

            // According to xml schema, there will be one chassis under the manager - no extra checks are required:
            auto chassis_uuid = CommonComponents::get_instance()->
                get_chassis_manager().get_keys(manager_uuid).front();

            response_add_submodules(CommonComponents::get_instance()->
                get_drive_manager().get_keys(chassis_uuid), response);
        }

        if (is_storage_controller(name)) {
            response_add_submodules(ComputeComponents::get_instance()->
                get_storage_controller_manager().get_keys(uuid), response);
        }


    }

    void process_processor(const std::string& uuid, GetCollection::Response& response) {
        response_add_submodules(get_manager<Processor>().get_keys(uuid), response);
    }

    void process_memories(const std::string& uuid, GetCollection::Response& response) {
        response_add_submodules(ComputeComponents::get_instance()->
                                get_memory_manager().get_keys(uuid), response);
    }

    void process_storage(const std::string& uuid, GetCollection::Response& response) {
        response_add_submodules(CommonComponents::get_instance()->
                                get_storage_subsystem_manager().
                                get_keys(uuid), response);
    }

    void process_network(const std::string& uuid, GetCollection::Response& response) {
        response_add_submodules(get_manager<NetworkInterface>().
                                get_keys(uuid), response);
    }

    void process_system(const std::string& uuid, const std::string& name,
                        GetCollection::Response& response) {
         if (is_processor(name)) {
            process_processor(uuid, response);
            return;
         }

         if (is_memories(name)) {
            process_memories(uuid, response);
            return;
         }

         if (is_storage_subsystem(name)) {
            process_storage(uuid, response);
            return;
         }

         if (is_network(name)) {
            process_network(uuid, response);
            return;
         }
    }

    void process_chassis(const std::string& uuid, const std::string& name,
                                   GetCollection::Response& response) {
        if (is_drives(name)) {
            response_add_submodules(CommonComponents::get_instance()->
                get_drive_manager().get_keys(uuid), response);
        }
    }

    void get_collection(const GetCollection::Request& req, GetCollection::Response& rsp) {
        auto uuid = req.get_uuid();
        auto name = req.get_name();
        log_debug("agent", "Getting collection of " + name + '.');

        if (CommonComponents::get_instance()->
                get_module_manager().entry_exists(uuid)) {
            process_manager(uuid, name, rsp);
            return;
        }

        if (CommonComponents::get_instance()->
                get_system_manager().entry_exists(uuid)) {
            process_system(uuid, name, rsp);
            return;
        }

        if (CommonComponents::get_instance()->get_storage_subsystem_manager().entry_exists(uuid)) {
            process_storage_subsystem(uuid, name, rsp);
            return;
        }

        if (CommonComponents::get_instance()->get_chassis_manager().entry_exists(uuid)) {
            process_chassis(uuid, name, rsp);
            return;
        }
    }
}

REGISTER_COMMAND(GetCollection, ::get_collection);
