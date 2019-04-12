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

#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/system/system_change_tpm_state.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/change_tpm_state.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

using namespace agent_framework::model::enums;
using namespace agent_framework::module;

namespace {

agent_framework::model::TrustedModule get_tpm_by_interface_type(const agent_framework::model::System& system,
                                                                const std::string& interface_type) {
    // If interface was not provided get first instance from TPM container
    if (interface_type.empty()) {
        auto trusted_modules = agent_framework::module::get_manager<agent_framework::model::TrustedModule>()
            .get_entries(system.get_uuid());
        if (trusted_modules.empty()) {
            throw agent_framework::exceptions::InvalidValue(
                "There is no TrustedModules under the System " + std::to_string(system.get_id())
            );
        }
        return trusted_modules.front();
    }
    else {
        auto trusted_modules = agent_framework::module::get_manager<agent_framework::model::TrustedModule>()
            .get_entries(system.get_uuid(), [interface_type](const agent_framework::model::TrustedModule& tpm) -> bool {
                return tpm.get_interface_type() == InterfaceType::from_string(interface_type);
            });
        if (trusted_modules.empty()) {
            throw error::ServerException(
                error::ErrorFactory::create_error_for_not_allowable_value(
                    "System does not support requested interface type target.",
                    constants::System::INTERFACE_TYPE, interface_type,
                    endpoint::PathBuilder(constants::Common::ACTIONS)
                        .append(constants::System::HASH_INTEL_OEM_CHANGE_TPM_STATE)
                        .append(constants::System::ALLOWABLE_INTERFACE_TYPE)
                        .build()
                )
            );
        }
        return trusted_modules.front();
    }
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::TrustedModule::DEVICE_ENABLED, constants::System::DEVICE_ENABLED},
    {agent_framework::model::literals::TrustedModule::CLEAR_OWNERSHIP, constants::System::CLEAR_OWNERSHIP}
};

void check_response_errors(const agent_framework::model::responses::SetComponentAttributes& response) {
    const auto& result_statuses = response.get_statuses();
    if (!result_statuses.empty()) {
        const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
            result_statuses, gami_to_rest_attributes);
        throw error::ServerException(error);
    }
}

}


endpoint::SystemChangeTPMState::SystemChangeTPMState(const std::string& path) : EndpointBase(path) {}


endpoint::SystemChangeTPMState::~SystemChangeTPMState() {}


void endpoint::SystemChangeTPMState::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostSystemChangeTpmState";
    const auto& json = JsonValidator::validate_request_body<schema::ChangeTPMStatePostSchema>(request);

    auto system = model::find<agent_framework::model::System>(request.params).get();
    agent_framework::model::attribute::Attributes attributes{};

    attributes.set_value(agent_framework::model::literals::TrustedModule::DEVICE_ENABLED, json::Json{});
    if (json.count(constants::System::DEVICE_ENABLED)) {
        auto device_enabled = json[constants::System::DEVICE_ENABLED];
        if (device_enabled.is_boolean()) {
            attributes.set_value(agent_framework::model::literals::TrustedModule::DEVICE_ENABLED,
                                 device_enabled.get<bool>());
        }
    }

    if (json.count(constants::System::CLEAR_OWNERSHIP)) {
        attributes.set_value(agent_framework::model::literals::TrustedModule::CLEAR_OWNERSHIP,
                             json[constants::System::CLEAR_OWNERSHIP]
        );
    }

    std::string interface_type{};
    if (json.count(constants::System::INTERFACE_TYPE)) {
        interface_type = json[constants::System::INTERFACE_TYPE].get<std::string>();
    }

    auto tpm = get_tpm_by_interface_type(system, interface_type);
    auto agent_id = tpm.get_agent_id();

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes gami_request{tpm.get_uuid(), attributes};
        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_system_attributes = [&, gami_agent] {
            // Call set component attribute method
            const auto& gami_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(gami_request);

            ::check_response_errors(gami_response);
            // Right now we know that this will be executed asynchronously.
            const auto task_uuid = gami_response.get_task();
            auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Task);
            task_handler->load(gami_agent,
                               "",
                               agent_framework::model::enums::Component::Task,
                               task_uuid,
                               false);

            auto response_renderer = [system, request](json::Json) -> server::Response {
                Response promised_response{};
                promised_response.set_status(server::status_2XX::NO_CONTENT);

                return promised_response;
            };

            auto completion_notifier = [system, agent_id](const std::string& set_tpm_task_uuid) {
                auto lambda_agent_manager = psme::core::agent::AgentManager::get_instance();

                {
                    auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                        .get_entry_reference(set_tpm_task_uuid);
                    if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
                        task->set_messages(task_service_utils::build_success_message());
                    }
                }

                psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                    agent_framework::model::enums::Component::System)->
                    load(lambda_agent_manager->get_agent(agent_id),
                         system.get_parent_uuid(), agent_framework::model::enums::Component::Manager,
                         system.get_uuid(), true);
            };

            MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
            {
                CommonComponents::get_instance()->get_task_manager()
                    .get_entry_reference(task_uuid)->add_completion_notifier(std::bind(completion_notifier, task_uuid));
            }

            std::string task_monitor_url =
                PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                    .append(Monitor::MONITOR).build();
            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_system_attributes);
    }

    response.set_status(server::status_2XX::ACCEPTED);
}
