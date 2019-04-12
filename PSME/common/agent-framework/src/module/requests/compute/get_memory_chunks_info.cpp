/*!
 * @brief Compute GetMemoryChunksInfo request
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_memory_chunks_info.cpp
 */

#include "agent-framework/module/requests/compute/get_memory_chunks_info.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetMemoryChunksInfo::GetMemoryChunksInfo(const Uuid& memory_chunks): m_memory_chunks{memory_chunks} {}

json::Json GetMemoryChunksInfo::to_json() const {
    json::Json value = json::Json();
    value[MemoryChunks::MEMORY_CHUNKS] = m_memory_chunks;
    return value;
}

GetMemoryChunksInfo GetMemoryChunksInfo::from_json(const json::Json& json) {
    return GetMemoryChunksInfo{json[MemoryChunks::MEMORY_CHUNKS].get<std::string>()};
}