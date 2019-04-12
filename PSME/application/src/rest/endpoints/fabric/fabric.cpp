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

#include "psme/rest/endpoints/fabric/fabric.hpp"
#include "psme/rest/utils/status_helpers.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Fabric.Fabric";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Fabric.v1_0_0.Fabric";
    r[Common::NAME] = "Fabric";
    r[Common::DESCRIPTION] = "Fabric";
    r[Common::ID] = json::Json::value_t::null;

    r[constants::Fabric::MAX_ZONES] = json::Json::value_t::null;
    r[constants::Fabric::FABRIC_TYPE] = json::Json::value_t::null;

    r[constants::Fabric::ZONES] = json::Json::value_t::object;
    r[constants::Fabric::ENDPOINTS] = json::Json::value_t::object;
    r[constants::Fabric::SWITCHES] = json::Json::value_t::object;

    r[Common::LINKS][Common::OEM] = json::Json::value_t::object;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.FabricLinks";
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::MANAGED_BY] = json::Json::value_t::array;
    r[Common::ACTIONS][Common::OEM] = json::Json::value_t::object;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[Common::OEM] = json::Json::value_t::object;

    return r;
}
}

endpoint::Fabric::Fabric(const std::string& path) : EndpointBase(path) {}

endpoint::Fabric::~Fabric() {}

void endpoint::Fabric::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    const auto fabric =
        psme::rest::model::find<agent_framework::model::Fabric>(req.params).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = req.params[PathParam::FABRIC_ID];

    json[constants::Fabric::ZONES][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::ZONES).build();
    json[constants::Fabric::ENDPOINTS][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::ENDPOINTS).build();
    json[constants::Fabric::SWITCHES][Common::ODATA_ID] = endpoint::PathBuilder(req).append(constants::Fabric::SWITCHES).build();

    json[constants::Fabric::FABRIC_TYPE] = fabric.get_protocol();
    endpoint::status_to_json(fabric, json);

    json::Json manager = json::Json();
    manager[Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Manager, fabric.get_parent_uuid());
    json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Common::MANAGED_BY].push_back(std::move(manager));

    if (fabric.get_oem_protocol().has_value()) {
        json[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Fabric";
        json[Common::OEM][Common::RACKSCALE][constants::Fabric::FABRIC_TYPE] = fabric.get_oem_protocol();
    }

    set_response(res, json);
}
