/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file requests/storage/get_physical_drive_info.cpp
 *
 * @brief Storage GetPhysicalDriveInfo requests implementation
 * */

#include "agent-framework/module/requests/storage/get_physical_drive_info.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetPhysicalDriveInfo::GetPhysicalDriveInfo(const std::string& drive): m_drive{drive} {}

json::Json GetPhysicalDriveInfo::to_json() const {
    json::Json value;
    value[PhysicalDrive::DRIVE] = m_drive;
    return value;
}

GetPhysicalDriveInfo GetPhysicalDriveInfo::from_json(const json::Json& json) {
    return GetPhysicalDriveInfo{
        json[PhysicalDrive::DRIVE].get<std::string>()};
}
