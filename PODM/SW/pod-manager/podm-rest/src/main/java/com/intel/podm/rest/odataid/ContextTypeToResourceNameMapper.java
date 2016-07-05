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

package com.intel.podm.rest.odataid;

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
import static com.intel.podm.rest.resources.ResourceNames.ADAPTERS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.CHASSIS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.COMPUTER_SYSTEM_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.DEVICES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MEMORY_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_SWITCHES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_SWITCH_PORTS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.LOGICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MEMORY_CHUNK_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.NETWORK_PROTOCOL_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.PHYSICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.POWER_ZONES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.PROCESSORS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.REMOTE_TARGETS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.SIMPLE_STORAGE_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.THERMAL_ZONES_RESOURCE_NAME;
import static java.util.Collections.unmodifiableMap;

public class ContextTypeToResourceNameMapper {
    static final Map<ContextType, String> MAPPING;

    static {
        HashMap<ContextType, String> map = new HashMap<>();
        map.put(PROCESSOR, PROCESSORS_RESOURCE_NAME);
        map.put(ETHERNET_INTERFACE, ETHERNET_INTERFACES_RESOURCE_NAME);
        map.put(MANAGER, MANAGERS_RESOURCE_NAME);
        map.put(STORAGE_SERVICE, STORAGE_SERVICES_RESOURCE_NAME);
        map.put(PHYSICAL_DRIVE, PHYSICAL_DRIVES_RESOURCE_NAME);
        map.put(LOGICAL_DRIVE, LOGICAL_DRIVES_RESOURCE_NAME);
        map.put(REMOTE_TARGET, REMOTE_TARGETS_RESOURCE_NAME);
        map.put(CHASSIS, CHASSIS_RESOURCE_NAME);
        map.put(COMPUTER_SYSTEM, COMPUTER_SYSTEM_RESOURCE_NAME);
        map.put(NETWORK_PROTOCOL, NETWORK_PROTOCOL_RESOURCE_NAME);
        map.put(MEMORY_CHUNK, MEMORY_CHUNK_RESOURCE_NAME);
        map.put(MEMORY, MEMORY_RESOURCE_NAME);
        map.put(SIMPLE_STORAGE, SIMPLE_STORAGE_RESOURCE_NAME);
        map.put(ADAPTER, ADAPTERS_RESOURCE_NAME);
        map.put(DEVICE, DEVICES_RESOURCE_NAME);

        map.put(ETHERNET_SWITCH, ETHERNET_SWITCHES_RESOURCE_NAME);
        map.put(ETHERNET_SWITCH_PORT, ETHERNET_SWITCH_PORTS_RESOURCE_NAME);
        map.put(ETHERNET_SWITCH_PORT_VLAN, ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME);
        map.put(COMPOSED_NODE, COMPOSED_NODES_RESOURCE_NAME);

        map.put(THERMAL_ZONE, THERMAL_ZONES_RESOURCE_NAME);
        map.put(POWER_ZONE, POWER_ZONES_RESOURCE_NAME);

        MAPPING = unmodifiableMap(map);
    }

    public String get(ContextType type) {
        if (type == null) {
            throw new IllegalArgumentException("contextType must not be null");
        }

        return MAPPING.get(type);
    }
}
