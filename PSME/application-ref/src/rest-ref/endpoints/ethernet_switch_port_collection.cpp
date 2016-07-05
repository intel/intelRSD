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

#include "psme/rest-ref/endpoints/ethernet_switch_port_collection.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/utils/lag_utils.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"
#include "agent-framework/module-ref/model/switch_port.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include <regex>

using namespace psme::rest;
using namespace psme::rest::error;
using namespace psme::rest::endpoint;
using namespace psme::rest::utils;
using namespace app::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/Members/__SWITCH_ID__/Ports";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchPortCollection.EthernetSwitchPortCollection";
    r[Common::NAME] = "Ethernet Switch Port Collection";
    r[Common::DESCRIPTION] = "Collection of Ethernet Switch Ports";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

json::Value validate_post_request(const server::Request& request) {
    using namespace agent_framework::model::enums;
    using json::Pair;

    json::Value port_modes{json::Value::Type::ARRAY};
    for(const auto& value : PortMode::get_values()) {
        port_modes.push_back(value);
    }

    json::Value schema({
        Pair(SwitchPort::PORT_ID,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
        Pair(SwitchPort::PORT_MODE,
            JsonValidator::any_of(JsonValidator::STRING_TYPE, port_modes, true)),
        Pair(Common::LINKS, json::Value({
            Pair(SwitchPort::PORT_MEMBERS,
                JsonValidator::type_of(JsonValidator::ARRAY_TYPE, true))
        }))
    });

    return JsonValidator::validate_request_body(request, schema);
}
}

EthernetSwitchPortCollection::EthernetSwitchPortCollection(const std::string& path) : Endpoint(path) {}
EthernetSwitchPortCollection::~EthernetSwitchPortCollection() {}

void EthernetSwitchPortCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto eth_switch = psme::rest::model::Find<agent_framework::model::Switch>(
        req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                     std::regex("__SWITCH_ID__"), std::to_string(eth_switch->get_id()));
    auto keys = agent_framework::module::NetworkManager::get_instance()->
        get_port_manager().get_ids(eth_switch->get_uuid());

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}

void EthernetSwitchPortCollection::post(const server::Request& req, server::Response& res) {
    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;
    using namespace agent_framework::model::enums;

    auto json = validate_post_request(req);

    auto port_members = LagUtils::get_port_members(json);
    LagUtils::validate_port_members(port_members);

    auto parent = psme::rest::model::Find<agent_framework::model::Switch>(
            req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();
    auto switch_uuid = parent->get_uuid();

    agent_framework::model::requests::AddEthernetSwitchPort add_port_request{
        switch_uuid,
        json[SwitchPort::PORT_ID].as_string(),
        agent_framework::model::enums::PortMode::from_string(
            json[SwitchPort::PORT_MODE].as_string()),
        port_members,
        agent_framework::model::attribute::Oem()
    };

    auto agent_manager = AgentManager::get_instance();

    auto add_port_response =
            agent_manager->call_method
            <agent_framework::model::responses::AddEthernetSwitchPort>(
            parent->get_agent_id(), add_port_request);

    HandlerManager::get_instance()->
        get_handler(agent_framework::model::enums::Component::SwitchPort)->
        load(agent_manager->get_agent(parent->get_agent_id()),
            switch_uuid, add_port_response.get_port(), true);

    // Update our information about member ports and their PortVlans
    for (const auto& member : port_members) {
        HandlerManager::get_instance()->get_handler(Component::SwitchPort)->
                load(agent_manager->get_agent(parent->get_agent_id()), switch_uuid, member, false);
        HandlerManager::get_instance()->get_handler(Component::SwitchPort)->
                load_collection(agent_manager->get_agent(parent->get_agent_id()), member, CollectionType::PortVlans, false);
    }

    auto created_port = agent_framework::module::NetworkManager::get_instance()->
                get_port_manager().get_entry_reference(add_port_response.get_port());

    ::psme::rest::endpoint::utils::set_location_header(
            res, req, PathBuilder(req).append(created_port->get_id()).build());
    res.set_status(server::status_2XX::CREATED);
}
