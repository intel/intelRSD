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

#include "agent-framework/module-ref/model/drive.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Drive::component = enums::Component::Drive;
const enums::CollectionName Drive::collection_name =
    enums::CollectionName::Drives;

Drive::Drive(const std::string& parent_uuid) :
    BlockDevice{"", parent_uuid, enums::Component::StorageController} {}

Drive::~Drive() {}

Json::Value Drive::to_json() const {
    Json::Value result;
    result[literals::Drive::STATUS] = get_status().to_json();
    result[literals::Drive::INTERFACE] = get_interface();
    result[literals::Drive::TYPE] = get_type();
    result[literals::Drive::RPM] = get_rpm();
    result[literals::Drive::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Drive::CAPACITY] = get_capacity_gb();
    result[literals::Drive::FRU_INFO] = get_fru_info().to_json();
    result[literals::Drive::PHYSICAL_ID] = get_physical_id();
    result[literals::Drive::OEM] = get_oem().to_json();
    return result;
}

Drive Drive::from_json(const Json::Value& json) {
    Drive drive;

    drive.set_status(attribute::Status::from_json(json[literals::Drive::STATUS]));
    drive.set_interface(json[literals::Drive::INTERFACE]);
    drive.set_type(json[literals::Drive::TYPE]);
    drive.set_firmware_version(json[literals::Drive::FIRMWARE_VERSION]);
    drive.set_rpm(json[literals::Drive::RPM]);
    drive.set_capacity_gb(json[literals::Drive::CAPACITY]);
    drive.set_fru_info(attribute::FruInfo::from_json(
        json[literals::Drive::FRU_INFO]));
    drive.set_physical_id(json[literals::Drive::PHYSICAL_ID]);
    drive.set_oem(attribute::Oem::from_json(json[literals::Drive::OEM]));
    drive.set_resource_hash(compute_hash(json));

    return drive;
}
