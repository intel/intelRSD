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
 * @file responses/common/add_zone_endpoints.cpp
 * @brief AddZoneEndpoints response
 * */

#include "agent-framework/module/responses/common/add_zone_endpoints.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;


AddZoneEndpoints::AddZoneEndpoints(const std::string& task, const attribute::Oem& oem) :
    m_task{task},
    m_oem{oem} {}


json::Json AddZoneEndpoints::to_json() const {
    json::Json value = json::Json();
    value[Zone::OEM] = m_oem.to_json();
    if (!m_task.empty()) {
        value[TaskEntry::TASK] = m_task;
    }
    return value;
}


AddZoneEndpoints AddZoneEndpoints::from_json(const json::Json& json) {
    if (json.count(TaskEntry::TASK) == 0) {
        return AddZoneEndpoints{{}, attribute::Oem::from_json(json[Zone::OEM])};
    }
    return AddZoneEndpoints{json[TaskEntry::TASK], attribute::Oem::from_json(json[Zone::OEM])};
}
