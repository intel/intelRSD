/*!
 * @brief GetTaskResult class implementation
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

#include "agent-framework/module/responses/common/get_task_result_info.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::responses;


json::Json GetTaskResultInfo::to_json() const {
    return m_task_result;
}


GetTaskResultInfo GetTaskResultInfo::from_json(const json::Json& json) {
    GetTaskResultInfo result;
    result.set_result(json);
    return result;
}
