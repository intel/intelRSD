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

#include "psme/rest/endpoints/fabric/ports_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#PortCollection.PortCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#PortCollection.PortCollection";
    r[Common::DESCRIPTION] = "Collection of PCIe Ports";
    r[Common::NAME] = "PCIe Port Collection";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}


PortsCollection::PortsCollection(const std::string& path) : EndpointBase(path) {}


PortsCollection::~PortsCollection() {}


void PortsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto switch_uuid =
        psme::rest::model::Find<agent_framework::model::Switch>
            (req.params[PathParam::SWITCH_ID]).via<agent_framework::model::Fabric>
            (req.params[PathParam::FABRIC_ID]).get_uuid();

    auto port_ids = agent_framework::module::get_manager<agent_framework::model::Port>().get_ids(switch_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(port_ids.size());

    for (const auto& port_id : port_ids) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(port_id).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}
