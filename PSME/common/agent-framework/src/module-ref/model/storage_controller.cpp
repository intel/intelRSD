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

#include "agent-framework/module-ref/model/storage_controller.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StorageController::component =
    enums::Component::StorageController;
const enums::CollectionName StorageController::collection_name =
    enums::CollectionName::StorageControllers;

StorageController::StorageController(const std::string& parent_uuid) :
    Resource{parent_uuid, enums::Component::System} {}

StorageController::~StorageController() {}

Json::Value StorageController::to_json() const {
    Json::Value result;
    result[literals::StorageController::STATUS] = get_status().to_json();
    result[literals::StorageController::INTERFACE] =
        get_interface();
    result[literals::StorageController::FRU_INFO] = get_fru_info().to_json();
    result[literals::StorageController::COLLECTIONS] =
        get_collections().to_json();
    result[literals::StorageController::PHYSICAL_ID] = get_physical_id();
    result[literals::StorageController::OEM] = get_oem().to_json();
    return result;
}

StorageController StorageController::from_json(const Json::Value& json) {
    StorageController controller;

    controller.set_status(attribute::Status::from_json(
        json[literals::StorageController::STATUS]));
    controller.set_interface(
        json[literals::StorageController::INTERFACE]);
    controller.set_fru_info(attribute::FruInfo::from_json(
        json[literals::StorageController::FRU_INFO]));
    controller.set_collections(Collections::from_json(
        json[literals::StorageController::COLLECTIONS]));
    controller.set_physical_id(
        json[literals::StorageController::PHYSICAL_ID]);
    controller.set_oem(attribute::Oem::from_json(json[literals::StorageController::OEM]));
    controller.set_resource_hash(compute_hash(json));

    return controller;
}
