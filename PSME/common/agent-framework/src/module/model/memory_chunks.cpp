/*!
 * @brief Memory Chunks model interface
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
 * @file memory_chunks.cpp
 */

#include "agent-framework/module/model/memory_chunks.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component MemoryChunks::component = enums::Component::MemoryChunks;
const enums::CollectionName MemoryChunks::collection_name = enums::CollectionName::MemoryChunks;

MemoryChunks::MemoryChunks(const Uuid& parent_uuid, enums::Component parent_type) :
                           Resource{parent_uuid, parent_type} {}

MemoryChunks::~MemoryChunks() {}

json::Json MemoryChunks::to_json() const {
    json::Json result = json::Json();
    result[literals::MemoryChunks::STATUS] = get_status().to_json();
    result[literals::MemoryChunks::NAME] = get_name();
    result[literals::MemoryChunks::DESCRIPTION] = get_description();
    result[literals::MemoryChunks::MEMORY_CHUNK_SIZE_MIB] = get_memory_chunk_size_mib();
    result[literals::MemoryChunks::ADDRESS_RANGE_TYPE] = get_address_range_type();
    result[literals::MemoryChunks::IS_MIRROR_ENABLED] = is_mirror_enabled();
    result[literals::MemoryChunks::IS_SPARE] = is_spare();
    result[literals::MemoryChunks::INTERLEAVE_SETS] = get_interleave_sets().to_json();
    result[literals::MemoryChunks::OEM] = get_oem().to_json();
    return result;
}

MemoryChunks MemoryChunks::from_json(const json::Json& json) {
    using namespace agent_framework::model::attribute;
    MemoryChunks memory_chunks{};
    memory_chunks.set_status(attribute::Status::from_json(json[literals::MemoryChunks::STATUS]));
    memory_chunks.set_name(json[literals::MemoryChunks::NAME]);
    memory_chunks.set_description(json[literals::MemoryChunks::DESCRIPTION]);
    memory_chunks.set_memory_chunk_size_mib(json[literals::MemoryChunks::MEMORY_CHUNK_SIZE_MIB]);
    memory_chunks.set_address_range_type(json[literals::MemoryChunks::ADDRESS_RANGE_TYPE]);
    memory_chunks.set_mirror_enabled(json[literals::MemoryChunks::IS_MIRROR_ENABLED]);
    memory_chunks.set_spare(json[literals::MemoryChunks::IS_SPARE]);
    memory_chunks.set_interleave_sets(attribute::Array<attribute::InterleaveSet>::from_json(json[literals::MemoryChunks::INTERLEAVE_SETS]));
    memory_chunks.set_oem(Oem::from_json(json[literals::MemoryChunks::OEM]));
    return memory_chunks;
}