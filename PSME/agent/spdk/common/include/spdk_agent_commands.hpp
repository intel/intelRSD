/*!
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
 * @file spdk_agent_commands.hpp
 */

#pragma once



#include "spdk_agent_context.hpp"
#include "agent-framework/command/nvme_commands.hpp"
#include "agent-framework/command/context_registry.hpp"



namespace agent {
namespace spdk {

// Shortened version of REGISTER_CONTEXT_COMMAND as Context type is known HERE
#define REGISTER_SPDK_COMMAND(CMD, FUNC) REGISTER_CONTEXT_COMMAND(::agent::spdk::AgentContext, CMD, FUNC)

// declarations of get collection methods
using GetManagersCollection = ::agent_framework::command::ContextGetManagersCollection<AgentContext>;
using GetCollection = ::agent_framework::command::ContextGetCollection<AgentContext>;
using GetTasksCollection = ::agent_framework::command::ContextGetTasksCollection<AgentContext>;
using GetMetricDefinitionsCollection = ::agent_framework::command::ContextGetMetricDefinitionsCollection<AgentContext>;
using GetMetrics = ::agent_framework::command::ContextGetMetrics<AgentContext>;

// declarations of all get info methods
using GetManagerInfo = ::agent_framework::command::ContextGetManagerInfo<AgentContext>;
using GetSystemInfo = ::agent_framework::command::ContextGetSystemInfo<AgentContext>;
using GetChassisInfo = ::agent_framework::command::ContextGetChassisInfo<AgentContext>;
using GetFabricInfo = ::agent_framework::command::ContextGetFabricInfo<AgentContext>;
using GetStorageServiceInfo = ::agent_framework::command::ContextGetStorageServiceInfo<AgentContext>;
using GetDriveInfo = ::agent_framework::command::ContextGetDriveInfo<AgentContext>;
using GetVolumeInfo = ::agent_framework::command::ContextGetVolumeInfo<AgentContext>;
using GetStoragePoolInfo = ::agent_framework::command::ContextGetStoragePoolInfo<AgentContext>;
using GetEndpointInfo = ::agent_framework::command::ContextGetEndpointInfo<AgentContext>;
using GetNetworkInterfaceInfo = ::agent_framework::command::ContextGetNetworkInterfaceInfo<AgentContext>;
using GetSystemInfo = ::agent_framework::command::ContextGetSystemInfo<AgentContext>;
using GetZoneInfo = ::agent_framework::command::ContextGetZoneInfo<AgentContext>;
using GetTaskInfo = ::agent_framework::command::ContextGetTaskInfo<AgentContext>;
using GetTaskResultInfo = ::agent_framework::command::ContextGetTaskResultInfo<AgentContext>;

// declarations of all add methods
using AddVolume = ::agent_framework::command::ContextAddVolume<AgentContext>;
using AddEndpoint = ::agent_framework::command::ContextAddEndpoint<AgentContext>;
using AddStoragePool = ::agent_framework::command::ContextAddStoragePool<AgentContext>;
using AddZone = ::agent_framework::command::ContextAddZone<AgentContext>;
using AddZoneEndpoints = ::agent_framework::command::ContextAddZoneEndpoints<AgentContext>;

// declarations of all delete methods
using DeleteVolume = ::agent_framework::command::ContextDeleteVolume<AgentContext>;
using DeleteZone = ::agent_framework::command::ContextDeleteZone<AgentContext>;
using DeleteEndpoint = ::agent_framework::command::ContextDeleteEndpoint<AgentContext>;
using DeleteZoneEndpoints = ::agent_framework::command::ContextDeleteZoneEndpoints<AgentContext>;
using DeleteStoragePool = ::agent_framework::command::ContextDeleteStoragePool<AgentContext>;
using DeleteDrive = ::agent_framework::command::ContextDeleteDrive<AgentContext>;

// declarations of all set methods
using SetComponentAttributes = ::agent_framework::command::ContextSetComponentAttributes<AgentContext>;

}
}

