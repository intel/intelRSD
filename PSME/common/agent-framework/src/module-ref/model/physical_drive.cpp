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

#include "agent-framework/module-ref/model/physical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component PhysicalDrive::component = enums::Component::PhysicalDrive;
const enums::CollectionName PhysicalDrive::collection_name =
    enums::CollectionName::PhysicalDrives;

PhysicalDrive::PhysicalDrive(const std::string& parent_uuid) :
    BlockDevice{"", parent_uuid, enums::Component::StorageServices} {}

PhysicalDrive::~PhysicalDrive() {}

Json::Value PhysicalDrive::to_json() const {
    Json::Value result;
    result[literals::PhysicalDrive::STATUS] = get_status().to_json();
    result[literals::PhysicalDrive::INTERFACE] = get_interface();
    result[literals::PhysicalDrive::TYPE] = get_type();
    result[literals::PhysicalDrive::CAPACITY] = get_capacity_gb();
    result[literals::PhysicalDrive::RPM] = get_rpm();
    result[literals::PhysicalDrive::FRU_INFO] = get_fru_info().to_json();
    result[literals::PhysicalDrive::OEM] = get_oem().to_json();
    return result;
}

PhysicalDrive PhysicalDrive::from_json(const Json::Value& json) {
    PhysicalDrive drive;

    drive.set_status(attribute::Status::from_json(
        json[literals::PhysicalDrive::STATUS]));
    drive.set_interface(json[literals::PhysicalDrive::INTERFACE]);
    drive.set_type(json[literals::PhysicalDrive::TYPE]);
    drive.set_capacity_gb(json[literals::PhysicalDrive::CAPACITY]);
    drive.set_rpm(json[literals::PhysicalDrive::RPM]);
    drive.set_fru_info(attribute::FruInfo::from_json(
        json[literals::PhysicalDrive::FRU_INFO]));
    drive.set_oem(attribute::Oem::from_json(json[literals::PhysicalDrive::OEM]));
    drive.set_resource_hash(compute_hash(json));

    return drive;
}
