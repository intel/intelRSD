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

#include "psme/rest-ref/endpoints/acl_collection.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"

using namespace psme::rest;
using namespace psme::core::agent;
using namespace psme::rest::utils;
using namespace app::rest::constants;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
        "Members/__SWITCH_ID__/ACLs";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] =
        "#EthernetSwitchACLCollection.EthernetSwitchACLCollection";
    r[Common::NAME] = "Ethernet Switch Access Control List Collection";
    r[Common::DESCRIPTION] = "Switch Access Control List. Each ACL entry can "
        "be bound to any switch port";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

json::Value validate_post_request(const server::Request& request) {
    // we only accept an empty json object for this POST
    json::Value schema{};
    return psme::rest::utils::JsonValidator::validate_request_body(request, schema);
}
}

endpoint::AclCollection::AclCollection(const std::string& path) :
    Endpoint(path) {}
endpoint::AclCollection::~AclCollection() {}


void endpoint::AclCollection::get(const server::Request& req,
    server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_CONTEXT] = std::regex_replace(
        r[Common::ODATA_CONTEXT].as_string(),
        std::regex("__SWITCH_ID__"),
        req.params[PathParam::ETHERNET_SWITCH_ID]);

    r[Common::ODATA_ID] = PathBuilder(req).build();

    const auto switch_uuid = NetworkManager::get_instance()->
        get_switch_manager().rest_id_to_uuid(
        endpoint::utils::id_to_uint64(req.params[PathParam::ETHERNET_SWITCH_ID]));

    const auto keys =
        agent_framework::module::NetworkManager::get_instance()->
        get_acl_manager().get_ids(switch_uuid);

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, r);
}

void endpoint::AclCollection::post(const server::Request& req,
    server::Response& res) {

    const auto json = validate_post_request(req);

    auto parent_switch = model::Find<agent_framework::model::Switch>
        (req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    const requests::AddAcl add_acl_request{
        parent_switch->get_uuid(),
        attribute::Array<std::string>(),
        attribute::Oem()
    };

    const auto add_acl_response = AgentManager::get_instance()
        ->call_method<responses::AddAcl>(parent_switch->get_agent_id(), add_acl_request);

    const auto acl_id = model::handler::HandlerManager::get_instance()->
            get_handler(enums::Component::Acl)->
            load(AgentManager::get_instance()->get_agent(parent_switch->get_agent_id()),
            parent_switch->get_uuid(), add_acl_response.get_acl(), true);

    endpoint::utils::set_location_header(
            res, req, PathBuilder(req).append(acl_id).build());

    res.set_status(server::status_2XX::CREATED);
}
