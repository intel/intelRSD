
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
 * @file network.cpp
 * @brief Contains constants used by network agents
 * */

#include "agent-framework/module-ref/constants/network.hpp"

namespace agent_framework {
namespace model {
namespace literals {

constexpr const char NeighborInfo::NEIGHBOR_INFO[];
constexpr const char NeighborInfo::SWITCH_IDENTIFIER[];
constexpr const char NeighborInfo::PORT_IDENTIFIER[];
constexpr const char NeighborInfo::CABLE_ID[];

constexpr const char SwitchPortMembers::PORT[];
constexpr const char SwitchPortMembers::MEMBERS[];
constexpr const char SwitchPortMembers::OEM[];

constexpr const char SwitchPort::PORT[];
constexpr const char SwitchPort::SWITCH[];
constexpr const char SwitchPort::STATUS[];
constexpr const char SwitchPort::PORT_IDENTIFIER[];
constexpr const char SwitchPort::MODE[];
constexpr const char SwitchPort::MEMBERS[];
constexpr const char SwitchPort::PORT_CLASS[];
constexpr const char SwitchPort::PORT_TYPE[];
constexpr const char SwitchPort::PORT_MODE[];
constexpr const char SwitchPort::LINK_TECHNOLOGY[];
constexpr const char SwitchPort::LINK_SPEED_MBPS[];
constexpr const char SwitchPort::MAX_SPEED_MBPS[];
constexpr const char SwitchPort::OPERATIONAL_STATE[];
constexpr const char SwitchPort::ADMINISTRATIVE_STATE[];
constexpr const char SwitchPort::PORT_WIDTH[];
constexpr const char SwitchPort::FRAME_SIZE[];
constexpr const char SwitchPort::AUTO_SENSE[];
constexpr const char SwitchPort::FULL_DUPLEX[];
constexpr const char SwitchPort::IS_MANAGEMENT_PORT[];
constexpr const char SwitchPort::LAST_ERROR_CODE[];
constexpr const char SwitchPort::ERROR_CLEARED[];
constexpr const char SwitchPort::LAST_STATE_CHANGE_TIME[];
constexpr const char SwitchPort::MAC_ADDRESS[];
constexpr const char SwitchPort::IPV4_ADDRESS[];
constexpr const char SwitchPort::IPV6_ADDRESS[];
constexpr const char SwitchPort::NEIGHBOR_INFO[];
constexpr const char SwitchPort::NEIGHBOR_MAC[];
constexpr const char SwitchPort::VLAN_ENABLE[];
constexpr const char SwitchPort::DEFAULT_VLAN[];
constexpr const char SwitchPort::COLLECTIONS[];
constexpr const char SwitchPort::OEM[];

constexpr const char Switch::SWITCH[];
constexpr const char Switch::STATUS[];
constexpr const char Switch::SWITCH_IDENTIFIER[];
constexpr const char Switch::TECHNOLOGY[];
constexpr const char Switch::MAC_ADDRESS[];
constexpr const char Switch::FIRMWARE_NAME[];
constexpr const char Switch::FIRMWARE_VERSION[];
constexpr const char Switch::ROLE[];
constexpr const char Switch::FRU_INFO[];
constexpr const char Switch::MANUFACTURING_DATE[];
constexpr const char Switch::LOCATION[];
constexpr const char Switch::CHASSIS[];
constexpr const char Switch::MAX_ACL_NUMBER[];
constexpr const char Switch::COLLECTIONS[];
constexpr const char Switch::OEM[];

constexpr const char PortVlan::PORT_VLAN[];
constexpr const char PortVlan::PORT[];
constexpr const char PortVlan::STATUS[];
constexpr const char PortVlan::VLAN_ID[];
constexpr const char PortVlan::VLAN_NAME[];
constexpr const char PortVlan::VLAN_ENABLE[];
constexpr const char PortVlan::TAGGED[];
constexpr const char PortVlan::OEM[];

constexpr const char Vlan::VLAN[];
constexpr const char Vlan::SWITCH[];
constexpr const char Vlan::STATUS[];
constexpr const char Vlan::VLAN_ID[];
constexpr const char Vlan::VLAN_NAME[];
constexpr const char Vlan::VLAN_ENABLE[];
constexpr const char Vlan::OEM[];

constexpr const char RemoteSwitch::SWITCH[];
constexpr const char RemoteSwitch::STATUS[];
constexpr const char RemoteSwitch::SWITCH_IDENTIFIER[];
constexpr const char RemoteSwitch::MAC_ADDRESS[];
constexpr const char RemoteSwitch::NEXT_HOP[];
constexpr const char RemoteSwitch::OEM[];

constexpr const char NextHop::METRIC[];
constexpr const char NextHop::PORT_IDENTIFIER[];
constexpr const char NextHop::IPV4_ADDRESS[];
constexpr const char NextHop::IPV6_ADDRESS[];

constexpr const char Acl::SWITCH[];
constexpr const char Acl::PORTS[];
constexpr const char Acl::OEM[];
constexpr const char Acl::ACL[];
constexpr const char Acl::STATUS[];
constexpr const char Acl::COLLECTIONS[];

constexpr const char AclRule::RULE[];
constexpr const char AclRule::STATUS[];
constexpr const char AclRule::OEM[];
constexpr const char AclRule::RULE_ID[];
constexpr const char AclRule::ACTION[];
constexpr const char AclRule::FORWARD_MIRROR_PORT[];
constexpr const char AclRule::MIRRORED_PORTS[];
constexpr const char AclRule::MIRROR_TYPE[];
constexpr const char AclRule::VLAN_ID[];
constexpr const char AclRule::SOURCE_IP[];
constexpr const char AclRule::DESTINATION_IP[];
constexpr const char AclRule::SOURCE_MAC[];
constexpr const char AclRule::DESTINATION_MAC[];
constexpr const char AclRule::SOURCE_L4_PORT[];
constexpr const char AclRule::DESTINATION_L4_PORT[];
constexpr const char AclRule::PROTOCOL[];
constexpr const char AclRule::ID[];
constexpr const char AclRule::ADDRESS[];
constexpr const char AclRule::MASK[];
constexpr const char AclRule::PORT[];

constexpr const char StaticMac::STATIC_MAC[];
constexpr const char StaticMac::PORT[];
constexpr const char StaticMac::OEM[];
constexpr const char StaticMac::STATUS[];
constexpr const char StaticMac::ADDRESS[];
constexpr const char StaticMac::VLAN_ID[];

}
}
}
