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
 * @file common.hpp
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once
#include "enum_builder.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM DriveType for Physical Drive class member
 */
ENUM(DriveType, uint32_t, HDD, SSD, NVMe);

/*!
 * @brief ENUM Interface for Storage Controller class member
 * and Physical Drive class member
 */
ENUM(DriveInterface, uint32_t, PCIe, SAS, SATA);

/*!
 * @brief ENUM IPv4AddressOrigin for attribute IPv4Address class member
 */
ENUM(Ipv4AddressOrigin, uint32_t, DHCP, Static);

/*!
 * @brief ENUM IPv6AddressOrigin for attribute IPv6Address class member
 */
ENUM(Ipv6AddressOrigin, uint32_t, DHCP, Static, SLAAC);

/*!
 * @brief ENUM IPv6AddressState for attribute IPv6Address class member
 */
ENUM(Ipv6AddressState, uint32_t, Preferred, Deprecated, Tentative, Failed);

/*!
 * @brief ENUM CollectionType for attribute class Collection subclass
 * CollectionEntry member
 */
ENUM(CollectionType, uint32_t, None, Chassis, Systems, EthernetSwitches,
        StorageServices, Processors, Dimms, StorageControllers,
        NetworkInterfaces, Drives, Ports, Vlans, NeighborSwitches, PortVlans,
        PortMembers, PowerZones, ThermalZones, PSUs, Fans, PhysicalDrives,
        LogicalDrives, iSCSITargets, Managers, MemoryChunks, Acls, Rules, StaticMacs,
        PCIePorts, PCIeDevices, PCIeZones, PCIeFunctions, PCIeSwitches);

/*!
 * @brief ENUM CollectionName for attribute class Collection subclass
 * CollectionEntry member
 */
ENUM(CollectionName, uint32_t, None, AuthorizationCertificates, Processors,
    Memories, iSCSITargets, LogicalDrives, NetworkInterfaces, Drives,
    PhysicalDrives, PowerZones, Psus, RemoteSwitches, StorageControllers,
    StorageServices, Ports, Systems, ThermalZones, Vlans, Fans, Chassis,
    Managers, EthernetSwitches, Neighbors, Members, PortVlans, PortMembers,
    PortAcls, Switches, NeighborSwitches, MemoryChunks, Acls, AclRules, StaticMacs,
    AclPorts, PciePorts, PcieDevices, PcieZones, PcieFunctions, PcieSwitches);

/*!
 * @brief ENUM State for attribute class Status State member
 */
ENUM(State, uint32_t, Enabled, Disabled, StandbyOffline, InTest, Starting, Absent);

/*!
 * @brief ENUM Health for attribute class Status Health member
 */
ENUM(Health, uint32_t, OK, Warning, Critical);

/*!
 * @brief ENUM Notification for attribute class ComponentNotification
 * notification member
 */
ENUM(Notification, uint32_t, Add, Remove, Update);

/*!
 * @brief ENUM ComponentType for attribute class ComponentNotification type
 * member
 */
ENUM(ComponentType, uint32_t, Chassis, ThermalZone, PowerZone, Fan, PSU,
        Drive, PhysicalDrive);

/*!
 * @brief ENUM ManagerInfoType describes type of the manager
 */
ENUM(ManagerInfoType, uint32_t,
     ManagementController, EnclosureManager, BMC, RackManager,
     EthernetSwitchManager, StorageSystemManager, AuxiliaryController);

/*!
 * @brief ENUM SerialConsoleSupprtedType describes type of communication with
 * serial
 */
ENUM(SerialConsoleSupprtedType, uint32_t,
     Telnet, IPMI, SSH);

/*!
 * @brief ENUM GraphicalConsoleSupprtedType describes type of communication with
 * graphical console
 */
ENUM(GraphicalConsoleSupprtedType, uint32_t,
     KVMIP, dummy);

/*!
 * @brief ENUM CommandShellSupprtedType describes type of communication with
 * graphical console
 */
ENUM(CommandShellSupprtedType, uint32_t,
     VirtualMedia, Telnet, SSH);

/*!
 * @brief ENUM Component describes type of the object
 */
ENUM(Component, uint32_t, None, Root, AuthorizationCertificate, Chassis, Drive,
        Fan, PhysicalDrive, IscsiTarget, LogicalDrive, Manager, Dimm,
        NetworkInterface, PowerZone, Processor, PSU, Switch, RemoteSwitch,
        StorageController, StorageServices, SwitchPort, System,
        ThermalZone, Vlan, PortVlan, PortMember, NeighborSwitch, MemoryChunk,
        Acl, AclRule, StaticMac);
/*!
 * @brief ENUM IndicatorLed for Chasiss and Computer System indicatorLed field
 *
 */
ENUM(IndicatorLed, uint32_t, Unknown, Lit, Blinking, Off);

}
}
}

