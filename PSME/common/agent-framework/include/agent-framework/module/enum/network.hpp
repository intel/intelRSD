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
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once

#include "enum_builder.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM SwitchTechnology for Switch class member
 *
 * */
ENUM(SwitchTechnology, uint32_t, Ethernet, PCIe);

/*!
 * @brief ENUM Switch role in the network
 * */
ENUM(SwitchRole, uint32_t, TOR, EOR, Drawer, Unknown);

/*!
 * @brief ENUM PortType for Switch Port class member
 *
 * */
ENUM(PortType, uint32_t, Upstream, Downstream, MeshPort, Unknown);

/*!
 * @brief ENUM PortClass for Switch Port class member
 *
 * */
ENUM(PortClass, uint32_t, Physical, Logical, Reserved);

/*!
 * @brief ENUM PortMode for Switch Port class member
 *
 * */
ENUM(PortMode, uint32_t,
        Unknown, LinkAggregationStatic, LinkAggregationDynamic);

/*!
 * @brief ENUM LinkTechnology for Switch Port class member
 *
 * */
ENUM(LinkTechnology, uint32_t, Ethernet, PCIe, Unknown);

/*!
 * @brief ENUM OperationalState for Switch Port class member
 *
 * */
ENUM(OperationalState, uint32_t, Up, Down, Unknown);

/*!
 * @brief ENUM AdministrativeState for Switch Port class member
 *
 * */
ENUM(AdministrativeState, uint32_t, Up, Down);

/*!
 * @brief ENUM TransportLayerProtocol for Dcbx Application Protocol member
 *
 * */
ENUM(TransportLayerProtocol, uint32_t, TCP, UDP, L2);

/*!
 * @brief ENUM DcbxState for Switch Port class member
 *
 * */
ENUM(DcbxState, uint32_t, Disabled, EnabledIEEE, EnabledCEE);

/*!
 * @brief ENUM NetworkServiceName for Manager class Network Service subclass
 *  member
 * */
ENUM(NetworkServiceName, uint32_t, HTTP, HTTPS, SNMP, VirtualMedia, Telnet,
        SSDP, IPMI, SSH, KVMIP);

/*!
 * @brief ENUM AclAction for Acl rule
 * */
ENUM(AclAction, uint32_t, Permit, Deny, Forward, Mirror);

/*!
 * @brief ENUM AclMirrorType for Acl mirror rule
 * */
ENUM(AclMirrorType, uint32_t, Egress, Ingress, Bidirectional, Redirect);

}
}
}
