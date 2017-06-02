/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @brief ENUM ResetType as defined in metadata in Resource.ResetType
 *
 */
ENUM(ResetType, uint32_t, None, On, ForceOff, GracefulShutdown, GracefulRestart,
     ForceRestart, Nmi, ForceOn, PushPowerButton);

/*!
 * @brief ENUM DriveType for Drive class member
 */
ENUM(DriveType, uint32_t, HDD, SSD, SMR);

/*!
 * @brief ENUM StorageProtocol types as specified by Redfish "Storage.1.0.0.Protocol"
 */
ENUM(StorageProtocol, uint32_t, PCIe, AHCI, UHCI, SAS, SATA, USB, NVMe, FC, iSCSI, FCoE, NVMeOverFabrics,
     SMB, NFSv3, NFSv4, HTTP, HTTPS, FTP, SFTP);

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
     StorageServices, Processors, Memory, StorageControllers,
     NetworkInterfaces, NetworkDevices, Drives, EthernetSwitchPorts, Vlans, NeighborSwitches, EthernetSwitchPortVlans,
     NetworkDeviceFunctions, PortMembers, PowerZones, ThermalZones, PSUs, Fans, PhysicalDrives,
     LogicalDrives, iSCSITargets, Managers, Ports, PCIeDevices,
     Zones, PCIeFunctions, Switches, Acls, Rules, StaticMacs,
     StorageSubsystems, Tasks, Fabrics, Endpoints);

/*!
 * @brief ENUM CollectionName for attribute class Collection subclass
 * CollectionEntry member
 */
ENUM(CollectionName, uint32_t, None, AuthorizationCertificates, Processors,
     Memory, iSCSITargets, LogicalDrives, NetworkInterfaces, NetworkDevices, Drives,
     NetworkDeviceFunctions, PhysicalDrives, PowerZones, Psus, RemoteEthernetSwitches, StorageControllers,
     StorageServices, EthernetSwitchPorts, Systems, ThermalZones, Vlans, Fans, Chassis,
     Managers, EthernetSwitches, Neighbors, Members, EthernetSwitchPortVlans, PortMembers,
     NeighborSwitches, Ports, PcieDevices, Zones, PcieFunctions, Switches, PortAcls,
     Acls, AclRules, AclPorts, StaticMacs, StorageSubsystems, Tasks, Fabrics,
     Endpoints);

/*!
 * @brief ENUM State for attribute class Status State member
 */
ENUM(State, uint32_t, Enabled, Disabled, StandbyOffline, StandbySpare, InTest, Starting, Absent, UnavailableOffline,
     Deferring, Quiesced, Updating);

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
     ManagementController, EnclosureManager, BMC, RackManager, AuxiliaryController);

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
     Fan, PhysicalDrive, IscsiTarget, LogicalDrive, Manager, Memory,
     NetworkInterface, NetworkDevice, PowerZone, Processor, PSU, EthernetSwitch, RemoteEthernetSwitch,
     NetworkDeviceFunction, StorageController, StorageServices, EthernetSwitchPort, System,
     ThermalZone, Vlan, EthernetSwitchPortVlan, PortMember, NeighborSwitch,
     Switch, PcieDevice, Zone, PcieFunction, Port,
     Acl, AclRule, StaticMac, StorageSubsystem, Task, Fabric, Endpoint);

/*!
 * @brief ENUM IdentifierType for Identifier attribute durableNameFormat field
 */
ENUM(IdentifierType, uint32_t, NAA, iQN, FC_WWN, UUID, EUI);

/*!
 * @brief ENUM IndicatorLed for Chassis and Drive and Computer System indicatorLed field
 */
ENUM(IndicatorLed, uint32_t, Unknown, Lit, Blinking, Off);

/*!
 * @brief ENUM StatusIndicator for Drive statusIndicator field
 */
ENUM(StatusIndicator, uint32_t, Ok, Fail, Rebuild, PredictiveFailureAnalysis,
     Hotspare, InACriticalArray, InAFailedArray);

/*!
 * @brief ENUM HotspareType for Drive hotSpare field
 */
ENUM(HotspareType, uint32_t, None, Global, Chassis, Dedicated);

/*!
 * @brief ENUM EncryptionAbility for Drive encryptionAbility field
 */
ENUM(EncryptionAbility, uint32_t, None, SelfEncryptingDrive, Other);

/*!
 * @brief ENUM EncryptionStatus for Drive encryptionStatus field
 */
ENUM(EncryptionStatus, uint32_t, Unencrypted, Unlocked, Locked, Foreign);


/*!
 * @brief ENUM TaskState for Task class
 * */
ENUM(TaskState, uint32_t, New, Starting, Running, Suspended, Interrupted, Pending,
    Stopping, Completed, Killed, Exception, Service);


/*!
 * @brief ENUM ManagerPowerState state enum for Manager class
 * */
ENUM(ManagerPowerState, uint32_t, On, Off);

}
}
}
