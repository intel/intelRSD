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
 * */

#include "agent-framework/command/compute/get_memory_chunk_info.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::generic;

using agent_framework::command::exception::NotFound;

namespace agent {
/* GetMemoryChunkInfo implementation */
class GetMemoryChunkInfo : public compute::GetMemoryChunkInfo {
public:
    GetMemoryChunkInfo() { }

    using agent_framework::command::compute::GetMemoryChunkInfo::execute;
    using ComputeComponents = agent_framework::module::ComputeManager;

    void execute(const Request& request, Response& response) {
        log_debug(GET_LOGGER("agent"), "Getting memory chunk info.");
        response.set_chunk(ComputeComponents::get_instance()->
                                    get_memory_chunk_manager().get_entry(
                                            request.get_chunk()));
    }
    ~GetMemoryChunkInfo();
};

GetMemoryChunkInfo::~GetMemoryChunkInfo() { }
}
static Command::Register<agent::GetMemoryChunkInfo> g("sdv");
