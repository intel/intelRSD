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
 * */

#include "psme/rest/constants/routes.hpp"
#include "psme/rest/endpoints/path_builder.hpp"



using namespace psme::rest::constants;
using namespace psme::rest::endpoint;

// "/redfish
const std::string Routes::REDFISH_PATH =
    PathBuilder()
        .append(constants::PathParam::REDFISH)
        .build();

// "/redfish/v1"
const std::string Routes::ROOT_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .build();

// "/redfish/v1/odata"
const std::string Routes::ODATA_SERVICE_DOCUMENT =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::PathParam::ODATA)
        .build();

// "/redfish/v1/$metadata"
const std::string Routes::METADATA_ROOT_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::PathParam::METADATA_ROOT)
        .build();

// "/redfish/v1/metadata/{metadata_file:*}"
const std::string Routes::METADATA_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::PathParam::METADATA)
        .append_regex(constants::PathParam::METADATA_FILE, constants::PathParam::XML_REGEX)
        .build();

// "/redfish/v1/EventService"
const std::string Routes::EVENT_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::EVENT_SERVICE)
        .build();

// "/redfish/v1/EventService/Subscriptions"
const std::string Routes::SUBSCRIPTION_COLLECTION_PATH =
    PathBuilder(EVENT_SERVICE_PATH)
        .append(constants::EventService::SUBSCRIPTIONS)
        .build();

// "/redfish/v1/EventService/Subscriptions/{subscriptionId:[0-9]+}"
const std::string Routes::SUBSCRIPTION_PATH =
    PathBuilder(SUBSCRIPTION_COLLECTION_PATH)
        .append_regex(constants::PathParam::SUBSCRIPTION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EventService/TestEventSubscription"
const std::string Routes::TEST_EVENT_SUBSCRIPTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::EVENT_SERVICE)
        .append("TestEventSubscription")
        .build();

// "/redfish/v1/MessageRegistryFiles"
const std::string Routes::MESSAGE_REGISTRY_FILE_COLLECTION_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::REGISTRIES)
        .build();

// "/redfish/v1/Registries/{MessageRegistryFileId: [0-9]+}"
const std::string Routes::MESSAGE_REGISTRY_FILE_PATH =
    PathBuilder(MESSAGE_REGISTRY_FILE_COLLECTION_PATH)
        .append_regex(constants::PathParam::MESSAGE_REGISTRY_FILE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/TaskService"
const std::string Routes::TASK_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::TaskService::TASK_SERVICE)
        .build();

// "/redfish/v1/TaskService/Tasks"
const std::string Routes::TASK_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::TaskService::TASK_SERVICE)
        .append(constants::Root::TASKS)
        .build();

// "/redfish/v1/TaskService/Tasks/{TaskId:[0-9]+}"
const std::string Routes::TASK_PATH =
    PathBuilder(TASK_COLLECTION_PATH)
        .append_regex(constants::PathParam::TASK_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/TaskService/Tasks/{TaskId:[0-9]+}/Monitor"
const std::string Routes::MONITOR_PATH =
    PathBuilder(TASK_PATH)
        .append(constants::Monitor::MONITOR)
        .build();

// "/redfish/v1/Chassis"
const std::string Routes::CHASSIS_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Common::CHASSIS)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}"
const std::string Routes::CHASSIS_PATH =
    endpoint::PathBuilder(CHASSIS_COLLECTION_PATH)
        .append_regex(constants::PathParam::CHASSIS_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives"
const std::string Routes::DRIVES_COLLECTION_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::DRIVES)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}"
const std::string Routes::DRIVE_PATH =
    PathBuilder(DRIVES_COLLECTION_PATH)
        .append_regex(constants::PathParam::DRIVE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}/Actions/Drive.SecureErase"
const std::string Routes::DRIVE_SECURE_ERASE_PATH =
    PathBuilder(DRIVE_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::PncDrive::SECURE_ERASE_ENDPOINT)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices"
const std::string Routes::PCIE_DEVICES_COLLECTION_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::PCIE_DEVICES)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}"
const std::string Routes::PCIE_DEVICE_PATH =
    PathBuilder(PCIE_DEVICES_COLLECTION_PATH)
        .append_regex(constants::PathParam::DEVICE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}/Functions"
const std::string Routes::PCIE_FUNCTIONS_COLLECTION_PATH =
    PathBuilder(PCIE_DEVICE_PATH)
        .append(constants::PathParam::FUNCTIONS)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}/Functions/{functionId:[0-9]+}"
const std::string Routes::PCIE_FUNCTION_PATH =
    PathBuilder(PCIE_FUNCTIONS_COLLECTION_PATH)
        .append_regex(constants::PathParam::FUNCTION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/ThermalZones"
const std::string Routes::THERMAL_ZONE_COLLECTION_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::THERMAL_ZONES)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/ThermalZones/{thermalZoneId:[0-9]+}"
const std::string Routes::THERMAL_ZONE_PATH =
    endpoint::PathBuilder(THERMAL_ZONE_COLLECTION_PATH)
        .append_regex(constants::PathParam::THERMAL_ZONE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PowerZones"
const std::string Routes::POWER_ZONE_COLLECTION_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::POWER_ZONES)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/PowerZones/{powerZoneId:[0-9]+}"
const std::string Routes::POWER_ZONE_PATH =
    endpoint::PathBuilder(POWER_ZONE_COLLECTION_PATH)
        .append_regex(constants::PathParam::POWER_ZONE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Managers"
const std::string Routes::MANAGER_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::MANAGERS)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}"
const std::string Routes::MANAGER_PATH =
    PathBuilder(MANAGER_COLLECTION_PATH)
        .append_regex(constants::PathParam::MANAGER_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces"
const std::string Routes::MANAGER_NETWORK_INTERFACE_COLLECTION_PATH =
    PathBuilder(MANAGER_PATH)
        .append(constants::Manager::ETHERNET_INTERFACES)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
const std::string Routes::MANAGER_NETWORK_INTERFACE_PATH =
    PathBuilder(MANAGER_NETWORK_INTERFACE_COLLECTION_PATH)
        .append_regex(constants::PathParam::NIC_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}/NetworkProtocol"
const std::string Routes::NETWORK_PROTOCOL_PATH =
    PathBuilder(MANAGER_PATH)
        .append(constants::Manager::NETWORK_PROTOCOL)
        .build();

// "/redfish/v1/Systems"
const std::string Routes::SYSTEMS_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::SYSTEMS)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}"
const std::string Routes::SYSTEM_PATH =
    PathBuilder(SYSTEMS_COLLECTION_PATH)
        .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/ComputerSystem.Reset"
const std::string Routes::SYSTEM_RESET_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::System::COMPUTER_SYSTEM_RESET)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Storage"
const std::string Routes::STORAGE_SUBSYSTEMS_COLLECTION_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::System::STORAGE)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Storage/{storageId:[0-9]+}"
const std::string Routes::STORAGE_SUBSYSTEM_PATH =
    PathBuilder(STORAGE_SUBSYSTEMS_COLLECTION_PATH)
        .append_regex(constants::PathParam::STORAGE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Storage/{storageId:[0-9]+}/Volumes"
const std::string Routes::VOLUMES_COLLECTION_PATH =
    PathBuilder(STORAGE_SUBSYSTEM_PATH)
        .append(constants::StorageSubsystem::VOLUMES)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces"
const std::string Routes::SYSTEM_ETHERNET_INTERFACES_COLLECTION_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::System::ETHERNET_INTERFACES)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
const std::string Routes::SYSTEM_ETHERNET_INTERFACE_PATH =
    PathBuilder(SYSTEM_ETHERNET_INTERFACES_COLLECTION_PATH)
        .append_regex(constants::PathParam::NIC_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Memory"
const std::string Routes::MEMORY_COLLECTION_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::System::MEMORY)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Memory/{memoryId:[0-9]+}"
const std::string Routes::MEMORY_PATH =
    PathBuilder(MEMORY_COLLECTION_PATH)
        .append_regex(constants::PathParam::MEMORY_ID, constants::PathParam::ID_REGEX)
        .build();

//  "/redfish/v1/Systems/{systemId:[0-9]+}/Processors"
const std::string Routes::PROCESSORS_COLLECTION_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::System::PROCESSORS)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}"
const std::string Routes::PROCESSOR_PATH =
    PathBuilder(PROCESSORS_COLLECTION_PATH)
        .append_regex(constants::PathParam::PROCESSOR_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces"
const std::string Routes::NETWORK_INTERFACES_COLLECTION_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::System::NETWORK_INTERFACES)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}"
const std::string Routes::NETWORK_INTERFACE_PATH =
    PathBuilder(NETWORK_INTERFACES_COLLECTION_PATH)
        .append_regex(constants::PathParam::NETWORK_INTERFACE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}/NetworkDeviceFunctions"
const std::string Routes::NETWORK_DEVICE_FUNCTIONS_COLLECTION_PATH =
    PathBuilder(NETWORK_INTERFACE_PATH)
        .append(constants::NetworkInterface::NETWORK_DEVICE_FUNCTIONS)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}/NetworkDeviceFunctions/{networkDeviceFunctionId:[0-9]+}"
const std::string Routes::NETWORK_DEVICE_FUNCTION_PATH =
    PathBuilder(NETWORK_DEVICE_FUNCTIONS_COLLECTION_PATH)
        .append_regex(constants::PathParam::NETWORK_DEVICE_FUNCTION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches"
const std::string Routes::ETHERNET_SWITCHES_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::ETHERNET_SWITCHES)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}"
const std::string Routes::ETHERNET_SWITCH_PATH =
    PathBuilder(ETHERNET_SWITCHES_COLLECTION_PATH)
        .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs"
const std::string Routes::ACL_COLLECTION_PATH =
    PathBuilder(ETHERNET_SWITCH_PATH)
        .append(constants::EthernetSwitch::ACLS)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}"
const std::string Routes::ACL_PATH =
    PathBuilder(ACL_COLLECTION_PATH)
        .append_regex(constants::PathParam::ACL_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Bind"
const std::string Routes::ACL_BIND_PATH =
    PathBuilder(ACL_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Acl::BIND_ACL)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Unbind"
const std::string Routes::ACL_UNBIND_PATH =
    PathBuilder(ACL_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Acl::UNBIND_ACL)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules"
const std::string Routes::RULES_COLLECTION_PATH =
    PathBuilder(ACL_PATH)
        .append(constants::Acl::RULES)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules/{ruleId:[0-9]+}"
const std::string Routes::RULE_PATH =
    PathBuilder(RULES_COLLECTION_PATH)
        .append_regex(constants::PathParam::RULE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports"
const std::string Routes::ETHERNET_SWITCH_PORT_COLLECTION_PATH =
    PathBuilder(ETHERNET_SWITCH_PATH)
        .append(constants::EthernetSwitch::PORTS)
        .build();

// "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}"
const std::string Routes::ETHERNET_SWITCH_PORT_PATH =
    PathBuilder(ETHERNET_SWITCH_PORT_COLLECTION_PATH)
        .append_regex(constants::PathParam::SWITCH_PORT_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs"
const std::string Routes::STATIC_MAC_COLLECTION_PATH =
    PathBuilder(ETHERNET_SWITCH_PORT_PATH)
        .append(constants::EthernetSwitchPort::STATIC_MACS)
        .build();

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs/{staticMacId:[0-9]+}"
const std::string Routes::STATIC_MAC_PATH =
    PathBuilder(STATIC_MAC_COLLECTION_PATH)
        .append_regex(constants::PathParam::STATIC_MAC_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs"
const std::string Routes::VLAN_NETWORK_INTERFACE_COLLECTION_PATH =
    PathBuilder(ETHERNET_SWITCH_PORT_PATH)
        .append(constants::EthernetSwitchPort::VLANS)
        .build();

// "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs/{vlanId:[0-9]+}"
const std::string Routes::VLAN_NETWORK_INTERFACE_PATH =
    PathBuilder(VLAN_NETWORK_INTERFACE_COLLECTION_PATH)
        .append_regex(constants::PathParam::VLAN_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Services"
const std::string Routes::STORAGE_SERVICE_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::SERVICES)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}"
const std::string Routes::STORAGE_SERVICE_PATH =
    PathBuilder(STORAGE_SERVICE_COLLECTION_PATH)
        .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/Drives"
const std::string Routes::PHYSICAL_DRIVES_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::DRIVES)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/Drives/{driveId:[0-9]+}"
const std::string Routes::PHYSICAL_DRIVE_PATH =
    PathBuilder(PHYSICAL_DRIVES_COLLECTION_PATH)
        .append_regex(constants::PathParam::DRIVE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/LogicalDrives"
const std::string Routes::LOGICAL_DRIVE_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::LOGICAL_DRIVES)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/LogicalDrives/{logicalDriveId:[0-9]+}"
const std::string Routes::LOGICAL_DRIVE_PATH =
    PathBuilder(LOGICAL_DRIVE_COLLECTION_PATH)
        .append_regex(constants::PathParam::LOGICAL_DRIVE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/Targets"
const std::string Routes::REMOTE_TARGET_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::TARGETS)
        .build();

// "/redfish/v1/Services/{serviceId:[0-9]+}/Targets/{remoteTargetId:[0-9]+}"
const std::string Routes::REMOTE_TARGET_PATH =
    PathBuilder(REMOTE_TARGET_COLLECTION_PATH)
        .append_regex(constants::PathParam::REMOTE_TARGET_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics"
const std::string Routes::FABRICS_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::FABRICS)
        .build();

// "/redfish/v1/Fabrics/{fabricId}"
const std::string Routes::FABRIC_PATH =
    PathBuilder(FABRICS_COLLECTION_PATH)
        .append_regex(constants::PathParam::FABRIC_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Endpoints"
const std::string Routes::ENDPOINTS_COLLECTION_PATH =
    PathBuilder(FABRIC_PATH)
        .append(constants::Fabric::ENDPOINTS)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Endpoints/{endpointId}"
const std::string Routes::ENDPOINT_PATH =
    PathBuilder(ENDPOINTS_COLLECTION_PATH)
        .append_regex(constants::PathParam::ENDPOINT_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches"
const std::string Routes::SWITCHES_COLLECTION_PATH =
    PathBuilder(FABRIC_PATH)
        .append(constants::Fabric::SWITCHES)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}"
const std::string Routes::SWITCH_PATH =
    PathBuilder(SWITCHES_COLLECTION_PATH)
        .append_regex(constants::PathParam::SWITCH_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Actions/Switch.Reset"
const std::string Routes::SWITCH_RESET_PATH =
    PathBuilder(SWITCH_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Switch::SWITCH_RESET_ENDPOINT)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Zones"
const std::string Routes::ZONES_COLLECTION_PATH =
    PathBuilder(FABRIC_PATH)
        .append(constants::Fabric::ZONES)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Zones/{zoneId:[0-9]+}"
const std::string Routes::ZONE_PATH =
    PathBuilder(ZONES_COLLECTION_PATH)
        .append_regex(constants::PathParam::ZONE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports"
const std::string Routes::PORTS_COLLECTION_PATH =
    PathBuilder(SWITCH_PATH)
        .append(constants::Switch::PORTS)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}"
const std::string Routes::PORT_PATH =
    PathBuilder(PORTS_COLLECTION_PATH)
        .append_regex(constants::PathParam::PORT_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}/Actions/Port.Reset"
const std::string Routes::PORT_RESET_PATH =
    PathBuilder(PORT_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Port::PORT_RESET_ENDPOINT)
        .build();
