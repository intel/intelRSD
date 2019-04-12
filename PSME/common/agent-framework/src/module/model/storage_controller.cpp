/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "agent-framework/module/model/storage_controller.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StorageController::component =
    enums::Component::StorageController;
const enums::CollectionName StorageController::collection_name =
    enums::CollectionName::StorageControllers;

StorageController::StorageController(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

StorageController::~StorageController() {}

json::Json StorageController::to_json() const {
    json::Json result = json::Json();
    result[literals::StorageController::STATUS] = get_status().to_json();
    result[literals::StorageController::FRU_INFO] = get_fru_info().to_json();
    result[literals::StorageController::PHYSICAL_ID] = get_physical_id();
    result[literals::StorageController::SKU] = get_sku();
    result[literals::StorageController::ASSET_TAG] = get_asset_tag();
    result[literals::StorageController::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::StorageController::SPEED_GBPS] = get_speed_gbps();
    result[literals::StorageController::SUPPORTED_CONTROLLER_PROTOCOLS] = get_supported_controller_protocols().to_json();
    result[literals::StorageController::SUPPORTED_DEVICE_PROTOCOLS] = get_supported_device_protocols().to_json();
    result[literals::StorageController::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::StorageController::COLLECTIONS] = get_collections().to_json();
    result[literals::StorageController::OEM] = get_oem().to_json();
    return result;
}

StorageController StorageController::from_json(const json::Json& json) {
    StorageController controller;

    controller.set_status(attribute::Status::from_json(json[literals::StorageController::STATUS]));
    controller.set_fru_info(attribute::FruInfo::from_json(json[literals::StorageController::FRU_INFO]));
    controller.set_collections(Collections::from_json(json[literals::StorageController::COLLECTIONS]));
    controller.set_physical_id(json[literals::StorageController::PHYSICAL_ID]);
    controller.set_asset_tag(json[literals::StorageController::ASSET_TAG]);
    controller.set_sku(json[literals::StorageController::SKU]);
    controller.set_firmware_version(json[literals::StorageController::FIRMWARE_VERSION]);
    controller.set_speed_gbps(json[literals::StorageController::SPEED_GBPS]);
    controller.set_identifiers(Identifiers::from_json(json[literals::StorageController::IDENTIFIERS]));
    controller.set_supported_controller_protocols(StorageProtocols::from_json(
        json[literals::StorageController::SUPPORTED_CONTROLLER_PROTOCOLS]));
    controller.set_supported_device_protocols(StorageProtocols::from_json(
        json[literals::StorageController::SUPPORTED_DEVICE_PROTOCOLS]));

    controller.set_oem(attribute::Oem::from_json(json[literals::StorageController::OEM]));

    return controller;
}
