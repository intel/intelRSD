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
 *
 * @file command/compute/json/get_memory_chunk_info.cpp
 *
 * @brief JSON command get memory information implementation
 * */

#include "agent-framework/command/compute/json/get_memory_chunk_info.hpp"
#include "agent-framework/command/compute/get_memory_chunk_info.hpp"

using namespace agent_framework::command;
using namespace agent_framework::command::compute::json;
using namespace agent_framework::model::literals;

GetMemoryChunkInfo::GetMemoryChunkInfo() :
    CommandJson(Procedure(TAG,
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                MemoryChunk::CHUNK, jsonrpc::JSON_STRING,
                nullptr)) { }

void GetMemoryChunkInfo::method(const Json::Value& params, Json::Value& result) {
        Command* command = get_command();

        compute::GetMemoryChunkInfo::Request request{};
        compute::GetMemoryChunkInfo::Response response{};

        request.m_chunk = params[MemoryChunk::CHUNK].asString();

        command->execute(request, response);

        result = response.m_chunk.to_json();
}

void GetMemoryChunkInfo::notification(const Json::Value&) { }

static CommandJson::Register<GetMemoryChunkInfo> g;
