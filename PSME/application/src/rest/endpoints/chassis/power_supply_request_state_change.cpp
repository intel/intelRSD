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

#include "agent-framework/module/constants/rmm.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/chassis/power.hpp"
#include "psme/rest/endpoints/chassis/power_supply_request_state_change.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/request_state_change.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Psu::REQUESTED_STATE, constants::Common::STATE}
};


void fill_post_attributes(const json::Json& post, agent_framework::model::attribute::Attributes& attributes) {
    if (post.count(constants::Common::STATE) && post[constants::Common::STATE].is_string()) {
        const auto state = post[constants::Common::STATE].get<std::string>();
        if (!JsonValidator::validate_allowable_values(endpoint::Power::get_state_allowable_values(), state)) {
            throw error::ServerException(
                error::ErrorFactory::create_error_for_not_allowable_value(
                    "The PowerSupply does not support the requested state target.",
                    Common::STATE, state,
                    endpoint::PathBuilder(constants::Common::OEM)
                        .append(constants::Common::RACKSCALE)
                        .append(constants::Common::ACTIONS)
                        .append(constants::PowerZone::HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE)
                        .append(constants::PowerZone::STATE_ALLOWABLE_VALUES)
                        .build()
                )
            );
        }

        attributes.set_value(agent_framework::model::literals::Psu::REQUESTED_STATE, state);
    }
}


void check_response_errors(const agent_framework::model::responses::SetComponentAttributes& response) {
    const auto& result_statuses = response.get_statuses();
    if (!result_statuses.empty()) {
        const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
            result_statuses, ::gami_to_rest_attributes);
        throw error::ServerException(error);
    }
}


void update_model_after_post(psme::core::agent::JsonAgentSPtr agent,
                              const agent_framework::model::Chassis& chassis) {
    psme::rest::model::handler::HandlerManager::get_instance()->
        get_handler(agent_framework::model::enums::Component::Chassis)->
        load(agent, chassis.get_parent_uuid(), agent_framework::model::enums::Component::Manager,
             chassis.get_uuid(), true);
}


std::string get_psu_uuid_via_chassis(const agent_framework::model::Chassis& chassis, const json::Json& json) {
    auto power_zones = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
        .get_keys(chassis.get_uuid());

    const auto& member_id = json[Common::MEMBER_ID].get<std::string>();
    if (power_zones.size() == 0) {
        // Power Zones as a resource are not related to Redfish resource
        // so if there is no PowerZones collection, it means there are no PSUs
        throw agent_framework::exceptions::NotFound(
            "Could not find PowerSupply with MemberId '" + member_id +
            "' in Power object of the Chassis " + std::to_string(chassis.get_id()) + ".");
    }

    auto psus = agent_framework::module::get_manager<agent_framework::model::Psu>()
        .get_keys(power_zones.front(), [&member_id](const agent_framework::model::Psu& p) {
            return std::to_string(p.get_id()) == member_id;
        });
    if (psus.size() == 0) {
        throw error::ServerException(
            error::ErrorFactory::create_error_for_not_allowable_value(
                "Could not find PowerSupply with '" + member_id + "' MemberId.",
                Common::MEMBER_ID, member_id,
                endpoint::PathBuilder(constants::Common::OEM)
                    .append(constants::Common::RACKSCALE)
                    .append(constants::Common::ACTIONS)
                    .append(constants::PowerZone::HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE)
                    .append(constants::PowerZone::MEMBER_ID_ALLOWABLE_VALUES)
                    .build()
            )
        );
    }
    if (psus.size() > 1) {
        throw error::ServerException(
            error::ErrorFactory::create_conflict_error("More than one PowerSupply with '" +
                                                       member_id + "' MemberId found!"));
    }
    return psus.front();
}

}


endpoint::PowerSupplyRequestStateChange::PowerSupplyRequestStateChange(const std::string& path) : EndpointBase(path) {}


endpoint::PowerSupplyRequestStateChange::~PowerSupplyRequestStateChange() {}


void endpoint::PowerSupplyRequestStateChange::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostPowerSupplyRequestStateChange";

    auto chassis = model::find<agent_framework::model::Chassis>(request.params).get();

    const auto& json = JsonValidator::validate_request_body<schema::RequestStateChangePostSchema>(request);
    const auto& psu_uuid = ::get_psu_uuid_via_chassis(chassis, json);

    agent_framework::model::attribute::Attributes attributes{};
    ::fill_post_attributes(json, attributes);

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes gami_request{psu_uuid, attributes};
        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(chassis.get_agent_id());

        auto set_psu_attributes = [&, gami_agent] {
            // Call set component attribute method
            auto gami_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(gami_request);

            ::check_response_errors(gami_response);
            ::update_model_after_post(gami_agent, chassis);
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_psu_attributes);
    }

    response.set_status(server::status_2XX::NO_CONTENT);
}
