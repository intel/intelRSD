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
 * */

#include "psme/rest/constants/ethernet.hpp"

namespace psme {
namespace rest {
namespace constants {

namespace EthernetSwitch {
const char SWITCH[] = "Switch";
const char FIRMWARE_NAME[] = "FirmwareName";
const char MANUFACTURING_DATE[] = "ManufacturingDate";
const char ROLE[] = "Role";
const char PORTS[] = "Ports";
const char SWITCH_ID[] = "SwitchId";
const char ACLS[] = "ACLs";
const char MAX_ACL_NUMBER[] = "MaxACLNumber";
const char PFC_ENABLED[] = "PFCEnabled";
const char LLDP_ENABLED[] = "LLDPEnabled";
const char ETS_ENABLED[] = "ETSEnabled";
const char DCBX_ENABLED[] = "DCBXEnabled";
const char DCBX_SHARED_CONFIGURATION[] = "DCBXSharedConfiguration";
const char APPLICATION_PROTOCOL[] = "ApplicationProtocol";
const char PRIORITY_TO_PRIORITY_GROUP_MAPPING[] = "PriorityToPriorityGroupMapping";
const char BANDWIDTH_ALLOCATION[] = "BandwidthAllocation";
const char PRIORITY_GROUP[] = "PriorityGroup";
const char PRIORITY[] = "Priority";
const char PROTOCOL[] = "Protocol";
const char PORT[] = "Port";
const char BANDWIDTH_PERCENT[] = "BandwidthPercent";
}

namespace EthernetSwitchPort {
const char PORT[] = "Port";
const char PORT_ID[] = "PortId";
const char LINK_TYPE[] = "LinkType";
const char OPERATIONAL_STATE[] = "OperationalState";
const char ADMINISTRATIVE_STATE[] = "AdministrativeState";
const char LINK_SPEED[] = "LinkSpeedMbps";
const char NEIGHBOR_INFO[] = "NeighborInfo";
const char SWITCH_ID[] = "SwitchId";
const char CABLE_ID[] = "CableId";
const char NEIGHBOR_MAC[] = "NeighborMAC";
const char SWITCH[] = "Switch";
const char AUTOSENSE[] = "Autosense";
const char FRAME_SIZE[] = "FrameSize";
const char PORT_CLASS[] = "PortClass";
const char PORT_MODE[] = "PortMode";
const char PORT_TYPE[] = "PortType";
const char MEMBER_OF_PORT[] = "MemberOfPort";
const char VLANS[] = "VLANs";
const char PORT_MEMBERS[] = "PortMembers";
const char PRIMARY_VLAN[] = "PrimaryVLAN";
const char IPv4_ADDRESSES[] = "IPv4Addresses";
const char IPv6_ADDRESSES[] = "IPv6Addresses";
const char PHYSICAL[] = "Physical";
const char LOGICAL[] = "Logical";
const char UPSTREAM[] = "Upstream";
const char FULL_DUPLEX[] = "FullDuplex";
const char ACTIVE_ACLS[] = "ActiveACLs";
const char NEIGHBOR_INTERFACE[] = "NeighborInterface";
const char STATIC_MACS[] = "StaticMACs";
const char LLDP_ENABLED[] = "LLDPEnabled";
const char DCBX_STATE[] = "DCBXState";
const char PRIORITY_FLOW_CONTROL[] = "PriorityFlowControl";
const char ENABLED[] = "Enabled";
const char ENABLED_PRIORITIES[] = "EnabledPriorities";
}


namespace EthernetSwitchPortMetrics {
const char RECEIVED[] = "Received";
const char TRANSMITTED[] = "Transmitted";
const char COLLISIONS[] = "Collisions";
const char PACKETS[] = "Packets";
const char DROPPED_PACKETS[] = "DroppedPackets";
const char ERROR_PACKETS[] = "ErrorPackets";
const char BROADCAST_PACKETS[] = "BroadcastPackets";
const char MULTICAST_PACKETS[] = "MulticastPackets";
const char BYTES[] = "Bytes";
const char ERRORS[] = "Errors";
}

namespace Vlan {
const char VLAN[] = "VLAN";
const char VLAN_ENABLE[] = "VLANEnable";
const char VLAN_ID[] = "VLANId";
const char TAGGED[] = "Tagged";
}

namespace Acl {
const char ACL[] = "ACL";
const char BOUND_PORTS[] = "BoundPorts";
const char RULES[] = "Rules";
const char PORT_ALLOWABLE_VALUES[] = "Port@Redfish.AllowableValues";
const char BIND_ACL[] = "EthernetSwitchACL.Bind";
const char UNBIND_ACL[] = "EthernetSwitchACL.Unbind";
const char PORT[] = "Port";
}

namespace Rule {
const char RULE[] = "Rule";
const char ACTION[] = "Action";
const char CONDITION[] = "Condition";
const char L4_DESTINATION_PORT[] = "L4DestinationPort";
const char IP_DESTINATION[] = "IPDestination";
const char IP_SOURCE[] = "IPSource";
const char IP_ADDRESS[] = "IPv4Address";
const char MASK[] = "Mask";
const char PORT[] = "Port";
const char MAC_DESTINATION[] = "MACDestination";
const char MAC_SOURCE[] = "MACSource";
const char L4_SOURCE_PORT[] = "L4SourcePort";
const char L4_PROTOCOL[] = "L4Protocol";
const char VLAN_ID[] = "VLANId";
const char FORWARD_MIRROR_INTERFACE[] = "ForwardMirrorInterface";
const char MIRROR_PORT_REGION[] = "MirrorPortRegion";
const char MIRROR_TYPE[] = "MirrorType";
const char RULE_ID[] = "RuleId";
}

namespace StaticMac {
const char STATIC_MAC[] = "StaticMAC";
const char VLAN_ID[] = "VLANId";
}

}
}
}

