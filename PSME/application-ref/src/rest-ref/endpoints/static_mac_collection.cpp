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

#include "psme/rest-ref/endpoints/static_mac_collection.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"

using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::endpoint;
using namespace psme::core::agent;
using namespace app::rest::constants;
using namespace app::rest;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;


namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
            "Members/__SWITCH_ID__/Ports/Members/__PORT_ID__/StaticMACs";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchStaticMACCollection.EthernetSwitchStaticMACCollection";
    r[Common::NAME] = "Static MAC Collection";
    r[Common::DESCRIPTION] = "Collection of Static MACs";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

json::Value validate_post_request(const server::Request& request) {
    json::Value schema({
                   json::Pair(constants::StaticMac::MAC_ADDRESS,
                              JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
                   json::Pair(constants::StaticMac::VLAN_ID,
                              JsonValidator::any_of(JsonValidator::UINT_TYPE, false))
    });

    return JsonValidator::validate_request_body(request, schema);
}
}

StaticMacCollection::StaticMacCollection(const std::string& path) : Endpoint(path) {}
StaticMacCollection::~StaticMacCollection() {}

void StaticMacCollection::get(const server::Request& req, server::Response& res) {
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

    const auto port_uuid = NetworkManager::get_instance()->
            get_port_manager().rest_id_to_uuid(
            endpoint::utils::id_to_uint64(req.params[PathParam::SWITCH_PORT_ID]),
            NetworkManager::get_instance()->get_switch_manager().rest_id_to_uuid(
            endpoint::utils::id_to_uint64(req.params[PathParam::ETHERNET_SWITCH_ID])));

    const auto keys = NetworkManager::get_instance()->
            get_static_mac_manager().get_ids(port_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}

void endpoint::StaticMacCollection::post(const server::Request& req, server::Response& res) {

    auto parent_port = model::Find<agent_framework::model::SwitchPort>
                            (req.params[PathParam::SWITCH_PORT_ID])
                            .via<agent_framework::model::Switch>
                            (req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    const auto json = validate_post_request(req);

    const requests::AddPortStaticMac add_port_static_mac_request{
           parent_port->get_uuid(),
           json[constants::StaticMac::MAC_ADDRESS].as_string(),
           json.is_member(constants::StaticMac::VLAN_ID) ? json[constants::StaticMac::VLAN_ID].as_uint() : Json::Value::null,
           attribute::Oem()
    };

    const auto add_port_static_mac_response = AgentManager::get_instance()
           ->call_method<responses::AddPortStaticMac>(parent_port->get_agent_id(), add_port_static_mac_request);

    const auto static_mac_id = model::handler::HandlerManager::get_instance()->
           get_handler(enums::Component::StaticMac)->
           load(AgentManager::get_instance()->get_agent(parent_port->get_agent_id()),
                parent_port->get_uuid(), add_port_static_mac_response.get_static_mac(), true);

    endpoint::utils::set_location_header(res, req, PathBuilder(req).append(static_mac_id).build());

    res.set_status(server::status_2XX::CREATED);
}
