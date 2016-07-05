/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.MemoryChunk;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PhysicalDrive;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.ThermalZone;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Produces;
import java.util.ArrayList;
import java.util.Collection;

import static java.util.Collections.unmodifiableCollection;

@Dependent
public class LinkerCollectionProducer {
    private static final Collection<Linker> LINKERS = new ArrayList<>();

    static {
        register(Adapter.class, Device.class, "devices", Adapter::link);

        register(Chassis.class, Chassis.class, "containsChassis", Chassis::contain);
        register(Chassis.class, ComputerSystem.class, "computerSystems", Chassis::link);
        register(Chassis.class, EthernetSwitch.class,  "containsSwitches", Chassis::linkEthernetSwitch);
        register(Chassis.class, ThermalZone.class, "thermalZones", Chassis::linkThermalZone);
        register(Chassis.class, PowerZone.class, "powerZones", Chassis::linkPowerZone);

        register(ComputerSystem.class, MemoryChunk.class, "memoryChunks", ComputerSystem::link);
        register(ComputerSystem.class, Memory.class, "memoryModules", ComputerSystem::link);
        register(ComputerSystem.class, Adapter.class, "adapters", ComputerSystem::link);
        register(ComputerSystem.class, Processor.class, "processors", ComputerSystem::linkProcessor);
        register(ComputerSystem.class, EthernetInterface.class, "ethernetInterfaces", ComputerSystem::linkEthernetInterface);
        register(ComputerSystem.class, SimpleStorage.class, "simpleStorages", ComputerSystem::linkSimpleStorage);

        register(EthernetInterface.class, EthernetSwitchPortVlan.class, "ethernetInterfaceVlans", EthernetInterface::addVlan);

        register(EthernetSwitch.class, EthernetSwitchPort.class, "switchPorts", EthernetSwitch::addPort);

        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "vlans", EthernetSwitchPort::addVlan);
        register(EthernetSwitchPort.class, EthernetSwitchPort.class, "portMembers", EthernetSwitchPort::addPortMember);
        register(EthernetSwitchPort.class, EthernetSwitchPortVlan.class, "primaryVlan", EthernetSwitchPort::setPrimaryVlan);

        register(LogicalDrive.class, LogicalDrive.class, "masterDrive", LogicalDrive::addMasterDrive);
        register(LogicalDrive.class, LogicalDrive.class, "logicalDrives", LogicalDrive::use);
        register(LogicalDrive.class, PhysicalDrive.class, "physicalDrives", LogicalDrive::addPhysicalDrive);

        register(Manager.class, NetworkProtocol.class, "networkProtocol", Manager::setNetworkProtocol);
        register(Manager.class, EthernetInterface.class, "ethernetInterfaces", Manager::addEthernetInterface);
        register(Manager.class, ComputerSystem.class, "managedComputerSystems", Manager::addComputerSystem);
        register(Manager.class, Chassis.class, "managedChassisCollection", Manager::addChassis);
        register(Manager.class, Chassis.class, "managerInChassis", Manager::setManagerInChassis);
        register(Manager.class, EthernetSwitch.class, "managedEthernetSwitches", Manager::addEthernetSwitch);
        register(Manager.class, StorageService.class, "managedServices", Manager::addStorageService);

        register(RemoteTarget.class, LogicalDrive.class, "logicalDrives", RemoteTarget::addLogicalDrive);

        register(StorageService.class, LogicalDrive.class, "logicalDrives", StorageService::addLogicalDrive);
        register(StorageService.class, RemoteTarget.class, "targets", StorageService::addRemoteTarget);
        register(StorageService.class, Manager.class, "managedBy", StorageService::addManager);
        register(StorageService.class, PhysicalDrive.class, "physicalDrives", StorageService::addPhysicalDrive);
    }

    @Produces
    public Collection<Linker> create() {
        return unmodifiableCollection(LINKERS);
    }

    private static <S extends DomainObject, T extends DomainObject>
    void register(Class<S> sourceClass, Class<T> targetClass, String linkName, Linker.LinkMethod<S, T> method) {
        LINKERS.add(new Linker(sourceClass, targetClass, linkName, method));
    }
}
