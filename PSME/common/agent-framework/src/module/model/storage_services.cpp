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
 * @file storage_services.cpp
 * @brief StorageServices model implementation
 * */

#include "agent-framework/module/model/storage_services.hpp"
#include "agent-framework/module/constants/storage.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StorageServices::component =
    enums::Component::StorageServices;
const enums::CollectionName StorageServices::collection_name =
    enums::CollectionName::StorageServices;

StorageServices::StorageServices(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


StorageServices::~StorageServices() { }


Json::Value StorageServices::to_json() const {
    Json::Value result;
    result[literals::StorageServices::STATUS] = get_status().to_json();
    result[literals::StorageServices::COLLECTIONS] =
        get_collections().to_json();
    result[literals::StorageServices::OEM] = get_oem().to_json();
    return result;
}


StorageServices StorageServices::from_json(const Json::Value& json) {
    StorageServices services;

    services.set_status(attribute::Status::from_json(
        json[literals::StorageServices::STATUS]));
    services.set_collections(Collections::from_json(
        json[literals::StorageServices::COLLECTIONS]));
    services.set_oem(attribute::Oem::from_json(json[literals::StorageServices::OEM]));
    services.set_resource_hash(json);

    return services;
}
