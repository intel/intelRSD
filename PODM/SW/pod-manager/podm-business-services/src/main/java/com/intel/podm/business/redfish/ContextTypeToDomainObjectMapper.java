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
import com.intel.podm.business.entities.redfish.MemoryChunk;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.PhysicalDrive;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.ThermalZone;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.services.context.ContextType;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.services.context.ContextType.ADAPTER;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.LOGICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.MEMORY_CHUNK;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PHYSICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.POWER_ZONE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.REMOTE_TARGET;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.business.services.context.ContextType.THERMAL_ZONE;
import static java.util.Collections.unmodifiableMap;

public class ContextTypeToDomainObjectMapper {
    static final Map<ContextType, Class<? extends DomainObject>> MAPPING;

    static {
        HashMap<ContextType, Class<? extends DomainObject>> map = new HashMap<>();

        map.put(MANAGER, Manager.class);
        map.put(PROCESSOR, Processor.class);
        map.put(STORAGE_SERVICE, StorageService.class);
        map.put(PHYSICAL_DRIVE, PhysicalDrive.class);
        map.put(LOGICAL_DRIVE, LogicalDrive.class);
        map.put(REMOTE_TARGET, RemoteTarget.class);
        map.put(CHASSIS, Chassis.class);
        map.put(COMPUTER_SYSTEM, ComputerSystem.class);
        map.put(ETHERNET_SWITCH, EthernetSwitch.class);
        map.put(ETHERNET_SWITCH_PORT, EthernetSwitchPort.class);
        map.put(ETHERNET_SWITCH_PORT_VLAN, EthernetSwitchPortVlan.class);
        map.put(ETHERNET_INTERFACE, EthernetInterface.class);
        map.put(ADAPTER, Adapter.class);
        map.put(DEVICE, Device.class);
        map.put(MEMORY_CHUNK, MemoryChunk.class);
        map.put(MEMORY, Memory.class);
        map.put(NETWORK_PROTOCOL, NetworkProtocol.class);
        map.put(SIMPLE_STORAGE, SimpleStorage.class);
        map.put(COMPOSED_NODE, ComposedNode.class);
        map.put(THERMAL_ZONE, ThermalZone.class);
        map.put(POWER_ZONE, PowerZone.class);

        MAPPING = unmodifiableMap(map);
    }

    public Class<? extends DomainObject> get(ContextType contextType) {
        if (contextType == null) {
            throw new IllegalArgumentException("contextType must not be null");
        }

        return MAPPING.get(contextType);
    }
}
