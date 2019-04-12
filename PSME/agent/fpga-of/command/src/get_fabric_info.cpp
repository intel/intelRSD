/*!
 * @brief Implementation of GetFabricInfo command.
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
 * @file get_fabric_info.cpp
 */

#include "fpgaof_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::fpgaof;
using namespace agent_framework;

namespace {
void get_fabric_info(GetFabricInfo::ContextPtr,
                     const GetFabricInfo::Request& request,
                     GetFabricInfo::Response& response) {
    log_debug("fpgaof-agent", "Getting fabric info.");
    response = module::get_manager<model::Fabric>().get_entry(request.get_uuid());
}
}

REGISTER_FPGAOF_COMMAND(GetFabricInfo, ::get_fabric_info);

