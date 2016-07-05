/*!
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
 *
 * @file responses/storage/add_logical_drive.cpp
 * @brief AddLogicalDrive response
 * */

#include "agent-framework/module-ref/responses/storage/add_logical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include <json/json.h>

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

AddLogicalDrive::AddLogicalDrive(const std::string& drive, Oem oem):
    m_drive{drive},
    m_oem{oem} {}


Json::Value AddLogicalDrive::to_json() const {
    Json::Value value;
    value[LogicalDrive::DRIVE] = m_drive;
    value[LogicalDrive::OEM] = m_oem.to_json();
    return value;
}

AddLogicalDrive AddLogicalDrive::from_json
    (const Json::Value& json) {
    return AddLogicalDrive{json[LogicalDrive::DRIVE].asString(), Oem::from_json(
        json[LogicalDrive::OEM])};
}
