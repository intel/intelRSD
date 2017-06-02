/*!
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
 *
 * @file responses/storage/delete_logical_drive.cpp
 * @brief DeleteLogicalDrive response
 * */

#include "agent-framework/module/responses/storage/delete_logical_drive.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include <json/json.h>

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

DeleteLogicalDrive::DeleteLogicalDrive(Oem oem):
    m_oem{oem} {}


Json::Value DeleteLogicalDrive::to_json() const {
    Json::Value value;
    value[LogicalDrive::OEM] = m_oem.to_json();
    return value;
}

DeleteLogicalDrive DeleteLogicalDrive::from_json
    (const Json::Value& json) {
    return DeleteLogicalDrive{Oem::from_json(
        json[LogicalDrive::OEM])};
}
