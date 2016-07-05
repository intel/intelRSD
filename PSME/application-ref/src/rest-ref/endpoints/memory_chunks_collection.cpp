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

#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/memory_chunks_collection.hpp"
#include <regex>

using namespace psme::rest::endpoint;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/MemoryChunks/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#MemoryChunkCollection.MemoryChunkCollection";
    r[Common::NAME] = "Memory Chunks Collection";
    r[Common::DESCRIPTION] = "Collection of Memory Chunks";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

MemoryChunksCollection::MemoryChunksCollection(const std::string& path) : Endpoint(path) {}
MemoryChunksCollection::~MemoryChunksCollection() {}

void MemoryChunksCollection::get(const server::Request& req, server::Response&
    res) {

    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto system =
        psme::rest::model::Find<agent_framework::model::System>(
            req.params[PathParam::SYSTEM_ID]).get_one();

    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                     std::regex("__SYSTEM_ID__"), std::to_string(system->get_id()));
    auto keys = agent_framework::module::ComputeManager::get_instance()->
                        get_memory_chunk_manager().get_ids(system->get_uuid());

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(Root::SYSTEMS)
                    .append(req.params[PathParam::SYSTEM_ID])
                    .append(System::MEMORY_CHUNKS)
                    .append(key)
                    .build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}
