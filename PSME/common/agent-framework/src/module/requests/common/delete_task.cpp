/*!
 * @brief DeleteTask request class implementation
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

#include "agent-framework/module/requests/common/delete_task.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


DeleteTask::DeleteTask(const std::string& task) : m_task{task} { }


json::Json DeleteTask::to_json() const {
    json::Json value = json::Json();
    value[Task::TASK] = m_task;
    return value;
}


DeleteTask DeleteTask::from_json(const json::Json& json) {
    return DeleteTask{json[Task::TASK].get<std::string>()};
}
