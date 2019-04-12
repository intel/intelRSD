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
 * @file requests/common/set_component_attributes.cpp
 * @brief Implementation of SetComponentAttributes request
 * */

#include "agent-framework/module/requests/common/set_component_attributes.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

SetComponentAttributes::SetComponentAttributes(const std::string& component,
            agent_framework::model::attribute::Attributes attributes):
        m_component{component},
        m_attributes{attributes} {}

json::Json SetComponentAttributes::to_json() const {
    json::Json value = json::Json();
    value[Component::COMPONENT] = m_component;
    value[Component::ATTRIBUTES] = m_attributes.to_json();
    return value;
}

SetComponentAttributes SetComponentAttributes::from_json(const json::Json& json) {
    return SetComponentAttributes{
        json[Component::COMPONENT],
        agent_framework::model::attribute::Attributes::from_json(json[Component::ATTRIBUTES])
    };
}
