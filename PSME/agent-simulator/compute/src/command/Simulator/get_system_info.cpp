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

#include "agent-framework/command/compute/get_system_info.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
using namespace agent_framework::model;

using namespace agent_framework::command;
using namespace agent_framework::module;

namespace agent {
class GetSystemInfo : public agent_framework::command::compute::GetSystemInfo {
public:
    GetSystemInfo() { }

    using agent_framework::command::compute::GetSystemInfo::execute;
    using ComputeComponents = agent_framework::module::ComputeManager;

    void execute(const Request& request, Response& response) {
        log_debug(GET_LOGGER("agent"), "Getting system info.");
        auto system = ComputeComponents::get_instance()->
                get_system_manager().get_entry(request.get_system());
        process_collections(system);
        response.set_system(std::move(system));
    }

    ~GetSystemInfo();
private:
    void process_collections(System& system) {
        if (ComputeComponents::get_instance()->
                get_processor_manager().get_entry_count(system.get_uuid())) {
            system.add_collection(attribute::Collection(enums::CollectionName::Processors,
                    enums::CollectionType::Processors, ""));
        }
        if (ComputeComponents::get_instance()->
                get_dimm_manager().get_entry_count(system.get_uuid())) {
            system.add_collection(attribute::Collection(enums::CollectionName::Memories,
                    enums::CollectionType::Dimms,""));
        }
        if (ComputeComponents::get_instance()->
                get_storage_controller_manager().get_entry_count(system.get_uuid())) {
            system.add_collection(attribute::Collection(enums::CollectionName::StorageControllers, enums::CollectionType::StorageControllers,""));
        }
        if (ComputeComponents::get_instance()->
                get_network_interface_manager().get_entry_count(system.get_uuid())) {
            system.add_collection(attribute::Collection(enums::CollectionName::NetworkInterfaces, enums::CollectionType::NetworkInterfaces,""));
        }
        if (ComputeComponents::get_instance()->
                get_memory_chunk_manager().get_entry_count(system.get_uuid())) {
            system.add_collection(attribute::Collection(enums::CollectionName::MemoryChunks, enums::CollectionType::MemoryChunks,""));
        }
    }
};

GetSystemInfo::~GetSystemInfo() { }
}
static Command::Register<agent::GetSystemInfo> g("Simulator");
