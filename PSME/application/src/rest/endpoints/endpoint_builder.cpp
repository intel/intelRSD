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

#include "psme/rest/endpoints/endpoint_builder.hpp"
#include "psme/rest/endpoints/endpoints.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/server/utils.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::server;


EndpointBuilder::~EndpointBuilder() {}


void EndpointBuilder::build_endpoints() {
    auto& mp = *(psme::rest::server::Multiplexer::get_instance());
    mp.use_before([this](const Request&, Response& res) {
        res.set_header(ContentType::CONTENT_TYPE, ContentType::JSON);
    });

    // "/redfish
    mp.register_handler(Redfish::UPtr(new Redfish(constants::Routes::REDFISH_PATH)), AccessType::ALL);

    // "/redfish/v1"
    mp.register_handler(Root::UPtr(new Root(constants::Routes::ROOT_PATH)), AccessType::ALL);

    // "/redfish/v1/odata"
    mp.register_handler(Root::UPtr(new OdataServiceDocument(constants::Routes::ODATA_SERVICE_DOCUMENT)),
                        AccessType::ALL);

    // "/redfish/v1/$metadata"
    mp.register_handler(MetadataRoot::UPtr(new MetadataRoot(constants::Routes::METADATA_ROOT_PATH)), AccessType::ALL);

    // "/redfish/v1/metadata/{metadata_file:*}"
    mp.register_handler(Metadata::UPtr(new Metadata(constants::Routes::METADATA_PATH)), AccessType::ALL);

    // "/redfish/v1/UpdateService"
    mp.register_handler(UpdateService::UPtr(new UpdateService(constants::Routes::UPDATE_SERVICE_PATH)));

    // "/redfish/v1/UpdateService/SimpleUpdateActionInfo"
    mp.register_handler(
        SimpleUpdateActionInfo::UPtr(new SimpleUpdateActionInfo(constants::Routes::SIMPLE_UPDATE_ACTION_INFO_PATH)));

    // "/redfish/v1/UpdateService/Actions/SimpleUpdate"
    mp.register_handler(SimpleUpdate::UPtr(new SimpleUpdate(constants::Routes::SIMPLE_UPDATE_PATH)));

    // "/redfish/v1/EventService"
    mp.register_handler(EventService::UPtr(new EventService(constants::Routes::EVENT_SERVICE_PATH)));

    // "/redfish/v1/EventService/Subscriptions"
    mp.register_handler(
        SubscriptionCollection::UPtr(new SubscriptionCollection(constants::Routes::SUBSCRIPTION_COLLECTION_PATH)));

    // "/redfish/v1/EventService/Subscriptions/{subscriptionId:[0-9]+}"
    mp.register_handler(Subscription::UPtr(new Subscription(constants::Routes::SUBSCRIPTION_PATH)));

    // "/redfish/v1/SessionService"
    mp.register_handler(SessionService::UPtr(new SessionService(constants::Routes::SESSION_SERVICE_PATH)));

    // "/redfish/v1/SessionService/Sessions"
    mp.register_handler(SessionCollection::UPtr(new SessionCollection(constants::Routes::SESSION_COLLECTION_PATH)));

    // "/redfish/v1/SessionService/Sessions/{sessionId:[0-9]+}"
    mp.register_handler(Session::UPtr(new Session(constants::Routes::SESSION_PATH)));

    // "/redfish/v1/Registries"
    mp.register_handler(MessageRegistryFileCollection::UPtr(
        new MessageRegistryFileCollection(constants::Routes::MESSAGE_REGISTRY_FILE_COLLECTION_PATH)));

    // "/redfish/v1/Registries/{MessageRegistryFileId: [0-9]+}"
    mp.register_handler(MessageRegistryFile::UPtr(
        new MessageRegistryFile(constants::Routes::MESSAGE_REGISTRY_FILE_PATH)));

    // "/redfish/v1/TaskService"
    mp.register_handler(TaskService::UPtr(new TaskService(constants::Routes::TASK_SERVICE_PATH)));

    // "/redfish/v1/TaskService/Tasks"
    mp.register_handler(TaskCollection::UPtr(new TaskCollection(constants::Routes::TASK_COLLECTION_PATH)));

    // "/redfish/v1/Task/Service/Tasks/{taskId:[0-9]+}"
    mp.register_handler(Task::UPtr(new Task(constants::Routes::TASK_PATH)));

    // "/redfish/v1/Task/Service/Tasks/{taskId:[0-9]+}/Monitor"
    mp.register_handler(Monitor::UPtr(new Monitor(constants::Routes::MONITOR_PATH)));

    // "/redfish/v1/Chassis"
    mp.register_handler(ChassisCollection::UPtr(new ChassisCollection(constants::Routes::CHASSIS_COLLECTION_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}"
    mp.register_handler(Chassis::UPtr(new Chassis(constants::Routes::CHASSIS_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Actions/Chassis.Reset"
    mp.register_handler(ChassisReset::UPtr(new ChassisReset(constants::Routes::CHASSIS_RESET_PATH)));

#ifndef NDEBUG
    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives"
    mp.register_handler(Chassis::UPtr(new DrivesCollection(constants::Routes::DRIVES_COLLECTION_PATH)));
#endif

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}"
    mp.register_handler(Chassis::UPtr(new Drive(constants::Routes::DRIVE_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}/Metrics"
    mp.register_handler(Chassis::UPtr(new DriveMetrics(constants::Routes::DRIVE_METRICS_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Drives/{driveId:[0-9]+}/Actions/Drive.SecureErase"
    mp.register_handler(DriveSecureErase::UPtr(new DriveSecureErase(constants::Routes::DRIVE_SECURE_ERASE_PATH)));

#ifndef NDEBUG
    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices"
    mp.register_handler(PcieDevicesCollection::UPtr(
        new PcieDevicesCollection(constants::Routes::PCIE_DEVICES_COLLECTION_PATH)));
#endif

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}"
    mp.register_handler(PcieDevice::UPtr(new PcieDevice(constants::Routes::PCIE_DEVICE_PATH)));

#ifndef NDEBUG
    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}/Functions"
    mp.register_handler(PcieFunctionsCollection::UPtr(
        new PcieFunctionsCollection(constants::Routes::PCIE_FUNCTIONS_COLLECTION_PATH)));
#endif

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/PCIeDevices/{deviceId:[0-9]+}/Functions/{functionId:[0-9]+}"
    mp.register_handler(PcieFunction::UPtr(new PcieFunction(constants::Routes::PCIE_FUNCTION_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Power"
    mp.register_handler(Power::UPtr(new Power(constants::Routes::POWER_PATH)));

// "/redfish/v1/Chassis/{chassisId:[0-9]+}/Power/Oem/Intel_RackScale/Actions/Intel.Oem.RequestPowerSupplyStateChange"
    mp.register_handler(PowerSupplyRequestStateChange::UPtr(
        new PowerSupplyRequestStateChange(constants::Routes::POWER_SUPPLY_REQUEST_STATE_CHANGE_PATH)));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}/Thermal"
    mp.register_handler(Thermal::UPtr(new Thermal(constants::Routes::THERMAL_PATH)));

    // "/redfish/v1/Managers"
    mp.register_handler(ManagerCollection::UPtr(new ManagerCollection(constants::Routes::MANAGER_COLLECTION_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}"
    mp.register_handler(Manager::UPtr(new Manager(constants::Routes::MANAGER_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/Actions/Manager.Reset"
    mp.register_handler(ManagerReset::UPtr(new ManagerReset(constants::Routes::MANAGER_RESET_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/Actions/Manager.LoadFactoryDefaults"
    mp.register_handler(ManagerLoadFactoryDefaults::UPtr(
        new ManagerLoadFactoryDefaults(constants::Routes::MANAGER_LOAD_FACTORY_DEFAULTS_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces"
    mp.register_handler(ManagerNetworkInterfaceCollection::UPtr(
        new ManagerNetworkInterfaceCollection(constants::Routes::MANAGER_NETWORK_INTERFACE_COLLECTION_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
    mp.register_handler(ManagerNetworkInterface::UPtr(
        new ManagerNetworkInterface(constants::Routes::MANAGER_NETWORK_INTERFACE_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}/VLANs"
    mp.register_handler(ManagerVlanNetworkInterfaceCollection::UPtr(
        new ManagerVlanNetworkInterfaceCollection(constants::Routes::MANAGER_VLAN_NETWORK_INTERFACE_COLLECTION_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}/VLANs/{vlanId:[0-9]+}"
    mp.register_handler(ManagerVlanNetworkInterface::UPtr(
        new ManagerVlanNetworkInterface(constants::Routes::MANAGER_VLAN_NETWORK_INTERFACE_PATH)));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/NetworkProtocol"
    mp.register_handler(NetworkProtocol::UPtr(new NetworkProtocol(constants::Routes::NETWORK_PROTOCOL_PATH)));

    // "/redfish/v1/Systems"
    mp.register_handler(SystemsCollection::UPtr(new SystemsCollection(constants::Routes::SYSTEMS_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}"
    mp.register_handler(System::UPtr(new System(constants::Routes::SYSTEM_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/ComputerSystem.Reset"
    mp.register_handler(SystemReset::UPtr(new SystemReset(constants::Routes::SYSTEM_RESET_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/Oem/Intel.Oem.ChangeTPMState"
    mp.register_handler(SystemReset::UPtr(new SystemChangeTPMState(constants::Routes::SYSTEM_CHANGE_TPM_STATE_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Storage"
    mp.register_handler(StorageSubsystemsCollection::UPtr(
        new StorageSubsystemsCollection(constants::Routes::STORAGE_SUBSYSTEMS_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Storage/{storageId:[0-9]+}"
    mp.register_handler(StorageSubsystem::UPtr(new StorageSubsystem(constants::Routes::STORAGE_SUBSYSTEM_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Storage/{storageId:[0-9]+}/Volumes"
    mp.register_handler(VolumesCollection::UPtr(new VolumesCollection(constants::Routes::VOLUMES_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces"
    mp.register_handler(SystemNetworkInterfaceCollection::UPtr(
        new SystemNetworkInterfaceCollection(constants::Routes::SYSTEM_ETHERNET_INTERFACES_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
    mp.register_handler(
        SystemNetworkInterface::UPtr(new SystemNetworkInterface(constants::Routes::SYSTEM_ETHERNET_INTERFACE_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Memory"
    mp.register_handler(MemoryCollection::UPtr(new MemoryCollection(constants::Routes::MEMORY_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Memory/{memoryId:[0-9]+}"
    mp.register_handler(Memory::UPtr(new Memory(constants::Routes::MEMORY_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Memory/{memoryId:[0-9]+}/Metrics"
    mp.register_handler(MemoryMetrics::UPtr(new MemoryMetrics(constants::Routes::MEMORY_METRICS_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
    mp.register_handler(
        SystemDcpmemSecureErase::UPtr(new SystemDcpmemSecureErase(constants::Routes::MEMORY_ERASE_DCPMEM_PATH)));

    //  "/redfish/v1/Systems/{systemId:[0-9]+}/Processors"
    mp.register_handler(
        ProcessorsCollection::UPtr(new ProcessorsCollection(constants::Routes::PROCESSORS_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}"
    mp.register_handler(Processor::UPtr(new Processor(constants::Routes::PROCESSOR_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}/Actions/Oem/Intel.Oem.SecureErase"
    mp.register_handler(ProcessorSecureErase::UPtr(new ProcessorSecureErase(constants::Routes::PROCESSORS_SECURE_ERASE_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces"
    mp.register_handler(NetworkInterfacesCollection::UPtr(new NetworkInterfacesCollection(
        constants::Routes::NETWORK_INTERFACES_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}"
    mp.register_handler(NetworkInterface::UPtr(new NetworkInterface(
        constants::Routes::NETWORK_INTERFACE_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}/NetworkDeviceFunctions"
    mp.register_handler(NetworkDeviceFunctionsCollection::UPtr(new NetworkDeviceFunctionsCollection(
        constants::Routes::NETWORK_DEVICE_FUNCTIONS_COLLECTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/NetworkInterfaces/{networkInterfaceId:[0-9]+}/NetworkDeviceFunctions/{networkDeviceFunctionId:[0-9]+}"
    mp.register_handler(NetworkDeviceFunction::UPtr(new NetworkDeviceFunction(
        constants::Routes::NETWORK_DEVICE_FUNCTION_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}/Oem/Intel_RackScale/Metrics"
    mp.register_handler(ProcessorMetrics::UPtr(new ProcessorMetrics(constants::Routes::PROCESSORS_METRICS_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Metrics"
    mp.register_handler(SystemMetrics::UPtr(new SystemMetrics(constants::Routes::SYSTEM_METRICS_PATH)));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/StorageServices"
    mp.register_handler(
        HostedStorageServices::UPtr(new HostedStorageServices(constants::Routes::SYSTEM_STORAGE_SERVICES_PATH)));

    // "/redfish/v1/EthernetSwitches"
    mp.register_handler(EthernetSwitchCollection::UPtr(
        new EthernetSwitchCollection(constants::Routes::ETHERNET_SWITCHES_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}"
    mp.register_handler(EthernetSwitch::UPtr(new EthernetSwitch(constants::Routes::ETHERNET_SWITCH_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs"
    mp.register_handler(AclCollection::UPtr(new AclCollection(constants::Routes::ACL_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}"
    mp.register_handler(Acl::UPtr(new Acl(constants::Routes::ACL_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Bind"
    mp.register_handler(AclBind<true>::UPtr(new AclBind<true>(constants::Routes::ACL_BIND_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Unbind"
    mp.register_handler(AclBind<false>::UPtr(new AclBind<false>(constants::Routes::ACL_UNBIND_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules"
    mp.register_handler(RuleCollection::UPtr(new RuleCollection(constants::Routes::RULES_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules/{ruleId:[0-9]+}"
    mp.register_handler(Rule::UPtr(new Rule(constants::Routes::RULE_PATH)));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports"
    mp.register_handler(EthernetSwitchPortCollection::UPtr(
        new EthernetSwitchPortCollection(constants::Routes::ETHERNET_SWITCH_PORT_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}"
    mp.register_handler(EthernetSwitchPort::UPtr(new EthernetSwitchPort(constants::Routes::ETHERNET_SWITCH_PORT_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs"
    mp.register_handler(StaticMacCollection::UPtr(
        new StaticMacCollection(constants::Routes::STATIC_MAC_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs/{staticMacId:[0-9]+}"
    mp.register_handler(StaticMac::UPtr(new StaticMac(constants::Routes::STATIC_MAC_PATH)));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs"
    mp.register_handler(VlanNetworkInterfaceCollection::UPtr(
        new VlanNetworkInterfaceCollection(constants::Routes::VLAN_NETWORK_INTERFACE_COLLECTION_PATH)));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs/{vlanId:[0-9]+}"
    mp.register_handler(VlanNetworkInterface::UPtr(
        new VlanNetworkInterface(constants::Routes::VLAN_NETWORK_INTERFACE_PATH)));

    // "/redfish/v1/StorageServices"
    mp.register_handler(StorageServicesCollection::UPtr(
        new StorageServicesCollection(constants::Routes::STORAGE_SERVICES_COLLECTION_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}"
    mp.register_handler(StorageService::UPtr(new StorageService(constants::Routes::STORAGE_SERVICE_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools"
    mp.register_handler(
        StoragePoolCollection::UPtr(new StoragePoolCollection(constants::Routes::STORAGE_POOL_COLLECTION_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}"
    mp.register_handler(StoragePool::UPtr(new StoragePool(constants::Routes::STORAGE_POOL_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/AllocatedVolumes"
    mp.register_handler(AllocatedVolumesCollection::UPtr(
        new AllocatedVolumesCollection(constants::Routes::ALLOCATED_VOLUMES_COLLECTION_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/AllocatedPools"
    mp.register_handler(AllocatedPoolsCollection::UPtr(
        new AllocatedPoolsCollection(constants::Routes::ALLOCATED_POOLS_COLLECTION_PATH)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+}"
    mp.register_handler(CapacitySource::UPtr(new CapacitySource(constants::Routes::STORAGE_POOL_CAPACITY_SOURCE)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+}"
    mp.register_handler(CapacitySource::UPtr(new CapacitySource(constants::Routes::VOLUME_CAPACITY_SOURCE)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingDrives}"
    mp.register_handler(ProvidingDrivesCollection::UPtr(
        new ProvidingDrivesCollection(constants::Routes::STORAGE_POOL_PROVIDING_DRIVES_COLLECTION)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingPools}"
    mp.register_handler(ProvidingPoolsCollection::UPtr(
        new ProvidingPoolsCollection(constants::Routes::STORAGE_POOL_PROVIDING_POOLS_COLLECTION)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/StoragePools/{storagePoolId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingVolumes}"
    mp.register_handler(ProvidingVolumesCollection::UPtr(
        new ProvidingVolumesCollection(constants::Routes::STORAGE_POOL_PROVIDING_VOLUMES_COLLECTION)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingPools}"
    mp.register_handler(ProvidingPoolsCollection::UPtr(
        new ProvidingPoolsCollection(constants::Routes::VOLUME_PROVIDING_POOLS_COLLECTION)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingDrives}"
    mp.register_handler(ProvidingDrivesCollection::UPtr(
        new ProvidingDrivesCollection(constants::Routes::VOLUME_PROVIDING_DRIVES_COLLECTION)));

    // "redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/CapacitySources/{capacitySourceId:[0-9]+/ProvidingVolumes}"
    mp.register_handler(ProvidingVolumesCollection::UPtr(
        new ProvidingVolumesCollection(constants::Routes::VOLUME_PROVIDING_VOLUMES_COLLECTION)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes"
    mp.register_handler(VolumeCollection::UPtr(new VolumeCollection(constants::Routes::VOLUME_COLLECTION_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}"
    mp.register_handler(Volume::UPtr(new Volume(constants::Routes::VOLUME_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/Metrics"
    mp.register_handler(Volume::UPtr(new VolumeMetrics(constants::Routes::VOLUME_METRICS_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Volumes/{volumeId:[0-9]+}/Actions/Volume.Initialize"
    mp.register_handler(VolumeInitialize::UPtr(new VolumeInitialize(constants::Routes::VOLUME_INITIALIZE_PATH)));

    // "/redfish/v1/StorageServices/{serviceId:[0-9]+}/Drives"
    mp.register_handler(DriveCollection::UPtr(
        new DriveCollection(constants::Routes::DRIVE_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics"
    mp.register_handler(FabricsCollection::UPtr(
        new FabricsCollection(constants::Routes::FABRICS_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}"
    mp.register_handler(Fabric::UPtr(
        new Fabric(constants::Routes::FABRIC_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Endpoints"
    mp.register_handler(EndpointsCollection::UPtr(
        new EndpointsCollection(constants::Routes::ENDPOINTS_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Endpoints/{endpointId}"
    mp.register_handler(Endpoint::UPtr(
        new Endpoint(constants::Routes::ENDPOINT_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches"
    mp.register_handler(SwitchesCollection::UPtr(
        new SwitchesCollection(constants::Routes::SWITCHES_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}"
    mp.register_handler(Switch::UPtr(new Switch(constants::Routes::SWITCH_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Actions/Switch.Reset"
    mp.register_handler(SwitchReset::UPtr(new SwitchReset(constants::Routes::SWITCH_RESET_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Zones"
    mp.register_handler(ZonesCollection::UPtr(
        new ZonesCollection(constants::Routes::ZONES_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Zones/{zoneId:[0-9]+}"
    mp.register_handler(Zone::UPtr(new Zone(constants::Routes::ZONE_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports"
    mp.register_handler(PortsCollection::UPtr(
        new PortsCollection(constants::Routes::PORTS_COLLECTION_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}"
    mp.register_handler(Port::UPtr(new Port(constants::Routes::PORT_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}/Metrics"
    mp.register_handler(PortMetrics::UPtr(new PortMetrics(constants::Routes::PORT_METRICS_PATH)));

    // "/redfish/v1/Fabrics/{fabricId}/Switches/{switchId}/Ports/{portId:[0-9]+}/Actions/Port.Reset"
    mp.register_handler(PortReset::UPtr(new PortReset(constants::Routes::PORT_RESET_PATH)));

    // "/redfish/v1/Oem/Intel_RackScale/TelemetryService"
    mp.register_handler(TelemetryService::UPtr(new TelemetryService(constants::Routes::TELEMETRY_SERVICE_PATH)));

    // "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricDefinitions"
    mp.register_handler(MetricDefinitionsCollection::UPtr(
        new MetricDefinitionsCollection(constants::Routes::METRIC_DEFINITIONS_COLLECTION_PATH)));

    // "/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricDefinitions/{metricDefinitionId}"
    mp.register_handler(MetricDefinition::UPtr(new MetricDefinition(constants::Routes::METRIC_DEFINITION_PATH)));

    // "/redfish/v1/AccountService"
    mp.register_handler(AccountService::UPtr(new AccountService(constants::Routes::ACCOUNT_SERVICE_PATH)));

    // "/redfish/v1/AccountService/Accounts"
    mp.register_handler(AccountCollection::UPtr(new AccountCollection(constants::Routes::ACCOUNTS_COLLECTION_PATH)));

    // "/redfish/v1/AccountService/Accounts/{accountId}"
    mp.register_handler(Account::UPtr(new Account(constants::Routes::ACCOUNT_PATH)));

    // "/redfish/v1/AccountService/Roles"
    mp.register_handler(RoleCollection::UPtr(new RoleCollection(constants::Routes::ROLES_COLLECTION_PATH)));

    // "/redfish/v1/AccountService/Roles/{rolesId}"
    mp.register_handler(Role::UPtr(new Role(constants::Routes::ROLE_PATH)));

    // "/registries/Intel_RackScale.1.0.0.json"
    mp.register_handler(IntelRegistry::UPtr(new IntelRegistry(constants::Routes::INTEL_RACKSCALE_REGISTRY_PATH)),
                        AccessType::ALL);

}
