/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/reset.hpp"
#include "psme/rest/endpoints/chassis/chassis.hpp"
#include "psme/rest/endpoints/chassis/chassis_reset.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Manager::RESET, constants::Common::RESET_TYPE}
};

}


endpoint::ChassisReset::ChassisReset(const std::string& path) : EndpointBase(path) {}


endpoint::ChassisReset::~ChassisReset() {}


void endpoint::ChassisReset::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostChassisReset";

    auto chassis = model::find<agent_framework::model::Chassis>(request.params).get();
    if (agent_framework::model::enums::ChassisType::Drawer != chassis.get_type()) {
        throw agent_framework::exceptions::NotFound("Chassis Reset can be applied only to Drawers.");
    }

    const auto& json = JsonValidator::validate_request_body<schema::ResetPostSchema>(request);
    agent_framework::model::attribute::Attributes attributes{};
    if (json.count(constants::Common::RESET_TYPE) && json[constants::Common::RESET_TYPE].is_string()) {
        const auto& reset_type = json[constants::Common::RESET_TYPE].get<std::string>();
        const auto reset_type_enum = agent_framework::model::enums::ResetType::from_string(reset_type);
        if (!JsonValidator::validate_allowable_values(chassis.get_allowed_reset_actions().get_array(), reset_type_enum)) {
            throw error::ServerException(
                error::ErrorFactory::create_error_for_not_allowable_value(
                    "The Chassis does not support the requested reset target.",
                    Common::RESET_TYPE, reset_type,
                    endpoint::PathBuilder(constants::Common::ACTIONS)
                        .append(constants::Chassis::HASH_CHASSIS_RESET)
                        .append(Common::ALLOWABLE_RESET_TYPES)
                        .build()
                )
            );
        }

        attributes.set_value(agent_framework::model::literals::Chassis::RESET, reset_type);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes reset_request{chassis.get_uuid(), attributes};
        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(chassis.get_agent_id());

        auto set_chassis_attributes = [&, gami_agent] {
            // Call set component attribute method
            auto reset_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(reset_request);

            const auto& result_statuses = reset_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, ::gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Chassis)->
                load(gami_agent, chassis.get_parent_uuid(), agent_framework::model::enums::Component::Manager,
                     chassis.get_uuid(), false);
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_chassis_attributes);
    }

    response.set_status(server::status_2XX::NO_CONTENT);
}
