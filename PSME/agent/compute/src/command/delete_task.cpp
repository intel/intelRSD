/*!
 * @brief Registers GetTasksInfo command in compute agent
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
 * @file get_task_info.cpp
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/compute_commands.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

[[ noreturn ]] void delete_task(const DeleteTask::Request& , DeleteTask::Response&) {
    THROW(agent_framework::exceptions::NotImplemented, "rpc", "Requested command is not implemented");
}

}


REGISTER_COMMAND(DeleteTask, delete_task);
