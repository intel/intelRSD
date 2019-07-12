/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.linker;

import com.intel.rsd.nodecomposer.persistence.redfish.CapacitySource;
import com.intel.rsd.nodecomposer.persistence.redfish.Chassis;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitch;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.Manager;
import com.intel.rsd.nodecomposer.persistence.redfish.Memory;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkAdapter;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkDeviceFunction;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.PcieDevice;
import com.intel.rsd.nodecomposer.persistence.redfish.PcieDeviceFunction;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorage;
import com.intel.rsd.nodecomposer.persistence.redfish.Storage;
import com.intel.rsd.nodecomposer.persistence.redfish.StoragePool;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.Switch;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Scope;

import java.util.ArrayList;
import java.util.Collection;

import static java.util.Collections.unmodifiableCollection;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Configuration
@Scope(SCOPE_PROTOTYPE)
public class LinkerCollectionProducer {
    private static final Collection<Linker> LINKERS = new ArrayList<>();

    static {
        register(Chassis.class, Chassis.class, "containsChassis", Chassis::addContainedChassis);
        register(Chassis.class, ComputerSystem.class, "computerSystems", Chassis::addComputerSystem);
        register(Chassis.class, EthernetSwitch.class, "containsSwitches", Chassis::addEthernetSwitch);
        register(Chassis.class, Drive.class, "drives", Chassis::addDrive);
        register(Chassis.class, Storage.class, "storage", Chassis::addStorage);
        register(Chassis.class, NetworkAdapter.class, "networkAdapters", Chassis::addNetworkAdapter);

        register(NetworkAdapter.class, NetworkDeviceFunction.class, "networkDeviceFunctionsInNetworkAdapter", NetworkAdapter::addNetworkDeviceFunction);

        register(ComputerSystem.class, Memory.class, "memoryModules", ComputerSystem::addMemoryModule);
        register(ComputerSystem.class, Processor.class, "processors", ComputerSystem::addProcessor);
        register(ComputerSystem.class, EthernetInterface.class, "ethernetInterfaces", ComputerSystem::addEthernetInterface);
        register(ComputerSystem.class, SimpleStorage.class, "simpleStorages", ComputerSystem::addSimpleStorage);
        register(ComputerSystem.class, Storage.class, "storage", ComputerSystem::addStorage);
        register(ComputerSystem.class, Storage.class, "adapters", ComputerSystem::addStorage);
        register(ComputerSystem.class, Endpoint.class, "endpoints", ComputerSystem::addEndpoint);
        register(ComputerSystem.class, NetworkInterface.class, "networkInterfaces", ComputerSystem::addNetworkInterface);
        register(ComputerSystem.class, StorageService.class, "storageServices", ComputerSystem::addStorageService);

        register(NetworkInterface.class, NetworkDeviceFunction.class, "networkDeviceFunctions", NetworkInterface::addNetworkDeviceFunction);

        register(Storage.class, Drive.class, "drives", Storage::addDrive);
        register(Storage.class, Drive.class, "devices", Storage::addDrive);

        register(Drive.class, Storage.class, "storageInDrive", Drive::setStorage);
        register(Drive.class, PcieDeviceFunction.class, "pcieDeviceFunctionInDrive", Drive::setPcieDeviceFunction);

        register(EthernetInterface.class, EthernetSwitchPortVlan.class, "ethernetInterfaceVlans", EthernetInterface::addEthernetSwitchPortVlan);

        register(EthernetSwitch.class, EthernetSwitchPort.class, "switchPorts", EthernetSwitch::addPort);

        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "vlans", EthernetSwitchPort::addEthernetSwitchPortVlan);
        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "primaryVlan", EthernetSwitchPort::setPrimaryVlan);

        register(StorageService.class, Drive.class, "physicalDrives", StorageService::addDrive);
        register(StorageService.class, Endpoint.class, "endpoints", StorageService::addEndpoint);
        register(StorageService.class, Volume.class, "volumes", StorageService::addVolume);
        register(StorageService.class, StoragePool.class, "storagePools", StorageService::addStoragePool);
        register(StorageService.class, ComputerSystem.class, "hostingSystemInStorage", StorageService::setComputerSystem);
        register(StorageService.class, Fabric.class, "fabric", StorageService::setFabric);

        register(Volume.class, Drive.class, "drives", Volume::addDrives);
        register(Volume.class, CapacitySource.class, "capacitySourcesInVolume", Volume::addCapacitySource);

        register(PcieDevice.class, PcieDeviceFunction.class, "pcieDeviceFunctions", PcieDevice::addPcieDeviceFunction);
        register(PcieDevice.class, Chassis.class, "deviceInChassis", PcieDevice::addChassis);

        register(Fabric.class, Endpoint.class, "endpointInFabric", Fabric::addEndpoint);
        register(Fabric.class, Switch.class, "switchInFabric", Fabric::addSwitch);
        register(Fabric.class, Zone.class, "zoneInFabric", Fabric::addZone);
        register(Fabric.class, Manager.class, "managedByFabric", Fabric::addManager);

        register(Manager.class, ComputerSystem.class, "managerForServers", Manager::addComputerSystem);
        register(Manager.class, StorageService.class, "managerForServices", Manager::addStorageService);

        register(Switch.class, Port.class, "portsInFabricSwitch", Switch::addPort);
        register(Switch.class, Chassis.class, "chassisInSwitch", Switch::addChassis);

        register(Endpoint.class, EthernetInterface.class, "ethernetInterfaceInEndpoint", Endpoint::addEthernetInterface);
        register(Endpoint.class, Port.class, "portsInEndpoint", Endpoint::addPort);
        register(Endpoint.class, ConnectedEntity.class, "connectedEntityInEndpoint", Endpoint::addConnectedEntity);
        register(Endpoint.class, IpTransportDetails.class, "transportInEndpoint", Endpoint::addIpTransportDetails);
        register(ConnectedEntity.class, Drive.class, "resourceInConnectedEntity", ConnectedEntity::setEntityLink);
        register(ConnectedEntity.class, Volume.class, "resourceInConnectedEntity", ConnectedEntity::setEntityLink);
        register(ConnectedEntity.class, ComputerSystem.class, "resourceInConnectedEntity", ConnectedEntity::setEntityLink);
        register(ConnectedEntity.class, Processor.class, "resourceInConnectedEntity", ConnectedEntity::setEntityLink);

        register(Zone.class, Endpoint.class, "endpointInZone", Zone::addEndpoint);

        register(PcieDeviceFunction.class, PcieDevice.class, "pcieDevice", PcieDeviceFunction::setPcieDevice);
        register(PcieDeviceFunction.class, Drive.class, "functionOfDrives", PcieDeviceFunction::addDrive);

        register(StoragePool.class, Volume.class, "allocatedVolumes", StoragePool::addAllocatedVolume);
        register(StoragePool.class, StoragePool.class, "allocatedPools", StoragePool::addAllocatedPool);

        register(CapacitySource.class, StoragePool.class, "providingPools", CapacitySource::addProvidingPool);
    }

    private static <S extends Entity, T extends Entity> void register(Class<S> sourceClass, Class<T> targetClass,
                                                                      String linkName, Linker.LinkMethod<S, T> method) {
        LINKERS.add(new Linker(sourceClass, targetClass, linkName, method));
    }

    @Bean
    public Collection<Linker> create() {
        return unmodifiableCollection(LINKERS);
    }
}
