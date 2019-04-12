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

#include "agent-framework/module/utils/time.hpp"
#include "configuration/configuration.hpp"
#include "psme/rest/endpoints/task_service/task_service.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#TaskService.TaskService";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#TaskService.v1_0_0.TaskService";
    r[Common::ID] = "TaskService";
    r[Common::NAME] = "Task Service";
    r[Common::STATUS][Common::STATE] = "Enabled";
    r[Common::STATUS][Common::HEALTH] = "OK";

    r[TaskService::DATETIME] = "";
    r[TaskService::COMPLETED_TASK_OVERWRITE_POLICY] = "Manual";
    r[TaskService::LIFE_CYCLE_EVENT_ON_TASK_STATE_CHANGE] = true;
    r[TaskService::SERVICE_ENABLED] = true;
    r[TaskService::TASKS] = json::Json::value_t::object;
    r[TaskService::TASKS][Common::ODATA_ID] = "/redfish/v1/TaskService/Tasks";

    return r;
}
}

namespace psme {
namespace rest {
namespace endpoint {

TaskService::TaskService(const std::string& path) : EndpointBase(path), service_enabled(false) {}


TaskService::~TaskService() { }


void TaskService::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[psme::rest::constants::TaskService::DATETIME] = agent_framework::utils::make_iso_8601_timestamp();

    set_response(res, r);
}

}
}
}
