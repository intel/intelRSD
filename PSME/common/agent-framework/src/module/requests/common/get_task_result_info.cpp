/*!
 * @brief GetTaskResultInfo class implementation
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
 * @file get_task_result_info.cpp
 * */

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/requests/common/get_task_result_info.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


GetTaskResultInfo::GetTaskResultInfo(const std::string& task) : m_task{task} { }


json::Json GetTaskResultInfo::to_json() const {
    json::Json value = json::Json();
    value[Task::TASK] = m_task;
    return value;
}


GetTaskResultInfo GetTaskResultInfo::from_json(const json::Json& json) {
    return GetTaskResultInfo{json[Task::TASK].get<std::string>()};
}
