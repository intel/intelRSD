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
 * @file command/compute/get_memory_info.cpp
 *
 * @brief Compute request get memory information implementation
 * */

#include "agent-framework/module/requests/compute/get_memory_info.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetMemoryInfo::GetMemoryInfo(const std::string& memory): m_memory{memory} {}

json::Json GetMemoryInfo::to_json() const {
    json::Json value = json::Json();
    value[Memory::MEMORY] = m_memory;
    return value;
}

GetMemoryInfo GetMemoryInfo::from_json(const json::Json& json) {
    return GetMemoryInfo{json[Memory::MEMORY].get<std::string>()};
}
