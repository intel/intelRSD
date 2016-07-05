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
 *
 * @file logical_drive_dependency_resolver.cpp
 *
 * @brief ...
 * */
#include "discovery/dependency_resolver/logical_drive_dependency_resolver.hpp"
#include "sysfs/sysfs_api.hpp"
#include "lvm/lvm_api.hpp"

using namespace agent::storage::discovery;
using namespace agent::storage::lvm;

using SysfsAPI = agent::storage::sysfs::SysfsAPI;
using HardDrive = agent_framework::generic::HardDrive;
using LogicalDrive = agent_framework::generic::LogicalDrive;

void LogicalDriveDependencyResolver::resolve() {
    auto& submodule = m_module.get_submodules().front();
    auto& logical_drives = submodule->get_logical_drives();
    for (auto& logical_drive : logical_drives) {
        const string& logical_drive_mode = logical_drive->get_mode();
        if (0 == logical_drive_mode.compare(LogicalDrive::LvmTypes::VOLUME_GROUP)) {
            resolve_volume_group(logical_drive);
        }
    }
}

void LogicalDriveDependencyResolver::resolve_physical_volume(LogicalDriveSharedPtr logical_drive) {
    const string& logical_drive_mode = logical_drive->get_mode();
    if (0 == logical_drive_mode.compare(LogicalDrive::LvmTypes::PHYSICAL_VOLUME)) {
        resolve_physical_volume_hard_disk(logical_drive);
    }
}

void LogicalDriveDependencyResolver::resolve_physical_volume_hard_disk(LogicalDriveSharedPtr physical_volume) {
    auto& hard_drive = m_hard_drive_map[physical_volume->get_name()];
    if (auto hd = hard_drive.lock()) {
        physical_volume->add_hard_drive(hd);
    }
}

void LogicalDriveDependencyResolver::resolve_volume_group(LogicalDriveSharedPtr volume_group) {
    auto& logical_volumes = volume_group->get_logical_drives();
    for (auto& logical_volume : logical_volumes) {
        resolve_physical_volume(logical_volume);
        const string& logical_volume_mode = logical_volume->get_mode();
        if (0 == logical_volume_mode.compare(LogicalDrive::LvmTypes::LOGICAL_VOLUME)) {
            logical_volume->set_volume_group(volume_group);
        }
    }
}

void LogicalDriveDependencyResolver::initialize() {
    auto& submodule = m_module.get_submodules().front();
    auto& storage_controller = submodule->get_storage_controllers().front();
    auto& hard_drives = storage_controller->get_hard_drives();
    for (auto& hard_drive : hard_drives) {
        const std::string device_path = hard_drive->get_device_path();
        m_hard_drive_map[device_path] = hard_drive;
    }
}
