/*!
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
 *
 * @file storage_subsystem.cpp
 * @brief StorageSubsystem model implementation
 * */

#include "agent-framework/module/model/storage_subsystem.hpp"
#include "agent-framework/module/constants/compute.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StorageSubsystem::component =
    enums::Component::StorageSubsystem;
const enums::CollectionName StorageSubsystem::collection_name =
    enums::CollectionName::StorageSubsystems;


StorageSubsystem::StorageSubsystem(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


StorageSubsystem::~StorageSubsystem() { }


json::Json StorageSubsystem::to_json() const {
    json::Json result = json::Json();
    result[literals::StorageSubsystem::STATUS] = get_status().to_json();
    result[literals::StorageSubsystem::COLLECTIONS] = get_collections().to_json();
    result[literals::StorageSubsystem::OEM] = get_oem().to_json();
    return result;
}


StorageSubsystem StorageSubsystem::from_json(const json::Json& json) {
    StorageSubsystem storage;
    storage.set_status(attribute::Status::from_json(json[literals::StorageSubsystem::STATUS]));
    storage.set_collections(Collections::from_json(json[literals::StorageSubsystem::COLLECTIONS]));
    storage.set_oem(attribute::Oem::from_json(json[literals::StorageSubsystem::OEM]));

    return storage;
}
