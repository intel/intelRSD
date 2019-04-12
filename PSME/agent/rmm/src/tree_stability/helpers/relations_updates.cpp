/*!
 * @brief Implements function for updating relations in the model to be used during tree stabilization.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file relations_updates.cpp
 */

#include "tree_stability/helpers/relations_updates.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;

using agent_framework::module::CommonComponents;

namespace agent {
namespace rmm {
namespace helpers {

void update_power_zone_in_relations(const std::string& old_uuid, const std::string& new_uuid) {
    // Power zone UUID is held by chassis
    auto& chassis_manager = get_manager<Chassis>();

    const auto& chassis_keys = chassis_manager.get_keys(
        [&old_uuid](const Chassis& chassis) {
            return chassis.get_power_zone() == old_uuid;
        }
    );

    for (const auto& chassis_key : chassis_keys) {
        chassis_manager.get_entry_reference(chassis_key)->set_power_zone(new_uuid);
    }
}


void update_thermal_zone_in_relations(const std::string& old_uuid, const std::string& new_uuid) {
    // Thermal zone UUID is held by chassis
    auto& chassis_manager = get_manager<Chassis>();

    const auto& chassis_keys = chassis_manager.get_keys(
        [&old_uuid](const Chassis& chassis) {
            return chassis.get_thermal_zone() == old_uuid;
        }
    );

    for (const auto& chassis_key : chassis_keys) {
        chassis_manager.get_entry_reference(chassis_key)->set_thermal_zone(new_uuid);
    }
}


void update_chassis_in_relations(const std::string& old_uuid, const std::string& new_uuid) {
    // Chassis UUID can be held as parent UUID by other chassis
    auto& chassis_manager = get_manager<Chassis>();

    const auto& chassis_keys = chassis_manager.get_keys(
        [&old_uuid](const Chassis& chassis) {
            return chassis.get_parent_chassis() == old_uuid;
        }
    );

    for (const auto& chassis_key : chassis_keys) {
        chassis_manager.get_entry_reference(chassis_key)->set_parent_chassis(new_uuid);
    }

    // Chassis UUID can be held as location UUID by managers
    auto& manager_manager = get_manager<Manager>();

    const auto& manager_keys = manager_manager.get_keys(
        [&old_uuid](const Manager& manager) {
            return manager.get_location() == old_uuid;
        }
    );

    for (const auto& manager_key : manager_keys) {
        manager_manager.get_entry_reference(manager_key)->set_location(new_uuid);
    }
}

}
}
}
