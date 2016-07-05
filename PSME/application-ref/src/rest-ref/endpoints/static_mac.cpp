/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/endpoints/static_mac.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"
#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"

using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::endpoint;
using namespace psme::rest::model::handler;
using namespace psme::core::agent;
using namespace app::rest;
using namespace app::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
            "Members/__SWITCH_ID__/Ports/Members/__PORT_ID__/StaticMACs/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchStaticMAC.1.0.0.EthernetSwitchStaticMAC";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Static MAC";
    r[Common::DESCRIPTION] = "Static MAC address rule";

    r[constants::StaticMac::MAC_ADDRESS] = json::Value::Type::NIL;
    r[constants::StaticMac::VLAN_ID] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

json::Value validate_patch_request(const server::Request& request) {
    json::Value schema({
                   json::Pair(constants::StaticMac::MAC_ADDRESS,
                              JsonValidator::type_of(JsonValidator::STRING_TYPE, false)),
                   json::Pair(constants::StaticMac::VLAN_ID,
                              JsonValidator::any_of(JsonValidator::UINT_TYPE, false))
    });

    return JsonValidator::validate_request_body(request, schema);
}
}

endpoint::StaticMac::StaticMac(const std::string& path) : Endpoint(path) {}
endpoint::StaticMac::~StaticMac() {}

void endpoint::StaticMac::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_CONTEXT] = std::regex_replace(
            json[Common::ODATA_CONTEXT].as_string(),
            std::regex("__SWITCH_ID__"),
            req.params[PathParam::ETHERNET_SWITCH_ID]);

    json[Common::ODATA_CONTEXT] = std::regex_replace(
            json[Common::ODATA_CONTEXT].as_string(),
            std::regex("__PORT_ID__"),
            req.params[PathParam::SWITCH_PORT_ID]);

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = req.params[PathParam::STATIC_MAC_ID];
    json[Common::NAME] = constants::StaticMac::STATIC_MAC + req.params[PathParam::STATIC_MAC_ID];

    const auto static_mac = psme::rest::model::Find<agent_framework::model::StaticMac>(req.params[PathParam::STATIC_MAC_ID])
            .via<agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID])
            .get();

    json[constants::StaticMac::MAC_ADDRESS] = static_mac.get_address();
    json[constants::StaticMac::VLAN_ID] = static_mac.get_vlan_id();

    set_response(res, json);
}

void endpoint::StaticMac::del(const server::Request& req, server::Response& res) {
   auto static_mac = psme::rest::model::Find<agent_framework::model::StaticMac>(
           req.params[PathParam::STATIC_MAC_ID])
           .via<agent_framework::model::Switch>(
                   req.params[PathParam::ETHERNET_SWITCH_ID])
           .via<agent_framework::model::SwitchPort>(
                   req.params[PathParam::SWITCH_PORT_ID])
           .get_one();

   auto delete_port_static_mac_request = requests::DeletePortStaticMac(static_mac->get_uuid());

   // try removing Static MAC from agent's model
   AgentManager::get_instance()->call_method<responses::DeletePortStaticMac>(static_mac->get_agent_id(), delete_port_static_mac_request);

   // remove the resource from application's model
   HandlerManager::get_instance()->get_handler(enums::Component::StaticMac)->remove(static_mac->get_uuid());

    res.set_status(server::status_2XX::NO_CONTENT);
}

void endpoint::StaticMac::patch(const server::Request& req, server::Response& res) {
    const auto static_mac = psme::rest::model::Find<agent_framework::model::StaticMac>
                                                (req.params[PathParam::STATIC_MAC_ID])
                                                .via<agent_framework::model::Switch>
                                                (req.params[PathParam::ETHERNET_SWITCH_ID])
                                                .via<agent_framework::model::SwitchPort>
                                                (req.params[PathParam::SWITCH_PORT_ID])
                                                .get();

    const auto json = validate_patch_request(req);

    attribute::Attributes attributes{};
    if (json.is_member(constants::StaticMac::MAC_ADDRESS)) {
        attributes.set_value(literals::StaticMac::ADDRESS, json[constants::StaticMac::MAC_ADDRESS].as_string());
    }

    if (json.is_member(constants::StaticMac::VLAN_ID)) {
        if(json[constants::StaticMac::VLAN_ID].is_null()) {
            attributes.set_value(literals::StaticMac::VLAN_ID, Json::Value::null);
        }
        else {
            attributes.set_value(literals::StaticMac::VLAN_ID, json[constants::StaticMac::VLAN_ID].as_uint());
        }
    }

    requests::SetComponentAttributes set_component_attributes_request{static_mac.get_uuid(), attributes};
    AgentManager::get_instance()->call_method<responses::SetComponentAttributes>(static_mac.get_agent_id(),
                                                                                 set_component_attributes_request);
    HandlerManager::get_instance()->get_handler(enums::Component::StaticMac)->
            load(AgentManager::get_instance()->get_agent(static_mac.get_agent_id()),
                 static_mac.get_parent_uuid(),
                 static_mac.get_uuid(),
                 true);

    res.set_status(server::status_2XX::NO_CONTENT);
}
