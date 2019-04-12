/*!
 * @brief Implementation of TaskResultManager class
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
 * @file task_result_manager.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/action/task_result_manager.hpp"
#include "agent-framework/exceptions/not_found.hpp"



namespace agent_framework {
namespace action {

TaskResultManager::~TaskResultManager() { }


const json::Json TaskResultManager::get_result(const std::string& task_uuid) {
    std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};

    // Verify that a given task exists and throw an exception if there is no such task
    if (!task_exists(task_uuid)) {
        THROW(agent_framework::exceptions::InvalidUuid, "action",
              "Cannot find requested task in the task manager.");
    }

    TaskResultValue result_value = m_results.at(task_uuid);

    // Check if error flag is set; if so throw the corresponding exception object, otherwise return the result
    if (std::get<1>(result_value)) {
        throw std::get<2>(result_value);
    }
    else {
        return std::get<0>(result_value);
    }
}


void TaskResultManager::set_result(const std::string& task_uuid, json::Json result) {
    std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};

    // The corresponding exception object must be created, use a dummy one
    m_results.insert({task_uuid, std::make_tuple(result, false, agent_framework::exceptions::GamiException(
        agent_framework::exceptions::ErrorCode::UNKNOWN_ERROR, ""))});
}


void TaskResultManager::set_exception(const std::string& task_uuid,
                                      const agent_framework::exceptions::GamiException& exception) {
    std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};

    m_results.insert({task_uuid, std::make_tuple(json::Json{}, true, exception)});
}


bool TaskResultManager::task_exists(const std::string& task_uuid) const {
    std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};

    if (m_results.end() == m_results.find(task_uuid)) {
        return false;
    }
    else {
        return true;
    }
}

}
}
