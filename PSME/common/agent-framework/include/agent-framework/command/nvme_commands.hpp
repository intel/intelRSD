/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file nvme_commands.hpp
 *
 * @brief Declarations of all nvme commands
 * */

#pragma once

#include "agent-framework/command/context_command.hpp"
#include "agent-framework/command/context_registry.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/storage/get_storage_service_info.hpp"
#include "agent-framework/module/requests/storage/get_storage_pool_info.hpp"
#include "agent-framework/module/requests/storage/get_volume_info.hpp"
#include "agent-framework/module/requests/storage/add_volume.hpp"
#include "agent-framework/module/requests/storage/add_storage_pool.hpp"
#include "agent-framework/module/requests/storage/delete_volume.hpp"
#include "agent-framework/module/requests/storage/delete_storage_pool.hpp"
#include "agent-framework/module/responses/storage/delete_storage_pool.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/responses/storage/add_volume.hpp"
#include "agent-framework/module/responses/storage/add_storage_pool.hpp"
#include "agent-framework/module/responses/storage/delete_volume.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/storage_service.hpp"
#include "agent-framework/module/model/storage_pool.hpp"
#include "agent-framework/module/model/network_interface.hpp"
#include "agent-framework/module/model/volume.hpp"
#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/requests/common/delete_drive.hpp"
#include "agent-framework/module/responses/common/delete_drive.hpp"



namespace agent_framework {
namespace command {

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
template <typename CONTEXT> using ContextAddStoragePool = ContextCommand<CONTEXT, model::requests::AddStoragePool, model::responses::AddStoragePool>;

// declarations of all delete methods
template <typename CONTEXT> using ContextDeleteEndpoint = ContextCommand<CONTEXT, model::requests::DeleteEndpoint, model::responses::DeleteEndpoint>;
template <typename CONTEXT> using ContextDeleteZone = ContextCommand<CONTEXT, model::requests::DeleteZone, model::responses::DeleteZone>;
template <typename CONTEXT> using ContextDeleteZoneEndpoints = ContextCommand<CONTEXT, model::requests::DeleteZoneEndpoints, model::responses::DeleteZoneEndpoints>;
template <typename CONTEXT> using ContextDeleteVolume = ContextCommand<CONTEXT, model::requests::DeleteVolume, model::responses::DeleteVolume>;
template <typename CONTEXT> using ContextDeleteStoragePool = ContextCommand<CONTEXT, model::requests::DeleteStoragePool, model::responses::DeleteStoragePool>;
template <typename CONTEXT> using ContextDeleteDrive = ContextCommand<CONTEXT, model::requests::DeleteDrive, model::responses::DeleteDrive>;

}
}
