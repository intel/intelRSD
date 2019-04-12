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
 * @file command.hpp
 * @brief string enums for GAMI commands
 * */

#pragma once

namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for GAMI Commands names
 */
class Command {
public:

    // psme commands
    static constexpr const char HEART_BEAT[] = "heartBeat";
    static constexpr const char ATTACH[] = "attach";
    static constexpr const char COMPONENT_NOTIFICATION[] = "componentNotification";

    // common commands
    static constexpr const char GET_COLLECTION[] = "getCollection";
    static constexpr const char GET_MANAGER_INFO[] = "getManagerInfo";
    static constexpr const char GET_MANAGERS_COLLECTION[] = "getManagersCollection";
    static constexpr const char SET_COMPONENT_ATTRIBUTES[] = "setComponentAttributes";
    static constexpr const char GET_TASKS_COLLECTION[] = "getTasksCollection";
    static constexpr const char GET_METRICS[] = "getMetrics";
    static constexpr const char GET_METRIC_DEFINITIONS_COLLECTION[] = "getMetricDefinitionsCollection";
    static constexpr const char GET_METRIC_DEFINITION_INFO[] = "getMetricDefinitionInfo";

    static constexpr const char GET_DRIVE_INFO[] = "getDriveInfo";
    static constexpr const char GET_CHASSIS_INFO[] = "getChassisInfo";
    static constexpr const char GET_TASK_INFO[] = "getTaskInfo";
    static constexpr const char GET_TASK_RESULT_INFO[] = "getTaskResultInfo";

    static constexpr const char DELETE_TASK[] = "deleteTask";
    static constexpr const char ADD_ENDPOINT[] = "addEndpoint";
    static constexpr const char DELETE_ENDPOINT[] = "deleteEndpoint";
    static constexpr const char DELETE_DRIVE[] = "deleteDrive";

    // compute commands
    static constexpr const char GET_MEMORY_INFO[] = "getMemoryInfo";
    static constexpr const char GET_MEMORY_DOMAIN_INFO[] = "getMemoryDomainInfo";
    static constexpr const char GET_MEMORY_CHUNKS_INFO[] = "getMemoryChunksInfo";
    static constexpr const char GET_NETWORK_INTERFACE_INFO[] = "getNetworkInterfaceInfo";
    static constexpr const char GET_NETWORK_DEVICE_INFO[] = "getNetworkDeviceInfo";
    static constexpr const char GET_NETWORK_DEVICE_FUNCTION_INFO[] = "getNetworkDeviceFunctionInfo";
    static constexpr const char GET_PROCESSOR_INFO[] = "getProcessorInfo";
    static constexpr const char GET_STORAGE_CONTROLLER_INFO[] = "getStorageControllerInfo";
    static constexpr const char GET_SYSTEM_INFO[] = "getComputerSystemInfo";
    static constexpr const char GET_STORAGE_SUBSYSTEM_INFO[] = "getStorageSubsystemInfo";
    static constexpr const char GET_TRUSTED_MODULE_INFO[] = "getTrustedModuleInfo";

    // chassis commands
    static constexpr const char GET_POWER_ZONE_INFO[] = "getPowerZoneInfo";
    static constexpr const char GET_PSU_INFO[] = "getPsuInfo";
    static constexpr const char GET_THERMAL_ZONE_INFO[] = "getThermalZoneInfo";
    static constexpr const char GET_FAN_INFO[] = "getFanInfo";
    static constexpr const char GET_AUTHORIZATION_CERTIFICATE[] = "getAuthorizationCertificate";
    static constexpr const char GET_CHASSIS_SENSOR_INFO[] = "getChassisSensorInfo";

    // storage commands
    static constexpr const char GET_STORAGE_SERVICE_INFO[] = "getStorageServiceInfo";
    static constexpr const char GET_STORAGE_POOL_INFO[] = "getStoragePoolInfo";
    static constexpr const char ADD_STORAGE_POOL[] = "addStoragePool";
    static constexpr const char DELETE_STORAGE_POOL[] = "deleteStoragePool";
    static constexpr const char GET_VOLUME_INFO[] = "getVolumeInfo";
    static constexpr const char ADD_VOLUME[] = "addVolume";
    static constexpr const char DELETE_VOLUME[] = "deleteVolume";


    // network commands
    static constexpr const char GET_ETHERNET_SWITCH_INFO[] = "getEthernetSwitchInfo";
    static constexpr const char GET_ETHERNET_SWITCH_PORT_INFO[] = "getEthernetSwitchPortInfo";
    static constexpr const char ADD_ETHERNET_SWITCH_PORT[] = "addEthernetSwitchPort";
    static constexpr const char DELETE_ETHERNET_SWITCH_PORT[] = "deleteEthernetSwitchPort";
    static constexpr const char GET_REMOTE_ETHERNET_SWITCH_INFO[] = "getRemoteEthernetSwitchInfo";
    static constexpr const char GET_VLAN_INFO[] = "getVlanInfo";
    static constexpr const char ADD_VLAN[] = "addVlan";
    static constexpr const char DELETE_VLAN[] = "deleteVlan";
    static constexpr const char GET_PORT_VLAN_INFO[] = "getPortVlanInfo";
    static constexpr const char ADD_PORT_VLAN[] = "addPortVlan";
    static constexpr const char DELETE_PORT_VLAN[] = "deletePortVlan";
    static constexpr const char GET_ACL_INFO[] = "getAclInfo";
    static constexpr const char ADD_ACL[] = "addAcl";
    static constexpr const char DELETE_ACL[] = "deleteAcl";
    static constexpr const char GET_ACL_RULE_INFO[] = "getAclRuleInfo";
    static constexpr const char ADD_ACL_RULE[] = "addAclRule";
    static constexpr const char ADD_ACL_PORT[] = "addAclPort";
    static constexpr const char DELETE_ACL_RULE[] = "deleteAclRule";
    static constexpr const char DELETE_ACL_PORT[] = "deleteAclPort";

    // pnc commands
    static constexpr const char GET_SWITCH_INFO[] = "getSwitchInfo";
    static constexpr const char GET_FABRIC_INFO[] = "getFabricInfo";
    static constexpr const char GET_PORT_INFO[] = "getPortInfo";
    static constexpr const char GET_PCIE_DEVICE_INFO[] = "getPCIeDeviceInfo";
    static constexpr const char GET_PCIE_FUNCTION_INFO[] = "getPCIeFunctionInfo";
    static constexpr const char GET_ZONE_INFO[] = "getZoneInfo";
    static constexpr const char GET_ENDPOINT_INFO[] = "getEndpointInfo";
    static constexpr const char ADD_ZONE[] = "addZone";
    static constexpr const char DELETE_ZONE[] = "deleteZone";
    static constexpr const char ADD_ZONE_ENDPOINTS[] = "addZoneEndpoints";
    static constexpr const char DELETE_ZONE_ENDPOINTS[] = "deleteZoneEndpoints";
    static constexpr const char ERASE_DRIVE_SECURELY[] = "eraseDriveSecurely";
};

}
}
}
