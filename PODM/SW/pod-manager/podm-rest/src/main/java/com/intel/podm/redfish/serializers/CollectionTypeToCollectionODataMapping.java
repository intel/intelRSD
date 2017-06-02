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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.CollectionDto.Type;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.CHASSIS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.COMPOSED_NODES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ENDPOINTS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_INTERFACES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCHES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORTS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT_VLANS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRICS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCHES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCH_PORTS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.LOGICAL_DRIVES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGERS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_DEVICE_FUNTIONS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_INTERFACES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICE_FUNCTIONS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PHYSICAL_DRIVES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.POWER_ZONES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PROCESSORS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.REMOTE_TARGETS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SERVICES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SIMPLE_STORAGE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.EVENT_SUBSCRIPTIONS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SYSTEMS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.THERMAL_ZONES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ZONES;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

public final class CollectionTypeToCollectionODataMapping {

    static final Map<Type, ODataForCollection> COLLECTION_TYPE_TO_ODATA_MAPPING;

    static {
        Map<Type, ODataForCollection> collection = new HashMap<>();

        collection.put(CHASSIS, ODataForCollection
            .newBuilder()
            .name("Chassis Collection")
            .oDataType("#ChassisCollection.ChassisCollection")
            .build());

        collection.put(MANAGERS, ODataForCollection
            .newBuilder()
            .name("Manager Collection")
            .oDataType("#ManagerCollection.ManagerCollection")
            .build());

        collection.put(SYSTEMS, ODataForCollection
            .newBuilder()
            .name("Computer System Collection")
            .oDataType("#ComputerSystemCollection.ComputerSystemCollection")
            .build());

        collection.put(SERVICES, ODataForCollection
            .newBuilder()
            .name("Storage Services Collection")
            .oDataType("#StorageServiceCollection.StorageServiceCollection")
            .build());

        collection.put(COMPOSED_NODES, ODataForCollection
            .newBuilder()
            .name("Composed Nodes Collection")
            .oDataType("#ComposedNodeCollection.ComposedNodeCollection")
            .build());

        collection.put(EVENT_SUBSCRIPTIONS, ODataForCollection
            .newBuilder()
            .name("Event Subscriptions Collection")
            .oDataType("#EventDestinationCollection.EventDestinationCollection")
            .build());

        collection.put(PROCESSORS, ODataForCollection
            .newBuilder()
            .name("Processors Collection")
            .oDataType("#ProcessorCollection.ProcessorCollection")
            .build());

        collection.put(ETHERNET_INTERFACES, ODataForCollection
            .newBuilder()
            .name("Ethernet Interface Collection")
            .oDataType("#EthernetInterfaceCollection.EthernetInterfaceCollection")
            .build());

        collection.put(SIMPLE_STORAGE, ODataForCollection
            .newBuilder()
            .name("Simple Storage Collection")
            .oDataType("#SimpleStorageCollection.SimpleStorageCollection")
            .build());

        collection.put(STORAGE, ODataForCollection
            .newBuilder()
            .name("Storage Collection")
            .oDataType("#StorageCollection.StorageCollection")
            .build());

        collection.put(MEMORY_MODULES, ODataForCollection
            .newBuilder()
            .name("Memory Collection")
            .oDataType("#MemoryCollection.MemoryCollection")
            .build());

        collection.put(ETHERNET_SWITCHES, ODataForCollection
            .newBuilder()
            .name("Ethernet Switches Collection")
            .oDataType("#EthernetSwitchCollection.EthernetSwitchCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORTS, ODataForCollection
            .newBuilder()
            .name("Switch Port Collection")
            .oDataType("#EthernetSwitchPortCollection.EthernetSwitchPortCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORT_VLANS, ODataForCollection
            .newBuilder()
            .name("VLAN Network Interface Collection")
            .oDataType("#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection")
            .build());

        collection.put(REMOTE_TARGETS, ODataForCollection
            .newBuilder()
            .name("Remote Targets Collection")
            .oDataType("#RemoteTargetCollection.RemoteTargetCollection")
            .build());

        collection.put(LOGICAL_DRIVES, ODataForCollection
            .newBuilder()
            .name("Logical Drives Collection")
            .oDataType("#LogicalDriveCollection.LogicalDriveCollection")
            .build());

        collection.put(PHYSICAL_DRIVES, ODataForCollection
            .newBuilder()
            .name("Physical Drives Collection")
            .oDataType("#PhysicalDriveCollection.PhysicalDriveCollection")
            .build());

        collection.put(THERMAL_ZONES, ODataForCollection
            .newBuilder()
            .name("Thermal Zones Collection")
            .oDataType("#ThermalZoneCollection.ThermalZoneCollection")
            .build());

        collection.put(POWER_ZONES, ODataForCollection
            .newBuilder()
            .name("Power Zones Collection")
            .oDataType("#PowerZoneCollection.PowerZoneCollection")
            .build());

        collection.put(DRIVES, ODataForCollection
            .newBuilder()
            .name("Drive Collection")
            .oDataType("#StorageDriveCollection.StorageDriveCollection")
            .build());

        collection.put(PCIE_DEVICES, ODataForCollection
            .newBuilder()
            .name("PCIe Device Collection")
            .oDataType("#PCIeDeviceCollection.PCIeDeviceCollection")
            .build());

        collection.put(PCIE_DEVICE_FUNCTIONS, ODataForCollection
            .newBuilder()
            .name("PCIe Function Collection")
            .oDataType("#PCIeFunctionCollection.PCIeFunctionCollection")
            .build());

        collection.put(NETWORK_INTERFACES, ODataForCollection
            .newBuilder()
            .name("Network Interfaces Collection")
            .oDataType("#NetworkInterfaceCollection.NetworkInterfaceCollection")
            .build());

        collection.put(NETWORK_DEVICE_FUNTIONS, ODataForCollection
            .newBuilder()
            .name("Network Device Functions Collection")
            .oDataType("#NetworkDeviceFunctionCollection.NetworkDeviceFunctionCollection")
            .build());

        collection.put(FABRICS, ODataForCollection
            .newBuilder()
            .name("Fabrics Collection")
            .oDataType("#FabricCollection.FabricCollection")
            .build());

        collection.put(ZONES, ODataForCollection
            .newBuilder()
            .name("Zones Collection")
            .oDataType("#ZoneCollection.ZoneCollection")
            .build());

        collection.put(FABRIC_SWITCHES, ODataForCollection
            .newBuilder()
            .name("Switch Collection")
            .oDataType("#SwitchCollection.SwitchCollection")
            .build());

        collection.put(FABRIC_SWITCH_PORTS, ODataForCollection
            .newBuilder()
            .name("Ports Collection")
            .oDataType("#PortCollection.PortCollection")
            .build());

        collection.put(ENDPOINTS, ODataForCollection
            .newBuilder()
            .name("Endpoints Collection")
            .oDataType("#EndpointCollection.EndpointCollection")
            .build());

        COLLECTION_TYPE_TO_ODATA_MAPPING = unmodifiableMap(collection);
    }

    private CollectionTypeToCollectionODataMapping() {
    }

    public static ODataForCollection getOdataForCollectionType(Type collectionDtoType) {
        if (!COLLECTION_TYPE_TO_ODATA_MAPPING.containsKey(collectionDtoType)) {
            String msg = format("Unknown Collection type: '%s'", collectionDtoType);
            throw new IllegalArgumentException(msg);
        }
        return COLLECTION_TYPE_TO_ODATA_MAPPING.get(collectionDtoType);
    }
}
