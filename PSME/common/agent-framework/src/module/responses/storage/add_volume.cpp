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
 *
 * @file add_volume.cpp
 * @brief AddVolume response
 * */

#include "agent-framework/module/responses/storage/add_volume.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;


AddVolume::AddVolume(const Uuid& volume, const Uuid& task, const attribute::Oem& oem) :
    m_volume{volume},
    m_task{task},
    m_oem{oem} { }


json::Json AddVolume::to_json() const {
    json::Json value = json::Json();
    if (!m_task.empty()) {
        value[TaskEntry::TASK] = m_task;
    }
    else {
        value[Volume::VOLUME] = m_volume;
    }

    value[Volume::OEM] = m_oem.to_json();
    return value;
}


AddVolume AddVolume::from_json(const json::Json& json) {
    return AddVolume{
        json.value(Volume::VOLUME, std::string{}),
        json.value(TaskEntry::TASK, std::string{}),
        attribute::Oem::from_json(json[Volume::OEM])
    };
}
