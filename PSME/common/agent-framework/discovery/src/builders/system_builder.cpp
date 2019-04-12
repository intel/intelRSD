/*!
 * @brief System builder class implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file system_builder.cpp
 */

#include "agent-framework/discovery/builders/system_builder.hpp"
#include "sysfs/sysfs_interface.hpp"

using namespace agent_framework::discovery;
using namespace agent_framework;


model::System SystemBuilder::build_virtual(const Uuid& parent_uuid, const Uuid& chassis_uuid) {
    model::System system{parent_uuid};

    system.set_parent_type(model::enums::Component::Manager);
    system.set_status({model::enums::State::Enabled, model::enums::Health::OK});
    system.set_system_type(model::enums::SystemType::Virtual);
    system.set_chassis(chassis_uuid);

    system.add_collection(model::attribute::Collection{
        model::enums::CollectionName::NetworkInterfaces,
        model::enums::CollectionType::NetworkInterfaces
    });
    system.add_collection(model::attribute::Collection{
        model::enums::CollectionName::StorageServices,
        model::enums::CollectionType::StorageServices
    });

    system.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Processors,
        model::enums::CollectionType::Processors
    ));

    return system;
}


void SystemBuilder::update_with_guid(model::System& system, const sysfs::SysfsFile& guid_file) {

    std::stringstream ss{guid_file.value};
    std::string guid{};

    // Read only first line
    std::getline(ss, guid);
    if (!guid.empty()) {
        system.set_guid(guid);
    }
}
