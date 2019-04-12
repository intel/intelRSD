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
 * @file processor_memory.cpp
 * @brief ProcessorMemory class implementation
 * */

#include "agent-framework/module/model/attributes/integrated_memory.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::attribute;

json::Json IntegratedMemory::to_json() const {
    json::Json json = json::Json();

    json[literals::ProcessorMemory::TYPE] = m_type;
    json[literals::ProcessorMemory::CAPACITY_MB] = m_capacity_mb;
    json[literals::ProcessorMemory::SPEED_MHZ] = m_speed_mhz;

    return json;
}

IntegratedMemory IntegratedMemory::from_json(const json::Json& json) {
    IntegratedMemory processor_memory{};

    processor_memory.set_type(json[literals::ProcessorMemory::TYPE]);
    processor_memory.set_capacity_mb(json[literals::ProcessorMemory::CAPACITY_MB]);
    processor_memory.set_speed_mhz(json[literals::ProcessorMemory::SPEED_MHZ]);

    return processor_memory;
}
