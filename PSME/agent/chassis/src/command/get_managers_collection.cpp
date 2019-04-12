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
 * @file command/get_managers_collection.cpp
 * @brief GetManagersCollection sdv implementation
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/chassis_commands.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

void get_manager_collection(const GetManagersCollection::Request&, GetManagersCollection::Response& response) {
    log_debug("chassis-agent", "GetManagersCollection");
    // Return only top level manager
    auto keys = CommonComponents::get_instance()->get_module_manager().get_keys("");

    for (const auto& key : keys) {
        const auto& manager = CommonComponents::get_instance()->get_module_manager().get_entry(key);
        // Chassis agent should only expose drawer manager.
        if (manager.has_persistent_uuid() && manager.get_manager_type() == enums::ManagerInfoType::EnclosureManager) {
            response.add_entry(agent_framework::model::attribute::ManagerEntry(key));
        }
    }
}

}

REGISTER_COMMAND(GetManagersCollection, ::get_manager_collection);
