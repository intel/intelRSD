/*!
 * @brief Provides implementation of task monitor endpoint
 *
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
 *
 * @file monitor.cpp
 * */

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/monitor.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/action/task_result_manager.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;


endpoint::Monitor::Monitor(const std::string& path) : EndpointBase(path) {}


endpoint::Monitor::~Monitor() {}


void endpoint::Monitor::get(const server::Request& request, server::Response& response) {
    auto agent_manager = psme::core::agent::AgentManager::get_instance();
    auto monitored_task = model::find<agent_framework::model::Task>(request.params).get();
    auto agent_id = monitored_task.get_agent_id();
    agent_framework::model::requests::GetTaskResultInfo get_task_result_info_request{monitored_task.get_uuid()};

    // If the task has finished, retrieve its result from the agent, otherwise return 202 Accepted
    if (monitored_task.get_end_time().has_value()) {
        try {
            if (agent_id.empty()) {
                auto task_result = agent_framework::action::TaskResultManager::get_instance()->
                    get_result(monitored_task.get_uuid());
                response = endpoint::MonitorContentBuilder::get_instance()->
                    get_response(monitored_task.get_uuid(), task_result);
            }
            else {
                auto task_result = agent_manager->call_method<agent_framework::model::responses::GetTaskResultInfo>(
                    agent_id, get_task_result_info_request);
                response = endpoint::MonitorContentBuilder::get_instance()->
                    get_response(monitored_task.get_uuid(), task_result.get_result());
            }
        }
        catch (const std::out_of_range&) {
            throw error::ServerException(error::ErrorFactory::create_resource_gone_error(
                request.get_url(), "The requested resource is no longer available on this server."));
        }
    }
    else {
        response.set_status(server::status_2XX::ACCEPTED);
        response.set_body(
            psme::rest::endpoint::task_service_utils::call_task_get(monitored_task.get_uuid()).get_body());
        psme::rest::endpoint::utils::set_location_header(request, response, request.get_url());
    }
}
