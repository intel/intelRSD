/*!
 * @brief Implementation of GetEndpointInfo command.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file get_endpoint_info.cpp
 */


#include "fpga_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent_framework;
using namespace agent::fpga_discovery;

namespace {
void get_endpoint_info(GetEndpointInfo::ContextPtr, const GetEndpointInfo::Request& request,
                       GetEndpointInfo::Response& response) {

    log_debug("fpga-discovery-agent", "Getting endpoint info.");
    response = module::get_manager<model::Endpoint>().get_entry(request.get_uuid());
}
}
REGISTER_FPGA_DISCOVERY_COMMAND(GetEndpointInfo, ::get_endpoint_info);

