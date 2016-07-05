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

#include "agent-framework/module-ref/model/logical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component LogicalDrive::component =
    enums::Component::LogicalDrive;
const enums::CollectionName LogicalDrive::collection_name =
    enums::CollectionName::LogicalDrives;

LogicalDrive::LogicalDrive(const std::string& parent_uuid) :
    BlockDevice{"", parent_uuid, enums::Component::StorageServices} {}

LogicalDrive::~LogicalDrive() {}

Json::Value LogicalDrive::to_json() const {
    Json::Value json;
    json[literals::LogicalDrive::STATUS] = get_status().to_json();
    json[literals::LogicalDrive::TYPE] = get_type();
    json[literals::LogicalDrive::CAPACITY] = get_capacity_gb();
    json[literals::LogicalDrive::MODE] = get_mode();
    json[literals::LogicalDrive::MASTER] = get_master();
    json[literals::LogicalDrive::SNAPSHOT] = get_snapshot();
    json[literals::LogicalDrive::IMAGE] = get_image();
    json[literals::LogicalDrive::BOOTABLE] = get_bootable();
    json[literals::LogicalDrive::PROTECTED] = get_protected();
    json[literals::LogicalDrive::COLLECTIONS] = get_collections().to_json();
    json[literals::LogicalDrive::OEM] = get_oem().to_json();
    return json;
}

LogicalDrive LogicalDrive::from_json(const Json::Value& json) {
    LogicalDrive logical_drive;

    logical_drive.set_status(attribute::Status::from_json(
        json[literals::LogicalDrive::STATUS]));
    logical_drive.set_type(json[literals::LogicalDrive::TYPE]);
    logical_drive.set_capacity_gb(json[literals::LogicalDrive::CAPACITY]);
    logical_drive.set_mode(json[literals::LogicalDrive::MODE]);
    logical_drive.set_master(json[literals::LogicalDrive::MASTER]);
    logical_drive.set_snapshot(json[literals::LogicalDrive::SNAPSHOT]);
    logical_drive.set_image(json[literals::LogicalDrive::IMAGE]);
    logical_drive.set_bootable(json[literals::LogicalDrive::BOOTABLE]);
    logical_drive.set_protected(json[literals::LogicalDrive::PROTECTED]);
    logical_drive.set_collections(Collections::from_json(
        json[literals::LogicalDrive::COLLECTIONS]));
    logical_drive.set_oem(attribute::Oem::from_json(
        json[literals::LogicalDrive::OEM]));
    logical_drive.set_resource_hash(compute_hash(json));

    return logical_drive;
}
