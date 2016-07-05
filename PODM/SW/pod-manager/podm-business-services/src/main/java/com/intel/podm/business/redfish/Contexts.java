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

package com.intel.podm.business.redfish;

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
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PhysicalDrive;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;

import java.util.Collection;
import java.util.List;
import java.util.function.Function;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.ADAPTER;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.LOGICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PHYSICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.REMOTE_TARGET;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

public final class Contexts {
    private Contexts() {
    }

    public static Collection<Id> getAsIdList(Collection<? extends DomainObject> domainObjects) {
        return transform(domainObjects, DomainObject::getId);
    }

    public static List<Context> asLogicalDriveContexts(Collection<LogicalDrive> logicalDrives) {
        return transform(logicalDrives, Contexts::toContext);
    }

    public static List<Context> asLocalDriveContexts(Collection<Device> localDrives) {
        return transform(localDrives, Contexts::toContext);
    }

    public static List<Context> asSimpleStorageContexts(Collection<SimpleStorage> simpleStorageCollection) {
        return transform(simpleStorageCollection, Contexts::toContextOrNull);
    }

    public static List<Context> asPhysicalDriveContexts(Collection<PhysicalDrive> physicalDrives) {
        return transform(physicalDrives, Contexts::toContext);
    }

    public static List<Context> asStorageServicesContexts(Collection<StorageService> storageServices) {
        return transform(storageServices, Contexts::toContext);
    }

    public static List<Context> asRemoteTargetContexts(Collection<RemoteTarget> remoteTargets) {
        return transform(remoteTargets, Contexts::toContext);
    }

    public static List<Context> asEthernetSwitchContexts(Collection<EthernetSwitch> ethernetSwitches) {
        return transform(ethernetSwitches, Contexts::toContext);
    }

    public static List<Context> asChassisContexts(Collection<Chassis> chassis) {
        return transform(chassis, Contexts::toContext);
    }

    public static List<Context> asComputerSystemContexts(Collection<ComputerSystem> computerSystems) {
        return transform(computerSystems, Contexts::toContext);
    }

    public static List<Context> asMemoryContexts(Collection<Memory> memoryModules) {
        return transform(memoryModules, Contexts::toContext);
    }

    public static List<Context> asProcessorContexts(Collection<Processor> processors) {
        return transform(processors, Contexts::toContext);
    }

    public static List<Context> asEthernetInterfaceContexts(Collection<EthernetInterface> ethernetInterfaces) {
        return transform(ethernetInterfaces, Contexts::toContext);
    }

    public static List<Context> asEthernetSwitchPortContexts(Collection<EthernetSwitchPort> ports) {
        return transform(ports, Contexts::toContext);
    }

    public static List<Context> asManagerContexts(Collection<Manager> managers) {
        return transform(managers, Contexts::toContext);
    }

    public static Context toContext(Adapter adapter) {
        return toContext(adapter.getComputerSystem()).child(adapter.getId(), ADAPTER);
    }

    public static Context toContext(Chassis chassis) {
        return contextOf(chassis.getId(), CHASSIS);
    }

    public static Context toContext(ComputerSystem computerSystem) {
        return contextOf(computerSystem.getId(), COMPUTER_SYSTEM);
    }

    public static Context toContext(Memory memory) {
        return toContext(memory.getComputerSystem()).child(memory.getId(), MEMORY);
    }

    public static Context toContext(Processor processor) {
        return toContext(processor.getComputerSystem()).child(processor.getId(), PROCESSOR);
    }

    public static Context toContext(EthernetInterface ethernetInterface) {
        return toContext(ethernetInterface.getComputerSystem()).child(ethernetInterface.getId(), ETHERNET_INTERFACE);
    }

    public static Context toContext(EthernetSwitchPort port) {
        return toContext(port.getEthernetSwitch()).child(port.getId(), ETHERNET_SWITCH_PORT);
    }

    public static Context toContext(EthernetSwitchPortVlan vlan) {
        return toContext(vlan.getSwitchPort()).child(vlan.getId(), ETHERNET_SWITCH_PORT_VLAN);
    }

    public static Context toContext(StorageService storageService) {
        if (storageService == null) {
            return null;
        }

        return contextOf(storageService.getId(), STORAGE_SERVICE);
    }

    public static Context toContextOrNull(SimpleStorage simpleStorage) {
        if (simpleStorage == null) {
            return null;
        }

        return toContext(simpleStorage.getComputerSystem()).child(simpleStorage.getId(), SIMPLE_STORAGE);
    }

    public static Context toContext(PhysicalDrive physicalDrive) {
        Id storageServiceId = physicalDrive.getStorageService().getId();
        return contextOf(storageServiceId, STORAGE_SERVICE).child(physicalDrive.getId(), PHYSICAL_DRIVE);
    }

    public static Context toContext(LogicalDrive logicalDrive) {
        if (logicalDrive == null) {
            return null;
        }

        return toContext(logicalDrive.getStorageService()).child(logicalDrive.getId(), LOGICAL_DRIVE);
    }

    public static Context toContext(Device localDrive) {
        if (localDrive == null) {
            return null;
        }

        return toContext(localDrive.getAdapter()).child(localDrive.getId(), DEVICE);
    }

    public static Context toContext(RemoteTarget remoteTarget) {
        if (remoteTarget == null) {
            return null;
        }

        return toContext(remoteTarget.getStorageService()).child(remoteTarget.getId(), REMOTE_TARGET);
    }

    public static Context toContext(NetworkProtocol networkProtocol) {
        if (networkProtocol == null) {
            return null;
        }

        return contextOf(networkProtocol.getId(), NETWORK_PROTOCOL);
    }

    public static Context toContext(Manager manager) {
        return contextOf(manager.getId(), MANAGER);
    }

    public static Context toContext(EthernetSwitch ethernetSwitch) {
        return contextOf(ethernetSwitch.getId(), ETHERNET_SWITCH);
    }

    private static <T, R> List<R> transform(Iterable<T> items, Function<T, R> function) {
        return stream(items.spliterator(), false).map(function).collect(toList());
    }
}
