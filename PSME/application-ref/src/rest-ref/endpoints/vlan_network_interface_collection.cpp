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

#include "agent-framework/module-ref/requests/network/add_port_vlan.hpp"
#include "agent-framework/module-ref/responses/network/add_port_vlan.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/vlan_network_interface_collection.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/core/agent/agent_manager.hpp"

using namespace app::rest::constants;
using namespace app::rest;
using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::endpoint;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] =
        "/redfish/v1/$metadata#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] =
        "#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::NAME] = "VLAN Network Interface Collection";
    r[Common::DESCRIPTION] = "Collection of VLAN Network Interfaces";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;
    return r;
}

const json::Value validate_post_request(const server::Request& request) {
    using namespace psme::rest::error;
    using json::Pair;

    const json::Value schema({
        Pair(Common::NAME,
            JsonValidator::type_of(JsonValidator::STRING_TYPE)),
        Pair(Common::OEM, json::Value({
            Pair(Common::RACKSCALE, json::Value({
                Pair(Vlan::TAGGED,
                    JsonValidator::type_of(JsonValidator::BOOL_TYPE, true))
            }))
        })),
        Pair(Vlan::VLAN_ENABLE,
             JsonValidator::type_of(JsonValidator::BOOL_TYPE, false)),
        Pair(Vlan::VLAN_ID,
             JsonValidator::type_of(JsonValidator::UINT_TYPE, true))
    });

    return JsonValidator::validate_request_body(request, schema);
}

}

VlanNetworkInterfaceCollection::VlanNetworkInterfaceCollection(const std::string& path) : Endpoint(path) {}
VlanNetworkInterfaceCollection::~VlanNetworkInterfaceCollection() {}

void VlanNetworkInterfaceCollection::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();

    auto port_uuid =
        psme::rest::model::Find<agent_framework::model::SwitchPort>
        (req.params[PathParam::SWITCH_PORT_ID]).via<agent_framework::model::Switch>
        (req.params[PathParam::ETHERNET_SWITCH_ID]).get_one()->get_uuid();

    auto keys = agent_framework::module::NetworkManager::get_instance()->
                        get_port_vlan_manager().get_ids(port_uuid);

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, r);
}

void VlanNetworkInterfaceCollection::post(const server::Request& req, server::Response& res) {
    auto port = psme::rest::model::Find
                    <agent_framework::model::SwitchPort>
        (req.params[PathParam::SWITCH_PORT_ID]).via<agent_framework::model::Switch>
        (req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    std::string agent_id = port->get_agent_id();
    std::string port_id = port->get_uuid();

    const auto json = validate_post_request(req);

    const auto vlan_id = json[Vlan::VLAN_ID].as_uint();
    const bool tagged = json[Common::OEM][Common::RACKSCALE][Vlan::TAGGED].as_bool();
    const auto oem = agent_framework::model::attribute::Oem{};
    agent_framework::model::requests::AddPortVlan add_port_vlan_req {
            port->get_uuid(),
            json.is_member(Common::NAME) ? json[Common::NAME].as_string() : Json::Value::null,
            vlan_id,
            tagged,
            oem
    };

    const auto& agent_manager = psme::core::agent::AgentManager::get_instance();

    auto response =
        agent_manager->call_method<agent_framework::model::responses::AddPortVlan,
        agent_framework::model::requests::AddPortVlan>(
            agent_id, add_port_vlan_req);

    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::PortVlan)->load(
            agent_manager->get_agent(agent_id), port_id, response.get_port_vlan());

    auto created_vlan =
        agent_framework::module::NetworkManager::get_instance()
            ->get_port_vlan_manager().get_entry(response.get_port_vlan());

    ::psme::rest::endpoint::utils::set_location_header(
        res, req, PathBuilder(req).append(created_vlan.get_id()).build());

    res.set_status(server::status_2XX::CREATED);
}
