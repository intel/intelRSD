/*!
 * @brief SystemDcpmemSecureErase endpoint definition
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file system_dcpmem_secure_erase.cpp
 */


#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "psme/rest/endpoints/system/system_dcpmem_secure_erase.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/system_dcpmem_secure_erase.hpp"
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

static const std::map <std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::System::RESET_CONFIGURATION, constants::System::RESET_CONFIGURATION},
    {agent_framework::model::literals::System::ERASE_CONFIGURATION_KEYS, constants::System::ERASE_CONFIGURATION_KEYS}
};

void check_response_errors(const agent_framework::model::responses::SetComponentAttributes& response) {
    const auto& result_statuses = response.get_statuses();
    if (!result_statuses.empty()) {
        const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
            result_statuses, gami_to_rest_attributes);
        throw error::ServerException(error);
    }
}

void check_dcpmem_presence(const agent_framework::model::System& system) {
    auto dcpmem_modules = agent_framework::module::get_manager<agent_framework::model::Memory>()
        .get_entries(system.get_uuid(), [](const agent_framework::model::Memory& memory) -> bool {
            return memory.get_memory_type() == enums::MemoryType::IntelOptane;
        });
    if (dcpmem_modules.empty()) {
        // TODO replace invalid_payload_error with new appropriate error type
        throw error::ServerException(
            error::ErrorFactory::create_invalid_payload_error(
                "System does not support Secure Erase operation on non Intel Optane DC Persistent Memory.")
        );
    }
}

}

endpoint::SystemDcpmemSecureErase::SystemDcpmemSecureErase(const std::string& path) : EndpointBase(path) {}


endpoint::SystemDcpmemSecureErase::~SystemDcpmemSecureErase() {}


void endpoint::SystemDcpmemSecureErase::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostEraseOptaneDCPersistentMemory";
    const auto& json = JsonValidator::validate_request_body<schema::SystemDcpmemSecureErasePostSchema>(request);

    auto system = model::find<agent_framework::model::System>(request.params).get();
    check_dcpmem_presence(system); // Action allowed only on System with Intel Optane DC Persistent Memory populated
    auto agent_id = system.get_agent_id();

    agent_framework::model::attribute::Attributes attributes{};

    if (json.count(constants::System::RESET_CONFIGURATION)) {
        attributes.set_value(agent_framework::model::literals::System::RESET_CONFIGURATION,
                             json[constants::System::RESET_CONFIGURATION]);
    }

    if (json.count(constants::System::ERASE_CONFIGURATION_KEYS)) {
        attributes.set_value(agent_framework::model::literals::System::ERASE_CONFIGURATION_KEYS,
                             json[constants::System::ERASE_CONFIGURATION_KEYS]);
    }

    if (!attributes.empty()) {
        const auto& set_component_attributes_request = requests::SetComponentAttributes{system.get_uuid(), attributes};

        auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_dcpmem_attributes = [&, gami_agent] {
            const auto& set_component_attributes_response =
                gami_agent->execute<responses::SetComponentAttributes>(set_component_attributes_request);

            ::check_response_errors(set_component_attributes_response);

            const auto task_uuid = set_component_attributes_response.get_task();
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

            auto completion_notifier = [system, agent_id](const std::string& set_task_uuid) {
                auto lambda_agent_manager = psme::core::agent::AgentManager::get_instance();

                {
                    auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                        .get_entry_reference(set_task_uuid);
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

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_dcpmem_attributes);
    }

    response.set_status(server::status_2XX::ACCEPTED);
}
