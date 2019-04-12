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
 *
 * @file request/common/get_collection.cpp
 *
 * @brief Common request get collection implementation
 * */

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/requests/common/get_collection.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetCollection::GetCollection(const std::string& component,
        const std::string& name): m_component{component}, m_name{name} {}

json::Json GetCollection::to_json() const {
    json::Json value = json::Json();
    value[Collection::COMPONENT] = m_component;
    value[Collection::NAME] = m_name;
    return value;
}

GetCollection GetCollection::from_json(const json::Json& json) {
    return GetCollection(
        json[Collection::COMPONENT].get<std::string>(),
        json[Collection::NAME].get<std::string>()
    );
}
