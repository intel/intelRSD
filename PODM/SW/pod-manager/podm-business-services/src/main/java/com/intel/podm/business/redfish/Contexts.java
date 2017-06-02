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

package com.intel.podm.business.redfish;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.entities.redfish.PhysicalDrive;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.redfish.ThermalZone;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.Map;
import java.util.Optional;
import java.util.function.Function;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.business.services.context.ContextType.ENDPOINT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.business.services.context.ContextType.LOGICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.PHYSICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.PORT;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.POWER_CONTROL;
import static com.intel.podm.business.services.context.ContextType.POWER_SUPPLY;
import static com.intel.podm.business.services.context.ContextType.POWER_VOLTAGE;
import static com.intel.podm.business.services.context.ContextType.POWER_ZONE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.REMOTE_TARGET;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.business.services.context.ContextType.SWITCH;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.context.ContextType.THERMAL_FAN;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.business.services.context.ContextType.THERMAL_ZONE;
import static com.intel.podm.business.services.context.ContextType.ZONE;
import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static com.intel.podm.common.types.Id.id;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toMap;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public final class Contexts {

    private static final Logger LOGGER = LoggerFactory.getLogger(Contexts.class);

    private static final Map<Class<?>, Method> CLASS_TO_CONTEXT_METHOD_MAPPING = Arrays.stream(Contexts.class.getDeclaredMethods())
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
                return Optional.of((Context) method.get().invoke(null, entity));
            }
        } catch (IllegalAccessException | InvocationTargetException e) {
            LOGGER.e("Cannot generate context for {}", entity);
        }
        return empty();
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

    private static Context toContext(ThermalZone thermalZone) {
        return getParentContext(thermalZone, ThermalZone::getChassis).child(thermalZone.getId(), THERMAL_ZONE);
    }

    private static Context toContext(PowerZone powerZone) {
        return getParentContext(powerZone, PowerZone::getChassis).child(powerZone.getId(), POWER_ZONE);
    }

    private static Context toContext(ComputerSystem computerSystem) {
        return contextOf(computerSystem.getId(), COMPUTER_SYSTEM);
    }

    private static Context toContext(Thermal thermal) {
        return getParentContext(thermal, Thermal::getChassis).child(id(""), THERMAL);
    }

    private static Context toContext(ThermalFan thermalFan) {
        return getParentContext(thermalFan, ThermalFan::getThermal).child(id(getIndex(thermalFan)), THERMAL_FAN);
    }

    private static Context toContext(ThermalTemperature thermalTemperature) {
        return getParentContext(thermalTemperature, ThermalTemperature::getThermal).child(id(getIndex(thermalTemperature)), THERMAL_TEMPERATURE);
    }

    private static Context toContext(Redundancy redundancy) {
        return getParentContext(redundancy, Redundancy::getRedundancyOwner).child(id(getIndex(redundancy)), REDUNDANCY);
    }

    private static Context toContext(Power power) {
        return getParentContext(power, Power::getChassis).child(id(""), POWER);
    }

    private static Context toContext(PowerVoltage powerVoltage) {
        return getParentContext(powerVoltage, PowerVoltage::getPower).child(id(getIndex(powerVoltage)), POWER_VOLTAGE);
    }

    private static Context toContext(PowerSupply powerSupply) {
        return getParentContext(powerSupply, PowerSupply::getPower).child(id(getIndex(powerSupply)), POWER_SUPPLY);
    }

    private static Context toContext(PowerControl powerControl) {
        return getParentContext(powerControl, PowerControl::getPower).child(id(getIndex(powerControl)), POWER_CONTROL);
    }

    private static Context toContext(Memory memory) {
        return getParentContext(memory, Memory::getComputerSystem).child(memory.getId(), MEMORY);
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

    private static Context toContext(StorageService storageService) {
        return contextOf(storageService.getId(), STORAGE_SERVICE);
    }

    private static Context toContext(SimpleStorage simpleStorage) {
        return getParentContext(simpleStorage, SimpleStorage::getComputerSystem).child(simpleStorage.getId(), SIMPLE_STORAGE);
    }

    private static Context toContext(PhysicalDrive physicalDrive) {
        return getParentContext(physicalDrive, PhysicalDrive::getStorageService).child(physicalDrive.getId(), PHYSICAL_DRIVE);
    }

    private static Context toContext(LogicalDrive logicalDrive) {
        return getParentContext(logicalDrive, LogicalDrive::getStorageService).child(logicalDrive.getId(), LOGICAL_DRIVE);
    }

    private static Context toContext(RemoteTarget remoteTarget) {
        return getParentContext(remoteTarget, RemoteTarget::getStorageService).child(remoteTarget.getId(), REMOTE_TARGET);
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

    public static String getIndex(DiscoverableEntity discoverableEntity) {
        String idValue = discoverableEntity.getId().getValue();
        return idValue.substring(idValue.lastIndexOf("/") + 1);
    }
}
