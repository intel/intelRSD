/*!
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
 *
 * @file responses/common/add_zone.cpp
 * @brief AddZone response
 * */

#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/responses/common/add_zone.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

AddZone::AddZone(const Uuid& zone, const Uuid& task, const attribute::Oem& oem):
    m_zone(zone), m_task(task), m_oem{oem} {}


json::Json AddZone::to_json() const {
    json::Json value = json::Json();
    value[Zone::ZONE] = m_zone;
    if (!m_task.empty()) {
        value[TaskEntry::TASK] = m_task;
    }
    value[Zone::OEM] = m_oem.to_json();
    return value;
}

AddZone AddZone::from_json(const json::Json& json) {
    return AddZone{json[Zone::ZONE],
                   json.value(Zone::TASK, std::string{}),
                   attribute::Oem::from_json(json[Zone::OEM])};
}
