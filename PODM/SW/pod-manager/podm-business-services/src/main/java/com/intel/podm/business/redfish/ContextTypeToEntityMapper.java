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
import com.intel.podm.business.entities.redfish.EthernetSwitchMetrics;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortMetrics;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.MemoryMetrics;
import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.business.entities.redfish.MetricReportDefinition;
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
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.ContextType;

import javax.enterprise.context.ApplicationScoped;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

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
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_STATIC_MAC;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.MEMORY_METRICS;
import static com.intel.podm.business.services.context.ContextType.METRIC_DEFINITION;
import static com.intel.podm.business.services.context.ContextType.METRIC_REPORT_DEFINITION;
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
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.context.ContextType.THERMAL_FAN;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.business.services.context.ContextType.VOLUME_METRICS;
import static com.intel.podm.business.services.context.ContextType.ZONE;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

@ApplicationScoped
public class ContextTypeToEntityMapper {
    static final Map<ContextType, Class<? extends Entity>> MAPPING;

    static {
        HashMap<ContextType, Class<? extends Entity>> map = new HashMap<>();

        map.put(MANAGER, Manager.class);
        map.put(PROCESSOR, Processor.class);
        map.put(STORAGE_SERVICE, StorageService.class);
        map.put(STORAGE_POOL, StoragePool.class);
        map.put(VOLUME, Volume.class);
        map.put(CHASSIS, Chassis.class);
        map.put(COMPUTER_SYSTEM, ComputerSystem.class);
        map.put(ENDPOINT, Endpoint.class);
        map.put(ETHERNET_SWITCH, EthernetSwitch.class);
        map.put(ETHERNET_SWITCH_METRICS, EthernetSwitchMetrics.class);
        map.put(ETHERNET_SWITCH_ACL, EthernetSwitchAcl.class);
        map.put(ETHERNET_SWITCH_ACL_RULE, EthernetSwitchAclRule.class);
        map.put(ETHERNET_SWITCH_PORT, EthernetSwitchPort.class);
        map.put(ETHERNET_SWITCH_PORT_METRICS, EthernetSwitchPortMetrics.class);
        map.put(ETHERNET_SWITCH_PORT_VLAN, EthernetSwitchPortVlan.class);
        map.put(ETHERNET_SWITCH_STATIC_MAC, EthernetSwitchStaticMac.class);
        map.put(ETHERNET_INTERFACE, EthernetInterface.class);
        map.put(MEMORY, Memory.class);
        map.put(NETWORK_PROTOCOL, NetworkProtocol.class);
        map.put(SIMPLE_STORAGE, SimpleStorage.class);
        map.put(STORAGE, Storage.class);
        map.put(COMPOSED_NODE, ComposedNode.class);
        map.put(DRIVE, Drive.class);
        map.put(DRIVE_METRICS, DriveMetrics.class);
        map.put(VOLUME_METRICS, VolumeMetrics.class);
        map.put(PCIE_DEVICE, PcieDevice.class);
        map.put(PCIE_DEVICE_FUNCTION, PcieDeviceFunction.class);
        map.put(THERMAL, Thermal.class);
        map.put(THERMAL_FAN, ThermalFan.class);
        map.put(THERMAL_TEMPERATURE, ThermalTemperature.class);
        map.put(REDUNDANCY, Redundancy.class);
        map.put(POWER, Power.class);
        map.put(POWER_VOLTAGE, PowerVoltage.class);
        map.put(POWER_CONTROL, PowerControl.class);
        map.put(POWER_SUPPLY, PowerSupply.class);
        map.put(FABRIC, Fabric.class);
        map.put(ZONE, Zone.class);
        map.put(SWITCH, Switch.class);
        map.put(PORT, Port.class);
        map.put(PORT_METRICS, PortMetrics.class);
        map.put(EVENT_SUBSCRIPTION, EventSubscription.class);
        map.put(NETWORK_INTERFACE, NetworkInterface.class);
        map.put(NETWORK_DEVICE_FUNCTION, NetworkDeviceFunction.class);
        map.put(METRIC_DEFINITION, MetricDefinition.class);
        map.put(METRIC_REPORT_DEFINITION, MetricReportDefinition.class);
        map.put(COMPUTER_SYSTEM_METRICS, ComputerSystemMetrics.class);
        map.put(PROCESSOR_METRICS, ProcessorMetrics.class);
        map.put(MEMORY_METRICS, MemoryMetrics.class);

        MAPPING = unmodifiableMap(map);
    }

    public Class<? extends Entity> getEntityClass(ContextType contextType) {
        requiresNonNull(contextType, "contextType");
        Class<? extends Entity> entityClass = MAPPING.get(contextType);
        if (entityClass == null) {
            throw new UnsupportedOperationException(format("ContextType %s has no entity class associated with it.", contextType));
        }
        return entityClass;
    }

    public ContextType tryGetContextType(Class<? extends Entity> clazz) {
        requiresNonNull(clazz, "clazz");
        return MAPPING.entrySet().stream()
            .filter(entry -> Objects.equals(entry.getValue(), clazz))
            .findFirst()
            .map(Map.Entry::getKey)
            .orElse(null);
    }
}
