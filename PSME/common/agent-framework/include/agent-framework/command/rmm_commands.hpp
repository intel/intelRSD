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
 * @file rmm_commands.hpp
 * @brief Declarations of all RMM commands
 * */

#pragma once
#include "agent-framework/command/command.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/rmm.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/responses/rmm.hpp"

#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/requests/compute.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"


namespace agent_framework {
namespace command {

// declarations of get collection methods
using GetManagersCollection = Command<model::requests::GetManagersCollection, model::attribute::Array<model::attribute::ManagerEntry>>;
using GetCollection = Command<model::requests::GetCollection, model::attribute::Array<model::attribute::SubcomponentEntry>>;
using GetTasksCollection = Command<model::requests::GetTasksCollection, model::attribute::Array<model::attribute::TaskEntry>>;
using GetMetricDefinitionsCollection = Command<model::requests::GetMetricDefinitionsCollection,
    model::attribute::Array<model::attribute::MetricDefinitionEntry>>;
using GetMetrics = Command<model::requests::GetMetrics, model::attribute::Array<model::Metric>>;

// declarations of all get info methods
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetPowerZoneInfo = Command<model::requests::GetPowerZoneInfo, model::PowerZone>;
using GetThermalZoneInfo = Command<model::requests::GetThermalZoneInfo, model::ThermalZone>;
using GetFanInfo = Command<model::requests::GetFanInfo, model::Fan>;
using GetPsuInfo = Command<model::requests::GetPsuInfo, model::Psu>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
using GetNetworkInterfaceInfo = Command<model::requests::GetNetworkInterfaceInfo, model::NetworkInterface>;
using GetPortVlanInfo = Command<model::requests::GetPortVlanInfo, model::EthernetSwitchPortVlan>;
using GetMetricDefinitionInfo = Command<model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;


}
}
