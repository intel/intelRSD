/*!
 * @brief System builder class implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file system_builder.cpp
 */

#include "discovery/builders/fpgaof_system_builder.hpp"


using namespace agent::fpgaof::discovery;
using namespace agent_framework::model;


System FpgaofSystemBuilder::build_virtual(const Uuid& parent_uuid, const Uuid& chassis_uuid) {
    System system{parent_uuid};

    system.set_parent_type(enums::Component::Manager);
    system.set_status({enums::State::Enabled, enums::Health::OK});
    system.set_system_type(enums::SystemType::Virtual);
    system.set_chassis(chassis_uuid);

    system.add_collection(attribute::Collection{
        enums::CollectionName::NetworkInterfaces,
        enums::CollectionType::NetworkInterfaces
    });

    system.add_collection(attribute::Collection(
        enums::CollectionName::Processors,
        enums::CollectionType::Processors
    ));

    return system;
}