/*!
 * @brief Provides class for stabilizing compute agent resource tree
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
 * @file update_relations.cpp
 * */

#include "tree_stability/helpers/update_relations.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace agent_framework::module;

namespace {

void update_managers(const std::string& temporary_uuid, const std::string& persistent_uuid) {
    // Chassis UUID is held by a manager
    auto& module_manager = get_manager<agent_framework::model::Manager>();
    const auto& managers_keys = module_manager.get_keys(
        [&temporary_uuid](const agent_framework::model::Manager& manager) {
            return manager.get_location() == temporary_uuid;
        });

    if (managers_keys.empty()) {
        throw std::runtime_error("No managers located in chassis: " + temporary_uuid);
    }

    module_manager.get_entry_reference(managers_keys.front())->set_location(persistent_uuid);
}

void update_systems(const std::string& temporary_uuid, const std::string& persistent_uuid) {
    // Chassis UUID is held by a system
    auto& system_manager = get_manager<agent_framework::model::System>();
    const auto& system_keys = system_manager.get_keys(
        [&temporary_uuid](const agent_framework::model::System& system) {
            return system.get_chassis() == temporary_uuid;
        });

    if (system_keys.empty()) {
        throw std::runtime_error("No system found for chassis: " + temporary_uuid);
    }

    system_manager.get_entry_reference(system_keys.front())->set_chassis(persistent_uuid);
}

}

namespace agent {
namespace compute {
namespace helpers {

void update_chassis_in_relations(const std::string& chassis_temporary_uuid, const std::string chassis_persistent_uuid) {
    update_managers(chassis_temporary_uuid, chassis_persistent_uuid);
    update_systems(chassis_temporary_uuid, chassis_persistent_uuid);
}

}
}
}
