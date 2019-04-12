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
 *
 * @file command.cpp
 * @brief Contains gami command literals
 * */

#include "agent-framework/module/constants/command.hpp"



namespace agent_framework {
namespace model {
namespace literals {

constexpr const char Command::HEART_BEAT[];
constexpr const char Command::ATTACH[];
constexpr const char Command::COMPONENT_NOTIFICATION[];

// common commands
constexpr const char Command::GET_COLLECTION[];
constexpr const char Command::GET_MANAGER_INFO[];
constexpr const char Command::GET_MANAGERS_COLLECTION[];
constexpr const char Command::SET_COMPONENT_ATTRIBUTES[];
constexpr const char Command::GET_TASKS_COLLECTION[];
constexpr const char Command::GET_METRICS[];
constexpr const char Command::GET_METRIC_DEFINITIONS_COLLECTION[];
constexpr const char Command::GET_METRIC_DEFINITION_INFO[];
constexpr const char Command::GET_TASK_INFO[];
constexpr const char Command::GET_TASK_RESULT_INFO[];
constexpr const char Command::GET_DRIVE_INFO[];
constexpr const char Command::ADD_ENDPOINT[];
constexpr const char Command::DELETE_ENDPOINT[];
constexpr const char Command::DELETE_DRIVE[];

constexpr const char Command::DELETE_TASK[];

// compute commands
constexpr const char Command::GET_MEMORY_INFO[];
constexpr const char Command::GET_MEMORY_DOMAIN_INFO[];
constexpr const char Command::GET_MEMORY_CHUNKS_INFO[];
constexpr const char Command::GET_NETWORK_INTERFACE_INFO[];
constexpr const char Command::GET_NETWORK_DEVICE_INFO[];
constexpr const char Command::GET_NETWORK_DEVICE_FUNCTION_INFO[];
constexpr const char Command::GET_PROCESSOR_INFO[];
constexpr const char Command::GET_STORAGE_CONTROLLER_INFO[];
constexpr const char Command::GET_SYSTEM_INFO[];
constexpr const char Command::GET_STORAGE_SUBSYSTEM_INFO[];
constexpr const char Command::GET_TRUSTED_MODULE_INFO[];

// chassis commands
constexpr const char Command::GET_CHASSIS_INFO[];
constexpr const char Command::GET_POWER_ZONE_INFO[];
constexpr const char Command::GET_PSU_INFO[];
constexpr const char Command::GET_THERMAL_ZONE_INFO[];
constexpr const char Command::GET_FAN_INFO[];
constexpr const char Command::GET_AUTHORIZATION_CERTIFICATE[];
constexpr const char Command::GET_CHASSIS_SENSOR_INFO[];

// storage commands
constexpr const char Command::GET_STORAGE_SERVICE_INFO[];
constexpr const char Command::GET_STORAGE_POOL_INFO[];
constexpr const char Command::ADD_STORAGE_POOL[];
constexpr const char Command::DELETE_STORAGE_POOL[];
constexpr const char Command::GET_VOLUME_INFO[];
constexpr const char Command::ADD_VOLUME[];
constexpr const char Command::DELETE_VOLUME[];

// network commands
constexpr const char Command::GET_ETHERNET_SWITCH_INFO[];
constexpr const char Command::GET_ETHERNET_SWITCH_PORT_INFO[];
constexpr const char Command::ADD_ETHERNET_SWITCH_PORT[];
constexpr const char Command::DELETE_ETHERNET_SWITCH_PORT[];
constexpr const char Command::GET_REMOTE_ETHERNET_SWITCH_INFO[];
constexpr const char Command::GET_VLAN_INFO[];
constexpr const char Command::ADD_VLAN[];
constexpr const char Command::DELETE_VLAN[];
constexpr const char Command::GET_PORT_VLAN_INFO[];
constexpr const char Command::ADD_PORT_VLAN[];
constexpr const char Command::DELETE_PORT_VLAN[];
constexpr const char Command::GET_ACL_INFO[];
constexpr const char Command::ADD_ACL[];
constexpr const char Command::DELETE_ACL[];
constexpr const char Command::GET_ACL_RULE_INFO[];
constexpr const char Command::ADD_ACL_RULE[];
constexpr const char Command::ADD_ACL_PORT[];
constexpr const char Command::DELETE_ACL_RULE[];
constexpr const char Command::DELETE_ACL_PORT[];

// pnc commands
constexpr const char Command::GET_SWITCH_INFO[];
constexpr const char Command::GET_FABRIC_INFO[];
constexpr const char Command::GET_PORT_INFO[];
constexpr const char Command::GET_PCIE_DEVICE_INFO[];
constexpr const char Command::GET_PCIE_FUNCTION_INFO[];
constexpr const char Command::GET_ZONE_INFO[];
constexpr const char Command::GET_ENDPOINT_INFO[];
constexpr const char Command::ADD_ZONE[];
constexpr const char Command::DELETE_ZONE[];
constexpr const char Command::ADD_ZONE_ENDPOINTS[];
constexpr const char Command::DELETE_ZONE_ENDPOINTS[];
constexpr const char Command::ERASE_DRIVE_SECURELY[];

}
}
}
