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
 * @file compute_commands.hpp
 *
 * @brief Declarations of all compute commands
 * */

#pragma once



#include "agent-framework/command/command.hpp"
#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/compute.hpp"
#include "agent-framework/module/requests/rmm.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"

// TODO: move Fans, ThermalZones and PowerZones model and request classes to common


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
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetMemoryInfo = Command<model::requests::GetMemoryInfo, model::Memory>;
using GetMemoryDomainInfo = Command<model::requests::GetMemoryDomainInfo, model::MemoryDomain>;
using GetMemoryChunksInfo = Command<model::requests::GetMemoryChunksInfo, model::MemoryChunks>;
using GetDriveInfo = Command<model::requests::GetDriveInfo, model::Drive>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetNetworkInterfaceInfo = Command<model::requests::GetNetworkInterfaceInfo, model::NetworkInterface>;
using GetNetworkDeviceInfo = Command<model::requests::GetNetworkDeviceInfo, model::NetworkDevice>;
using GetNetworkDeviceFunctionInfo = Command<model::requests::GetNetworkDeviceFunctionInfo, model::NetworkDeviceFunction>;
using GetProcessorInfo = Command<model::requests::GetProcessorInfo, model::Processor>;
using GetPcieDeviceInfo = Command<model::requests::GetPcieDeviceInfo, model::PcieDevice>;
using GetPcieFunctionInfo = Command<model::requests::GetPcieFunctionInfo, model::PcieFunction>;
using GetStorageControllerInfo = Command<model::requests::GetStorageControllerInfo, model::StorageController>;
using GetSystemInfo = Command<model::requests::GetSystemInfo, model::System>;
using GetStorageSubsystemInfo = Command<model::requests::GetStorageSubsystemInfo, model::StorageSubsystem>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
using GetMetricDefinitionInfo = Command<model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;
using GetTrustedModuleInfo = Command<model::requests::GetTrustedModuleInfo, model::TrustedModule>;
using GetPowerZoneInfo = Command<model::requests::GetPowerZoneInfo, model::PowerZone>;
using GetThermalZoneInfo = Command<model::requests::GetThermalZoneInfo, model::ThermalZone>;
using GetFanInfo = Command<model::requests::GetFanInfo, model::Fan>;


// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

//declerations of all delete methods
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;
using DeleteDrive = Command<model::requests::DeleteDrive, model::responses::DeleteDrive>;

}
}
