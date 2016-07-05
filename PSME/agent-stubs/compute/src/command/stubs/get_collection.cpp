/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file command/stubs/get_collection.cpp
 * @brief Implementation of GetCollection command
 * */
#include "agent-framework/command/compute/get_collection.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

/*! @brief GetCollection implementation */
class GetCollection : public compute::GetCollection {
public:
    GetCollection() {}

    ~GetCollection();

    using KeysVec = std::vector<std::string>;
    using compute::GetCollection::execute;
    using Collections = attribute::Array<attribute::Collection>;
    using ComputeComponents = agent_framework::module::ComputeManager;
    using ChassisManager = agent_framework::module::ChassisManager;

    void execute(const Request& request, Response& response) {
        const auto& uuid = request.get_component();
        const auto& name = request.get_name();

        if(ComputeComponents::get_instance()->
            get_module_manager().entry_exists(uuid)) {

            const Manager& manager = ComputeComponents::get_instance()->
                get_module_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(manager.get_collections(), name);

            process_manager(collection, uuid, response, name);
        }
        else if(ComputeComponents::get_instance()->
            get_system_manager().entry_exists(uuid)) {

            const System& sys = ComputeComponents::get_instance()->
                get_system_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(sys.get_collections(), name);

            process_system(collection, uuid, response, name);
        }
        else if(ComputeComponents::get_instance()->
            get_storage_controller_manager().entry_exists(uuid)) {

            const StorageController& controller =
                ComputeComponents::get_instance()->
                get_storage_controller_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(controller.get_collections(), name);

            process_storage_controller(collection, uuid, response, name);
        }
        else if(ComputeComponents::get_instance()->
            get_network_interface_manager().entry_exists(uuid)) {

            const NetworkInterface& interface =
                ComputeComponents::get_instance()->
                get_network_interface_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(interface.get_collections(), name);

            process_network_interface(collection, uuid, response, name);
        }
        else if(ComputeComponents::get_instance()->
                    get_dimm_manager().entry_exists(uuid)
                || ComputeComponents::get_instance()->
                    get_processor_manager().entry_exists(uuid)
                || ComputeComponents::get_instance()->
                    get_drive_manager().entry_exists(uuid)
                || ComputeComponents::get_instance()->
                    get_port_vlan_manager().entry_exists(uuid)) {

            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                "This component type has no collections");
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "agent",
                "Component not found, invalid uuid: " + uuid);
        }

        log_debug(GET_LOGGER("compute-agent"),
            "Getting collection successful");
    }

private:

    void process_manager(const Collection& collection, const std::string&
        uuid, Response& response, const std::string& name) {
        //two possible collection types of a manager
        if(enums::CollectionType::Chassis == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_chassis_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::Systems == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_system_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                "Collection not found: \'" + name + "\'");
        }
    }

    void process_system(const Collection& collection, const std::string& uuid,
        Response& response, const std::string& name) {
            //five possible collection types of a system
        if(enums::CollectionType::Processors == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_processor_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::Dimms == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_dimm_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::MemoryChunks == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_memory_chunk_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::StorageControllers ==
                collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_storage_controller_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::NetworkInterfaces ==
                collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_network_interface_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                "Collection not found: \'" + name + "\'");
        }
    }

    void process_storage_controller(const Collection& collection, const
        std::string& uuid, Response& response, const std::string& name) {
        //one possible collection type of a storage controller
        if(enums::CollectionType::Drives == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_drive_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters,
                "agent", "Collection not found: \'" + name + "\'");
        }
    }

    void process_network_interface(const Collection& collection, const
        std::string& uuid, Response& response, const std::string& name) {
        //one possible collection type of a network interface
        if(enums::CollectionType::PortVlans == collection.get_type()) {
            response_add_subcomponents(ComputeComponents::get_instance()->
                get_port_vlan_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters,
                "agent", "Collection not found: \'" + name + "\'");
        }
    }

    const Collection find_collection(const Collections& collections,
        const std::string& name) {
        for(const auto& collection : collections) {
            if(name == collection.get_name()) {
                return collection;
            }
        }
        //returning a non-existent collection
        Collection c;
        c.set_type(enums::CollectionType::None);
        return c;
    }

    void response_add_subcomponents(const KeysVec& keys, Response& response) {
        for (const auto& key : keys) {
            SubcomponentEntry s;
            s.set_subcomponent(key);
            response.add_subcomponent(s);
        }
    }
};



GetCollection::~GetCollection() {}

static Command::Register<GetCollection> g("Stubs");
