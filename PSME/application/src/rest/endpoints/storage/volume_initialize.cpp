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
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/endpoints/storage/volume_initialize.hpp"
#include "psme/rest/validators/schemas/volume.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Volume::INITIALIZATION, constants::Swordfish::INITIALIZE_TYPE}
};

}


endpoint::VolumeInitialize::VolumeInitialize(const std::string& path) : EndpointBase(path) {}


endpoint::VolumeInitialize::~VolumeInitialize() {}


void endpoint::VolumeInitialize::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostVolumeInitialize";

    // validate request
    const auto& json = JsonValidator::validate_request_body<schema::VolumeInitializePostSchema>(request);

    // get volume and agent
    auto volume = psme::rest::model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(
        request.params).get();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(volume.get_agent_id());

    // prepare set component attributes request
    agent_framework::model::attribute::Attributes attributes{};
    std::string value = agent_framework::model::enums::VolumeInitializationType(
        agent_framework::model::enums::VolumeInitializationType::Slow).to_string();
    if (json.count(constants::Swordfish::INITIALIZE_TYPE)) {
        value = json[constants::Swordfish::INITIALIZE_TYPE].get<std::string>();
    }
    attributes.set_value(agent_framework::model::literals::Volume::INITIALIZATION, value);

    agent_framework::model::requests::SetComponentAttributes set_component_attributes{volume.get_uuid(), attributes};

    // prepare response which will be presented on task completion
    auto response_renderer = [](json::Json) -> server::Response {
        server::Response res;
        res.set_status(server::status_2XX::NO_CONTENT);
        return res;
    };

    // reload volume resource on task completion
    auto update_on_completion = [volume](const Uuid& task_uuid) {
        auto agent = psme::core::agent::AgentManager::get_instance()->get_agent(volume.get_agent_id());
        {
            auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                .get_entry_reference(task_uuid);
            if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
                task->set_messages(task_service_utils::build_success_message());
            }
        }

        psme::rest::model::handler::HandlerManager::get_instance()
            ->get_handler(agent_framework::model::enums::Component::Volume)
            ->load(agent, volume.get_parent_uuid(),
                   agent_framework::model::enums::Component::StorageService, volume.get_uuid(), false);
    };

    auto initialize = [&, gami_agent] {
        // Initialize transaction - send GAMI request, then re-load Volume resource or load received Task resource
        auto set_component_attributes_response =
            gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes);

        if (set_component_attributes_response.get_task().empty()) {
            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, ::gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()
                ->get_handler(agent_framework::model::enums::Component::Volume)
                ->load(gami_agent, volume.get_parent_uuid(),
                       agent_framework::model::enums::Component::StorageService, volume.get_uuid(), false);

            response.set_status(server::status_2XX::NO_CONTENT);
        }
        else {
            const auto task_uuid = set_component_attributes_response.get_task();

            auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()
                ->get_handler(agent_framework::model::enums::Component::Task);
            task_handler->load(gami_agent,
                               "",
                               agent_framework::model::enums::Component::Task,
                               task_uuid,
                               false);
            MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
            {
                agent_framework::module::get_manager<agent_framework::model::Task>().get_entry_reference(task_uuid)
                    ->add_completion_notifier(std::bind(update_on_completion, task_uuid));
            }

            std::string task_monitor_url =
                PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                    .append(Monitor::MONITOR).build();
            psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
            response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            response.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, initialize);
}
