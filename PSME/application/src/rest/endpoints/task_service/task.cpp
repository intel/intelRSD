/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/endpoints/task_service/task.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "agent-framework/module/requests/common/delete_task.hpp"
#include "agent-framework/module/responses/common/delete_task.hpp"



using namespace agent_framework::model::enums;
using namespace psme::rest;
using namespace psme::rest::error;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[constants::Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Task.Task";
    r[constants::Common::ODATA_ID] = json::Json::value_t::null;
    r[constants::Common::ODATA_TYPE] = "#Task.v1_0_0.Task";
    r[constants::Common::ID] = json::Json::value_t::null;
    r[constants::Common::NAME] = "Task";
    r[constants::Common::DESCRIPTION] = "Task description";

    r[constants::Task::TASK_STATE] = json::Json::value_t::null;
    r[constants::Task::TASK_STATUS] = json::Json::value_t::null;
    r[constants::Task::MESSAGES] = json::Json::value_t::array;

    return r;
}


void remove_task(const agent_framework::model::Task& task) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;
    auto delete_task_request = agent_framework::model::requests::DeleteTask(task.get_uuid());

    psme::core::agent::AgentManager::get_instance()->
        call_method<agent_framework::model::responses::DeleteTask>(
        task.get_agent_id(), delete_task_request);

    auto task_handler = HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Task);
    task_handler->remove(task.get_uuid());
}

}


endpoint::Task::Task(const std::string& path) : EndpointBase(path) {}


endpoint::Task::~Task() {}


void endpoint::Task::get(const server::Request& request, server::Response& response) {
    json::Json r = make_prototype();
    auto s = psme::rest::model::find<agent_framework::model::Task>(request.params).get();

    r[constants::Common::ODATA_ID] = PathBuilder(request).build();
    r[constants::Common::ID] = request.params[constants::PathParam::TASK_ID];
    r[constants::Task::TASK_STATE] = s.get_state();

    // According to redfish metadata these fields should not be shown if no value is held in them
    if (s.get_end_time().has_value()) {
        r[constants::Task::END_TIME] = s.get_end_time();
    }
    if (s.get_start_time().has_value()) {
        r[constants::Task::START_TIME] = s.get_start_time();
    }

    // in metadata, TaskStatus has values from Health enum
    r[constants::Task::TASK_STATUS] = s.get_status().get_health();

    for (const auto& message : s.get_messages()) {
        json::Json p = json::Json::value_t::object;

        p[constants::Common::ODATA_TYPE] = "#Message.v1_0_0.Message";
        p[constants::MessageObject::MESSAGE_ID] = message.get_message_id();
        p[constants::MessageObject::MESSAGE] = message.get_content();
        p[constants::MessageObject::RELATED_PROPERTIES] = json::Json::value_t::array;
        p[constants::MessageObject::MESSAGE_ARGS] = json::Json::value_t::array;
        p[constants::MessageObject::SEVERITY] = message.get_severity();
        p[constants::MessageObject::RESOLUTION] = message.get_resolution();

        for (const auto& related_property : message.get_related_properties()) {
            p[constants::MessageObject::RELATED_PROPERTIES].push_back(related_property);
        }

        for (const auto& message_arg : message.get_message_args()) {
            p[constants::MessageObject::MESSAGE_ARGS].push_back(message_arg);
        }

        r[constants::Task::MESSAGES].push_back(p);
    }

    set_response(response, r);
}


void endpoint::Task::del(const server::Request& request, server::Response& response) {
    const auto task = psme::rest::model::find<agent_framework::model::Task>(
        request.params).get();
    const auto state = task.get_state();

    switch (state.value()) {
        case TaskState::Interrupted:
        case TaskState::Completed:
        case TaskState::Killed:
        case TaskState::Exception: {
            remove_task(task);
            response.set_status(server::status_2XX::NO_CONTENT);
            break;
        }
        case TaskState::New:
        case TaskState::Starting:
        case TaskState::Running:
        case TaskState::Suspended:
        case TaskState::Pending:
        case TaskState::Stopping:
        case TaskState::Service: {
            throw ServerException(ErrorFactory::create_resource_in_use_error(
                "Could not delete Task with state: " + std::string(state.value().to_string()))
            );
        }
        default: {
            throw ServerException(ErrorFactory::create_internal_error(
                "Unsupported state of Task resource: " + std::string(state.value().to_string())));
        }
    }
}

