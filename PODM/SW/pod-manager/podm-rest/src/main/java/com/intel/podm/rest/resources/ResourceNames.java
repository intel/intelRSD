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

package com.intel.podm.rest.resources;

import java.lang.reflect.Field;
import java.util.HashSet;
import java.util.Set;

import static java.util.Collections.unmodifiableSet;

public final class ResourceNames {
    public static final Set<String> ALL_RESOURCE_NAMES;

    public static final String PROCESSORS_RESOURCE_NAME = "Processors";
    public static final String ETHERNET_INTERFACES_RESOURCE_NAME = "EthernetInterfaces";
    public static final String MANAGERS_RESOURCE_NAME = "Managers";
    public static final String STORAGE_SERVICES_RESOURCE_NAME = "Services";
    public static final String LOGICAL_DRIVES_RESOURCE_NAME = "LogicalDrives";
    public static final String PHYSICAL_DRIVES_RESOURCE_NAME = "Drives";
    public static final String REMOTE_TARGETS_RESOURCE_NAME = "Targets";
    public static final String CHASSIS_RESOURCE_NAME = "Chassis";
    public static final String COMPUTER_SYSTEM_RESOURCE_NAME = "Systems";
    public static final String MEMORY_CHUNK_RESOURCE_NAME = "MemoryChunks";
    public static final String MEMORY_RESOURCE_NAME = "Memory";
    public static final String SIMPLE_STORAGE_RESOURCE_NAME = "SimpleStorage";
    public static final String NETWORK_PROTOCOL_RESOURCE_NAME = "NetworkProtocol";
    public static final String COMPOSED_NODES_RESOURCE_NAME = "Nodes";
    public static final String ETHERNET_SWITCHES_RESOURCE_NAME = "EthernetSwitches";
    public static final String ETHERNET_SWITCH_PORTS_RESOURCE_NAME = "Ports";
    public static final String ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME = "VLANs";
    public static final String ADAPTERS_RESOURCE_NAME = "Adapters";
    public static final String DEVICES_RESOURCE_NAME = "Devices";
    public static final String THERMAL_ZONES_RESOURCE_NAME = "ThermalZones";
    public static final String POWER_ZONES_RESOURCE_NAME = "PowerZones";

    /**
     * It fills ALL_RESOURCE_NAMES collection with all static String field values
     */
    static {
        Set<String> names = new HashSet<>();

        for (Field field : ResourceNames.class.getDeclaredFields()) {
            if (field.getType() != String.class) {
                continue;
            }

            try {
                names.add((String) field.get(null));
            } catch (IllegalAccessException e) {
                throw new RuntimeException(e);
            }
        }

        ALL_RESOURCE_NAMES = unmodifiableSet(names);
    }

    private ResourceNames() {
    }
}
