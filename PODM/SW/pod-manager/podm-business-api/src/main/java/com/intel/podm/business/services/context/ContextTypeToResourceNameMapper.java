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

package com.intel.podm.business.services.context;

import java.util.HashMap;
import java.util.Map;

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
import static com.intel.podm.business.services.context.ContextType.NETWORK_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.NETWORK_INTERFACE;
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
import static com.intel.podm.common.types.redfish.ResourceNames.CHASSIS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPUTER_SYSTEM_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SERVICE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SUBSCRIPTION_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.FABRIC_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.LOGICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MEMORY_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_DEVICE_FUNCTIONS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_PROTOCOL_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PCIE_DEVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PCIE_DEVICE_FUNCTIONS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PCIE_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PHYSICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PORTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_CONTROL_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_SUPPLY_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_VOLTAGE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_ZONES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PROCESSORS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.REDUNDANCY_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.REMOTE_TARGETS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.SIMPLE_STORAGE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_FAN_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_TEMPERATURE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_ZONES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ZONES_RESOURCE_NAME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
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
        map.put(NETWORK_INTERFACE, NETWORK_INTERFACES_RESOURCE_NAME);
        map.put(NETWORK_DEVICE_FUNCTION, NETWORK_DEVICE_FUNCTIONS_RESOURCE_NAME);
        map.put(MEMORY, MEMORY_RESOURCE_NAME);
        map.put(SIMPLE_STORAGE, SIMPLE_STORAGE_RESOURCE_NAME);
        map.put(STORAGE, STORAGE_RESOURCE_NAME);

        map.put(ETHERNET_SWITCH, ETHERNET_SWITCHES_RESOURCE_NAME);
        map.put(ETHERNET_SWITCH_PORT, PORTS_RESOURCE_NAME);
        map.put(ETHERNET_SWITCH_PORT_VLAN, ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME);
        map.put(COMPOSED_NODE, COMPOSED_NODES_RESOURCE_NAME);

        map.put(POWER, POWER_RESOURCE_NAME);
        map.put(POWER_VOLTAGE, POWER_VOLTAGE_RESOURCE_NAME);
        map.put(POWER_CONTROL, POWER_CONTROL_RESOURCE_NAME);
        map.put(POWER_SUPPLY, POWER_SUPPLY_RESOURCE_NAME);
        map.put(THERMAL, THERMAL_RESOURCE_NAME);
        map.put(THERMAL_FAN, THERMAL_FAN_RESOURCE_NAME);
        map.put(THERMAL_TEMPERATURE, THERMAL_TEMPERATURE_RESOURCE_NAME);
        map.put(REDUNDANCY, REDUNDANCY_RESOURCE_NAME);
        map.put(THERMAL_ZONE, THERMAL_ZONES_RESOURCE_NAME);
        map.put(POWER_ZONE, POWER_ZONES_RESOURCE_NAME);

        map.put(DRIVE, PCIE_DRIVES_RESOURCE_NAME);
        map.put(PCIE_DEVICE, PCIE_DEVICES_RESOURCE_NAME);
        map.put(PCIE_DEVICE_FUNCTION, PCIE_DEVICE_FUNCTIONS_RESOURCE_NAME);

        map.put(FABRIC, FABRIC_RESOURCE_NAME);
        map.put(ZONE, ZONES_RESOURCE_NAME);
        map.put(SWITCH, SWITCHES_RESOURCE_NAME);
        map.put(PORT, PORTS_RESOURCE_NAME);
        map.put(ENDPOINT, ENDPOINTS_RESOURCE_NAME);

        map.put(EVENT_SUBSCRIPTION, EVENT_SUBSCRIPTION_RESOURCE_NAME);
        map.put(EVENT_SERVICE, EVENT_SERVICE_RESOURCE_NAME);

        MAPPING = unmodifiableMap(map);
    }

    public String get(ContextType contextType) {
        requiresNonNull(contextType, "contextType");

        return MAPPING.get(contextType);
    }
}
