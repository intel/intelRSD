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

import java.util.ArrayList;
import java.util.Collection;

import static com.google.common.collect.Lists.newArrayList;

public final class PathParamConstants {

    public static final String ETHERNET_SWITCH_ID = "{ethernetSwitchId}";
    public static final String ETHERNET_SWITCH_PORT_ID = "{ethernetSwitchPortId}";
    public static final String ETHERNET_SWITCH_PORT_VLAN_ID = "{ethernetSwitchPortVlanId}";
    public static final String COMPOSED_NODE_ID = "{composedNodeId}";
    public static final String PROCESSOR_ID = "{processorId}";
    public static final String ETHERNET_INTERFACE_ID = "{ethernetInterfaceId}";
    public static final String PHYSICAL_DRIVE_ID = "{physicalDriveId}";
    public static final String LOGICAL_DRIVE_ID = "{logicalDriveId}";
    public static final String REMOTE_TARGET_ID = "{remoteTargetId}";
    public static final String MANAGER_ID = "{managerId}";
    public static final String STORAGE_SERVICE_ID = "{storageServiceId}";
    public static final String CHASSIS_ID = "{chassisId}";
    public static final String COMPUTER_SYSTEM_ID = "{computerSystemId}";
    public static final String NETWORK_PROTOCOL_ID = "{networkProtocolId}";
    public static final String ADAPTER_ID = "{adapterId}";
    public static final String DEVICE_ID = "{deviceId}";
    public static final String MEMORY_CHUNK_ID = "{memoryChunkId}";
    public static final String MEMORY_ID = "{memoryId}";
    public static final String SIMPLE_STORAGE_ID = "{simpleStorageId}";
    public static final String THERMAL_ZONE_ID = "{thermalZoneId}";
    public static final String POWER_ZONE_ID = "{powerZoneId}";

    private static final ArrayList<String> TOPOLOGICALLY_ORDERED_PARAMETERS;

    static {
        TOPOLOGICALLY_ORDERED_PARAMETERS = newArrayList(
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
                MEMORY_ID,
                ETHERNET_SWITCH_ID,
                ETHERNET_SWITCH_PORT_ID,
                ETHERNET_SWITCH_PORT_VLAN_ID,
                NETWORK_PROTOCOL_ID,
                MEMORY_CHUNK_ID,
                ADAPTER_ID,
                DEVICE_ID,
                THERMAL_ZONE_ID,
                POWER_ZONE_ID
        );
    }

    private PathParamConstants() {
    }

    public static Collection<String> getPathParameterNames() {
        return TOPOLOGICALLY_ORDERED_PARAMETERS;
    }
}
