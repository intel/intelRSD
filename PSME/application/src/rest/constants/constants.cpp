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

#include "psme/rest/constants/constants.hpp"



namespace psme {
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
const char OEM_INTEL_RACKSCALE[] = "Oem/Intel_RackScale";
const char METADATA_FILE[] = "metadataFile";
const char SUBSCRIPTION_ID[] = "subscriptionId";
const char CHASSIS_ID[] = "chassisId";
const char SYSTEM_ID[] = "systemId";
const char ADAPTER_ID[] = "adapterId";
const char DEVICE_ID[] = "deviceId";
const char SERVICE_ID[] = "serviceId";
const char STORAGE_POOL_ID[] = "storagePoolId";
const char CAPACITY_SOURCE_ID[] = "capacitySourceId";
const char VOLUME_ID[] = "volumeId";
const char DRIVE_ID[] = "driveId";
const char MANAGER_ID[] = "managerId";
const char NIC_ID[] = "nicId";
const char NETWORK_INTERFACE_ID[] = "networkInterfaceId";
const char NETWORK_DEVICE_FUNCTION_ID[] = "networkDeviceFunctionId";
const char NODE_ID[] = "nodeId";
const char ETHERNET_SWITCH_ID[] = "ethernetSwitchId";
const char SWITCH_PORT_ID[] = "switchPortId";
const char VLAN_ID[] = "vlanId";
const char PROCESSOR_ID[] = "processorId";
const char SIMPLE_STORAGE_ID[] = "simpleStorageId";
const char MEMORY_ID[] = "memoryId";
const char FABRIC_ID[] = "fabricId";
const char ENDPOINT_ID[] = "endpointId";
const char SWITCH_ID[] = "switchId";
const char FUNCTION_ID[] = "functionId";
const char ZONE_ID[] = "zoneId";
const char PORT_ID[] = "portId";
const char FUNCTIONS[] = "Functions";
const char ACL_ID[] = "aclId";
const char RULE_ID[] = "ruleId";
const char STORAGE_ID[] = "storageId";
const char STATIC_MAC_ID[] = "staticMacId";
const char TASK_ID[] = "taskId";
const char ODATA[] = "odata";
const char MESSAGE_REGISTRY_FILE_ID[] = "messageRegistryFileId";
const char PSU_ID[] = "psuId";
const char METRIC_DEFINITION_ID[] = "metricDefinitionId";
const char METRIC_REPORT_DEFINITION_ID[] = "metricReportDefinitionId";
const char TRIGGER_ID[] = "triggerId";
const char SESSION_ID[] = "sessionId";
const char ACCOUNT_ID[] = "accountId";
const char ROLE_ID[] = "roleId";
const char INTEL_RACKSCALE_REGISTRY_URL[] = "/registries/Intel_RackScale.1.0.0.json";

const char PATH_SEP = '/';
const char VARIABLE_BEGIN = '{';
const char VARIABLE_END = '}';
const char REGEX_MARK = ':';
const char ID_REGEX[] = "[0-9]+";
const char XML_REGEX[] = ".*.xml";
const char STRING_ID_REGEX[] = "[A-Za-z]+";

}

namespace Common {
const char INTEL[] = "Intel Corporation";
const char ODATA_CONTEXT[] = "@odata.context";
const char ODATA_ID[] = "@odata.id";
const char ODATA_TYPE[] = "@odata.type";
const char ID[] = "Id";
const char MEMBER_ID[] = "MemberId";
const char NAME[] = "Name";
const char LINKS[] = "Links";
const char MANUFACTURER[] = "Manufacturer";
const char MODEL[] = "Model";
const char SERIAL_NUMBER[] = "SerialNumber";
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
const char HASH[] = "#";
const char CHASSIS[] = "Chassis";
const char RESET_TYPE[] = "ResetType";
const char ALLOWABLE_RESET_TYPES[] = "ResetType@Redfish.AllowableValues";
const char TARGET[] = "target";
const char SKU[] = "SKU";
const char RELATED_ITEM[] = "RelatedItem";
const char MAC_ADDRESS[] = "MACAddress";
const char METRICS[] = "Metrics";
const char STORAGE_SERVICES[] = "StorageServices";
const char REDUNDANCY[] = "Redundancy";
const char IDENTIFIERS[] = "Identifiers";
const char DURABLE_NAME[] = "DurableName";
const char DURABLE_NAME_FORMAT[] = "DurableNameFormat";
const char LUN[] = "LUN";
const char ADMINISTRATOR[] = "Administrator";
const char PERFORMANCE_CONFIGURATION[] = "PerformanceConfiguration";
const char CURRENT_CONFIGURATION_ID[] = "CurrentConfigurationId";
const char CONFIGURATIONS[] = "Configurations";

}

namespace ErrorMessage {
const char ERROR[] = "error";
const char CODE[] = "code";
const char MESSAGE[] = "message";
const char EXTENDED_INFO[] = "@Message.ExtendedInfo";
}

namespace MessageObject {
const char JSON_POINTER_PREFIX[] = "#/";
const char MESSAGE_ID[] = "MessageId";
const char MESSAGE[] = "Message";
const char MESSAGE_ARGS[] = "MessageArgs";
const char RELATED_PROPERTIES[] = "RelatedProperties";
const char SEVERITY[] = "Severity";
const char RESOLUTION[] = "Resolution";
}

namespace Collection {
const char ODATA_COUNT[] = "Members@odata.count";
const char MEMBERS[] = "Members";
}

namespace Root {
const char REDFISH_VERSION[] = "RedfishVersion";
const char EVENT_SERVICE[] = "EventService";
const char SESSION_SERVICE[] = "SessionService";
const char TASKS[] = "Tasks";
const char SYSTEMS[] = "Systems";
const char STORAGE_SERVICES[] = "StorageServices";
const char MANAGERS[] = "Managers";
const char ETHERNET_SWITCHES[] = "EthernetSwitches";
const char REGISTRIES[] = "Registries";
const char MESSAGE_REGISTRY[] = "MessageRegistry";
const char FABRICS[] = "Fabrics";
const char TELEMETRY_SERVICE[] = "TelemetryService";
const char UPDATE_SERVICE[] = "UpdateService";
const char SERVICE[] = "Service";
const char ACCOUNT_SERVICE[] = "AccountService";
}

namespace Redfish {
const char V1[] = "v1";
}

namespace OdataServiceDocument {
const char VALUE[] = "value";
const char NAME[] = "name";
const char KIND[] = "kind";
const char URL[] = "url";
}

namespace Manager {
const char MANAGER_TYPE[] = "ManagerType";
const char POWER_STATE[] = "PowerState";
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
const char MANAGER_FOR_ETHERNET_SWITCHES[] = "ManagerForEthernetSwitches";
const char MANAGER_FOR_FABRICS[] = "ManagerForFabrics";
const char MANAGER_IN_CHASSIS[] = "ManagerInChassis";
const char MANAGER_FOR_SERVICES[] = "ManagerForServices";
const char NETWORK_PROTOCOL[] = "NetworkProtocol";
const char ETHERNET_INTERFACES[] = "EthernetInterfaces";
const char HASH_MANAGER_RESET[] = "#Manager.Reset";
const char MANAGER_RESET[] = "Manager.Reset";
const char HASH_LOAD_FACTORY_DEFAULTS_ACTION[] = "#Intel_RackScale.LoadFactoryDefaults";
const char LOAD_FACTORY_DEFAULTS_ACTION[] = "Intel_RackScale.LoadFactoryDefaults";
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
// Common
const char ADDRESS[] = "Address";
const char ADDRESS_ORIGIN[] = "AddressOrigin";
const char GATEWAY[] = "Gateway";
const char HOST_NAME[] = "HostName";
const char FQDN[] = "FQDN";
// IPv4:
const char SUBNET_MASK[] = "SubnetMask";
// IPv6:
const char PREFIX_LENGTH[] = "PrefixLength";
const char SCOPE[] = "Scope";
const char ADDRESS_STATE[] = "AddressState";
}

namespace NetworkInterface {
const char PERMANENT_MAC_ADDRESS[] = "PermanentMACAddress";
const char SPEED_MBPS[] = "SpeedMbps";
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
const char VLAN[] = "VLAN";
const char NEIGHBOR_PORT[] = "NeighborPort";
const char SUPPORTED_PROTOCOLS[] = "SupportedProtocols";
const char LINK_STATUS[] = "LinkStatus";
}

namespace ActionInfo {
const char REDFISH_ACTION_INFO[] = "@Redfish.ActionInfo";
const char PARAMETERS[] = "Parameters";
const char REQUIRED[] = "Required";
const char DATA_TYPE[] = "DataType";
const char ALLOWABLE_VALUES[] = "AllowableValues";
}

namespace UpdateService {
const char SERVICE_ENABLED[] = "ServiceEnabled";
const char HTTP_PUSH_URI[] = "HttpPushUri";
const char HASH_UPDATE_SERVICE_SIMPLE_UPDATE[] = "#UpdateService.SimpleUpdate";
const char SIMPLE_UPDATE[] = "SimpleUpdate";
const char SIMPLE_UPDATE_ACTION_INFO[] = "SimpleUpdateActionInfo";
const char IMAGE_URI[] = "ImageURI";
const char TRANSFER_PROTOCOL[] = "TransferProtocol";
const char TARGETS[] = "Targets";
}

namespace EventService {
const char SERVICE_ENABLED[] = "ServiceEnabled";
const char DELIVERY_RETRY_ATTEMPTS[] = "DeliveryRetryAttempts";
const char DELIVERY_RETRY_INTERVAL_SECONDS[] = "DeliveryRetryIntervalSeconds";
const char EVENT_TYPES_FOR_SUBSCRIPTION[] = "EventTypesForSubscription";
const char SUBSCRIPTIONS[] = "Subscriptions";
}

namespace EventArray {
const char CONTEXT[] = "Context";
const char EVENTS[] = "Events";
}

namespace Event {
const char EVENT_TYPE[] = "EventType";
const char EVENT_ID[] = "EventId";
const char EVENT_TIMESTAMP[] = "EventTimestamp";
const char SEVERITY[] = "Severity";
const char MESSAGE[] = "Message";
const char MESSAGE_ID[] = "MessageId";
const char MESSAGE_ARGS[] = "MessageArgs";
const char CONTEXT[] = "Context";
const char ORIGIN_OF_CONDITION[] = "OriginOfCondition";
}

namespace EventSubscription {
const char DESTINATION[] = "Destination";
const char CONTEXT[] = "Context";
const char PROTOCOL[] = "Protocol";
const char EVENT_TYPES[] = "EventTypes";
const char ORIGIN_RESOURCES[] = "OriginResources";
}

namespace Capability {
const char CHASSIS[] = "Chassis";
const char COMPUTE[] = "Compute";
const char NETWORK[] = "Network";
const char STORAGE[] = "Storage";
const char PNC[] = "Pnc";
const char RMM[] = "Rmm";
const char NVME[] = "NVMe";
const char FPGAOF[] = "FPGA-oF";
}

namespace TaskService {
const char TASK_SERVICE[] = "TaskService";
const char SERVICE_ENABLED[] = "ServiceEnabled";
const char DATETIME[] = "DateTime";
const char COMPLETED_TASK_OVERWRITE_POLICY[] = "CompletedTaskOverWritePolicy";
const char LIFE_CYCLE_EVENT_ON_TASK_STATE_CHANGE[] = "LifeCycleEventOnTaskStateChange";
const char TASKS[] = "Tasks";
}

namespace Task {
const char TASK_STATE[] = "TaskState";
const char START_TIME[] = "StartTime";
const char END_TIME[] = "EndTime";
const char TASK_STATUS[] = "TaskStatus";
const char MESSAGES[] = "Messages";
}

namespace Monitor {
const char MONITOR[] = "Monitor";
}

namespace MessageRegistryFile {
const char LANGUAGES[] = "Languages";
const char REGISTRY[] = "Registry";
const char LOCATION[] = "Location";
const char LANGUAGE[] = "Language";
const char URI[] = "Uri";
const char ARCHIVE_URI[] = "ArchiveUri";
const char PUBLICATION_URI[] = "PublicationUri";
const char ARCHIVE_FILE[] = "ArchiveFile";
}

namespace MessageRegistry {
const char LANGUAGE[] = "Language";
const char MESSAGES[] = "Messages";
const char OWNING_ENTITY[] = "OwningEntity";
const char REGISTRY_PREFIX[] = "RegistryPrefix";
const char REGISTRY_VERSION[] = "RegistryVersion";
}

namespace SessionService {
extern const char SERVICE_ENABLED[] = "ServiceEnabled";
extern const char SESSION_TIMEOUT[] = "SessionTimeout";
extern const char SESSIONS[] = "Sessions";
}

namespace Session {
extern const char USER_NAME[] = "UserName";
extern const char PASSWORD[] = "Password";
extern const char OEM[] = "Oem";
}

namespace AccountService {
const char SERVICE_ENABLED[] = "ServiceEnabled";
const char AUTH_FAILURE_LOGGING_THRESHOLD[] = "AuthFailureLoggingThreshold";
const char MIN_PASSWORD_LENGTH[] = "MinPasswordLength";
const char LOCAL_ACCOUNT_AUTH[] = "LocalAccountAuth";
const char ACCOUNTS[] = "Accounts";
const char ROLES[] = "Roles";
}

namespace Account {
const char ENABLED[] = "Enabled";
const char PASSWORD[] = "Password";
const char USER_NAME[] = "UserName";
const char ROLE_ID[] = "RoleId";
const char LOCKED[] = "Locked";
const char ROLE[] = "Role";
}

namespace Role {
const char IS_PREDEFINED[] = "IsPredefined";
const char ASSIGNED_PRIVILEGES[] = "AssignedPrivileges";
const char OEM_PRIVILEGES[] = "OemPrivileges";
const char ROLE_ID[] = "RoleId";
}

}
}
}
