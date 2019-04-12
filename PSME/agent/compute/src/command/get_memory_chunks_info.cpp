/*!
 * @brief Compute GetMemoryChunksInfo command register
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

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/compute_commands.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;

REGISTER_COMMAND(GetMemoryChunksInfo,
    [] (const GetMemoryChunksInfo::Request& req, GetMemoryChunksInfo::Response& rsp) {
        log_debug("agent", "Getting memory chunks info.");
        rsp = ComputeComponents::get_instance()->get_memory_chunks_manager().get_entry(req.get_uuid());
    }
);