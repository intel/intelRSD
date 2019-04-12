/*!
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
 * @file fpgaof_agent_commands.hpp
 * @brief Declarations of commands used in PSME FPGA-oF agent
 */

#pragma once



#include "discovery_agent_context.hpp"
#include "agent-framework/command/fpgaof_commands.hpp"
#include "agent-framework/command/context_registry.hpp"



namespace agent {
namespace fpga_discovery {

// Declare context class used in the following declarations
// Change this declaration to change the context class used in commands
using AgentContext = agent::fpga_discovery::DiscoveryAgentContext;

// Shortened version of REGISTER_CONTEXT_COMMAND as Context type is known HERE
#define REGISTER_FPGA_DISCOVERY_COMMAND(CMD, FUNC) REGISTER_CONTEXT_COMMAND(::agent::fpga_discovery::AgentContext, CMD, FUNC)

// declarations of get collection methods
using GetManagersCollection = ::agent_framework::command::ContextGetManagersCollection<AgentContext>;
using GetCollection = ::agent_framework::command::ContextGetCollection<AgentContext>;
using GetTasksCollection = ::agent_framework::command::ContextGetTasksCollection<AgentContext>;
using GetMetricDefinitionsCollection = ::agent_framework::command::ContextGetMetricDefinitionsCollection<AgentContext>;
using GetMetrics = ::agent_framework::command::ContextGetMetrics<AgentContext>;

// declarations of all get info methods
using GetManagerInfo = ::agent_framework::command::ContextGetManagerInfo<AgentContext>;
using GetFabricInfo = ::agent_framework::command::ContextGetFabricInfo<AgentContext>;
using GetEndpointInfo = ::agent_framework::command::ContextGetEndpointInfo<AgentContext>;
using GetZoneInfo = ::agent_framework::command::ContextGetZoneInfo<AgentContext>;

//declarations of all add methods
using AddEndpoint = ::agent_framework::command::ContextAddEndpoint<AgentContext>;
using AddZone = ::agent_framework::command::ContextAddZone<AgentContext>;
using AddZoneEndpoints = ::agent_framework::command::ContextAddZoneEndpoints<AgentContext>;

//declarations of all delete methods
using DeleteZone = ::agent_framework::command::ContextDeleteZone<AgentContext>;
using DeleteEndpoint = ::agent_framework::command::ContextDeleteEndpoint<AgentContext>;
using DeleteZoneEndpoints = ::agent_framework::command::ContextDeleteZoneEndpoints<AgentContext>;

// declarations of all set methods
using SetComponentAttributes = ::agent_framework::command::ContextSetComponentAttributes<AgentContext>;

}
}

