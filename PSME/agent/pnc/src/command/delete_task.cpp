/*!
 * @brief Registers GetTasksInfo command in pnc agent
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
 * @file delete_task.cpp
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

void delete_task(const DeleteTask::Request& req, DeleteTask::Response&) {
    log_debug("pnc-agent", "Delete task " << req.get_task());
    get_manager<Task>().remove_entry(req.get_task());
}

}


REGISTER_COMMAND(DeleteTask, delete_task);
