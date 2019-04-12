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

#include "psme/rest/endpoints/ethernet/ethernet_switch_port_collection.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/ethernet_switch_port_collection.hpp"
#include "psme/rest/utils/lag_utils.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include <regex>



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint;
using namespace psme::rest::error;
using namespace psme::rest::utils;
using namespace psme::rest::validators;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchPortCollection.EthernetSwitchPortCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchPortCollection.EthernetSwitchPortCollection";
    r[Common::NAME] = "Ethernet Switch Port Collection";
    r[Common::DESCRIPTION] = "Collection of Ethernet Switch Ports";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


EthernetSwitchPortCollection::EthernetSwitchPortCollection(const std::string& path) : EndpointBase(path) {}


EthernetSwitchPortCollection::~EthernetSwitchPortCollection() {}


void EthernetSwitchPortCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto switch_uuid =
        psme::rest::model::find<agent_framework::model::EthernetSwitch>(req.params).get_uuid();

    auto keys = agent_framework::module::NetworkComponents::get_instance()->
        get_port_manager().get_ids(switch_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}


void EthernetSwitchPortCollection::post(const server::Request& req, server::Response& res) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;
    static const constexpr char TRANSACTION_NAME[] = "PostEthernetSwitchPortCollection";

    auto json = JsonValidator::validate_request_body<schema::EthernetSwitchPortCollectionPostSchema>(req);

    auto port_members = LagUtils::get_port_members(json);
    LagUtils::validate_port_members(port_members);

    auto parent = psme::rest::model::find<agent_framework::model::EthernetSwitch>(
        req.params).get();
    auto switch_uuid = parent.get_uuid();

    requests::AddEthernetSwitchPort add_port_request{
        switch_uuid,
        json[constants::EthernetSwitchPort::PORT_ID].get<std::string>(),
        agent_framework::model::enums::PortMode::from_string(
            json[constants::EthernetSwitchPort::PORT_MODE].get<std::string>()),
        port_members,
        agent_framework::model::attribute::Oem()
    };

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(parent.get_agent_id());

    auto add_port = [&, gami_agent] {
        auto add_port_response =
            gami_agent->execute<responses::AddEthernetSwitchPort>(add_port_request);

        HandlerManager::get_instance()->
            get_handler(enums::Component::EthernetSwitchPort)->
            load(gami_agent, switch_uuid, enums::Component::EthernetSwitch, add_port_response.get_port(), true);

        auto created_port = agent_framework::module::NetworkComponents::get_instance()->
            get_port_manager().get_entry(add_port_response.get_port());

        // Update our information about member ports and their PortVlans
        for (const auto& member : port_members) {
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load(gami_agent,
                     switch_uuid,
                     enums::Component::EthernetSwitch,
                     member,
                     false);
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load_collection(gami_agent,
                                member,
                                enums::Component::EthernetSwitchPort,
                                enums::CollectionType::EthernetSwitchPortVlans,
                                false);
        }

        ::psme::rest::endpoint::utils::set_location_header(
            req, res, PathBuilder(req).append(created_port.get_id()).build());

        res.set_status(server::status_2XX::CREATED);
    };
    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_port);
}
