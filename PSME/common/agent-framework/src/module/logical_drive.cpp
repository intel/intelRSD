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
 * */

#include "agent-framework/module/logical_drive.hpp"

#include <algorithm>

using namespace agent_framework::generic;

constexpr const char LogicalDrive::LvmTypes::VOLUME_GROUP[];
constexpr const char LogicalDrive::LvmTypes::LOGICAL_VOLUME[];
constexpr const char LogicalDrive::LvmTypes::PHYSICAL_VOLUME[];

LogicalDrive::~LogicalDrive(){}

LogicalDrive::LogicalDriveWeakPtr
LogicalDrive::find_logical_drive(const std::string& uuid) const {
    for (const auto& logical_drive : get_logical_drives()) {
        if (logical_drive->get_uuid() == uuid) {
            return logical_drive;
        }
        else {
            auto sub_logical_drive = logical_drive->find_logical_drive(uuid);
            if (!sub_logical_drive.expired()) {
                return sub_logical_drive;
            }
        }
    }
    return {};
}

bool LogicalDrive::delete_logical_drive(const std::string& uuid) {

    auto it = std::remove_if(m_logical_drives.begin(), m_logical_drives.end(),
            [&uuid](LogicalDriveSharedPtr & ld) {
                return uuid == ld->get_uuid();
            });

    bool ret = (it != m_logical_drives.end());

    m_logical_drives.erase(it, m_logical_drives.end());

    return ret;
}
