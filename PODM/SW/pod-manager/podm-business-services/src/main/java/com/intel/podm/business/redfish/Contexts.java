/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ComputerSystemMetrics;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.DriveMetrics;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.MemoryMetrics;
import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.PortMetrics;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.ProcessorMetrics;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.VolumeMetrics;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;
import java.util.Optional;
import java.util.function.Function;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM_METRICS;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.business.services.context.ContextType.DRIVE_METRICS;
import static com.intel.podm.business.services.context.ContextType.ENDPOINT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_ACL;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_ACL_RULE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_STATIC_MAC;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.MEMORY_METRICS;
import static com.intel.podm.business.services.context.ContextType.METRIC_DEFINITION;
import static com.intel.podm.business.services.context.ContextType.NETWORK_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.NETWORK_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.PORT;
import static com.intel.podm.business.services.context.ContextType.PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.POWER_CONTROL;
import static com.intel.podm.business.services.context.ContextType.POWER_SUPPLY;
import static com.intel.podm.business.services.context.ContextType.POWER_VOLTAGE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR_METRICS;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_POOL;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.business.services.context.ContextType.SWITCH;
import static com.intel.podm.business.services.context.ContextType.TELEMETRY_SERVICE;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.context.ContextType.THERMAL_FAN;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.business.services.context.ContextType.VOLUME_METRICS;
import static com.intel.podm.business.services.context.ContextType.ZONE;
import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static com.intel.podm.common.types.Id.id;
import static java.lang.String.format;
import static java.util.Arrays.stream;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toMap;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public final class Contexts {

    private static final Map<Class<?>, Method> CLASS_TO_CONTEXT_METHOD_MAPPING = stream(Contexts.class.getDeclaredMethods())
        .filter(method -> method.getReturnType().equals(Context.class))
        .filter(method -> method.getParameterTypes().length == 1)
        .collect(toMap(method -> method.getParameterTypes()[0], m -> m))
        .entrySet().stream()
        .filter(entry -> Entity.class.isAssignableFrom(entry.getKey()))
        .filter(entry -> !Entity.class.equals(entry.getKey()))
        .collect(toMap(Map.Entry::getKey, Map.Entry::getValue));

    private Contexts() {
    }

    public static Context toContext(Entity entity) {
        if (entity == null) {
            return null;
        }
        try {
            return (Context) ofNullable(CLASS_TO_CONTEXT_METHOD_MAPPING.get(unproxy(entity.getClass())))
                .orElseThrow(() -> new UnsupportedOperationException("Contexts.toContext(" + entity.getClass() + ") method is not implemented"))
                .invoke(null, entity);
        } catch (IllegalAccessException | InvocationTargetException e) {
            throw new RuntimeException(e);
        }
    }

    public static Optional<Context> tryConvertToContext(Entity entity) {
        if (entity == null) {
            return empty();
        }

        Optional<Method> method = ofNullable(CLASS_TO_CONTEXT_METHOD_MAPPING.get(unproxy(entity.getClass())));
        try {
            if (method.isPresent()) {
                return of((Context) method.get().invoke(null, entity));
            }
            return empty();
        } catch (IllegalAccessException | InvocationTargetException e) {
            throw new RuntimeException(e);
        }
    }

    private static <T extends DiscoverableEntity, R extends DiscoverableEntity> Context getParentContext(T entity, Function<T, R> function) {
        R parent = function.apply(entity);
        if (parent == null) {
            throw new RuntimeException(
                format("Parent context for %s#%s could not be created. Parent does not exist.", entity.getClass().getSimpleName(), entity.getId())
            );
        }
        return toContext(parent);
    }

    private static Context toContext(Port port) {
        return getParentContext(port, Port::getSwitch).child(port.getId(), PORT);
    }

    private static Context toContext(Switch fabricSwitch) {
        return getParentContext(fabricSwitch, Switch::getFabric).child(fabricSwitch.getId(), SWITCH);
    }

    private static Context toContext(Zone zone) {
        return getParentContext(zone, Zone::getFabric).child(zone.getId(), ZONE);
    }

    private static Context toContext(Chassis chassis) {
        return contextOf(chassis.getId(), CHASSIS);
    }

    private static Context toContext(ComputerSystem computerSystem) {
        return contextOf(computerSystem.getId(), COMPUTER_SYSTEM);
    }

    private static Context toContext(PortMetrics portMetrics) {
        return getParentContext(portMetrics, PortMetrics::getPort).child(id(""), PORT_METRICS);
    }

    private static Context toContext(Thermal thermal) {
        return getParentContext(thermal, Thermal::getChassis).child(id(""), THERMAL);
    }

    private static Context toContext(ThermalFan thermalFan) {
        return getParentContext(thermalFan, ThermalFan::getThermal).child(id(getLastSegment(thermalFan.getId())), THERMAL_FAN);
    }

    private static Context toContext(ThermalTemperature thermalTemperature) {
        return getParentContext(thermalTemperature, ThermalTemperature::getThermal)
            .child(id(getLastSegment(thermalTemperature.getId())), THERMAL_TEMPERATURE);
    }

    private static Context toContext(Redundancy redundancy) {
        return getParentContext(redundancy, Redundancy::getRedundancyOwner).child(id(getLastSegment(redundancy.getId())), REDUNDANCY);
    }

    private static Context toContext(Power power) {
        return getParentContext(power, Power::getChassis).child(id(""), POWER);
    }

    private static Context toContext(PowerVoltage powerVoltage) {
        return getParentContext(powerVoltage, PowerVoltage::getPower).child(id(getLastSegment(powerVoltage.getId())), POWER_VOLTAGE);
    }

    private static Context toContext(PowerSupply powerSupply) {
        return getParentContext(powerSupply, PowerSupply::getPower).child(id(getLastSegment(powerSupply.getId())), POWER_SUPPLY);
    }

    private static Context toContext(PowerControl powerControl) {
        return getParentContext(powerControl, PowerControl::getPower).child(id(getLastSegment(powerControl.getId())), POWER_CONTROL);
    }

    private static Context toContext(Memory memory) {
        return getParentContext(memory, Memory::getComputerSystem).child(memory.getId(), MEMORY);
    }

    private static Context toContext(MemoryMetrics memoryMetrics) {
        return getParentContext(memoryMetrics, MemoryMetrics::getMemory).child(id(""), MEMORY_METRICS);
    }

    private static Context toContext(Fabric fabric) {
        return contextOf(fabric.getId(), FABRIC);
    }

    private static Context toContext(Processor processor) {
        return getParentContext(processor, Processor::getComputerSystem).child(processor.getId(), PROCESSOR);
    }

    private static Context toContext(EthernetInterface ethernetInterface) {
        if (ethernetInterface.getComputerSystem() != null) {
            return getParentContext(ethernetInterface, EthernetInterface::getComputerSystem).child(ethernetInterface.getId(), ETHERNET_INTERFACE);
        }
        Manager parent = ethernetInterface.getManagers().stream()
            .findFirst()
            .orElseThrow(() -> new UnsupportedOperationException(
                format("Contexts.toContext(%s) could not be performed. Parent Manager could not be found.", ethernetInterface.getClass())));
        return toContext(parent).child(ethernetInterface.getId(), ETHERNET_INTERFACE);
    }

    private static Context toContext(EthernetSwitchPort port) {
        return getParentContext(port, EthernetSwitchPort::getEthernetSwitch).child(port.getId(), ETHERNET_SWITCH_PORT);
    }

    private static Context toContext(EthernetSwitchPortVlan vlan) {
        if (vlan.getEthernetSwitchPort() != null) {
            return getParentContext(vlan, EthernetSwitchPortVlan::getEthernetSwitchPort).child(vlan.getId(), ETHERNET_SWITCH_PORT_VLAN);
        }
        return getParentContext(vlan, EthernetSwitchPortVlan::getEthernetInterface).child(vlan.getId(), ETHERNET_SWITCH_PORT_VLAN);
    }

    private static Context toContext(EthernetSwitchStaticMac ethernetSwitchStaticMac) {
        return getParentContext(ethernetSwitchStaticMac, EthernetSwitchStaticMac::getEthernetSwitchPort).child(ethernetSwitchStaticMac.getId(),
            ETHERNET_SWITCH_STATIC_MAC);
    }

    private static Context toContext(EthernetSwitchAcl acl) {
        return getParentContext(acl, EthernetSwitchAcl::getEthernetSwitch).child(acl.getId(), ETHERNET_SWITCH_ACL);
    }

    private static Context toContext(EthernetSwitchAclRule aclRule) {
        return getParentContext(aclRule, EthernetSwitchAclRule::getEthernetSwitchAcl).child(aclRule.getId(), ETHERNET_SWITCH_ACL_RULE);
    }

    private static Context toContext(StorageService storageService) {
        return contextOf(storageService.getId(), STORAGE_SERVICE);
    }

    private static Context toContext(SimpleStorage simpleStorage) {
        return getParentContext(simpleStorage, SimpleStorage::getComputerSystem).child(simpleStorage.getId(), SIMPLE_STORAGE);
    }

    private static Context toContext(StoragePool storagePool) {
        return getParentContext(storagePool, StoragePool::getStorageService).child(storagePool.getId(), STORAGE_POOL);
    }

    private static Context toContext(Volume volume) {
        return getParentContext(volume, Volume::getStorageService).child(volume.getId(), VOLUME);
    }

    private static Context toContext(NetworkProtocol networkProtocol) {
        return contextOf(networkProtocol.getId(), NETWORK_PROTOCOL);
    }

    private static Context toContext(Manager manager) {
        return contextOf(manager.getId(), MANAGER);
    }

    private static Context toContext(Endpoint endpoint) {
        return getParentContext(endpoint, Endpoint::getFabric).child(endpoint.getId(), ENDPOINT);
    }

    private static Context toContext(Drive drive) {
        return getParentContext(drive, Drive::getChassis).child(drive.getId(), DRIVE);
    }

    private static Context toContext(DriveMetrics driveMetrics) {
        return getParentContext(driveMetrics, DriveMetrics::getDrive).child(id(""), DRIVE_METRICS);
    }

    private static Context toContext(VolumeMetrics volumeMetrics) {
        return getParentContext(volumeMetrics, VolumeMetrics::getVolume).child(id(""), VOLUME_METRICS);
    }

    private static Context toContext(Storage storage) {
        return getParentContext(storage, Storage::getComputerSystem).child(storage.getId(), STORAGE);
    }

    private static Context toContext(EthernetSwitch ethernetSwitch) {
        return contextOf(ethernetSwitch.getId(), ETHERNET_SWITCH);
    }

    private static Context toContext(PcieDevice pcieDevice) {
        Chassis chassis = pcieDevice.getChassis().stream().findFirst()
            .orElseThrow(() -> new UnsupportedOperationException(format("Contexts.toContext(%s) could not be performed. "
                + "Parent chassis could not be found.", pcieDevice.getClass())));
        return toContext(chassis).child(pcieDevice.getId(), PCIE_DEVICE);
    }

    private static Context toContext(PcieDeviceFunction pcieDeviceFunction) {
        return getParentContext(pcieDeviceFunction, PcieDeviceFunction::getPcieDevice).child(pcieDeviceFunction.getId(), PCIE_DEVICE_FUNCTION);
    }

    private static Context toContext(ComposedNode composedNode) {
        return contextOf(composedNode.getId(), COMPOSED_NODE);
    }

    private static Context toContext(EventSubscription eventSubscription) {
        return contextOf(id(""), EVENT_SERVICE).child(eventSubscription.getId(), EVENT_SUBSCRIPTION);
    }

    private static Context toContext(MetricDefinition metricDefinition) {
        return contextOf(id(""), TELEMETRY_SERVICE).child(metricDefinition.getId(), METRIC_DEFINITION);
    }

    private static Context toContext(ComputerSystemMetrics computerSystemMetrics) {
        return getParentContext(computerSystemMetrics, ComputerSystemMetrics::getComputerSystem).child(id(""), COMPUTER_SYSTEM_METRICS);
    }

    private static Context toContext(ProcessorMetrics processorMetrics) {
        return getParentContext(processorMetrics, ProcessorMetrics::getProcessor).child(id(""), PROCESSOR_METRICS);
    }

    private static Context toContext(NetworkInterface networkInterface) {
        return getParentContext(networkInterface, NetworkInterface::getComputerSystem).child(networkInterface.getId(), NETWORK_INTERFACE);
    }

    private static Context toContext(NetworkDeviceFunction networkDeviceFunction) {
        return getParentContext(networkDeviceFunction, NetworkDeviceFunction::getNetworkInterface).child(networkDeviceFunction.getId(),
            NETWORK_DEVICE_FUNCTION);
    }

    private static String getLastSegment(Id id) {
        return id.getValue().substring(id.getValue().lastIndexOf('-') + 1);
    }
}
