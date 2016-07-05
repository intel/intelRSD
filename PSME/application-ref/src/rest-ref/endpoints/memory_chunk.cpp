/*!
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
 * */

#include "psme/rest-ref/endpoints/memory_chunk.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include <regex>

using namespace psme::rest;
using namespace app::rest;
using namespace app::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/MemoryChunks/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#MemoryChunk.1.0.0.MemoryChunk";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Memory Chunk";
    r[Common::DESCRIPTION] = "Memory Chunk description";
    r[MemoryChunk::MEMORY_CHUNK_NAME] = json::Value::Type::NIL;
    r[MemoryChunk::MEMORY_CHUNK_UID] = json::Value::Type::NIL;
    r[MemoryChunk::MEMORY_CHUNK_SIZE_MIB] = json::Value::Type::NIL;
    r[MemoryChunk::ADDRESS_RANGE_TYPE] = json::Value::Type::NIL;
    r[MemoryChunk::IS_MIRROR_ENABLED] = json::Value::Type::NIL;
    r[MemoryChunk::IS_SPARE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[MemoryChunk::INTERLEAVE_SETS] = json::Value::Type::ARRAY;

    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}
}

endpoint::MemoryChunk::MemoryChunk(const std::string& path) : Endpoint(path) {}
endpoint::MemoryChunk::~MemoryChunk() {}

void endpoint::MemoryChunk::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    auto system_id = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID])
        .get_one()->get_id();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    r[Common::ID] = req.params[PathParam::MEMORY_CHUNK_ID];

    auto chunk = psme::rest::model::Find
        <agent_framework::model::MemoryChunk>
        (req.params[PathParam::MEMORY_CHUNK_ID]).via
        <agent_framework::model::System>
        (req.params[PathParam::SYSTEM_ID]).get();

    psme::rest::endpoint::utils::status_to_json(chunk,r);

    r[constants::MemoryChunk::MEMORY_CHUNK_NAME] = chunk.get_chunk_name();
    r[constants::MemoryChunk::MEMORY_CHUNK_UID] = chunk.get_chunk_id();
    if (chunk.get_chunk_size_mb().has_value()) {
        r[constants::MemoryChunk::MEMORY_CHUNK_SIZE_MIB] =
            uint32_t(endpoint::utils::mb_to_mib(chunk.get_chunk_size_mb().value()));
    }
    // currently hardcoded
    r[constants::MemoryChunk::ADDRESS_RANGE_TYPE] = chunk.get_type();
    r[constants::MemoryChunk::IS_MIRROR_ENABLED] = chunk.get_mirrored();
    r[constants::MemoryChunk::IS_SPARE] = chunk.get_spare();

    r[constants::MemoryChunk::INTERLEAVE_SETS] = json::Value::Type::ARRAY;

    for (const auto& iset : chunk.get_interleave_sets()) {
        json::Value elem(json::Value::Type::OBJECT);
        try {
            auto id = agent_framework::module::ComputeManager::get_instance()->
                get_dimm_manager().get_entry(iset.get_dimm()).get_id();
            elem[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                                .append(Root::SYSTEMS)
                                .append(req.params[PathParam::SYSTEM_ID])
                                .append(System::DIMM_CONFIG)
                                .append(id)
                                .build();
            elem[constants::MemoryChunk::REGION_ID] = iset.get_region_id();
            r[constants::MemoryChunk::INTERLEAVE_SETS].push_back(std::move(elem));
        }
        catch (agent_framework::exceptions::InvalidParameters&) {
            log_error(GET_LOGGER("rest"), "Model MemoryChunk/InterleaveSets link error");
            log_error(GET_LOGGER("rest"), "DimmConfig " << iset.get_dimm() <<
                " is present as a member of InterleaveSets of a MemoryChunk ("<<
                chunk.get_uuid() <<") but it does not exist as a resource");
        }
    }

    set_response(res, r);
}
