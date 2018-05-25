/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
#include "psme/rest/endpoints/chassis/drives_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#DriveCollection.DriveCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#DriveCollection.DriveCollection";
    r[Common::NAME] = "Drive Collection";
    r[Common::DESCRIPTION] = "Collection of Drives";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}


DrivesCollection::DrivesCollection(const std::string& path) : EndpointBase(path) {}


DrivesCollection::~DrivesCollection() {}


void DrivesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    auto chassis_uuid = psme::rest::model::Find<agent_framework::model::Chassis>(
        req.params[PathParam::CHASSIS_ID]).get_uuid();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto keys = agent_framework::module::get_manager<agent_framework::model::Drive>().get_ids(chassis_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}
