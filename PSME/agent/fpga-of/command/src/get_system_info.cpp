/*!
 * @brief Implementation of GetSystemInfo command.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_system_info.cpp
 */

#include "fpgaof_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::fpgaof;
using namespace agent_framework;

namespace {
void get_system_info(GetSystemInfo::ContextPtr, const GetSystemInfo::Request& request,
                      GetSystemInfo::Response& response) {
    log_debug("fpgaof-agent", "Getting system info.");
    response = module::get_manager<model::System>().get_entry(request.get_uuid());
}
}

REGISTER_FPGAOF_COMMAND(GetSystemInfo, ::get_system_info);
