/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.discovery.external.linker;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ComputerSystemMetrics;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.EthernetSwitchMetrics;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortMetrics;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.MemoryMetrics;
import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.business.entities.redfish.MetricItem;
import com.intel.podm.business.entities.redfish.MetricReportDefinition;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.entities.redfish.PhysicalDrive;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.PortMetrics;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.ProcessorMetrics;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Produces;
import java.util.ArrayList;
import java.util.Collection;

import static java.util.Collections.unmodifiableCollection;

@Dependent
public class LinkerCollectionProducer {
    private static final Collection<Linker> LINKERS = new ArrayList<>();

    static {
        register(Chassis.class, Chassis.class, "containsChassis", Chassis::addContainedChassis);
        register(Chassis.class, ComputerSystem.class, "computerSystems", Chassis::addComputerSystem);
        register(Chassis.class, EthernetSwitch.class, "containsSwitches", Chassis::addEthernetSwitch);
        register(Chassis.class, Power.class, "poweredBy", Chassis::addPoweredBy);
        register(Chassis.class, Thermal.class, "cooledBy", Chassis::addCooledBy);
        register(Chassis.class, Drive.class, "drives", Chassis::addDrive);
        register(Chassis.class, Storage.class, "storage", Chassis::addStorage);
        register(Chassis.class, Thermal.class, "thermal", Chassis::setThermal);
        register(Chassis.class, Power.class, "power", Chassis::setPower);

        register(Thermal.class, ThermalTemperature.class, "temperatures", Thermal::addTemperature);
        register(Thermal.class, ThermalFan.class, "fans", Thermal::addFan);
        register(Thermal.class, Redundancy.class, "redundancy", Thermal::addOwnedRedundancy);
        register(ThermalFan.class, DiscoverableEntity.class, "relatedItems", ThermalFan::addRelatedItem);
        register(ThermalFan.class, Redundancy.class, "fanRedundancy", ThermalFan::addRedundancy);
        register(ThermalTemperature.class, DiscoverableEntity.class, "relatedItems", ThermalTemperature::addRelatedItem);

        register(Power.class, PowerControl.class, "powerControls", Power::addPowerControl);
        register(PowerControl.class, DiscoverableEntity.class, "relatedItems", PowerControl::addRelatedItem);
        register(Power.class, PowerVoltage.class, "voltages", Power::addVoltage);
        register(PowerVoltage.class, DiscoverableEntity.class, "relatedItems", PowerVoltage::addRelatedItem);
        register(Power.class, PowerSupply.class, "powerSupplies", Power::addPowerSupply);
        register(PowerSupply.class, DiscoverableEntity.class, "relatedItems", PowerSupply::addRelatedItem);
        register(PowerSupply.class, Redundancy.class, "powerSupplyRedundancy", PowerSupply::addRedundancy);
        register(Power.class, Redundancy.class, "redundancy", DiscoverableEntity::addOwnedRedundancy);

        register(ComputerSystem.class, Memory.class, "memoryModules", ComputerSystem::addMemoryModule);
        register(ComputerSystem.class, Processor.class, "processors", ComputerSystem::addProcessor);
        register(ComputerSystem.class, EthernetInterface.class, "ethernetInterfaces", ComputerSystem::addEthernetInterface);
        register(ComputerSystem.class, SimpleStorage.class, "simpleStorages", ComputerSystem::addSimpleStorage);
        register(ComputerSystem.class, Storage.class, "storage", ComputerSystem::addStorage);
        register(ComputerSystem.class, Storage.class, "adapters", ComputerSystem::addStorage);
        register(ComputerSystem.class, Endpoint.class, "endpoints", ComputerSystem::addEndpoint);
        register(ComputerSystem.class, PcieDevice.class, "pcieDevices", ComputerSystem::addPcieDevice);
        register(ComputerSystem.class, PcieDeviceFunction.class, "pcieFunctions", ComputerSystem::addPcieDeviceFunction);
        register(ComputerSystem.class, NetworkInterface.class, "networkInterfaces", ComputerSystem::addNetworkInterface);
        register(NetworkInterface.class, NetworkDeviceFunction.class, "networkDeviceFunctions", NetworkInterface::addNetworkDeviceFunction);
        register(ComputerSystem.class, ComputerSystemMetrics.class, "computerSystemMetrics", ComputerSystem::setComputerSystemMetrics);

        register(Processor.class, ProcessorMetrics.class, "processorMetrics", Processor::setProcessorMetrics);

        register(Memory.class, MemoryMetrics.class, "memoryMetrics", Memory::setMemoryMetrics);

        register(Storage.class, Drive.class, "drives", Storage::addDrive);
        register(Storage.class, Drive.class, "devices", Storage::addDrive);
        register(Storage.class, StorageController.class, "storageControllers", Storage::addStorageController);

        register(EthernetInterface.class, EthernetSwitchPortVlan.class, "ethernetInterfaceVlans", EthernetInterface::addEthernetSwitchPortVlan);

        register(EthernetSwitch.class, EthernetSwitchAcl.class, "switchAcls", EthernetSwitch::addAcl);
        register(EthernetSwitch.class, EthernetSwitchPort.class, "switchPorts", EthernetSwitch::addPort);
        register(EthernetSwitch.class, EthernetSwitchMetrics.class, "switchMetrics", EthernetSwitch::setSwitchMetrics);

        register(EthernetSwitchAcl.class, EthernetSwitchAclRule.class, "rules", EthernetSwitchAcl::addRule);
        register(EthernetSwitchAcl.class, EthernetSwitchPort.class, "boundPorts", EthernetSwitchAcl::addBoundPort);
        register(EthernetSwitchAcl.class, EthernetSwitchPort.class, "bindActionAllowableValues", EthernetSwitchAcl::addBindActionAllowableValue);

        register(EthernetSwitchAclRule.class, EthernetSwitchPort.class, "forwardMirrorInterface", EthernetSwitchAclRule::setForwardMirrorInterface);
        register(EthernetSwitchAclRule.class, EthernetSwitchPort.class, "mirrorPortRegion", EthernetSwitchAclRule::addMirrorPort);

        register(EthernetSwitchPort.class, EthernetSwitchPortMetrics.class, "switchPortMetrics", EthernetSwitchPort::setMetrics);
        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "vlans", EthernetSwitchPort::addEthernetSwitchPortVlan);
        register(EthernetSwitchPort.class, EthernetSwitchStaticMac.class, "staticMacs", EthernetSwitchPort::addEthernetSwitchStaticMac);
        register(EthernetSwitchPort.class, EthernetSwitchPort.class, "portMembers", EthernetSwitchPort::addPortMember);
        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "primaryVlan", EthernetSwitchPort::setPrimaryVlan);

        register(LogicalDrive.class, LogicalDrive.class, "masterDrive", LogicalDrive::setMasterDrive);
        register(LogicalDrive.class, LogicalDrive.class, "logicalDrives", LogicalDrive::addUsedLogicalDrive);
        register(LogicalDrive.class, PhysicalDrive.class, "physicalDrives", LogicalDrive::addPhysicalDrive);

        register(Manager.class, NetworkProtocol.class, "networkProtocol", Manager::setNetworkProtocol);
        register(Manager.class, EthernetInterface.class, "ethernetInterfaces", Manager::addEthernetInterface);
        register(Manager.class, ComputerSystem.class, "managedComputerSystems", Manager::addComputerSystem);
        register(Manager.class, Chassis.class, "managedChassisCollection", Manager::addManagedChassis);
        register(Manager.class, Chassis.class, "managerInChassis", Manager::setInChassisManager);
        register(Manager.class, EthernetSwitch.class, "managedEthernetSwitches", Manager::addEthernetSwitch);
        register(Manager.class, StorageService.class, "managedServices", Manager::addStorageService);

        register(RemoteTarget.class, LogicalDrive.class, "logicalDrives", RemoteTarget::addLogicalDrive);

        register(StorageService.class, LogicalDrive.class, "logicalDrives", StorageService::addLogicalDrive);
        register(StorageService.class, RemoteTarget.class, "targets", StorageService::addRemoteTarget);
        register(StorageService.class, Manager.class, "managedBy", StorageService::addManager);
        register(StorageService.class, PhysicalDrive.class, "physicalDrives", StorageService::addPhysicalDrive);

        register(PcieDevice.class, PcieDeviceFunction.class, "pcieDeviceFunctions", PcieDevice::addPcieDeviceFunction);
        register(PcieDevice.class, Chassis.class, "deviceInChassis", PcieDevice::addChassis);

        register(Fabric.class, Endpoint.class, "endpointInFabric", Fabric::addEndpoint);
        register(Fabric.class, Switch.class, "switchInFabric", Fabric::addSwitch);
        register(Fabric.class, Zone.class, "zoneInFabric", Fabric::addZone);
        register(Port.class, PortMetrics.class, "portMetrics", Port::setPortMetrics);

        register(Switch.class, Port.class, "portsInFabricSwitch", Switch::addPort);
        register(Switch.class, Chassis.class, "chassisInSwitch", Switch::addChassis);
        register(Switch.class, Manager.class, "managedByInSwitch", Switch::addManager);

        register(Endpoint.class, Port.class, "portsInEndpoint", Endpoint::addPort);
        register(Endpoint.class, ConnectedEntity.class, "connectedEntityInEndpoint", Endpoint::addConnectedEntity);
        register(ConnectedEntity.class, Drive.class, "drivesInConnectedEntity", ConnectedEntity::setEntityLink);

        register(Zone.class, Endpoint.class, "endpointInZone", Zone::addEndpoint);
        register(Zone.class, Switch.class, "switchInZone", Zone::addSwitch);

        register(Drive.class, Storage.class, "storageInDrive", Drive::setStorage);
        register(Drive.class, PcieDeviceFunction.class, "pcieDeviceFunctionInDrive", Drive::setPcieDeviceFunction);

        register(PcieDeviceFunction.class, PcieDevice.class, "pcieDevice", PcieDeviceFunction::setPcieDevice);
        register(PcieDeviceFunction.class, Drive.class, "functionOfDrives", PcieDeviceFunction::addDrive);
        register(PcieDeviceFunction.class, StorageController.class, "functionStorageControllers", PcieDeviceFunction::addStorageController);
        register(PcieDeviceFunction.class, EthernetInterface.class, "functionEthernetInterfaces", PcieDeviceFunction::addEthernetInterface);

        register(MetricReportDefinition.class, MetricItem.class, "metricsInMetricReportDefinition", MetricReportDefinition::addMetricItem);
        register(MetricItem.class, MetricDefinition.class, "metricDefinitionsInMetrics", MetricItem::setMetricDefinition);
    }

    private static <S extends Entity, T extends Entity>
    void register(Class<S> sourceClass, Class<T> targetClass, String linkName, Linker.LinkMethod<S, T> method) {
        LINKERS.add(new Linker(sourceClass, targetClass, linkName, method));
    }

    @Produces
    public Collection<Linker> create() {
        return unmodifiableCollection(LINKERS);
    }
}
