/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

// common commands
    static constexpr const char GET_COLLECTION[] = "getCollection";
    static constexpr const char GET_MANAGER_INFO[] = "getManagerInfo";
    static constexpr const char GET_MANAGER_COLLECTION[] = "getManagerCollection";
    static constexpr const char SET_COMPONENT_ATTRIBUTES[] = "setComponentAttributes";

    static constexpr const char GET_DRIVE_INFO[] = "getDriveInfo";
    static constexpr const char GET_CHASSIS_INFO[] = "getChassisInfo";

// compute commands
    static constexpr const char GET_DIMM_INFO[] = "getDimmInfo";
    static constexpr const char GET_MEMORY_CHUNK_INFO[] = "getMemoryChunkInfo";
    static constexpr const char GET_NETWORK_INTERFACE_INFO[] = "getNetworkInterfaceInfo";
    static constexpr const char GET_PROCESSOR_INFO[] = "getProcessorInfo";
    static constexpr const char GET_STORAGE_CONTROLLER_INFO[] = "getStorageControllerInfo";
    static constexpr const char GET_SYSTEM_INFO[] = "getComputerSystemInfo";

// chassis commands
    static constexpr const char GET_POWER_ZONE_INFO[] = "getPowerZoneInfo";
    static constexpr const char GET_PSU_INFO[] = "getPsuInfo";
    static constexpr const char GET_THERMAL_ZONE_INFO[] = "getThermalZoneInfo";
    static constexpr const char GET_FAN_INFO[] = "getFanInfo";
    static constexpr const char GET_AUTHORIZATION_CERTIFICATE[] = "getAuthorizationCertificate";

// storage commands
    static constexpr const char GET_STORAGE_SERVICES_INFO[] = "getStorageServicesInfo";
    static constexpr const char GET_PHYSICAL_DRIVE_INFO[] = "getPhysicalDriveInfo";
    static constexpr const char GET_LOGICAL_DRIVE_INFO[] = "getLogicalDriveInfo";
    static constexpr const char ADD_LOGICAL_DRIVE[] = "addLogicalDrive";
    static constexpr const char DELETE_LOGICAL_DRIVE[] = "deleteLogicalDrive";
    static constexpr const char GET_ISCSI_TARGET_INFO[] = "getiSCSITarget";
    static constexpr const char ADD_ISCSI_TARGET[] = "addiSCSITarget";
    static constexpr const char DELETE_ISCSI_TARGET[] = "deleteiSCSITarget";

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

};

}
}
}
