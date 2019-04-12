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

#pragma once
namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for EthernetSwitch endpoint.
 */
namespace EthernetSwitch {
extern const char SWITCH[];
extern const char FIRMWARE_NAME[];
extern const char MANUFACTURING_DATE[];
extern const char ROLE[];
extern const char PORTS[];
extern const char SWITCH_ID[];
extern const char ACLS[];
extern const char MAX_ACL_NUMBER[];
extern const char PFC_ENABLED[];
extern const char LLDP_ENABLED[];
extern const char ETS_ENABLED[];
extern const char DCBX_ENABLED[];
extern const char DCBX_SHARED_CONFIGURATION[];
extern const char APPLICATION_PROTOCOL[];
extern const char PRIORITY_TO_PRIORITY_GROUP_MAPPING[];
extern const char BANDWIDTH_ALLOCATION[];
extern const char PRIORITY_GROUP[];
extern const char PRIORITY[];
extern const char PROTOCOL[];
extern const char PORT[];
extern const char BANDWIDTH_PERCENT[];
}

/*!
 * @brief Constant literals for EthernetSwitchPort endpoint.
 */
namespace EthernetSwitchPort {
extern const char PORT[];
extern const char PORT_ID[];
extern const char LINK_TYPE[];
extern const char OPERATIONAL_STATE[];
extern const char ADMINISTRATIVE_STATE[];
extern const char LINK_SPEED[];
extern const char NEIGHBOR_INFO[];
extern const char SWITCH_ID[];
extern const char CABLE_ID[];
extern const char NEIGHBOR_MAC[];
extern const char SWITCH[];
extern const char AUTOSENSE[];
extern const char FRAME_SIZE[];
extern const char PORT_CLASS[];
extern const char PORT_MODE[];
extern const char PORT_TYPE[];
extern const char PRIMARY_VLAN[];
extern const char MEMBER_OF_PORT[];
extern const char VLANS[];
extern const char PORT_MEMBERS[];
extern const char PRIMARY_VLAN[];
extern const char IPv4_ADDRESSES[];
extern const char IPv6_ADDRESSES[];
extern const char PHYSICAL[];
extern const char LOGICAL[];
extern const char UPSTREAM[];
extern const char FULL_DUPLEX[];
extern const char ACTIVE_ACLS[];
extern const char NEIGHBOR_INTERFACE[];
extern const char STATIC_MACS[];
extern const char LLDP_ENABLED[];
extern const char DCBX_STATE[];
extern const char PRIORITY_FLOW_CONTROL[];
extern const char ENABLED[];
extern const char ENABLED_PRIORITIES[];
}

/*!
 * @brief Constant literals for EthernetSwitchPortMetrics endpoint.
 */
namespace EthernetSwitchPortMetrics {
extern const char RECEIVED[];
extern const char TRANSMITTED[];
extern const char COLLISIONS[];
extern const char PACKETS[];
extern const char DROPPED_PACKETS[];
extern const char ERROR_PACKETS[];
extern const char BROADCAST_PACKETS[];
extern const char MULTICAST_PACKETS[];
extern const char BYTES[];
extern const char ERRORS[];
}

/*!
 * @brief Constant Vlan literals for rest endpoints.
 */
namespace Vlan {
extern const char VLAN[];
extern const char VLAN_ENABLE[];
extern const char VLAN_ID[];
extern const char TAGGED[];
}

namespace Acl {
extern const char ACL[];
extern const char BOUND_PORTS[];
extern const char RULES[];
extern const char PORT_ALLOWABLE_VALUES[];
extern const char BIND_ACL[];
extern const char UNBIND_ACL[];
extern const char PORT[];
}

namespace Rule {
extern const char RULE[];
extern const char ACTION[];
extern const char CONDITION[];
extern const char L4_DESTINATION_PORT[];
extern const char IP_DESTINATION[];
extern const char IP_SOURCE[];
extern const char IP_ADDRESS[];
extern const char MASK[];
extern const char PORT[];
extern const char MAC_DESTINATION[];
extern const char MAC_SOURCE[];
extern const char L4_SOURCE_PORT[];
extern const char L4_PROTOCOL[];
extern const char VLAN_ID[];
extern const char FORWARD_MIRROR_INTERFACE[];
extern const char MIRROR_PORT_REGION[];
extern const char MIRROR_TYPE[];
extern const char RULE_ID[];
}

namespace StaticMac {
extern const char STATIC_MAC[];
extern const char VLAN_ID[];
}

}
}
}

