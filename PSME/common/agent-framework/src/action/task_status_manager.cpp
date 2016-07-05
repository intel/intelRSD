/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/action/task_status_manager.hpp"
#include "logger/logger_factory.hpp"

using namespace agent_framework::action;

std::once_flag TaskStatusManager::m_once_flag{};

namespace {
TaskStatusManager* g_task_runner = nullptr;
}

void
TaskStatusManager::add_status(const std::string& uuid, const Status status) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_status.emplace(uuid, status);
}

void
TaskStatusManager::add_status(const std::string& uuid, const bool status) {
    status ? add_status(uuid, TaskStatusManager::Status::Success)
           : add_status(uuid, TaskStatusManager::Status::Fail);
}

TaskStatusManager::Status
TaskStatusManager::get_status(const std::string& uuid) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    const auto elem = m_status.find(uuid);
    if (elem != m_status.cend()) {
        return elem->second;
    }
    return TaskStatusManager::Status::NotFound;
}

void
TaskStatusManager::remove_status(const std::string& uuid) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_status.erase(uuid);
}

TaskStatusManager& TaskStatusManager::get_instance() {
    std::call_once(m_once_flag, &TaskStatusManager::init);
    return *g_task_runner;
}

void TaskStatusManager::cleanup() {
    if (g_task_runner) {
        delete g_task_runner;
        g_task_runner = nullptr;
    }
}

void TaskStatusManager::init() {
    try {
        g_task_runner = new TaskStatusManager;
    } catch (const std::exception& e) {
        log_error(GET_LOGGER("threading"), e.what());
    }
}

