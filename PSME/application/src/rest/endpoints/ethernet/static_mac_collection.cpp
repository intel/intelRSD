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

#include "psme/rest/endpoints/ethernet/static_mac_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/static_mac_collection.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"



using namespace psme::rest;
using namespace psme::rest::validators;
using namespace psme::rest::endpoint;
using namespace psme::core::agent;
using namespace psme::rest::constants;
using namespace agent_framework::model;
using NetworkComponents = agent_framework::module::NetworkComponents;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchStaticMACCollection.EthernetSwitchStaticMACCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchStaticMACCollection.EthernetSwitchStaticMACCollection";
    r[Common::NAME] = "Static MAC Collection";
    r[Common::DESCRIPTION] = "Collection of Static MACs";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}


StaticMacCollection::StaticMacCollection(const std::string& path) : EndpointBase(path) {}


StaticMacCollection::~StaticMacCollection() {}


void StaticMacCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto port_uuid =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
            req.params).get_uuid();

    const auto keys = NetworkComponents::get_instance()->
        get_static_mac_manager().get_ids(port_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}


void endpoint::StaticMacCollection::post(const server::Request& req, server::Response& res) {
    static const constexpr char TRANSACTION_NAME[] = "PostStaticMacCollection";

    auto parent_port = model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
        req.params).get();

    const auto json = JsonValidator::validate_request_body<schema::StaticMacCollectionPostSchema>(req);

    const requests::AddPortStaticMac add_port_static_mac_request{
        parent_port.get_uuid(),
        json[constants::Common::MAC_ADDRESS].get<std::string>(),
        json.value(constants::StaticMac::VLAN_ID, OptionalField<uint32_t>()),
        attribute::Oem()
    };

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(parent_port.get_agent_id());

    auto add_static_mac = [&, gami_agent] {
        const auto add_port_static_mac_response =
            gami_agent->execute<responses::AddPortStaticMac>(add_port_static_mac_request);

        const auto static_mac_id = model::handler::HandlerManager::get_instance()->
            get_handler(enums::Component::StaticMac)->
            load(gami_agent, parent_port.get_uuid(), enums::Component::EthernetSwitchPort,
                 add_port_static_mac_response.get_static_mac(), true);

        endpoint::utils::set_location_header(req, res, PathBuilder(req).append(static_mac_id).build());

        res.set_status(server::status_2XX::CREATED);
    };
    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_static_mac);
}
