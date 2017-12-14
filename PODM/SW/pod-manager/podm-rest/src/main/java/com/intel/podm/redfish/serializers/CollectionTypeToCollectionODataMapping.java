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
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_ACLS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_ACL_RULES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORTS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT_VLANS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_STATIC_MACS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRICS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCHES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCH_PORTS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.LOGICAL_DRIVES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGERS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.METRICS_DEFINITION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.METRICS_REPORT_DEFINITION;
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
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SYSTEMS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.THERMAL_ZONES;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ZONES;
import static com.intel.podm.redfish.serializers.ODataForCollection.newOdataForCollectionBuilder;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

final class CollectionTypeToCollectionODataMapping {
    static final Map<Type, ODataForCollection> COLLECTION_TYPE_TO_ODATA_MAPPING;

    static {
        Map<Type, ODataForCollection> collection = new HashMap<>();

        collection.put(CHASSIS, newOdataForCollectionBuilder()
            .name("Chassis Collection")
            .oDataType("#ChassisCollection.ChassisCollection")
            .build());

        collection.put(MANAGERS, newOdataForCollectionBuilder()
            .name("Manager Collection")
            .oDataType("#ManagerCollection.ManagerCollection")
            .build());

        collection.put(SYSTEMS, newOdataForCollectionBuilder()
            .name("Computer System Collection")
            .oDataType("#ComputerSystemCollection.ComputerSystemCollection")
            .build());

        collection.put(SERVICES, newOdataForCollectionBuilder()
            .name("Storage Services Collection")
            .oDataType("#StorageServiceCollection.StorageServiceCollection")
            .build());

        collection.put(COMPOSED_NODES, newOdataForCollectionBuilder()
            .name("Composed Nodes Collection")
            .oDataType("#ComposedNodeCollection.ComposedNodeCollection")
            .build());

        collection.put(EVENT_SUBSCRIPTION, newOdataForCollectionBuilder()
            .name("Event Subscriptions Collection")
            .oDataType("#EventDestinationCollection.EventDestinationCollection")
            .build());

        collection.put(PROCESSORS, newOdataForCollectionBuilder()
            .name("Processors Collection")
            .oDataType("#ProcessorCollection.ProcessorCollection")
            .build());

        collection.put(ETHERNET_INTERFACES, newOdataForCollectionBuilder()
            .name("Ethernet Interface Collection")
            .oDataType("#EthernetInterfaceCollection.EthernetInterfaceCollection")
            .build());

        collection.put(SIMPLE_STORAGE, newOdataForCollectionBuilder()
            .name("Simple Storage Collection")
            .oDataType("#SimpleStorageCollection.SimpleStorageCollection")
            .build());

        collection.put(STORAGE, newOdataForCollectionBuilder()
            .name("Storage Collection")
            .oDataType("#StorageCollection.StorageCollection")
            .build());

        collection.put(MEMORY_MODULES, newOdataForCollectionBuilder()
            .name("Memory Collection")
            .oDataType("#MemoryCollection.MemoryCollection")
            .build());

        collection.put(ETHERNET_SWITCHES, newOdataForCollectionBuilder()
            .name("Ethernet Switches Collection")
            .oDataType("#EthernetSwitchCollection.EthernetSwitchCollection")
            .build());

        collection.put(ETHERNET_SWITCH_ACLS, newOdataForCollectionBuilder()
            .name("Ethernet Switch Access Control List Collection")
            .oDataType("#EthernetSwitchACLCollection.EthernetSwitchACLCollection")
            .build());

        collection.put(ETHERNET_SWITCH_ACL_RULES, newOdataForCollectionBuilder()
            .name("Ethernet Switch Access Control List Rules Collection")
            .oDataType("#EthernetSwitchACLRuleCollection.EthernetSwitchACLRuleCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORTS, newOdataForCollectionBuilder()
            .name("Switch Port Collection")
            .oDataType("#EthernetSwitchPortCollection.EthernetSwitchPortCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORT_VLANS, newOdataForCollectionBuilder()
            .name("VLAN Network Interface Collection")
            .oDataType("#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection")
            .build());

        collection.put(ETHERNET_SWITCH_STATIC_MACS, newOdataForCollectionBuilder()
            .name("Static MAC Collection")
            .oDataType("#EthernetSwitchStaticMACCollection.EthernetSwitchStaticMACCollection")
            .build());

        collection.put(REMOTE_TARGETS, newOdataForCollectionBuilder()
            .name("Remote Targets Collection")
            .oDataType("#RemoteTargetCollection.RemoteTargetCollection")
            .build());

        collection.put(LOGICAL_DRIVES, newOdataForCollectionBuilder()
            .name("Logical Drives Collection")
            .oDataType("#LogicalDriveCollection.LogicalDriveCollection")
            .build());

        collection.put(PHYSICAL_DRIVES, newOdataForCollectionBuilder()
            .name("Physical Drives Collection")
            .oDataType("#PhysicalDriveCollection.PhysicalDriveCollection")
            .build());

        collection.put(THERMAL_ZONES, newOdataForCollectionBuilder()
            .name("Thermal Zones Collection")
            .oDataType("#ThermalZoneCollection.ThermalZoneCollection")
            .build());

        collection.put(POWER_ZONES, newOdataForCollectionBuilder()
            .name("Power Zones Collection")
            .oDataType("#PowerZoneCollection.PowerZoneCollection")
            .build());

        collection.put(DRIVES, newOdataForCollectionBuilder()
            .name("Drive Collection")
            .oDataType("#StorageDriveCollection.StorageDriveCollection")
            .build());

        collection.put(PCIE_DEVICES, newOdataForCollectionBuilder()
            .name("PCIe Device Collection")
            .oDataType("#PCIeDeviceCollection.PCIeDeviceCollection")
            .build());

        collection.put(PCIE_DEVICE_FUNCTIONS, newOdataForCollectionBuilder()
            .name("PCIe Function Collection")
            .oDataType("#PCIeFunctionCollection.PCIeFunctionCollection")
            .build());

        collection.put(NETWORK_INTERFACES, newOdataForCollectionBuilder()
            .name("Network Interfaces Collection")
            .oDataType("#NetworkInterfaceCollection.NetworkInterfaceCollection")
            .build());

        collection.put(NETWORK_DEVICE_FUNTIONS, newOdataForCollectionBuilder()
            .name("Network Device Functions Collection")
            .oDataType("#NetworkDeviceFunctionCollection.NetworkDeviceFunctionCollection")
            .build());

        collection.put(FABRICS, newOdataForCollectionBuilder()
            .name("Fabrics Collection")
            .oDataType("#FabricCollection.FabricCollection")
            .build());

        collection.put(ZONES, newOdataForCollectionBuilder()
            .name("Zones Collection")
            .oDataType("#ZoneCollection.ZoneCollection")
            .build());

        collection.put(FABRIC_SWITCHES, newOdataForCollectionBuilder()
            .name("Switch Collection")
            .oDataType("#SwitchCollection.SwitchCollection")
            .build());

        collection.put(FABRIC_SWITCH_PORTS, newOdataForCollectionBuilder()
            .name("Ports Collection")
            .oDataType("#PortCollection.PortCollection")
            .build());

        collection.put(ENDPOINTS, newOdataForCollectionBuilder()
            .name("Endpoints Collection")
            .oDataType("#EndpointCollection.EndpointCollection")
            .build());

        collection.put(METRICS_DEFINITION, newOdataForCollectionBuilder()
            .name("Metrics Definition Collection")
            .oDataType("#MetricDefinitionCollection.MetricDefinitionCollection")
            .build());

        collection.put(METRICS_REPORT_DEFINITION, newOdataForCollectionBuilder()
            .name("Metrics Report Definition Collection")
            .oDataType("#MetricReportDefinitionCollection.MetricReportDefinitionCollection")
            .build());

        COLLECTION_TYPE_TO_ODATA_MAPPING = unmodifiableMap(collection);
    }

    private CollectionTypeToCollectionODataMapping() {
    }

    static ODataForCollection getOdataForCollectionType(Type collectionDtoType) {
        if (!COLLECTION_TYPE_TO_ODATA_MAPPING.containsKey(collectionDtoType)) {
            String msg = format("Unknown Collection type: '%s'", collectionDtoType);
            throw new IllegalArgumentException(msg);
        }
        return COLLECTION_TYPE_TO_ODATA_MAPPING.get(collectionDtoType);
    }
}
