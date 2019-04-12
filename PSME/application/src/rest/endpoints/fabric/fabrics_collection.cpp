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
 * */

#include "psme/rest/endpoints/fabric/fabrics_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#FabricCollection.FabricCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#FabricCollection.FabricCollection";
    r[Common::DESCRIPTION] = "Fabric Collection";
    r[Common::NAME] = "Fabric Collection";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}


FabricsCollection::FabricsCollection(const std::string& path) : EndpointBase(path) {}


FabricsCollection::~FabricsCollection() {}


void FabricsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto fabric_ids = agent_framework::module::get_manager<agent_framework::model::Fabric>().get_ids();

    json[Collection::ODATA_COUNT] = std::uint32_t(fabric_ids.size());

    for (const auto& id : fabric_ids) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }
    set_response(res, json);
}
