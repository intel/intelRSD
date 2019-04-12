/*!
 * @brief GetTasksCollection class implementation
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
 * @file get_task_collection.cpp
 * */

#include "agent-framework/module/requests/common/get_tasks_collection.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;


GetTasksCollection::GetTasksCollection() { }


json::Json GetTasksCollection::to_json() const {
    json::Json json = json::Json();
    return json;
}


GetTasksCollection GetTasksCollection::from_json(const json::Json&) {
    return GetTasksCollection{};
}
