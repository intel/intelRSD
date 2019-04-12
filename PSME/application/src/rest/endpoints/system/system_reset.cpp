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
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/constants/compute.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/reset.hpp"
#include "psme/rest/endpoints/system/system_reset.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::System::RESET, constants::Common::RESET_TYPE}
};

}


endpoint::SystemReset::SystemReset(const std::string& path) : EndpointBase(path) {}


endpoint::SystemReset::~SystemReset() {}


void endpoint::SystemReset::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostSystemReset";

    // Gets necessary data from model and does not block system reference
    auto system = model::find<agent_framework::model::System>(request.params).get();

    const auto& agent_id = system.get_agent_id();
    const auto& system_uuid = system.get_uuid();

    const auto& json = JsonValidator::validate_request_body<schema::ResetPostSchema>(request);
    agent_framework::model::attribute::Attributes attributes{};
    const auto& reset_type = json[constants::Common::RESET_TYPE].get<std::string>();
    attributes.set_value(agent_framework::model::literals::System::RESET, reset_type);

    agent_framework::model::requests::SetComponentAttributes set_component_attributes_request{system_uuid, attributes};

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto set_system_attributes = [&] {
        // Call set component attribute method
        const auto set_component_attribute_response = gami_agent->
            execute<responses::SetComponentAttributes>(set_component_attributes_request);

        const auto task_uuid = set_component_attribute_response.get_task();
        if (task_uuid.empty()) {
            const auto& result_statuses = set_component_attribute_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, ::gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            response.set_status(server::status_2XX::OK);
        }
        else {
            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(enums::Component::Task)->
                load(gami_agent, {}, enums::Component::Task, task_uuid, false);

            MonitorContentBuilder::get_instance()->add_builder(task_uuid, [system](json::Json) -> server::Response {
                Response promised_response{};
                promised_response.set_status(server::status_2XX::NO_CONTENT);
                return promised_response;
            });

            auto completion_notifier = [system, agent_id](const std::string& set_task_uuid) {
                auto agent_manager = psme::core::agent::AgentManager::get_instance();

                {
                    auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                        .get_entry_reference(set_task_uuid);
                    if (task->get_state() == enums::TaskState::Completed) {
                        task->set_messages(psme::rest::endpoint::task_service_utils::build_success_message());
                    }
                }

                psme::rest::model::handler::HandlerManager::get_instance()->get_handler(enums::Component::System)
                    ->load(agent_manager->get_agent(agent_id),
                           system.get_parent_uuid(), enums::Component::Manager,
                           system.get_uuid(),
                           true);
            };

            {
                CommonComponents::get_instance()->get_task_manager().get_entry_reference(task_uuid)->
                    add_completion_notifier([task_uuid, completion_notifier]() { completion_notifier(task_uuid); });
            }

            std::string task_monitor_url = PathBuilder(
                utils::get_component_url(enums::Component::Task, task_uuid)).append(Monitor::MONITOR).build();

            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());

            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, set_system_attributes);

}
