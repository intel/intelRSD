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

#include "psme/rest/endpoints/ethernet/acl.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"



using namespace psme::rest;
using namespace psme::rest::model::handler;
using namespace psme::core::agent;
using namespace psme::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model;
using NetworkComponents = agent_framework::module::NetworkComponents;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchACL.EthernetSwitchACL";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchACL.v1_0_0.EthernetSwitchACL";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "ACL";

    r[Common::DESCRIPTION] = "Access Control List";
    r[Common::OEM] = json::Json::value_t::object;
    r[constants::Acl::RULES] = json::Json::value_t::object;

    r[Common::LINKS][constants::Acl::BOUND_PORTS] = json::Json::value_t::array;
    r[Common::LINKS][Common::OEM] = json::Json::value_t::object;

    json::Json actions = json::Json();
    std::string hash_bind_acl = constants::Common::HASH + std::string(constants::Acl::BIND_ACL);
    actions[hash_bind_acl][constants::Common::TARGET] = json::Json::value_t::null;
    actions[hash_bind_acl][constants::Acl::PORT_ALLOWABLE_VALUES] = json::Json::value_t::array;

    std::string hash_unbind_acl = constants::Common::HASH + std::string(constants::Acl::UNBIND_ACL);
    actions[hash_unbind_acl][constants::Common::TARGET] = json::Json::value_t::null;
    actions[hash_unbind_acl][constants::Acl::PORT_ALLOWABLE_VALUES] = json::Json::value_t::array;
    r[Common::ACTIONS] = std::move(actions);

    return r;
}


void add_bound_ports_links(json::Json& json, const std::string& acl_uuid) {
    const auto bound_ports = NetworkComponents::get_instance()->
        get_acl_port_manager().get_children(acl_uuid);

    for (const auto& bound_port : bound_ports) {
        try {
            json::Json link = json::Json();
            link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                enums::Component::EthernetSwitchPort, bound_port);
            json[Common::LINKS][constants::Acl::BOUND_PORTS].
                push_back(std::move(link));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error("rest", "Model Acl/BoundPorts link error");
            log_error("rest", "Port " << bound_port <<
                                      " is present in the BoundPorts table but it does not exist"
                                      " as a resource");
        }
    }
}
}


endpoint::Acl::Acl(const std::string& path) : EndpointBase(path) {}


endpoint::Acl::~Acl() {}


void endpoint::Acl::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::ACL_ID];
    r[Common::NAME] = constants::Acl::ACL + req.params[PathParam::ACL_ID];

    r[constants::Acl::RULES][Common::ODATA_ID] =
        PathBuilder(req).append(constants::Acl::RULES).build();

    const auto acl = psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::Acl>(
        req.params).get();

    ::add_bound_ports_links(r, acl.get_uuid());

    // acl bind action
    std::string hash_bind_acl = constants::Common::HASH + std::string(constants::Acl::BIND_ACL);
    r[constants::Common::ACTIONS][hash_bind_acl][constants::Common::TARGET] =
        PathBuilder(req)
            .append(constants::Common::ACTIONS)
            .append(constants::Acl::BIND_ACL).build();

    // allowable values for binding are all the unbound ports
    auto is_not_bound_predicate =
        [&acl](const agent_framework::model::EthernetSwitchPort& port) {
            return false == NetworkComponents::get_instance()->
                get_acl_port_manager().entry_exists(acl.get_uuid(), port.get_uuid());
        };

    const auto unbound_ports = NetworkComponents::get_instance()->get_port_manager()
        .get_keys(acl.get_parent_uuid(), is_not_bound_predicate);

    for (const auto& unbound_port : unbound_ports) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = endpoint::utils::get_component_url(enums::Component::EthernetSwitchPort, unbound_port);
        r[constants::Common::ACTIONS][hash_bind_acl][constants::Acl::PORT_ALLOWABLE_VALUES].push_back(std::move(link));
    }

    // acl unbind action
    std::string hash_unbind_acl = constants::Common::HASH + std::string(constants::Acl::UNBIND_ACL);
    r[constants::Common::ACTIONS][hash_unbind_acl][constants::Common::TARGET] =
        PathBuilder(req)
            .append(constants::Common::ACTIONS)
            .append(constants::Acl::UNBIND_ACL)
            .build();

    // allowable values for unbinding are all the bound ports
    r[constants::Common::ACTIONS][hash_unbind_acl][constants::Acl::PORT_ALLOWABLE_VALUES] =
        r[Common::LINKS][constants::Acl::BOUND_PORTS];

    set_response(res, r);
}


void endpoint::Acl::del(const server::Request& req, server::Response& res) {
    static const constexpr char TRANSACTION_NAME[] = "DeleteAcl";

    auto acl = psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::Acl>(
        req.params).get();

    auto delete_acl_request = requests::DeleteAcl(acl.get_uuid());
    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(acl.get_agent_id());

    auto delete_acl = [&, gami_agent] {
        // try removing port from agent's model
        gami_agent->execute<responses::DeleteAcl>(delete_acl_request);

        // remove the resource from application's model
        HandlerManager::get_instance()->get_handler(enums::Component::Acl)->remove(acl.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, delete_acl);
}
