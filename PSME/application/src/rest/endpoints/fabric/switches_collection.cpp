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

#include "psme/rest/endpoints/fabric/switches_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#SwitchCollection.SwitchCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#SwitchCollection.SwitchCollection";
    r[Common::DESCRIPTION] = "Collection of Fabric Switches";
    r[Common::NAME] = "Switch Collection";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}


SwitchesCollection::SwitchesCollection(const std::string& path) : EndpointBase(path) {}


SwitchesCollection::~SwitchesCollection() {}


void SwitchesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto fabric_uuid = psme::rest::model::Find<agent_framework::model::Fabric>(
        req.params[PathParam::FABRIC_ID]).get_uuid();

    auto switch_ids = agent_framework::module::get_manager<agent_framework::model::Switch>().get_ids(fabric_uuid);

    json[Collection::ODATA_COUNT] = std::uint32_t(switch_ids.size());

    for (const auto& id : switch_ids) {
        json::Value link;
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}
