/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "agent-framework/module/constants/compute.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/reset.hpp"
#include "psme/rest/endpoints/system/system_reset.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;


endpoint::SystemReset::SystemReset(const std::string& path) : EndpointBase(path) {}


endpoint::SystemReset::~SystemReset() {}


void endpoint::SystemReset::post(const server::Request& request, server::Response& response) {

    std::string agent_id{};
    std::string system_uuid{};
    std::string parent_uuid{};

    // Gets necessary data from model and does not block system reference
        auto system = model::Find<agent_framework::model::System>(request.params[PathParam::SYSTEM_ID]).get();

        agent_id = system.get_agent_id();
        system_uuid = system.get_uuid();
        parent_uuid = system.get_parent_uuid();

    const auto& json = JsonValidator::validate_request_body<schema::ResetPostSchema>(request);
    agent_framework::model::attribute::Attributes attributes{};

    if (json.is_member(constants::Common::RESET_TYPE)) {
        const auto& reset_type = json[constants::Common::RESET_TYPE].as_string();
        attributes.set_value(agent_framework::model::literals::System::POWER_STATE, reset_type);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes
            set_component_attributes_request{system_uuid, attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_system_attributes = [&, gami_agent] {
            // Call set component attribute method
            gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(
                set_component_attributes_request);

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::System)->
                load(gami_agent, parent_uuid, agent_framework::model::enums::Component::Manager,
                     system_uuid, false);
        };

        gami_agent->execute_in_transaction(set_system_attributes);
    }

    response.set_status(server::status_2XX::NO_CONTENT);
}
