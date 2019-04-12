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
 * @file chassis_commands.hpp
 *
 * @brief Declarations of all chassis commands
 * */

#pragma once
#include "agent-framework/command/command.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/chassis.hpp"
#include "agent-framework/module/responses/common.hpp"
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
using GetAuthorizationCertificate = Command<model::requests::GetAuthorizationCertificate, model::AuthorizationCertificate>;
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
using GetMetricDefinitionInfo = Command<model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

// declarations of all delete methods
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;

}
}

