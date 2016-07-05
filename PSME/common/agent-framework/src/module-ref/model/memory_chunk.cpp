/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file memory_chunk.cpp
 * @brief Memory Chunk class
 *
 * */

#include "agent-framework/module-ref/model/memory_chunk.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"


using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component MemoryChunk::component = enums::Component::MemoryChunk;
const enums::CollectionName MemoryChunk::collection_name =
    enums::CollectionName::MemoryChunks;

MemoryChunk::MemoryChunk(const std::string& parent) : Resource{parent} {}

MemoryChunk::~MemoryChunk() {}

Json::Value MemoryChunk::to_json() const {
    Json::Value result;
    result[literals::MemoryChunk::STATUS] = get_status().to_json();
    result[literals::MemoryChunk::CHUNK_NAME] = get_chunk_name();
    result[literals::MemoryChunk::CHUNK_ID] = get_chunk_id();
    result[literals::MemoryChunk::CHUNK_SIZE] = get_chunk_size_mb();
    result[literals::MemoryChunk::TYPE] = get_type();
    result[literals::MemoryChunk::MIRRORED] = get_mirrored();
    result[literals::MemoryChunk::SPARE] = get_spare();
    result[literals::MemoryChunk::INTERLEAVE_SETS] = get_interleave_sets().to_json();
    result[literals::MemoryChunk::OEM] = get_oem().to_json();
    return result;
}

MemoryChunk MemoryChunk::from_json(const Json::Value& json) {
    MemoryChunk chunk;
    chunk.set_status(
        attribute::Status::from_json(json[literals::MemoryChunk::STATUS]));
    chunk.set_chunk_name(json[literals::MemoryChunk::CHUNK_NAME]);
    chunk.set_chunk_id(json[literals::MemoryChunk::CHUNK_ID]);
    chunk.set_chunk_size_mb(json[literals::MemoryChunk::CHUNK_SIZE]);
    chunk.set_type(json[literals::MemoryChunk::TYPE]);
    chunk.set_mirrored(json[literals::MemoryChunk::MIRRORED]);
    chunk.set_spare(json[literals::MemoryChunk::SPARE]);
    chunk.set_interleave_sets(
        InterleaveSets::from_json(json[literals::MemoryChunk::INTERLEAVE_SETS]));
    chunk.set_oem(attribute::Oem::from_json(json[literals::MemoryChunk::OEM]));
    return chunk;
}
