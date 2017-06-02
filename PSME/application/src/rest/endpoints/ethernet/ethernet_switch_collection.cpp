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

#include "psme/rest/endpoints/ethernet/ethernet_switch_collection.hpp"
#include "psme/rest/constants/constants.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] =
        "#EthernetSwitchCollection.EthernetSwitchCollection";
    r[Common::NAME] = "Ethernet Switches Collection";
    r[Common::DESCRIPTION] = "Collection of Ethernet Switches";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

endpoint::EthernetSwitchCollection::EthernetSwitchCollection(const std::string& path) : EndpointBase(path) {}
endpoint::EthernetSwitchCollection::~EthernetSwitchCollection() {}


void endpoint::EthernetSwitchCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto switch_ids = agent_framework::module::NetworkComponents::get_instance()->
        get_switch_manager().get_ids();

    json[Collection::ODATA_COUNT] = std::uint32_t(switch_ids.size());

    for (const auto& id : switch_ids ){
        json::Value link;
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}
