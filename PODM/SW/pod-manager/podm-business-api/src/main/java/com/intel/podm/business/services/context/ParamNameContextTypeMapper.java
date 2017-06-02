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

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;

import static com.google.common.collect.Maps.unmodifiableBiMap;
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
import static com.intel.podm.business.services.context.PathParamConstants.CHASSIS_ID;
import static com.intel.podm.business.services.context.PathParamConstants.COMPOSED_NODE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.COMPUTER_SYSTEM_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ENDPOINT_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_INTERFACE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_PORT_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_PORT_VLAN_ID;
import static com.intel.podm.business.services.context.PathParamConstants.EVENT_SERVICE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.EVENT_SUBSCRIPTION_ID;
import static com.intel.podm.business.services.context.PathParamConstants.FABRIC_ID;
import static com.intel.podm.business.services.context.PathParamConstants.LOGICAL_DRIVE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.MANAGER_ID;
import static com.intel.podm.business.services.context.PathParamConstants.MEMORY_ID;
import static com.intel.podm.business.services.context.PathParamConstants.NETWORK_DEVICE_FUNCTION_ID;
import static com.intel.podm.business.services.context.PathParamConstants.NETWORK_INTERFACE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.NETWORK_PROTOCOL_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PCIE_DEVICE_FUNCTION_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PCIE_DEVICE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PCIE_DRIVE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PHYSICAL_DRIVE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PORT_ID;
import static com.intel.podm.business.services.context.PathParamConstants.POWER_CONTROL_ID;
import static com.intel.podm.business.services.context.PathParamConstants.POWER_ID;
import static com.intel.podm.business.services.context.PathParamConstants.POWER_SUPPLY_ID;
import static com.intel.podm.business.services.context.PathParamConstants.POWER_VOLTAGE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.POWER_ZONE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.PROCESSOR_ID;
import static com.intel.podm.business.services.context.PathParamConstants.REDUNDANCY_ID;
import static com.intel.podm.business.services.context.PathParamConstants.REMOTE_TARGET_ID;
import static com.intel.podm.business.services.context.PathParamConstants.SIMPLE_STORAGE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.STORAGE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.STORAGE_SERVICE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.SWITCH_ID;
import static com.intel.podm.business.services.context.PathParamConstants.THERMAL_FAN_ID;
import static com.intel.podm.business.services.context.PathParamConstants.THERMAL_ID;
import static com.intel.podm.business.services.context.PathParamConstants.THERMAL_TEMPERATURE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.THERMAL_ZONE_ID;
import static com.intel.podm.business.services.context.PathParamConstants.ZONE_ID;
import static java.lang.String.format;

public final class ParamNameContextTypeMapper {
    static final BiMap<String, ContextType> MAPPING;

    static {
        BiMap<String, ContextType> map = HashBiMap.create();

        map.put(PROCESSOR_ID, PROCESSOR);
        map.put(ETHERNET_INTERFACE_ID, ETHERNET_INTERFACE);
        map.put(MANAGER_ID, MANAGER);
        map.put(STORAGE_SERVICE_ID, STORAGE_SERVICE);
        map.put(STORAGE_ID, STORAGE);
        map.put(PHYSICAL_DRIVE_ID, PHYSICAL_DRIVE);
        map.put(LOGICAL_DRIVE_ID, LOGICAL_DRIVE);
        map.put(REMOTE_TARGET_ID, REMOTE_TARGET);
        map.put(CHASSIS_ID, CHASSIS);
        map.put(COMPUTER_SYSTEM_ID, COMPUTER_SYSTEM);
        map.put(MEMORY_ID, MEMORY);
        map.put(ETHERNET_SWITCH_ID, ETHERNET_SWITCH);
        map.put(ETHERNET_SWITCH_PORT_ID, ETHERNET_SWITCH_PORT);
        map.put(ETHERNET_SWITCH_PORT_VLAN_ID, ETHERNET_SWITCH_PORT_VLAN);
        map.put(NETWORK_PROTOCOL_ID, NETWORK_PROTOCOL);
        map.put(SIMPLE_STORAGE_ID, SIMPLE_STORAGE);
        map.put(COMPOSED_NODE_ID, COMPOSED_NODE);
        map.put(THERMAL_ID, THERMAL);
        map.put(THERMAL_FAN_ID, THERMAL_FAN);
        map.put(THERMAL_TEMPERATURE_ID, THERMAL_TEMPERATURE);
        map.put(REDUNDANCY_ID, REDUNDANCY);
        map.put(POWER_ID, POWER);
        map.put(POWER_VOLTAGE_ID, POWER_VOLTAGE);
        map.put(POWER_CONTROL_ID, POWER_CONTROL);
        map.put(POWER_SUPPLY_ID, POWER_SUPPLY);
        map.put(THERMAL_ZONE_ID, THERMAL_ZONE);
        map.put(POWER_ZONE_ID, POWER_ZONE);
        map.put(PCIE_DEVICE_ID, PCIE_DEVICE);
        map.put(PCIE_DEVICE_FUNCTION_ID, PCIE_DEVICE_FUNCTION);
        map.put(PCIE_DRIVE_ID, DRIVE);
        map.put(FABRIC_ID, FABRIC);
        map.put(ZONE_ID, ZONE);
        map.put(SWITCH_ID, SWITCH);
        map.put(PORT_ID, PORT);
        map.put(ENDPOINT_ID, ENDPOINT);
        map.put(EVENT_SERVICE_ID, EVENT_SERVICE);
        map.put(EVENT_SUBSCRIPTION_ID, EVENT_SUBSCRIPTION);
        map.put(NETWORK_INTERFACE_ID, NETWORK_INTERFACE);
        map.put(NETWORK_DEVICE_FUNCTION_ID, NETWORK_DEVICE_FUNCTION);

        MAPPING = unmodifiableBiMap(map);
    }

    public boolean contains(String paramName) {
        return MAPPING.containsKey(paramName);
    }

    public boolean contains(ContextType contextType) {
        return MAPPING.inverse().containsKey(contextType);
    }

    public ContextType get(String paramName) {
        if (!MAPPING.containsKey(paramName)) {
            String msg = format("paramName '%s' is not known", paramName);
            throw new IllegalArgumentException(msg);
        }

        return MAPPING.get(paramName);
    }

    public String get(ContextType contextType) {
        if (!MAPPING.inverse().containsKey(contextType)) {
            String msg = format("contextType '%s' is not known", contextType);
            throw new IllegalArgumentException(msg);
        }

        return MAPPING.inverse().get(contextType);
    }
}
