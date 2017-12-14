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
 * @file requests/storage/add_logical_drive.cpp
 *
 * @brief Storage AddLogicalDrive request implementation
 * */

#include "agent-framework/module/requests/storage/add_logical_drive.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddLogicalDrive::AddLogicalDrive(
    const std::string& type,
    const double capacityGB,
    const std::string& mode,
    const std::string& master,
    const OptionalField<std::string>& image,
    const bool snapshot,
    const bool bootable,
    const bool mark_as_protected,
    const std::vector<std::string>& drives,
    const Oem& oem):
    m_type(type),
    m_capacityGB(capacityGB),
    m_mode(mode),
    m_master(master),
    m_image(image),
    m_snapshot(snapshot),
    m_bootable(bootable),
    m_protected(mark_as_protected),
    m_drives(drives),
    m_oem(oem) {}

json::Json AddLogicalDrive::to_json() const {
    json::Json value;
    value[LogicalDrive::TYPE] = m_type;
    value[LogicalDrive::CAPACITY] = m_capacityGB;
    value[LogicalDrive::MODE] = m_mode;
    value[LogicalDrive::MASTER] = m_master;
    value[LogicalDrive::IMAGE] = m_image;
    value[LogicalDrive::SNAPSHOT] = m_snapshot;
    value[LogicalDrive::BOOTABLE] = m_bootable;
    value[LogicalDrive::PROTECTED] = m_protected;
    value[LogicalDrive::DRIVES] = json::Json::array();
    for(auto& drive_uuid : m_drives){
        value[LogicalDrive::DRIVES].emplace_back(drive_uuid);
    }
    value[LogicalDrive::OEM] = m_oem.to_json();
    return value;
}

AddLogicalDrive AddLogicalDrive::from_json(const json::Json& json) {
    std::vector<std::string> drives;
    for(auto& drive_json : json[LogicalDrive::DRIVES]){
        drives.push_back(drive_json);
    }
    return AddLogicalDrive{
        json[LogicalDrive::TYPE],
        json[LogicalDrive::CAPACITY],
        json[LogicalDrive::MODE],
        json[LogicalDrive::MASTER],
        json[LogicalDrive::IMAGE],
        json[LogicalDrive::SNAPSHOT],
        json[LogicalDrive::BOOTABLE],
        json[LogicalDrive::PROTECTED],
        std::move(drives),
        Oem::from_json(json[LogicalDrive::OEM])
    };
}
