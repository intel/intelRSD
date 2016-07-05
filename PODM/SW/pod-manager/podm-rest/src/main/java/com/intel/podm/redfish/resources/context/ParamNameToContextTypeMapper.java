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

package com.intel.podm.redfish.resources.context;

import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.rest.resources.PathParamConstants;

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
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

final class ParamNameToContextTypeMapper {
    static final Map<String, ContextType> MAPPING;

    static {
        Map<String, ContextType> map = new HashMap<>();

        map.put(PathParamConstants.PROCESSOR_ID, PROCESSOR);
        map.put(PathParamConstants.ETHERNET_INTERFACE_ID, ETHERNET_INTERFACE);
        map.put(PathParamConstants.MANAGER_ID, MANAGER);
        map.put(PathParamConstants.STORAGE_SERVICE_ID, STORAGE_SERVICE);
        map.put(PathParamConstants.PHYSICAL_DRIVE_ID, PHYSICAL_DRIVE);
        map.put(PathParamConstants.LOGICAL_DRIVE_ID, LOGICAL_DRIVE);
        map.put(PathParamConstants.REMOTE_TARGET_ID, REMOTE_TARGET);

        // Redfish resources
        map.put(PathParamConstants.CHASSIS_ID, CHASSIS);
        map.put(PathParamConstants.COMPUTER_SYSTEM_ID, COMPUTER_SYSTEM);
        map.put(PathParamConstants.MEMORY_ID, MEMORY);
        map.put(PathParamConstants.MEMORY_CHUNK_ID, MEMORY_CHUNK);
        map.put(PathParamConstants.ETHERNET_SWITCH_ID, ETHERNET_SWITCH);
        map.put(PathParamConstants.ETHERNET_SWITCH_PORT_ID, ETHERNET_SWITCH_PORT);
        map.put(PathParamConstants.ETHERNET_SWITCH_PORT_VLAN_ID, ETHERNET_SWITCH_PORT_VLAN);
        map.put(PathParamConstants.PROCESSOR_ID, PROCESSOR);
        map.put(PathParamConstants.NETWORK_PROTOCOL_ID, NETWORK_PROTOCOL);
        map.put(PathParamConstants.ADAPTER_ID, ADAPTER);
        map.put(PathParamConstants.DEVICE_ID, DEVICE);
        map.put(PathParamConstants.SIMPLE_STORAGE_ID, SIMPLE_STORAGE);
        map.put(PathParamConstants.COMPOSED_NODE_ID, COMPOSED_NODE);
        map.put(PathParamConstants.THERMAL_ZONE_ID, THERMAL_ZONE);
        map.put(PathParamConstants.POWER_ZONE_ID, POWER_ZONE);

        MAPPING = unmodifiableMap(map);
    }

    public boolean contains(String paramName) {
        return MAPPING.containsKey(paramName);
    }

    public ContextType get(String paramName) {
        if (!MAPPING.containsKey(paramName)) {
            String msg = format("paramName '%s' is not known", paramName);
            throw new IllegalArgumentException(msg);
        }

        return MAPPING.get(paramName);
    }
}
