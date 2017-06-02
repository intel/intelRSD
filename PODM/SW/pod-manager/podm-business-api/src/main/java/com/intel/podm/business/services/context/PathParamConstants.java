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

import java.util.List;

import static java.util.Arrays.asList;

public final class PathParamConstants {
    public static final String ETHERNET_SWITCH_ID = "{ethernetSwitchId}";
    public static final String ETHERNET_SWITCH_PORT_ID = "{ethernetSwitchPortId}";
    public static final String ETHERNET_SWITCH_PORT_VLAN_ID = "{ethernetSwitchPortVlanId}";
    public static final String COMPOSED_NODE_ID = "composedNodeId";
    public static final String PROCESSOR_ID = "{processorId}";
    public static final String ETHERNET_INTERFACE_ID = "{ethernetInterfaceId}";
    public static final String PHYSICAL_DRIVE_ID = "{physicalDriveId}";
    public static final String LOGICAL_DRIVE_ID = "{logicalDriveId}";
    public static final String REMOTE_TARGET_ID = "{remoteTargetId}";
    public static final String MANAGER_ID = "{managerId}";
    public static final String STORAGE_SERVICE_ID = "{storageServiceId}";
    public static final String CHASSIS_ID = "{chassisId}";
    public static final String COMPUTER_SYSTEM_ID = "{computerSystemId}";
    public static final String NETWORK_INTERFACE_ID = "{networkInterfaceId}";
    public static final String NETWORK_DEVICE_FUNCTION_ID = "{networkDeviceFunctionId}";
    public static final String NETWORK_PROTOCOL_ID = "{networkProtocolId}";
    public static final String MEMORY_ID = "{memoryId}";
    public static final String SIMPLE_STORAGE_ID = "{simpleStorageId}";
    public static final String STORAGE_ID = "{storageId}";
    public static final String THERMAL_ID = "{thermalId}";
    public static final String THERMAL_TEMPERATURE_ID = "{thermalTemperatureId}";
    public static final String THERMAL_FAN_ID = "{thermalFanId}";
    public static final String REDUNDANCY_ID = "{redundancyId}";
    public static final String POWER_ID = "{powerId}";
    public static final String POWER_VOLTAGE_ID = "{powerVoltageId}";
    public static final String POWER_CONTROL_ID = "{powerControlId}";
    public static final String POWER_SUPPLY_ID = "{powerSupplyId}";
    public static final String THERMAL_ZONE_ID = "{thermalZoneId}";
    public static final String POWER_ZONE_ID = "{powerZoneId}";
    public static final String PCIE_DRIVE_ID = "{pcieDriveId}";
    public static final String PCIE_DEVICE_ID = "{pcieDeviceId}";
    public static final String PCIE_DEVICE_FUNCTION_ID = "{pcieDeviceFunctionId}";
    public static final String FABRIC_ID = "{fabricId}";
    public static final String ZONE_ID = "{zoneId}";
    public static final String SWITCH_ID = "{switchId}";
    public static final String PORT_ID = "{portId}";
    public static final String ENDPOINT_ID = "{endpointId}";
    public static final String EVENT_SERVICE_ID = "{eventServiceId}";
    public static final String EVENT_SUBSCRIPTION_ID = "{eventSubscriptionId}";

    private static final List<String> TOPOLOGICALLY_ORDERED_PARAMETERS;

    static {
        TOPOLOGICALLY_ORDERED_PARAMETERS = asList(
            COMPOSED_NODE_ID,
            MANAGER_ID,
            STORAGE_SERVICE_ID,
            PHYSICAL_DRIVE_ID,
            LOGICAL_DRIVE_ID,
            REMOTE_TARGET_ID,
            CHASSIS_ID,
            COMPUTER_SYSTEM_ID,
            PROCESSOR_ID,
            ETHERNET_INTERFACE_ID,
            SIMPLE_STORAGE_ID,
            STORAGE_ID,
            MEMORY_ID,
            ETHERNET_SWITCH_ID,
            ETHERNET_SWITCH_PORT_ID,
            ETHERNET_SWITCH_PORT_VLAN_ID,
            NETWORK_INTERFACE_ID,
            NETWORK_DEVICE_FUNCTION_ID,
            NETWORK_PROTOCOL_ID,
            THERMAL_ID,
            THERMAL_TEMPERATURE_ID,
            THERMAL_FAN_ID,
            REDUNDANCY_ID,
            POWER_ID,
            POWER_CONTROL_ID,
            POWER_VOLTAGE_ID,
            POWER_SUPPLY_ID,
            THERMAL_ZONE_ID,
            POWER_ZONE_ID,
            PCIE_DEVICE_ID,
            PCIE_DEVICE_FUNCTION_ID,
            PCIE_DRIVE_ID,
            FABRIC_ID,
            ZONE_ID,
            SWITCH_ID,
            PORT_ID,
            ENDPOINT_ID,
            EVENT_SERVICE_ID,
            EVENT_SUBSCRIPTION_ID
        );
    }

    private PathParamConstants() {
    }

    public static List<String> getPathParameterNames() {
        return TOPOLOGICALLY_ORDERED_PARAMETERS;
    }
}
