/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */
#include "agent-framework/command/compute/get_collection.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;

/*! GetCollection implementation */
class GetCollection : public compute::GetCollection {
public:
    using KeysList = std::vector<std::string>;

    GetCollection() {}

    using compute::GetCollection::execute;
    using ComputeComponents = agent_framework::module::ComputeManager;

    void execute(const Request& request, Response& response) {
        auto uuid = request.get_component();
        auto name = request.get_name();
        log_debug(GET_LOGGER("agent"), "Getting collection of " + name + '.');

        if (ComputeComponents::get_instance()->
                get_module_manager().entry_exists(uuid)) {
            process_manager(uuid, name, response);
            return;
        }

        if (ComputeComponents::get_instance()->
                get_system_manager().entry_exists(uuid)) {
            process_system(uuid, name, response);
            return;
        }

        if (ComputeComponents::get_instance()->
                get_storage_controller_manager().entry_exists(uuid)) {
            process_storage_controller(uuid, name, response);
            return;
        }
    }

    ~GetCollection();

private:

    void process_manager(const std::string& uuid,
                                const std::string& name, Response& response) {
        if (is_systems(name)) {
            response_add_submodules(ComputeComponents::get_instance()->
                    get_system_manager().get_keys(uuid), response);
        }


        if (is_chassis(name)) {
            response_add_submodules(ComputeComponents::get_instance()->
                    get_chassis_manager().get_keys(uuid), response);
        }

        if (is_manager(name)) {
            response_add_submodules(ComputeComponents::get_instance()->
                    get_module_manager().get_keys(uuid), response);
        }
    }

    void process_storage_controller(const std::string& uuid,
                                    const std::string& name,
                                                        Response& response) {
        if (!is_drives(name)) {
            return;
        }

        response_add_submodules(ComputeComponents::get_instance()->
                            get_drive_manager().get_keys(uuid), response);
    }

    void process_system(const std::string& uuid, const std::string& name,
                                                Response& response) {
         if (is_processor(name)) {
            process_processor(uuid, response);
            return;
         }

         if (is_memories(name)) {
            process_memories(uuid, response);
            return;
         }

         if (is_storage(name)) {
            process_storage(uuid, response);
            return;
         }

         if (is_network(name)) {
            process_network(uuid, response);
            return;
         }
    }

    void process_processor(const std::string& uuid, Response& response) {
        response_add_submodules(ComputeComponents::get_instance()->
                            get_processor_manager().get_keys(uuid), response);
    }

    void process_memories(const std::string& uuid, Response& response) {
        response_add_submodules(ComputeComponents::get_instance()->
                                get_dimm_manager().get_keys(uuid), response);
    }

    void process_storage(const std::string& uuid, Response& response) {
        response_add_submodules(ComputeComponents::get_instance()->
                                get_storage_controller_manager().
                                get_keys(uuid), response);
    }

    void process_network(const std::string& uuid, Response& response) {
        response_add_submodules(ComputeComponents::get_instance()->
                                get_network_interface_manager().
                                get_keys(uuid), response);
    }

    /* @TODO replace all literals with enums */
    bool is_processor(const std::string& name) {
        return name == "Processors";
    }

    bool is_memories(const std::string& name) {
        return name == "Memories";
    }

    bool is_storage(const std::string& name) {
        return name == "StorageControllers";
    }

    bool is_network(const std::string& name) {
        return name == "NetworkInterfaces";
    }

    bool is_drives(const std::string& name) {
        return name == "Drives";
    }

    bool is_systems(const std::string& name) {
        return name == "Systems";
    }

    bool is_chassis(const std::string& name) {
        return name == "Chassis";
    }

    bool is_manager(const std::string& name) {
        return name == "Managers";
    }

    void response_add_submodules(const KeysList& keys, Response& response) {
        for (const auto& key : keys) {
            response.add_subcomponent(key);
        }
    }
};

GetCollection::~GetCollection() {}

static Command::Register<GetCollection> g("Intel");
