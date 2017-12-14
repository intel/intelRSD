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
 * */

#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/chassis/chassis_collection.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ChassisCollection.ChassisCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ChassisCollection.ChassisCollection";
    r[Common::NAME] = "Chassis Collection";
    r[Common::DESCRIPTION] = "Collection of Chassis";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

ChassisCollection::ChassisCollection(const std::string& path) : EndpointBase(path) {}

ChassisCollection::~ChassisCollection() {}

void ChassisCollection::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();

    auto chassis_ids = agent_framework::module::CommonComponents::get_instance()->get_chassis_manager()
        .get_ids();

    json[Common::ODATA_ID] = PathBuilder(request).build();
    json[Collection::ODATA_COUNT] = std::uint32_t(chassis_ids.size());

    for (const auto& id : chassis_ids) {
        json::Value link{};
        link[Common::ODATA_ID] = PathBuilder(request).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, json);
}
