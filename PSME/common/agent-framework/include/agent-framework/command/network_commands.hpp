/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file network_commands.hpp
 *
 * @brief Declarations of all network commands
 * */

#pragma once

#include "agent-framework/command/command.hpp"
#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/responses/network.hpp"
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
using GetChassisInfo = Command<model::requests::GetChassisInfo, model::Chassis>;
using GetEthernetSwitchInfo = Command<model::requests::GetEthernetSwitchInfo, model::EthernetSwitch>;
using GetEthernetSwitchPortInfo = Command<model::requests::GetEthernetSwitchPortInfo, model::EthernetSwitchPort>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetPortVlanInfo = Command<model::requests::GetPortVlanInfo, model::EthernetSwitchPortVlan>;
using GetRemoteEthernetSwitchInfo = Command<model::requests::GetRemoteEthernetSwitchInfo, model::RemoteEthernetSwitch>;
using GetVlanInfo = Command<model::requests::GetVlanInfo, model::Vlan>;
using GetAclInfo = Command<model::requests::GetAclInfo, model::Acl>;
using GetAclRuleInfo = Command<model::requests::GetAclRuleInfo, model::AclRule>;
using GetPortStaticMacInfo = Command<model::requests::GetPortStaticMacInfo, model::StaticMac>;
using GetTaskInfo = Command<model::requests::GetTaskInfo, model::Task>;
using GetTaskResultInfo = Command<model::requests::GetTaskResultInfo, model::responses::GetTaskResultInfo>;
using GetMetricDefinitionInfo = Command<model::requests::GetMetricDefinitionInfo, model::MetricDefinition>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;

// declarations of all add methods
using AddPortVlan = Command<model::requests::AddPortVlan, model::responses::AddPortVlan>;
using AddEthernetSwitchPort = Command<model::requests::AddEthernetSwitchPort, model::responses::AddEthernetSwitchPort>;
using AddEthernetSwitchPortMembers = Command<model::requests::AddEthernetSwitchPortMembers, model::responses::AddEthernetSwitchPortMembers>;
using AddPortVlan = Command<model::requests::AddPortVlan, model::responses::AddPortVlan>;
using AddAcl = Command<model::requests::AddAcl, model::responses::AddAcl>;
using AddAclRule = Command<model::requests::AddAclRule, model::responses::AddAclRule>;
using AddAclPort = Command<model::requests::AddAclPort, model::responses::AddAclPort>;
using AddPortStaticMac = Command<model::requests::AddPortStaticMac, model::responses::AddPortStaticMac>;
using AddVlan = Command<model::requests::AddVlan, model::responses::AddVlan>;

// declarations of all delete methods
using DeleteEthernetSwitchPort = Command<model::requests::DeleteEthernetSwitchPort, model::responses::DeleteEthernetSwitchPort>;
using DeleteEthernetSwitchPortMembers = Command<model::requests::DeleteEthernetSwitchPortMembers, model::responses::DeleteEthernetSwitchPortMembers>;
using DeletePortVlan = Command<model::requests::DeletePortVlan, model::responses::DeletePortVlan>;
using DeleteAcl = Command<model::requests::DeleteAcl, model::responses::DeleteAcl>;
using DeleteAclRule = Command<model::requests::DeleteAclRule, model::responses::DeleteAclRule>;
using DeleteAclPort = Command<model::requests::DeleteAclPort, model::responses::DeleteAclPort>;
using DeletePortStaticMac = Command<model::requests::DeletePortStaticMac, model::responses::DeletePortStaticMac>;
using DeleteTask = Command<model::requests::DeleteTask, model::responses::DeleteTask>;
using DeleteVlan = Command<model::requests::DeleteVlan, model::responses::DeleteVlan>;

}
}
