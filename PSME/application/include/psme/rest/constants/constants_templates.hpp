/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file constants_templates.hpp
 *
 * @brief Definition of resource's IDs as templates of the resource type.
 * */

#pragma once



#include "constants.hpp"
#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/model/model_pnc.hpp"

#include <type_traits>


namespace psme {
namespace rest {
namespace constants {

template<typename T>
constexpr const char* RESOURCE_ID = "";

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Memory> = PathParam::MEMORY_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::System> = PathParam::SYSTEM_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Volume> = PathParam::VOLUME_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Chassis> = PathParam::CHASSIS_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Drive> = PathParam::DRIVE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Acl> = PathParam::ACL_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::EthernetSwitch> = PathParam::ETHERNET_SWITCH_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::EthernetSwitchPort> = PathParam::SWITCH_PORT_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::EthernetSwitchPortVlan> = PathParam::VLAN_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::AclRule> = PathParam::RULE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::StaticMac> = PathParam::STATIC_MAC_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Endpoint> = PathParam::ENDPOINT_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::StorageService> = PathParam::SERVICE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::StorageSubsystem> = PathParam::STORAGE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Manager> = PathParam::MANAGER_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::NetworkInterface> = PathParam::NIC_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Fabric> = PathParam::FABRIC_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Switch> = PathParam::SWITCH_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Port> = PathParam::PORT_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Zone> = PathParam::ZONE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::StoragePool> = PathParam::STORAGE_POOL_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::NetworkDevice> = PathParam::NETWORK_INTERFACE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Task> = PathParam::TASK_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::NetworkDeviceFunction> = PathParam::NETWORK_DEVICE_FUNCTION_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::MetricDefinition> = PathParam::METRIC_DEFINITION_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::Processor> = PathParam::PROCESSOR_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::PcieDevice> = PathParam::DEVICE_ID;

template<>
constexpr const char* RESOURCE_ID<agent_framework::model::PcieFunction> = PathParam::FUNCTION_ID;

template <typename T>
constexpr const char* get_resource_id() {
    static_assert(RESOURCE_ID<T> != nullptr, "RESOURCE_ID<T> for T template type is not defined.");
    return RESOURCE_ID<T>;
}

}
}
}
