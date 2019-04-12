/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @brief Registers GetTaskResultInfo command in storage agent
 * @file get_task_result_info.cpp
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/action/task_result_manager.hpp"
#include "storage_agent_commands.hpp"

using namespace agent::storage;
using namespace agent_framework::module;
using namespace agent_framework::action;

namespace {

void get_task_result_info(GetTaskResultInfo::ContextPtr,
                          const GetTaskResultInfo::Request& request,
                          GetTaskResultInfo::Response& response) {
    auto task = get_manager<agent_framework::model::Task>().get_entry(request.get_uuid());
    auto task_result_manager = TaskResultManager::get_instance();

    // Sanity check: task result must be present in the task result manager
    if (!task_result_manager->task_exists(task.get_uuid())) {
        THROW(agent_framework::exceptions::InvalidUuid, "storage-agent", "Could not find requested task.");
    }

    auto result = TaskResultManager::get_instance()->get_result(task.get_uuid());
    response.set_result(result);
}

}

REGISTER_STORAGE_COMMAND(GetTaskResultInfo, ::get_task_result_info);
