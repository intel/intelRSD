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

#include "psme/rest/endpoints/fabric/fabric.hpp"
#include "psme/rest/utils/status_helpers.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Fabric.Fabric";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Fabric.v1_0_0.Fabric";
    r[Common::NAME] = "Fabric";
    r[Common::DESCRIPTION] = "Fabric";
    r[Common::ID] = json::Value::Type::NIL;

    r[constants::Fabric::MAX_ZONES] = json::Value::Type::NIL;
    r[constants::Fabric::FABRIC_TYPE] = json::Value::Type::NIL;

    r[constants::Fabric::ZONES] = json::Value::Type::OBJECT;
    r[constants::Fabric::ENDPOINTS] = json::Value::Type::OBJECT;
    r[constants::Fabric::SWITCHES] = json::Value::Type::OBJECT;

    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;
    r[Common::ACTIONS][Common::OEM] = json::Value::Type::OBJECT;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}
}

endpoint::Fabric::Fabric(const std::string& path) : EndpointBase(path) {}

endpoint::Fabric::~Fabric() {}

void endpoint::Fabric::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    const auto fabric =
        psme::rest::model::Find<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID]).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = req.params[PathParam::FABRIC_ID];

    json[constants::Fabric::ZONES][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::ZONES).build();
    json[constants::Fabric::ENDPOINTS][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::ENDPOINTS).build();
    json[constants::Fabric::SWITCHES][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::SWITCHES).build();

    json[constants::Fabric::FABRIC_TYPE] = fabric.get_protocol();
    endpoint::status_to_json(fabric, json);

    set_response(res, json);
}
