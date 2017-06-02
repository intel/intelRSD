/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file responses/common/set_component_attributes.hpp
 * @brief SetComponentAttributes response
 * */

#include "agent-framework/module/responses/common/set_component_attributes.hpp"
#include "agent-framework/module/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

void SetComponentAttributes::add_status(const model::attribute::ResultStatus &status) {
    m_statuses.add_entry(status);
}

Json::Value SetComponentAttributes::to_json() const {
    Json::Value value;
    if (!m_task.empty()) {
        value[TaskEntry::TASK] = m_task;
        value[TaskEntry::OEM] = m_oem.to_json();
    }
    else {
        value = m_statuses.to_json();
    }
    return value;
}

SetComponentAttributes SetComponentAttributes::from_json(const Json::Value& json) {
    if (json.isObject() && json.isMember(TaskEntry::TASK)) {
        return SetComponentAttributes{json[TaskEntry::TASK].asString(),
            attribute::Oem::from_json(json[TaskEntry::OEM])};
    }
    else if (json.isArray()) {
        return SetComponentAttributes{Statuses::from_json(json)};
    }
    else {
        throw std::runtime_error("Invalid setComponentAttributes response format");
    }

}
