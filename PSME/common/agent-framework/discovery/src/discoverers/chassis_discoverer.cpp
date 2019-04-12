/*!
 * @brief Chassis discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file chassis_discoverer.cpp
 */

#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"

#include "agent-framework/discovery/discoverers/chassis_discoverer.hpp"
#include "agent-framework/discovery/builders/chassis_builder.hpp"


using namespace agent_framework::discovery;

agent_framework::model::Chassis ChassisDiscoverer::discover(const Uuid& parent_uuid) {
    auto chassis = ChassisBuilder::build_default(parent_uuid);
    return chassis;
}

void ChassisDiscoverer::discover_database(agent_framework::model::Chassis& chassis) {
    database::ChassisEntity entity{chassis.get_uuid()};
    try {
        std::string value = entity.get(database::CHASSIS_ASSET_TAG_PROPERTY);
        log_debug("chassis-discoverer", "Loaded chassis asset tag: " << value << "for chassis: " << chassis.get_uuid());
        chassis.set_asset_tag(value);
    }
    catch (const std::exception&) {
        log_debug("chassis-discoverer", "Asset Tag attribute is not stored for chassis: " << chassis.get_uuid());
    }
}



