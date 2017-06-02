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

#include "psme/rest/endpoints/fabric/port.hpp"
#include "psme/rest/utils/status_helpers.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Port.Port";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Port.v1_0_0.Port";
    r[Common::NAME] = "PCIe Port";

    json::Value actions;
    actions[constants::Port::PORT_RESET][FabricCommon::TARGET] = json::Value::Type::NIL;
    actions[constants::Port::PORT_RESET][FabricCommon::ALLOWABLE_RESET_TYPES] = json::Value::Type::ARRAY;
    actions[Common::OEM] = json::Value::Type::OBJECT;
    r[FabricCommon::ACTIONS] = std::move(actions);

    r[Common::DESCRIPTION] = "PCIe Port description";
    r[Common::ID] = json::Value::Type::NIL;
    r[constants::Port::MAX_SPEED] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Port";
    r[Common::OEM][Common::RACKSCALE][constants::Port::PCIE_CONNECTION_ID] = json::Value::Type::ARRAY;
    r[constants::Port::PORT_ID] = json::Value::Type::NIL;
    r[constants::Port::PORT_PROTOCOL] = json::Value::Type::NIL;
    r[constants::Port::PORT_TYPE] = json::Value::Type::NIL;
    r[constants::Port::CURRENT_SPEED] = json::Value::Type::NIL;

    r[Common::LINKS][constants::Port::ASSOCIATED_ENDPOINTS] = json::Value::Type::ARRAY;
    r[Common::LINKS][constants::Port::CONNECTED_SWITCHES] = json::Value::Type::ARRAY;
    r[Common::LINKS][constants::Port::CONNECTED_SWITCH_PORTS] = json::Value::Type::ARRAY;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[constants::Port::WIDTH] = json::Value::Type::NIL;



    return r;
}
}

endpoint::Port::Port(const std::string& path) : EndpointBase(path) {}

endpoint::Port::~Port() {}

void endpoint::Port::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();


    const auto port = psme::rest::model::Find<agent_framework::model::Port>(req.params[PathParam::PORT_ID])
        .via<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID])
        .via<agent_framework::model::Switch>(req.params[PathParam::SWITCH_ID]).get();

    json[Common::ID] = req.params[PathParam::PORT_ID];

    json[FabricCommon::ACTIONS][constants::Port::PORT_RESET][FabricCommon::TARGET] = endpoint::PathBuilder(req)
        .append(Common::ACTIONS)
        .append(constants::Port::PORT_RESET_ENDPOINT).build();
    for(const auto& allowed_reset_type : port.get_allowed_actions()) {
        json[FabricCommon::ACTIONS][constants::Port::PORT_RESET][FabricCommon::ALLOWABLE_RESET_TYPES].push_back(allowed_reset_type.to_string());
    }

    for (const auto& cable_id : port.get_cables_ids()) {
        json[Common::OEM][Common::RACKSCALE][constants::Port::PCIE_CONNECTION_ID].push_back(cable_id);
    }

    json[constants::Port::PORT_ID] = port.get_port_id();
    json[constants::Port::PORT_PROTOCOL] = port.get_protocol();
    json[constants::Port::PORT_TYPE] = port.get_port_type();

    // Rounding Speed value for metadata compliance
    if (port.get_speed_gbps().has_value()) {
        json[constants::Port::CURRENT_SPEED] = uint64_t(port.get_speed_gbps());
    }
    if (port.get_max_speed_gbps().has_value()) {
        json[constants::Port::MAX_SPEED] = uint64_t(port.get_max_speed_gbps());
    }

    for (const auto& endpoint_uuid : agent_framework::module::PncComponents::get_instance()->get_endpoint_port_manager().get_parents(port.get_uuid())) {
        json::Value endpoint_link;
        endpoint_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Endpoint, endpoint_uuid);
        json[Common::LINKS][constants::Port::ASSOCIATED_ENDPOINTS].push_back(std::move(endpoint_link));
    }


    endpoint::status_to_json(port, json);
    json[Common::STATUS][Common::HEALTH_ROLLUP] = port.get_status().get_health();

    json[constants::Port::WIDTH] = port.get_width();

    set_response(res, json);
}
