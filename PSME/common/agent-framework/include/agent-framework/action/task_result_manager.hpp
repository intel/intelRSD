/*!
 * @brief TaskResultManager class definition
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"
#include "agent-framework/generic/singleton.hpp"

#include <json/json.h>

#include <map>
#include <mutex>
#include <string>
#include <tuple>



namespace agent_framework {
namespace action {

/*!
 * This class acts as a registry of task results.
 * */
class TaskResultManager : public agent_framework::generic::Singleton<TaskResultManager> {
public:
    ~TaskResultManager();


    // The first parameter is a task response, second is an exception flag, third is an associated exception object
    // (only classes derived from agent_framework::exceptions::ExceptionBase are supported)
    using TaskResultValue = std::tuple<Json::Value, bool, agent_framework::exceptions::GamiException>;
    using TaskResultMap = std::map<std::string, TaskResultValue>;


    /*!
     * Get task result.
     *
     * @param[in] task_uuid Task UUID
     *
     * @return Json::Value object representing task result
     * */
    const Json::Value get_result(const std::string& task_uuid);


    /*!
     * Set task result.
     *
     * @param[in] task_uuid Task UUID
     * @param[in] result Json::Value object representing task result
     * */
    void set_result(const std::string& task_uuid, Json::Value result);


    /*!
     * Set task result to exception state.
     *
     * @param[in] task_uuid Task UUID
     * @param[in] exception Associated exception object
     * */
    void set_exception(const std::string& task_uuid, const agent_framework::exceptions::GamiException& exception);


    /*!
     * Check if a task result exists in the result manager
     *
     * @param[in] task_uuid Task UUID
     *
     * @return True if task result exists, false otherwise
     * */
    bool task_exists(const std::string& task_uuid) const;


private:
    TaskResultMap m_results{};
    mutable std::recursive_mutex m_resource_mutex{};
};

}
}
