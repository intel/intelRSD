/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file storage_commands.cpp
 *
 * @brief Declarations of all storage commands
 * */

#pragma once

#include "agent-framework/command/command.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/responses/storage.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



namespace agent_framework {
namespace command {

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
using AddZone = Command<model::requests::AddZone, model::responses::AddZone>;
using AddZoneEndpoints = Command<model::requests::AddZoneEndpoints, model::responses::AddZoneEndpoints>;

// declarations of all delete methods
using DeleteVolume = Command<model::requests::DeleteVolume, model::responses::DeleteVolume>;
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;
using DeleteEndpoint = Command<model::requests::DeleteEndpoint, model::responses::DeleteEndpoint>;
using DeleteZone = Command<model::requests::DeleteZone, model::responses::DeleteZone>;
using DeleteZoneEndpoints = Command<model::requests::DeleteZoneEndpoints, model::responses::DeleteZoneEndpoints>;

}
}
