/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/storage_pool_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::validators;
using namespace psme::rest::constants;

using namespace agent_framework::module;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StoragePools";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#StoragePoolCollection.StoragePoolCollection";
    r[Common::NAME] = "StoragePools Collection";
    r[Common::DESCRIPTION] = "Collection of Storage Pools";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

}


StoragePoolCollection::StoragePoolCollection(const std::string& path) : EndpointBase(path) {}


StoragePoolCollection::~StoragePoolCollection() {}


void StoragePoolCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto storage_service_uuid =
        psme::rest::model::Find<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get_uuid();

    auto keys = agent_framework::module::get_manager<agent_framework::model::StoragePool>().get_ids(storage_service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link(json::Value::Type::OBJECT);
        link[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}

