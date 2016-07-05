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
 * */

#include "psme/rest-ref/constants/constants.hpp"

namespace app {
namespace rest {
namespace constants {

namespace Metadata {
    const char METADATA_ROOT_FILE[] = "metadata_root.xml";
}

namespace PathParam {
    const char REDFISH[] = "redfish";
    const char METADATA_ROOT[] = "$metadata";
    const char METADATA[] = "metadata";
    const char BASE_URL[] = "/redfish/v1";
    const char METADATA_FILE[] = "metadataFile";
    const char SUBSCRIPTION_ID[] = "subscriptionId";
    const char CHASSIS_ID[] = "chassisId";
    const char POWER_ZONE_ID[] = "powerZoneId";
    const char SYSTEM_ID[] = "systemId";
    const char ADAPTER_ID[] = "adapterId";
    const char DEVICE_ID[] = "deviceId";
    const char SERVICE_ID[] = "serviceId";
    const char DRIVE_ID[] = "driveId";
    const char LOGICAL_DRIVE_ID[] = "logicalDriveId";
    const char REMOTE_TARGET_ID[] = "remoteTargetId";
    const char MANAGER_ID[] = "managerId";
    const char NIC_ID[] = "nicId";
    const char NODE_ID[] = "nodeId";
    const char ETHERNET_SWITCH_ID[] = "ethernetSwitchId";
    const char SWITCH_PORT_ID[] = "switchPortId";
    const char VLAN_ID[] = "vlanId";
    const char PROCESSOR_ID[] = "processorId";
    const char MEMORY_CHUNK_ID[] = "memoryChunkId";
    const char SIMPLE_STORAGE_ID[] = "simpleStorageId";
    const char DIMM_CONFIG_ID[] = "dimmConfigId";
    const char ACL_ID[] = "aclId";
    const char RULE_ID[] = "ruleId";
    const char STATIC_MAC_ID[] = "staticMacId";

    const char PATH_SEP = '/';
    const char VARIABLE_BEGIN = '{';
    const char VARIABLE_END = '}';
    const char REGEX_MARK = ':';
    const char ID_REGEX[] = "[0-9]+";
    const char XML_REGEX[] = ".*.xml";
}

namespace Common {
     const char INTEL[] = "Intel Corporation";
     const char ODATA_CONTEXT[] = "@odata.context";
     const char ODATA_ID[] = "@odata.id";
     const char ODATA_TYPE[] = "@odata.type";
     const char ID[] = "Id";
     const char NAME[] = "Name";
     const char LINKS[] = "Links";
     const char MANUFACTURER[] = "Manufacturer";
     const char MODEL[] = "Model";
     const char SERIAL[] = "SerialNumber";
     const char PART_NUMBER[] = "PartNumber";
     const char ASSET_TAG[] = "AssetTag";
     const char STATUS[] = "Status";
     const char STATE[] = "State";
     const char HEALTH[] = "Health";
     const char HEALTH_ROLLUP[] = "HealthRollup";
     const char ENUM_STATUS[] = "EnumStatus";
     const char OEM[] = "Oem";
     const char RACKSCALE[] = "Intel_RackScale";
     const char API_VERSION[] = "ApiVersion";
     const char LOCATION[] = "Location";
     const char CONTAINED_BY[] = "ContainedBy";
     const char MANAGED_BY[] = "ManagedBy";
     const char DESCRIPTION[] = "Description";
     const char VENDOR_ID[] = "VendorId";
     const char DEVICE_ID[] = "DeviceId";
     const char FIRMWARE_VERSION[] = "FirmwareVersion";
     const char UUID[] = "UUID";
     const char ACTIONS[] = "Actions";
}

namespace Collection {
     const char ODATA_COUNT[] = "Members@odata.count";
     const char ODATA_ID[] = "@odata.id";
     const char MEMBERS[] = "Members";
}

namespace Root {
     const char REDFISH_VERSION[] = "RedfishVersion";
     const char EVENT_SERVICE[] = "EventService";
     const char CHASSIS[] = "Chassis";
     const char SYSTEMS[] = "Systems";
     const char SERVICES[] = "Services";
     const char MANAGERS[] = "Managers";
     const char ETHERNET_SWITCHES[] = "EthernetSwitches";
}

namespace Redfish {
     const char V1[] = "v1";
}

namespace Chassis {
     const char CHASSIS_TYPE[] = "ChassisType";
     const char SKU[] = "SKU";
     const char INDICATOR_LED[] = "IndicatorLED";
     const char LOCATION[] = "Location";
     const char PARENT_ID[] = "ParentId";
     const char CONTAINS[] = "Contains";
     const char COMPUTER_SYSTEMS[] = "ComputerSystems";
     const char SWITCHES[] = "Switches";
     const char MANAGERS_IN[] = "ManagersInChassis";
     const char THERMAL_ZONES[] = "ThermalZones";
     const char POWER_ZONES[] = "PowerZones";
}

namespace Zone {
     const char RACK_LOCATION[] = "RackLocation";
     const char UHEIGHT[] = "Uheight";
     const char ULOCATION[] = "ULocation";
     const char UNIT[] = "Unit";
     const char XLOCATION[] = "XLocation";
     const char MEMBER_ID[] = "MemberId";
     const char RELATED_ITEM[] = "RelatedItem";
}

namespace PowerZone {
     const char POWER_CONTROL[] = "PowerControl";
     const char MAX_PSUS_SUPPORTED[] = "MaxPSUsSupported";
     const char POWER_AVAILABLE[] = "PowerAvailableWatts";
     const char POWER_CAPACITY[] = "PowerCapacity";
     const char POWER_CONSUMED[] = "PowerConsumedWatts";
     const char POWER_SUPPLIES[] = "PowerSupplies";
     const char LAST_POWER_OUTPUT[] = "LastPowerOutputWatts";
}

namespace ThermalZone {
     const char FANS[] = "Fans";
     const char FAN_NAME[] = "FanName";
     const char PHYSICAL_CONTEXT[] = "PhysicalContext";
     const char READING[] = "Reading";
     const char READING_UNITS[] = "ReadingUnits";
     const char DESIRED_SPEED_RPM[] = "DesiredSpeedRPM";
     const char MAX_FANS_SUPPORTED[] = "MaxFansSupported";
     const char PWM[] = "PWM";
     const char POLICY[] = "Policy";
     const char TEMPERATURES[] = "Temperatures";
     const char READING_CELCIUS[] = "Intake";
     const char SENSOR_NUMBER[] = "SensorNumber";
}

namespace Manager {
     const char TARGET[] = "target";
     const char MANAGER_TYPE[] = "ManagerType";
     const char DATE_TIME[] = "DateTime";
     const char DATE_TIME_LOCAL_OFFSET[] = "DateTimeLocalOffset";
     const char SERVICE_ENTRY_POINT_UUID[] = "ServiceEntryPointUUID";
     const char GRAPHICAL_CONSOLE[] = "GraphicalConsole";
     const char MAX_CONCURRENT_SESSIONS[] = "MaxConcurrentSessions";
     const char CONNECT_TYPES_SUPPORTED[] = "ConnectTypesSupported";
     const char ENABLED[] = "Enabled";
     const char SERVICE_ENABLED[] = "ServiceEnabled";
     const char SERIAL_CONSOLE[] = "SerialConsole";
     const char COMMAND_SHELL[] = "CommandShell";
     const char MANAGER_FOR_CHASSIS[] = "ManagerForChassis";
     const char MANAGER_FOR_SERVERS[] = "ManagerForServers";
     const char MANAGER_FOR_SWITCHES[] = "ManagerForSwitches";
     const char MANAGER_LOCATION[] = "ManagerInChassis";
     const char MANAGER_FOR_SERVICES[] = "ManagerForServices";
     const char MANAGER_FOR_REMOTE_TARGETS[] = "ManagerForRemoteTargets";
     const char MANAGER_FOR_LOGICAL_DRIVES[] = "ManagerForLogicalDrives";
     const char MANAGER_FOR_PHYSICAL_DRIVES[] = "ManagerForPhysicalDrives";
     const char NETWORK_PROTOCOL[] = "NetworkProtocol";
     const char ETHERNET_INTERFACES[] = "EthernetInterfaces";
}

namespace NetworkProtocol {
     const char HOST_NAME[] = "HostName";
     const char FQDN[] = "FQDN";
     const char HTTP[] = "HTTP";
     const char HTTPS[] = "HTTPS";
     const char SNMP[] = "SNMP";
     const char VIRTUAL_MEDIA[] = "VirtualMedia";
     const char TELNET[] = "Telnet";
     const char SSDP[] = "SSDP";
     const char IPMI[] = "IPMI";
     const char SSH[] = "SSH";
     const char KVMIP[] = "KVMIP";
     const char PROTOCOL_ENABLED[] = "ProtocolEnabled";
     const char PORT[] = "Port";
     const char NOTIFY_IPV6_SCOPE[] = "NotifyIPv6Scope";
     const char NOTIFY_MULTICAST_INTERVAL[] = "NotifyMulticastIntervalSeconds";
     const char NOTIFY_TTL[] = "NotifyTTL";
}

namespace IpAddress {
    //common
     const char ADDRESS[] = "Address";
     const char ADDRESS_ORIGIN[] = "AddressOrigin";
     const char GATEWAY[] = "Gateway";
     const char HOST_NAME[] = "HostName";
     const char FQDN[] = "FQDN";
     //IPv4:
     const char SUBNET_MASK[] = "SubnetMask";
     //IPv6:
     const char PREFIX_LENGTH[] = "PrefixLength";
     const char SCOPE[] = "Scope";
     const char ADDRESS_STATE[] = "AddressState";
}

namespace NetworkInterface {
     const char MAC_ADDRESS[] = "MACAddress";
     const char PERMANENT_MAC_ADDRESS[] = "PermanentMACAddress";
     const char SPEED[] = "SpeedMbps";
     const char AUTO_NEG[] = "AutoNeg";
     const char FULL_DUPLEX[] = "FullDuplex";
     const char MTU_SIZE[] = "MTUSize";
     const char HOST_NAME[] = "HostName";
     const char FQDN[] = "FQDN";
     const char MAX_IPv6_STATIC_ADDRESSES[] = "MaxIPv6StaticAddresses";
     const char IPv4_ADDRESSES[] = "IPv4Addresses";
     const char IPv6_ADDRESS_POLICY_TABLE[] = "IPv6AddressPolicyTable";
     const char PREFIX[] = "Prefix";
     const char PRECEDENCE[] = "Precedence";
     const char LABEL[] = "Label";
     const char IPv6_STATIC_ADDRESSES[] = "IPv6StaticAddresses";
     const char IPv6_ADDRESSES[] = "IPv6Addresses";
     const char IPv6_DEFAULT_GATEWAY[] = "IPv6DefaultGateway";
     const char NAME_SERVERS[] = "NameServers";
     const char INTERFACE_ENABLED[] = "InterfaceEnabled";
     const char UEFI_DEVICE_PATH[] = "UefiDevicePath";
     const char VLANS[] = "VLANs";
     const char NEIGHBOR_PORT[] = "NeighborPort";

}

namespace EventService {
     const char SERVICE_ENABLED[] = "ServiceEnabled";
     const char DELIVERY_RETRY_ATTEMPTS[] = "DeliveryRetryAttempts";
     const char DELIVERY_RETRY_INTERVAL_SECONDS[] = "DeliveryRetryIntervalSeconds";
     const char EVENT_TYPES_FOR_SUBSCRIPTION[] = "EventTypesForSubscription";
     const char STATUS_CHANGE[] = "StatusChange";
     const char RESOURCE_UPDATED[] = "ResourceUpdated";
     const char RESOURCE_ADDED[] = "ResourceAdded";
     const char RESOURCE_REMOVED[] = "ResourceRemoved";
     const char ALERT[] = "Alert";
     const char SUBSCRIPTIONS[] = "Subscriptions";
     const char ACTIONS[] = "Actions";
     const char SEND_TEST_EVENT[] = "#EventService.SendTestEvent";
     const char TARGET[] = "target";
     const char ALLOWABLE_EVENT_TYPES[] = "EventType@Redfish.AllowableValues";
}

namespace EventSubscription {
     const char DESTINATION[] = "Destination";
     const char CONTEXT[] = "Context";
     const char PROTOCOL[] = "Protocol";
     const char EVENT_TYPES[] = "EventTypes";
}

namespace Capability {
    extern const char CHASSIS[] = "Chassis";
    extern const char COMPUTE[] = "Compute";
    extern const char NETWORK[] = "Network";
    extern const char STORAGE[] = "Storage";
}

}
}
}
