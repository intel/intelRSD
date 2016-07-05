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

#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"
#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"

#include "psme/rest-ref/constants/constants.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/endpoints/acl_bind.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"

using namespace app::rest;
using namespace app::rest::constants;
using namespace psme::core::agent;
using namespace psme::rest;
using namespace psme::rest::utils;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;

namespace {
json::Value validate_post_request(const server::Request& request) {
    json::Value schema({
        json::Pair(constants::Acl::BIND_PORT, json::Value({
            json::Pair(Common::ODATA_ID, JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
        }))
    });

    return  JsonValidator::validate_request_body(request, schema);
}

std::pair<std::string, std::string> get_acl_and_port_uuid(const server::Request& request) {
    const auto json = validate_post_request(request);
    const auto acl_uuid = NetworkManager::get_instance()->
        get_acl_manager().rest_id_to_uuid(
        endpoint::utils::id_to_uint64(request.params[PathParam::ACL_ID]),
        NetworkManager::get_instance()->
        get_switch_manager().rest_id_to_uuid(
        endpoint::utils::id_to_uint64(request.params[PathParam::ETHERNET_SWITCH_ID])));
    const auto port_uuid = endpoint::utils::get_port_uuid_from_url(
        json[constants::Acl::BIND_PORT][Common::ODATA_ID].as_string());
    return std::make_pair(acl_uuid, port_uuid);
}
}

namespace psme {
namespace rest {
namespace endpoint {

// Action Bind
template<>
void endpoint::AclBind<true>::post(const server::Request& request,
    server::Response& response) {
    const auto acl_port_pair = get_acl_and_port_uuid(request);

    if (NetworkManager::get_instance()->get_acl_port_manager().entry_exists(acl_port_pair.first, acl_port_pair.second)) {
        THROW(agent_framework::exceptions::InvalidParameters, "rest",
              "Port " + std::to_string(NetworkManager::get_instance()->get_port_manager().uuid_to_rest_id(acl_port_pair.second))
              + " is already bound to ACL " + request.params[PathParam::ACL_ID]);
    }

    const requests::AddAclPort add_acl_port_request{
        acl_port_pair.first,
        attribute::Array<std::string>(std::vector<std::string>{acl_port_pair.second}),
        attribute::Oem()
    };

    const auto agent_id = model::Find<agent_framework::model::Switch>
        (request.params[PathParam::ETHERNET_SWITCH_ID]).get().get_agent_id();

    const auto add_acl_port_response = AgentManager::get_instance()
        ->call_method<responses::AddAclPort>(agent_id, add_acl_port_request);

    NetworkManager::get_instance()->get_acl_port_manager().add_entry(acl_port_pair.first, acl_port_pair.second, agent_id);
    response.set_status(server::status_2XX::NO_CONTENT);
}

// Action UnBind
template<>
void endpoint::AclBind<false>::post(const server::Request& request,
    server::Response& response) {
    const auto acl_port_pair = get_acl_and_port_uuid(request);

    if (!NetworkManager::get_instance()->get_acl_port_manager().entry_exists(acl_port_pair.first, acl_port_pair.second)) {
        THROW(agent_framework::exceptions::InvalidParameters, "rest",
              "Port " + std::to_string(NetworkManager::get_instance()->get_port_manager().uuid_to_rest_id(acl_port_pair.second))
              + " is not bound to ACL " + request.params[PathParam::ACL_ID]);
    }

    const requests::DeleteAclPort delete_acl_port_request{
        acl_port_pair.first,
        attribute::Array<std::string>(std::vector<std::string>{acl_port_pair.second}),
        attribute::Oem()
    };

    const auto agent_id = model::Find<agent_framework::model::Switch>
        (request.params[PathParam::ETHERNET_SWITCH_ID]).get().get_agent_id();

    const auto delete_acl_port_response = AgentManager::get_instance()
        ->call_method<responses::DeleteAclPort>(agent_id, delete_acl_port_request);

    NetworkManager::get_instance()->get_acl_port_manager().remove_entry(acl_port_pair.first, acl_port_pair.second);
    response.set_status(server::status_2XX::NO_CONTENT);
}

}
}
}
