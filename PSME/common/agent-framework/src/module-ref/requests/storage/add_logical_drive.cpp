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
 * @file requests/storage/add_logical_drive.cpp
 *
 * @brief Storage AddLogicalDrive request implementation
 * */

#include "agent-framework/module-ref/requests/storage/add_logical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddLogicalDrive::AddLogicalDrive(
    const std::string& type,
    double capacityGB,
    const std::string& mode,
    const std::string& master,
    const OptionalField<std::string>& image,
    bool snapshot,
    bool mark_as_protected,
    const std::vector<std::string>& drives,
    agent_framework::model::attribute::Oem):
    m_type{type},
    m_capacityGB{capacityGB},
    m_mode{mode},
    m_master{master},
    m_image{image},
    m_snapshot{snapshot},
    m_protected{mark_as_protected},
    m_drives(drives),
    m_oem{} {}

Json::Value AddLogicalDrive::to_json() const {
    Json::Value value;
    value[LogicalDrive::TYPE] = m_type;
    value[LogicalDrive::CAPACITY] = m_capacityGB;
    value[LogicalDrive::MODE] = m_mode;
    value[LogicalDrive::MASTER] = m_master;
    value[LogicalDrive::IMAGE] = get_image();
    value[LogicalDrive::SNAPSHOT] = m_snapshot;
    value[LogicalDrive::PROTECTED] = m_protected;
    value[LogicalDrive::DRIVES] = Json::ValueType::arrayValue;
    for(auto& drive_uuid : m_drives){
        value[LogicalDrive::DRIVES].append(drive_uuid);
    }
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddLogicalDrive AddLogicalDrive::from_json(const Json::Value& json) {
    std::vector<std::string> drives;
    for(auto& drive_json : json[LogicalDrive::DRIVES]){
        drives.push_back(drive_json.asString());
    }
    return AddLogicalDrive{
        json[LogicalDrive::TYPE].asString(),
        json[LogicalDrive::CAPACITY].asDouble(),
        json[LogicalDrive::MODE].asString(),
        json[LogicalDrive::MASTER].asString(),
        json[LogicalDrive::IMAGE],
        json[LogicalDrive::SNAPSHOT].asBool(),
        json[LogicalDrive::PROTECTED].asBool(),
        std::move(drives),
        agent_framework::model::attribute::Oem{}
    };
}
