/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file storage_commands.hpp
 *
 * @brief Declarations of all storage commands
 * */

#pragma once

#include "agent-framework/command/context_command.hpp"
#include "agent-framework/command/context_registry.hpp"
#include "agent-framework/command/command.hpp"

#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/responses/storage.hpp"

namespace agent_framework {
namespace command {

// TODO: Remove non-context commands after hybrid agent integration

// declarations of get collection methods
using GetCollection = Command<model::requests::GetCollection, model::attribute::Array<model::attribute::SubcomponentEntry>>;
using GetManagersCollection = Command<model::requests::GetManagersCollection, model::attribute::Array<model::attribute::ManagerEntry>>;
using GetTasksCollection = Command<model::requests::GetTasksCollection, model::attribute::Array<model::attribute::TaskEntry>>;
using GetMetricDefinitionsCollection = Command<model::requests::GetMetricDefinitionsCollection,
    model::attribute::Array<model::attribute::MetricDefinitionEntry>>;
using GetMetrics = Command<model::requests::GetMetrics, model::attribute::Array<model::Metric>>;

// declarations of all get info methods
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetSystemInfo = Command<model::requests::GetSystemInfo, model::System>;
using GetNetworkInterfaceInfo = Command<model::requests::GetNetworkInterfaceInfo, model::NetworkInterface>;
using GetDriveInfo = Command<model::requests::GetDriveInfo, model::Drive>;
using GetFabricInfo = Command<model::requests::GetFabricInfo, model::Fabric>;
using GetZoneInfo = Command<model::requests::GetZoneInfo, model::Zone>;
using GetEndpointInfo = Command<model::requests::GetEndpointInfo, model::Endpoint>;
using GetStorageServiceInfo = Command<model::requests::GetStorageServiceInfo, model::StorageService>;
using GetStoragePoolInfo = Command<model::requests::GetStoragePoolInfo, model::StoragePool>;
using GetVolumeInfo = Command<model::requests::GetVolumeInfo, model::Volume>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
using GetMetricDefinitionInfo = Command<model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

// declarations of all add methods
using AddEndpoint = Command<model::requests::AddEndpoint, model::responses::AddEndpoint>;
using AddVolume = Command<model::requests::AddVolume, model::responses::AddVolume>;
using AddStoragePool = Command<model::requests::AddStoragePool, model::responses::AddStoragePool>;
using AddZone = Command<model::requests::AddZone, model::responses::AddZone>;
using AddZoneEndpoints = Command<model::requests::AddZoneEndpoints, model::responses::AddZoneEndpoints>;

// declarations of all delete methods
using DeleteVolume = Command<model::requests::DeleteVolume, model::responses::DeleteVolume>;
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;
using DeleteEndpoint = Command<model::requests::DeleteEndpoint, model::responses::DeleteEndpoint>;
using DeleteZone = Command<model::requests::DeleteZone, model::responses::DeleteZone>;
using DeleteZoneEndpoints = Command<model::requests::DeleteZoneEndpoints, model::responses::DeleteZoneEndpoints>;


// declarations of get collection methods
template <typename CONTEXT> using ContextGetCollection = ContextCommand<CONTEXT, model::requests::GetCollection, model::attribute::Array<model::attribute::SubcomponentEntry>>;
template <typename CONTEXT> using ContextGetManagersCollection = ContextCommand<CONTEXT, model::requests::GetManagersCollection, model::attribute::Array<model::attribute::ManagerEntry>>;
template <typename CONTEXT> using ContextGetTasksCollection = ContextCommand<CONTEXT, model::requests::GetTasksCollection, model::attribute::Array<model::attribute::TaskEntry>>;
template <typename CONTEXT> using ContextGetMetricDefinitionsCollection = ContextCommand<CONTEXT, model::requests::GetMetricDefinitionsCollection, model::attribute::Array<model::attribute::MetricDefinitionEntry>>;
template <typename CONTEXT> using ContextGetMetrics = ContextCommand<CONTEXT, model::requests::GetMetrics, model::attribute::Array<model::Metric>>;

// declarations of all get info methods
template <typename CONTEXT> using ContextGetChassisInfo = ContextCommand<CONTEXT, model::requests::GetChassisInfo, model::Chassis>;
template <typename CONTEXT> using ContextGetManagerInfo = ContextCommand<CONTEXT, model::requests::GetManagerInfo, model::Manager>;
template <typename CONTEXT> using ContextGetTaskInfo = ContextCommand<CONTEXT, model::requests::GetTaskInfo, model::Task>;
template <typename CONTEXT> using ContextGetTaskResultInfo = ContextCommand<CONTEXT, model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
template <typename CONTEXT> using ContextGetMetricDefinitionInfo = ContextCommand<CONTEXT, model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;
template <typename CONTEXT> using ContextGetNetworkInterfaceInfo = ContextCommand<CONTEXT, model::requests::GetNetworkInterfaceInfo, model::NetworkInterface>;
template <typename CONTEXT> using ContextGetFabricInfo = ContextCommand<CONTEXT, model::requests::GetFabricInfo, model::Fabric>;
template <typename CONTEXT> using ContextGetDriveInfo = ContextCommand<CONTEXT, model::requests::GetDriveInfo, model::Drive>;
template <typename CONTEXT> using ContextGetEndpointInfo = ContextCommand<CONTEXT, model::requests::GetEndpointInfo, model::Endpoint>;
template <typename CONTEXT> using ContextGetZoneInfo = ContextCommand<CONTEXT, model::requests::GetZoneInfo, model::Zone>;
template <typename CONTEXT> using ContextGetStorageServiceInfo = ContextCommand<CONTEXT, model::requests::GetStorageServiceInfo, model::StorageService>;
template <typename CONTEXT> using ContextGetStoragePoolInfo = ContextCommand<CONTEXT, model::requests::GetStoragePoolInfo, model::StoragePool>;
template <typename CONTEXT> using ContextGetSystemInfo = ContextCommand<CONTEXT, model::requests::GetSystemInfo, model::System>;
template <typename CONTEXT> using ContextGetVolumeInfo = ContextCommand<CONTEXT, model::requests::GetVolumeInfo, model::Volume>;

// declarations of all set methods
template <typename CONTEXT> using ContextSetComponentAttributes = ContextCommand<CONTEXT, model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

// declarations of all add methods
template <typename CONTEXT> using ContextAddEndpoint = ContextCommand<CONTEXT, model::requests::AddEndpoint, model::responses::AddEndpoint>;
template <typename CONTEXT> using ContextAddZone = ContextCommand<CONTEXT, model::requests::AddZone, model::responses::AddZone>;
template <typename CONTEXT> using ContextAddZoneEndpoints = ContextCommand<CONTEXT, model::requests::AddZoneEndpoints, model::responses::AddZoneEndpoints>;
template <typename CONTEXT> using ContextAddVolume = ContextCommand<CONTEXT, model::requests::AddVolume, model::responses::AddVolume>;

// declarations of all delete methods
template <typename CONTEXT> using ContextDeleteEndpoint = ContextCommand<CONTEXT, model::requests::DeleteEndpoint, model::responses::DeleteEndpoint>;
template <typename CONTEXT> using ContextDeleteZone = ContextCommand<CONTEXT, model::requests::DeleteZone, model::responses::DeleteZone>;
template <typename CONTEXT> using ContextDeleteZoneEndpoints = ContextCommand<CONTEXT, model::requests::DeleteZoneEndpoints, model::responses::DeleteZoneEndpoints>;
template <typename CONTEXT> using ContextDeleteVolume = ContextCommand<CONTEXT, model::requests::DeleteVolume, model::responses::DeleteVolume>;
template <typename CONTEXT> using ContextDeleteDrive = ContextCommand<CONTEXT, model::requests::DeleteDrive, model::responses::DeleteDrive>;
template <typename CONTEXT> using ContextDeleteStoragePool = ContextCommand<CONTEXT, model::requests::DeleteStoragePool, model::responses::DeleteStoragePool>;
template <typename CONTEXT> using ContextDeleteTask = ContextCommand<CONTEXT, model::requests::DeleteTask, model::responses::DeleteTask>;

}
}
