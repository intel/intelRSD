/*!
 * @brief Drive builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file drive_builder.cpp
 */

#include "discovery/builders/drive_builder.hpp"
#include "discovery/builders/identifier_builder.hpp"



using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


model::Drive DriveBuilder::build_default(const Uuid& parent_uuid) {
    model::Drive drive{parent_uuid};
    drive.set_status(true);
    return drive;
}

void DriveBuilder::update_with_sysfs(agent_framework::model::Drive& drive,
                                     const SysfsApi::HardDrive& block_device) {

    drive.set_type(model::enums::DriveType::from_string(block_device.get_type()));
    if (drive.get_type() == model::enums::DriveType::HDD) {
        drive.set_rpm(block_device.get_rpm());
    }

    /* Interface cannot be read from Sysfs */
    drive.set_interface({});

    drive.set_capacity_gb(block_device.get_capacity_gb());
    drive.set_fru_info(model::attribute::FruInfo{
        block_device.get_serial_number(),
        block_device.get_manufacturer(),
        block_device.get_model(),
        {} // null
    });
}

void DriveBuilder::add_identifier_with_system_path(agent_framework::model::Drive& drive,
                                                   const SysfsApi::HardDrive& block_device) {

    IdentifierBuilder::add_system_path_identifier(drive, block_device.get_device_path());
}

void DriveBuilder::add_identifier_with_uuid(agent_framework::model::Drive& drive) {
    IdentifierBuilder::add_uuid_identifier(drive, drive.get_uuid());
}

void DriveBuilder::update_identifier_with_uuid(agent_framework::model::Drive& drive) {
    IdentifierBuilder::update_uuid_identifier(drive, drive.get_uuid());
}
