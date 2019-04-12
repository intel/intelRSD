/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
*/

#include "agent-framework/action/task_runner.hpp"
#include "logger/logger_factory.hpp"

using namespace agent_framework::action;

std::once_flag TaskRunner::m_once_flag{};

namespace {
TaskRunner* g_task_runner = nullptr;
}

TaskRunner& TaskRunner::get_instance() {
    std::call_once(m_once_flag, &TaskRunner::init);
    return *g_task_runner;
}

void TaskRunner::cleanup() {
    if (g_task_runner) {
        delete g_task_runner;
        g_task_runner = nullptr;
    }
}

void TaskRunner::init() {
    try {
        g_task_runner = new TaskRunner;
    } catch (const std::exception& e) {
        log_error("threading", e.what());
    }
}

