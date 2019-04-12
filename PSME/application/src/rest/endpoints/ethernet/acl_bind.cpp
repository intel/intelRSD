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

#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/acl.hpp"
#include "psme/rest/endpoints/ethernet/acl_bind.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::core::agent;
using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using NetworkComponents = agent_framework::module::NetworkComponents;

namespace {

std::pair<std::string, std::string> get_acl_and_port_uuid(const server::Request& request, const json::Json& json) {
    const auto acl_uuid = psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::Acl>(
        request.params).get_uuid();
    const auto port_uuid =
        endpoint::utils::get_port_uuid_from_url(json[constants::Acl::PORT][Common::ODATA_ID].get<std::string>());

    return std::make_pair(acl_uuid, port_uuid);
}
}

namespace psme {
namespace rest {
namespace endpoint {

// Action Bind
template<>
void endpoint::AclBind<true>::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostAclBind";

    const auto json = JsonValidator::validate_request_body<schema::AclPostSchema>(request);
    const auto acl_port_pair = get_acl_and_port_uuid(request, json);

    if (NetworkComponents::get_instance()->get_acl_port_manager().entry_exists(acl_port_pair.first,
                                                                               acl_port_pair.second)) {
        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Port " + std::to_string(
                  NetworkComponents::get_instance()->get_port_manager().uuid_to_rest_id(acl_port_pair.second))
              + " is already bound to ACL " + request.params[PathParam::ACL_ID]);
    }

    const requests::AddAclPort add_acl_port_request{
        acl_port_pair.first,
        attribute::Array<std::string>(std::vector<std::string>{acl_port_pair.second}),
        attribute::Oem()
    };

    const auto parent_switch =
        model::find<agent_framework::model::EthernetSwitch>(request.params).get();
    const auto agent_id = parent_switch.get_agent_id();
    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto add_acl_port = [&, gami_agent] {
        const auto add_acl_port_response = gami_agent->execute<responses::AddAclPort>(add_acl_port_request);

        model::handler::HandlerManager::get_instance()->
            get_handler(enums::Component::Acl)->
            load(gami_agent, parent_switch.get_uuid(),
                 enums::Component::EthernetSwitch, acl_port_pair.first, true);

        NetworkComponents::get_instance()->get_acl_port_manager().add_entry(acl_port_pair.first, acl_port_pair.second,
                                                                            agent_id);
        response.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_acl_port);
}


// Action UnBind
template<>
void endpoint::AclBind<false>::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostAclUnbind";
    const auto json = JsonValidator::validate_request_body<schema::AclPostSchema>(request);
    const auto acl_port_pair = get_acl_and_port_uuid(request, json);

    if (!NetworkComponents::get_instance()->get_acl_port_manager().entry_exists(acl_port_pair.first,
                                                                                acl_port_pair.second)) {
        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Port " + std::to_string(
                  NetworkComponents::get_instance()->get_port_manager().uuid_to_rest_id(acl_port_pair.second))
              + " is not bound to ACL " + request.params[PathParam::ACL_ID]);
    }

    const requests::DeleteAclPort delete_acl_port_request{
        acl_port_pair.first,
        attribute::Array<std::string>(std::vector<std::string>{acl_port_pair.second}),
        attribute::Oem()
    };

    const auto parent_switch =
        model::find<agent_framework::model::EthernetSwitch>(request.params).get();
    const auto agent_id = parent_switch.get_agent_id();
    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto delete_acl_port = [&, gami_agent] {
        const auto delete_acl_port_response = gami_agent->execute<responses::DeleteAclPort>(delete_acl_port_request);

        model::handler::HandlerManager::get_instance()->get_handler(enums::Component::Acl)->
            load(gami_agent, parent_switch.get_uuid(), enums::Component::EthernetSwitch, acl_port_pair.first, true);

        NetworkComponents::get_instance()->get_acl_port_manager().remove_entry(acl_port_pair.first,
                                                                               acl_port_pair.second);
        response.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, delete_acl_port);
}

}
}
}
