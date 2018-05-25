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

package com.intel.podm.common.types.redfish;

import java.lang.reflect.Field;
import java.util.HashSet;
import java.util.Set;

import static java.util.Collections.unmodifiableSet;

public final class ResourceNames {
    public static final Set<String> ALL_RESOURCE_NAMES;

    public static final String PROCESSORS_RESOURCE_NAME = "Processors";
    public static final String ETHERNET_INTERFACES_RESOURCE_NAME = "EthernetInterfaces";
    public static final String MANAGERS_RESOURCE_NAME = "Managers";
    public static final String STORAGE_SERVICES_RESOURCE_NAME = "StorageServices";
    public static final String VOLUMES_RESOURCE_NAME = "Volumes";
    public static final String VOLUME_METRICS_RESOURCE_NAME = "Metrics";
    public static final String CHASSIS_RESOURCE_NAME = "Chassis";
    public static final String COMPUTER_SYSTEM_RESOURCE_NAME = "Systems";
    public static final String MEMORY_RESOURCE_NAME = "Memory";
    public static final String SIMPLE_STORAGE_RESOURCE_NAME = "SimpleStorage";
    public static final String STORAGE_RESOURCE_NAME = "Storage";
    public static final String NETWORK_PROTOCOL_RESOURCE_NAME = "NetworkProtocol";
    public static final String THERMAL_RESOURCE_NAME = "Thermal";
    public static final String THERMAL_FAN_RESOURCE_NAME = "Fans";
    public static final String THERMAL_TEMPERATURE_RESOURCE_NAME = "Temperatures";
    public static final String REDUNDANCY_RESOURCE_NAME = "Redundancy";
    public static final String POWER_RESOURCE_NAME = "Power";
    public static final String POWER_VOLTAGE_RESOURCE_NAME = "Voltages";
    public static final String POWER_CONTROL_RESOURCE_NAME = "PowerControl";
    public static final String POWER_SUPPLY_RESOURCE_NAME = "PowerSupplies";
    public static final String COMPOSED_NODES_RESOURCE_NAME = "Nodes";
    public static final String ETHERNET_SWITCHES_RESOURCE_NAME = "EthernetSwitches";
    public static final String ETHERNET_SWITCH_METRICS_RESOURCE_NAME = "Metrics";
    public static final String ACLS_RESOURCE_NAME = "ACLs";
    public static final String RULES_RESOURCE_NAME = "Rules";
    public static final String PORTS_RESOURCE_NAME = "Ports";
    public static final String PORT_METRICS_RESOURCE_NAME = "Metrics";
    public static final String ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME = "VLANs";
    public static final String ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME = "StaticMACs";
    public static final String ETHERNET_SWITCH_PORT_METRICS_RESOURCE_NAME = "Metrics";
    public static final String DRIVES_RESOURCE_NAME = "Drives";
    public static final String DRIVE_METRICS_RESOURCE_NAME = "Metrics";
    public static final String PCIE_DEVICES_RESOURCE_NAME = "PCIeDevices";
    public static final String PCIE_DEVICE_FUNCTIONS_RESOURCE_NAME = "Functions";
    public static final String FABRIC_RESOURCE_NAME = "Fabrics";
    public static final String ZONES_RESOURCE_NAME = "Zones";
    public static final String SWITCHES_RESOURCE_NAME = "Switches";
    public static final String ENDPOINTS_RESOURCE_NAME = "Endpoints";
    public static final String EVENT_SUBSCRIPTION_RESOURCE_NAME = "Subscriptions";
    public static final String EVENT_SERVICE_RESOURCE_NAME = "EventService";
    public static final String TELEMETRY_SERVICE_RESOURCE_NAME = "TelemetryService";
    public static final String METRIC_DEFINITION_RESOURCE_NAME = "MetricDefinitions";
    public static final String METRIC_REPORT_DEFINITION_RESOURCE_NAME = "MetricReportDefinitions";
    public static final String COMPUTER_SYSTEM_METRICS_RESOURCE_NAME = "Metrics";
    public static final String PROCESSOR_METRICS_RESOURCE_NAME = "Metrics";
    public static final String MEMORY_METRICS_RESOURCE_NAME = "Metrics";
    public static final String NETWORK_INTERFACES_RESOURCE_NAME = "NetworkInterfaces";
    public static final String NETWORK_DEVICE_FUNCTIONS_RESOURCE_NAME = "NetworkDeviceFunctions";
    public static final String STORAGE_POOL_RESOURCE_NAME = "StoragePools";

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
