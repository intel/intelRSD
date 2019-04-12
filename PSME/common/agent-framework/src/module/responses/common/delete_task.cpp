/*!
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
 * @file responses/common/set_component_attributes.cpp
 *
 * @brief DeleteTask response class implementation
 *
 * */

#include "agent-framework/module/responses/common/delete_task.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::responses;


json::Json DeleteTask::to_json() const {
    return json::Json();
}


DeleteTask DeleteTask::from_json(const json::Json&) {
    return DeleteTask();
}
