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
 * @file responses/common/delete_zone.cpp
 * @brief DeleteZone response
 * */

#include "agent-framework/module/responses/common/delete_zone.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

DeleteZone::DeleteZone(const attribute::Oem& oem):
    m_oem{oem} {}


json::Json DeleteZone::to_json() const {
    json::Json value = json::Json();
    value[Zone::OEM] = m_oem.to_json();
    if (!m_task.empty()) {
        value[TaskEntry::TASK] = m_task;
    }
    return value;
}

DeleteZone DeleteZone::from_json(const json::Json& json) {
    DeleteZone delete_zone{attribute::Oem::from_json(json[Zone::OEM])};
    if (json.count(TaskEntry::TASK)) {
        delete_zone.set_task(json[TaskEntry::TASK]);
    }
    return delete_zone;
}
