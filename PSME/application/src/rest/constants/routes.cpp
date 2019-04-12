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

// "/redfish/v1/UpdateService"
const std::string Routes::UPDATE_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::UPDATE_SERVICE)
        .build();

// "/redfish/v1/UpdateService/Actions/SimpleUpdate"
const std::string Routes::SIMPLE_UPDATE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::UPDATE_SERVICE)
        .append(constants::Common::ACTIONS)
        .append(constants::UpdateService::SIMPLE_UPDATE)
        .build();

// "/redfish/v1/UpdateService/SimpleUpdateActionInfo"
const std::string Routes::SIMPLE_UPDATE_ACTION_INFO_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::UPDATE_SERVICE)
        .append(constants::UpdateService::SIMPLE_UPDATE_ACTION_INFO)
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

// "/redfish/v1/SessionService"
const std::string Routes::SESSION_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::SESSION_SERVICE)
        .build();

// "/redfish/v1/SessionService/Sessions"
const std::string Routes::SESSION_COLLECTION_PATH =
    PathBuilder(SESSION_SERVICE_PATH)
        .append(constants::SessionService::SESSIONS)
        .build();

// "/redfish/v1/SessionService/Sessions/{sessionId:[0-9]+}"
const std::string Routes::SESSION_PATH =
    PathBuilder(SESSION_COLLECTION_PATH)
        .append_regex(constants::PathParam::SESSION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/MessageRegistryFiles"
const std::string Routes::MESSAGE_REGISTRY_FILE_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
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

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Actions/Chassis.Reset"
const std::string Routes::CHASSIS_RESET_PATH =
    endpoint::PathBuilder(CHASSIS_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Chassis::CHASSIS_RESET)
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

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}"
const std::string Routes::DRIVE_METRICS_PATH =
    PathBuilder(DRIVE_PATH)
        .append(constants::Common::METRICS)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}/Actions/Drive.SecureErase"
const std::string Routes::DRIVE_SECURE_ERASE_PATH =
    PathBuilder(DRIVE_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Drive::SECURE_ERASE_ENDPOINT)
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

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Thermal"
const std::string Routes::THERMAL_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::THERMAL)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Power"
const std::string Routes::POWER_PATH =
    PathBuilder(CHASSIS_PATH)
        .append(constants::Chassis::POWER)
        .build();

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Power/Oem/Intel_RackScale/Actions/Intel.Oem.RequestPowerSupplyStateChange"
const std::string Routes::POWER_SUPPLY_REQUEST_STATE_CHANGE_PATH =
    PathBuilder(POWER_PATH)
        .append(constants::Common::OEM)
        .append(constants::Common::RACKSCALE)
        .append(constants::Common::ACTIONS)
        .append(constants::PowerZone::INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE)
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

// "/redfish/v1/Managers/{managerId:[0-9]+}/Actions/Manager.Reset"
const std::string Routes::MANAGER_RESET_PATH =
    PathBuilder(MANAGER_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Manager::MANAGER_RESET)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}/Actions/Oem/Intel_RackScale.LoadFactoryDefaults"
const std::string Routes::MANAGER_LOAD_FACTORY_DEFAULTS_PATH =
    PathBuilder(MANAGER_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Common::OEM)
        .append(constants::Manager::LOAD_FACTORY_DEFAULTS_ACTION)
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

// "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}/VLANs"
const std::string Routes::MANAGER_VLAN_NETWORK_INTERFACE_COLLECTION_PATH =
    PathBuilder(MANAGER_NETWORK_INTERFACE_PATH)
        .append(constants::NetworkInterface::VLANS)
        .build();

// "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}/VLANs/{vlanId:[0-9]+}"
const std::string Routes::MANAGER_VLAN_NETWORK_INTERFACE_PATH =
    PathBuilder(MANAGER_VLAN_NETWORK_INTERFACE_COLLECTION_PATH)
        .append_regex(constants::PathParam::VLAN_ID, constants::PathParam::ID_REGEX)
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

// "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/Oem/Intel.Oem.ChangeTPMState"
const std::string Routes::SYSTEM_CHANGE_TPM_STATE_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Common::OEM)
        .append(constants::System::INTEL_OEM_CHANGE_TPM_STATE)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
const std::string Routes::MEMORY_ERASE_DCPMEM_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Common::OEM)
        .append(constants::System::INTEL_OEM_ERASE_OPTANE_DC_PERSISTENT_MEMORY)
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

// "/redfish/v1/Systems/{systemId:[0-9]+}/Memory/{memoryId:[0-9]+}/Metrics"
const std::string Routes::MEMORY_METRICS_PATH =
    PathBuilder(MEMORY_PATH)
        .append(constants::Common::METRICS)
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

// "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}/Oem/Intel_RackScale/Metrics"
const std::string Routes::PROCESSORS_METRICS_PATH =
    PathBuilder(PROCESSOR_PATH)
        .append(constants::PathParam::OEM_INTEL_RACKSCALE)
        .append(constants::Common::METRICS)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}/Actions/Oem/Intel.Oem.SecureErase"
const std::string Routes::PROCESSORS_SECURE_ERASE_PATH =
    PathBuilder(PROCESSOR_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Common::OEM)
        .append(constants::Fpga::SECURE_ERASE)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/Metrics"
const std::string Routes::SYSTEM_METRICS_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::Common::METRICS)
        .build();

// "/redfish/v1/Systems/{systemId:[0-9]+}/StorageServices"
const std::string Routes::SYSTEM_STORAGE_SERVICES_PATH =
    PathBuilder(SYSTEM_PATH)
        .append(constants::Common::STORAGE_SERVICES)
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

// "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Metrics"
const std::string Routes::ETHERNET_SWITCH_METRICS_PATH =
    PathBuilder(ETHERNET_SWITCH_PATH)
        .append(constants::Common::METRICS)
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

// "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/Metrics"
const std::string Routes::ETHERNET_SWITCH_PORT_METRICS_PATH =
    PathBuilder(ETHERNET_SWITCH_PORT_PATH)
        .append(constants::Common::METRICS)
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

// "/redfish/v1/StorageServices"
const std::string Routes::STORAGE_SERVICES_COLLECTION_PATH =
    PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::STORAGE_SERVICES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}"
const std::string Routes::STORAGE_SERVICE_PATH =
    PathBuilder(STORAGE_SERVICES_COLLECTION_PATH)
        .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/"
const std::string Routes::STORAGE_POOL_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::STORAGE_POOLS)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}"
const std::string Routes::STORAGE_POOL_PATH =
    PathBuilder(STORAGE_POOL_COLLECTION_PATH)
        .append_regex(constants::PathParam::STORAGE_POOL_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/AllocatedVolumes"
const std::string Routes::ALLOCATED_VOLUMES_COLLECTION_PATH =
    PathBuilder(STORAGE_POOL_PATH)
        .append(constants::Swordfish::ALLOCATED_VOLUMES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/AllocatedPools"
const std::string Routes::ALLOCATED_POOLS_COLLECTION_PATH =
    PathBuilder(STORAGE_POOL_PATH)
        .append(constants::Swordfish::ALLOCATED_POOLS)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+}
const std::string Routes::STORAGE_POOL_CAPACITY_SOURCE =
    PathBuilder(STORAGE_POOL_PATH)
        .append(constants::Swordfish::CAPACITY_SOURCES)
        .append_regex(constants::PathParam::CAPACITY_SOURCE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingDrives}
const std::string Routes::STORAGE_POOL_PROVIDING_DRIVES_COLLECTION =
    PathBuilder(STORAGE_POOL_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_DRIVES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingPools}
const std::string Routes::STORAGE_POOL_PROVIDING_POOLS_COLLECTION =
    PathBuilder(STORAGE_POOL_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_POOLS)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingVolumes}
const std::string Routes::STORAGE_POOL_PROVIDING_VOLUMES_COLLECTION =
    PathBuilder(STORAGE_POOL_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_VOLUMES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/"
const std::string Routes::VOLUME_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::VOLUMES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}"
const std::string Routes::VOLUME_PATH =
    PathBuilder(VOLUME_COLLECTION_PATH)
        .append_regex(constants::PathParam::VOLUME_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/Metrics"
const std::string Routes::VOLUME_METRICS_PATH =
    PathBuilder(VOLUME_PATH)
        .append(constants::Common::METRICS)
        .build();

// "redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+}"
const std::string Routes::VOLUME_CAPACITY_SOURCE =
    PathBuilder(VOLUME_PATH)
        .append(constants::Swordfish::CAPACITY_SOURCES)
        .append_regex(constants::PathParam::CAPACITY_SOURCE_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{VolumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingPools}
const std::string Routes::VOLUME_PROVIDING_POOLS_COLLECTION =
    PathBuilder(VOLUME_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_POOLS)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{StoragePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingVolumes}
const std::string Routes::VOLUME_PROVIDING_VOLUMES_COLLECTION =
    PathBuilder(VOLUME_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_VOLUMES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{StoragePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingDrives}
const std::string Routes::VOLUME_PROVIDING_DRIVES_COLLECTION =
    PathBuilder(VOLUME_CAPACITY_SOURCE)
        .append(constants::Swordfish::PROVIDING_DRIVES)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/Actions/Volume.Initialize"
const std::string Routes::VOLUME_INITIALIZE_PATH =
    PathBuilder(VOLUME_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Swordfish::VOLUME_INITIALIZE_ENDPOINT)
        .build();

// "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Drives"
const std::string Routes::DRIVE_COLLECTION_PATH =
    PathBuilder(STORAGE_SERVICE_PATH)
        .append(constants::StorageService::DRIVES)
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

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}/Metrics"
const std::string Routes::PORT_METRICS_PATH =
    PathBuilder(PORT_PATH)
        .append(constants::Common::METRICS)
        .build();

// "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}/Actions/Port.Reset"
const std::string Routes::PORT_RESET_PATH =
    PathBuilder(PORT_PATH)
        .append(constants::Common::ACTIONS)
        .append(constants::Port::PORT_RESET_ENDPOINT)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService"
const std::string Routes::TELEMETRY_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::PathParam::OEM_INTEL_RACKSCALE)
        .append(constants::Root::TELEMETRY_SERVICE)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricDefinitions"
const std::string Routes::METRIC_DEFINITIONS_COLLECTION_PATH =
    endpoint::PathBuilder(TELEMETRY_SERVICE_PATH)
        .append(constants::TelemetryService::METRIC_DEFINITIONS)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricDefinitions/{metricDefinitionId}"
const std::string Routes::METRIC_DEFINITION_PATH =
    endpoint::PathBuilder(METRIC_DEFINITIONS_COLLECTION_PATH)
        .append_regex(constants::PathParam::METRIC_DEFINITION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricReportDefinitions"
const std::string Routes::METRIC_REPORT_DEFINITIONS_COLLECTION_PATH =
    endpoint::PathBuilder(TELEMETRY_SERVICE_PATH)
        .append(constants::TelemetryService::METRIC_REPORT_DEFINITIONS)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricReportDefinitions/{metricReportDefinitionId}"
const std::string Routes::METRIC_REPORT_DEFINITION_PATH =
    endpoint::PathBuilder(METRIC_REPORT_DEFINITIONS_COLLECTION_PATH)
        .append_regex(constants::PathParam::METRIC_REPORT_DEFINITION_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/Triggers"
const std::string Routes::TRIGGERS_COLLECTION_PATH =
    endpoint::PathBuilder(TELEMETRY_SERVICE_PATH)
        .append(constants::TelemetryService::TRIGGERS)
        .build();

// "/redfish/v1/Oem/Intel_RackScale/TelemetryService/Triggers/{trigger}"
const std::string Routes::TRIGGER_PATH =
    endpoint::PathBuilder(TRIGGERS_COLLECTION_PATH)
        .append_regex(constants::PathParam::TRIGGER_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/AccountService"
const std::string Routes::ACCOUNT_SERVICE_PATH =
    endpoint::PathBuilder(constants::PathParam::BASE_URL)
        .append(constants::Root::ACCOUNT_SERVICE)
        .build();

// "/redfish/v1/AccountService/Accounts"
const std::string Routes::ACCOUNTS_COLLECTION_PATH =
    endpoint::PathBuilder(ACCOUNT_SERVICE_PATH)
        .append(constants::AccountService::ACCOUNTS)
        .build();

// "/redfish/v1/AccountService/Accounts/{accountId}
const std::string Routes::ACCOUNT_PATH =
    endpoint::PathBuilder(ACCOUNTS_COLLECTION_PATH)
        .append_regex(constants::PathParam::ACCOUNT_ID, constants::PathParam::ID_REGEX)
        .build();

// "/redfish/v1/AccountService/Roles"
const std::string Routes::ROLES_COLLECTION_PATH =
    endpoint::PathBuilder(ACCOUNT_SERVICE_PATH)
        .append(constants::AccountService::ROLES)
        .build();

// "/redfish/v1/AccountService/Roles/{rolesId}"
const std::string Routes::ROLE_PATH =
    endpoint::PathBuilder(ROLES_COLLECTION_PATH)
        .append_regex(constants::PathParam::ROLE_ID, constants::PathParam::STRING_ID_REGEX)
        .build();

// "/registries/Intel_RackScale.1.0.0.json"
const std::string Routes::INTEL_RACKSCALE_REGISTRY_PATH =
    PathBuilder(constants::PathParam::INTEL_RACKSCALE_REGISTRY_URL)
        .build();
