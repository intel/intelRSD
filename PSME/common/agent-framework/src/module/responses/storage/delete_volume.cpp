/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file delete_volume.cpp
 * @brief DeleteVolume response
 * */

#include "agent-framework/module/responses/storage/delete_volume.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

DeleteVolume::DeleteVolume(attribute::Oem oem) : m_oem{oem} {}


json::Json DeleteVolume::to_json() const {
    json::Json value = json::Json();
    value[Volume::OEM] = m_oem.to_json();
    if (m_task.has_value()) {
        value[TaskEntry::TASK] = m_task;
    }
    return value;
}

DeleteVolume DeleteVolume::from_json(const json::Json& json) {
    auto delete_response = DeleteVolume{attribute::Oem::from_json(json[Volume::OEM])};
    if (json.count(TaskEntry::TASK)) {
        delete_response.set_task(json[TaskEntry::TASK]);
    }
    return delete_response;
}
