/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file chassis_commands.cpp
 *
 * @brief Declarations of all chassis commands
 * */

#pragma once
#include "agent-framework/command-ref/command.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/chassis.hpp"
#include "agent-framework/module/responses/common.hpp"

namespace agent_framework {
namespace command_ref {

// declarations of get collection methods
using GetCollection = Command<model::requests::GetCollection, model::attribute::Array<model::attribute::SubcomponentEntry>>;
using GetManagersCollection = Command<model::requests::GetManagersCollection, model::attribute::Array<model::attribute::ManagerEntry>>;
using GetTasksCollection = Command<model::requests::GetTasksCollection, model::attribute::Array<model::attribute::TaskEntry>>;

// declarations of all get info methods
using GetAuthorizationCertificate = Command<model::requests::GetAuthorizationCertificate, model::AuthorizationCertificate>;
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetFanInfo = Command<model::requests::GetFanInfo, model::Fan>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetPowerZoneInfo = Command<model::requests::GetPowerZoneInfo, model::PowerZone>;
using GetPsuInfo = Command<model::requests::GetPsuInfo, model::Psu>;
using GetThermalZoneInfo = Command<model::requests::GetThermalZoneInfo, model::ThermalZone>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

// declarations of all delete methods
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;

}
}

