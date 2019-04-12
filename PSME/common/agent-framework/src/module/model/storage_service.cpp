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
 * @file storage_service.cpp
 * @brief StorageService model implementation
 * */

#include "agent-framework/module/model/storage_service.hpp"
#include "agent-framework/module/constants/storage.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StorageService::component = enums::Component::StorageService;
const enums::CollectionName StorageService::collection_name = enums::CollectionName::StorageServices;


StorageService::StorageService(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


StorageService::~StorageService() {}


json::Json StorageService::to_json() const {
    json::Json result = json::Json();
    result[literals::StorageService::STATUS] = get_status().to_json();
    result[literals::StorageService::COLLECTIONS] = get_collections().to_json();
    result[literals::StorageService::OEM] = get_oem().to_json();
    return result;
}


StorageService StorageService::from_json(const json::Json& json) {
    StorageService service{};

    service.set_status(attribute::Status::from_json(json[literals::StorageService::STATUS]));
    service.set_collections(Collections::from_json(json[literals::StorageService::COLLECTIONS]));
    service.set_oem(attribute::Oem::from_json(json[literals::StorageService::OEM]));

    return service;
}
