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
 *
 * @file requests/common/get_managers_collection.cpp
 * @brief Common request get manager collection implementation
 * */

#include "agent-framework/module/requests/common/get_managers_collection.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;


GetManagersCollection::GetManagersCollection() {}


json::Json GetManagersCollection::to_json() const {
    json::Json value = json::Json();
    return value;
}


GetManagersCollection GetManagersCollection::from_json(const json::Json&) {
    return GetManagersCollection{};
}
