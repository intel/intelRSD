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
 * @file network.hpp
 * @brief Contains constants used by network agents
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for NeighborInfo model objects
 * */
class NeighborInfo {
public:
    static constexpr const char NEIGHBOR_INFO[] = "neighborInfo";
    static constexpr const char SWITCH_IDENTIFIER[] = "switchIdentifier";
    static constexpr const char PORT_IDENTIFIER[] = "portIdentifier";
    static constexpr const char CABLE_ID[] = "cableId";
};

/*!
 * @brief Class consisting of literals for SwitchPortMembers
 * */
class SwitchPortMembers {
public:
    static constexpr const char PORT[] = "port";
    static constexpr const char MEMBERS[] = "members";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for EthernetSwitchPort model objects
 * */
class EthernetSwitchPort {
public:
    static constexpr const char PORT[] = "port";
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char STATUS[] = "status";
    static constexpr const char PORT_IDENTIFIER[] = "portIdentifier";
    static constexpr const char MODE[] = "mode";
    static constexpr const char MEMBERS[] = "members";
    static constexpr const char PORT_CLASS[] = "portClass";
    static constexpr const char PORT_TYPE[] = "portType";
    static constexpr const char PORT_MODE[] = "portMode";
    static constexpr const char LINK_TECHNOLOGY[] = "linkTechnology";
    static constexpr const char LINK_SPEED_MBPS[] = "linkSpeedMbps";
    static constexpr const char MAX_SPEED_MBPS[] = "maxSpeedMbps";
    static constexpr const char OPERATIONAL_STATE[] = "operationalState";
    static constexpr const char ADMINISTRATIVE_STATE[] = "administrativeState";
    static constexpr const char PORT_WIDTH[] = "portWidth";
    static constexpr const char FRAME_SIZE[] = "frameSize";
    static constexpr const char AUTO_SENSE[] = "autoSense";
    static constexpr const char FULL_DUPLEX[] = "fullDuplex";
    static constexpr const char IS_MANAGEMENT_PORT[] = "isManagementPort";
    static constexpr const char LAST_ERROR_CODE[] = "lastErrorCode";
    static constexpr const char ERROR_CLEARED[] = "errorCleared";
    static constexpr const char LAST_STATE_CHANGE_TIME[] =
        "lastStateChangeTime";
    static constexpr const char MAC_ADDRESS[] = "macAddress";
    static constexpr const char IPV4_ADDRESS[] = "ipv4Address";
    static constexpr const char IPV6_ADDRESS[] = "ipv6Address";
    static constexpr const char NEIGHBOR_INFO[] = "neighborInfo";
    static constexpr const char NEIGHBOR_MAC[] = "neighborMac";
    static constexpr const char VLAN_ENABLE[] = "vlanEnable";
    static constexpr const char DEFAULT_VLAN[] = "defaultVlan";
    static constexpr const char LLDP_ENABLED[] = "lldpEnabled";
    static constexpr const char PFC_ENABLED[] ="pfcEnabled";
    static constexpr const char PFC_ENABLED_PRIORITIES[] ="pfcEnabledPriorities";
    static constexpr const char DCBX_STATE[] = "dcbxState";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for Vlan model objects
 * */
class Vlan {
public:
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char VLAN[] = "vlan";
    static constexpr const char STATUS[] = "status";
    static constexpr const char VLAN_ID[] = "vlanId";
    static constexpr const char VLAN_NAME[] = "vlanName";
    static constexpr const char VLAN_ENABLE[] = "vlanEnable";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for EthernetSwitchPortVlan model objects
 * */
class EthernetSwitchPortVlan {
public:
    static constexpr const char PORT_VLAN[] = "portVlan";
    static constexpr const char PORT[] = "port";
    static constexpr const char STATUS[] = "status";
    static constexpr const char VLAN_ID[] = "vlanId";
    static constexpr const char VLAN_NAME[] = "vlanName";
    static constexpr const char VLAN_ENABLE[] = "vlanEnable";
    static constexpr const char TAGGED[] = "tagged";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for EthernetSwitch model objects
 * */
class EthernetSwitch {
public:
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SWITCH_IDENTIFIER[] = "switchIdentifier";
    static constexpr const char TECHNOLOGY[] = "technology";
    static constexpr const char MAC_ADDRESS[] = "macAddress";
    static constexpr const char FIRMWARE_NAME[] = "firmwareName";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char ROLE[] = "role";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char MANUFACTURING_DATE[] = "manufacturingDate";
    static constexpr const char LOCATION[] = "location";
    static constexpr const char CHASSIS[] = "chassis";
    static constexpr const char MAX_ACL_NUMBER[] = "maxAclNumber";
    static constexpr const char LLDP_ENABLED[] = "lldpEnabled";
    static constexpr const char PFC_ENABLED[] = "pfcEnabled";
    static constexpr const char ETS_ENABLED[] = "etsEnabled";
    static constexpr const char DCBX_ENABLED[] = "dcbxEnabled";
    static constexpr const char QOS_APPLICATION_PROTOCOL[] = "qosApplicationProtocol";
    static constexpr const char QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING[] = "qosPriorityToPriorityGroupMapping";
    static constexpr const char QOS_BANDWIDTH_ALLOCATION[] = "qosBandwidthAllocation";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for Remote model objects
 * */
class RemoteEthernetSwitch {
public:
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SWITCH_IDENTIFIER[] = "switchIdentifier";
    static constexpr const char MAC_ADDRESS[] = "macAddress";
    static constexpr const char NEXT_HOP[] = "nextHop";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for NextHop model objects
 * */
class NextHop {
public:
    static constexpr const char METRIC[] = "metric";
    static constexpr const char PORT_IDENTIFIER[] = "portIdentifier";
    static constexpr const char IPV4_ADDRESS[] = "ipv4Address";
    static constexpr const char IPV6_ADDRESS[] = "ipv6Address";
};

/*!
 * @brief Class consisting of literals for ACL
 * */
class Acl {
public:
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char PORTS[] = "ports";
    static constexpr const char OEM[] = "oem";
    static constexpr const char ACL[] = "acl";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
};

/*!
 * @brief Class consisting of literals for Rule
 * */
class AclRule {
public:
    static constexpr const char RULE[] = "rule";
    static constexpr const char STATUS[] = "status";
    static constexpr const char OEM[] = "oem";
    static constexpr const char RULE_ID[] = "ruleId";
    static constexpr const char ACTION[] = "action";
    static constexpr const char FORWARD_MIRROR_PORT[] = "forwardMirrorPort";
    static constexpr const char MIRRORED_PORTS[] = "mirroredPorts";
    static constexpr const char MIRROR_TYPE[] = "mirrorType";
    static constexpr const char VLAN_ID[] = "vlanId";
    static constexpr const char SOURCE_IP[] = "sourceIp";
    static constexpr const char DESTINATION_IP[] = "destinationIp";
    static constexpr const char SOURCE_MAC[] = "sourceMac";
    static constexpr const char DESTINATION_MAC[] = "destinationMac";
    static constexpr const char SOURCE_L4_PORT[] = "sourceL4Port";
    static constexpr const char DESTINATION_L4_PORT[] = "destinationL4Port";
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char ID[] = "id";
    static constexpr const char MASK[] = "mask";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char PORT[] = "port";
};

/*!
 * @brief Class consisting of literals for Static MACs
 */
class StaticMac {
public:
    static constexpr const char STATIC_MAC[] = "staticMac";
    static constexpr const char PORT[] = "port";
    static constexpr const char OEM[] = "oem";
    static constexpr const char STATUS[] = "status";
    static constexpr const char ADDRESS[] = "macAddress";
    static constexpr const char VLAN_ID[] = "vlanId";
};

/*!
 * @brief Class consisting of literals for Network QoS Configuration
 * */
class NetworkQosAttribute {
public:
    static constexpr const char PRIORITY_GROUP[] = "priorityGroup";
    static constexpr const char PRIORITY[] = "priority";
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char PORT[] = "port";
    static constexpr const char BANDWIDTH_PERCENT[] = "bandwidthPercent";
};

}
}
}