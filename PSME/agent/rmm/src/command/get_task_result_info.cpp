/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/command/get_task_result_info.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/action/task_result_manager.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::action;
using namespace agent_framework::exceptions;

namespace {

void get_task_result_info(const GetTaskResultInfo::Request& req, GetTaskResultInfo::Response& rsp) {
    auto& task_manager = get_manager<Task>();
    auto task_result_manager = TaskResultManager::get_instance();
    auto task = task_manager.get_entry(req.get_uuid());

    // Sanity check: task result must be present in the task result manager
    if (!task_result_manager->task_exists(task.get_uuid())) {
        THROW(InvalidUuid, "rmm-agent", "Could not find requested task.");
    }

    auto result = TaskResultManager::get_instance()->get_result(task.get_uuid());
    rsp.set_result(result);
}

}

REGISTER_COMMAND(GetTaskResultInfo, get_task_result_info);
