/*!
 * @brief Manager builder class implementation.
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
 * @file manager_builder.cpp
 */

#include "discovery/builders/manager_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


Manager ManagerBuilder::build_default() {
    Manager manager{};

    manager.add_collection({enums::CollectionName::Systems,
                            enums::CollectionType::Systems
                           });

    manager.add_collection({enums::CollectionName::Chassis,
                            enums::CollectionType::Chassis
                           });

    manager.add_collection({enums::CollectionName::PcieDevices,
                           enums::CollectionType::PCIeDevices
                           });

    manager.set_status({enums::State::Enabled, {}});

    return manager;
}


void ManagerBuilder::update_device_id_data(Manager& manager,
                                           const ipmi::command::generic::response::GetDeviceId& get_device_id) {
    manager.set_firmware_version(get_device_id.get_firmware_version());
}


void ManagerBuilder::update_guid_data(Manager& manager,
                                      const ipmi::command::generic::response::GetDeviceGuid& get_device_guid) {
    manager.set_guid(get_device_guid.get_guid());
}


void ManagerBuilder::update_chassis(agent_framework::model::Manager& manager, const std::string& chassis_uuid) {
    manager.set_location(chassis_uuid);
}


