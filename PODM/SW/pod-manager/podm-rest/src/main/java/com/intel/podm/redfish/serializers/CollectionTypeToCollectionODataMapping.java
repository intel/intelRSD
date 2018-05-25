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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.CollectionDto.Type;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.CHASSIS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.COMPOSED_NODE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ENDPOINT;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_INTERFACE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_ACLS;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_ACL_RULE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_STATIC_MAC;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCH;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCH_PORT;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGER;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.METRIC_DEFINITION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.METRIC_REPORT_DEFINITION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_DEVICE_FUNCTION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_INTERFACE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICE_FUNCTION;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.POWER_ZONE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PROCESSOR;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SERVICE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SIMPLE_STORAGE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE_POOL;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SYSTEM;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.THERMAL_ZONE;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.VOLUME;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ZONE;
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

        collection.put(MANAGER, newOdataForCollectionBuilder()
            .name("Manager Collection")
            .oDataType("#ManagerCollection.ManagerCollection")
            .build());

        collection.put(SYSTEM, newOdataForCollectionBuilder()
            .name("Computer System Collection")
            .oDataType("#ComputerSystemCollection.ComputerSystemCollection")
            .build());

        collection.put(SERVICE, newOdataForCollectionBuilder()
            .name("Storage Service Collection")
            .oDataType("#StorageServiceCollection.StorageServiceCollection")
            .build());

        collection.put(COMPOSED_NODE, newOdataForCollectionBuilder()
            .name("Composed Node Collection")
            .oDataType("#ComposedNodeCollection.ComposedNodeCollection")
            .build());

        collection.put(EVENT_SUBSCRIPTION, newOdataForCollectionBuilder()
            .name("Event Subscription Collection")
            .oDataType("#EventDestinationCollection.EventDestinationCollection")
            .build());

        collection.put(PROCESSOR, newOdataForCollectionBuilder()
            .name("Processor Collection")
            .oDataType("#ProcessorCollection.ProcessorCollection")
            .build());

        collection.put(ETHERNET_INTERFACE, newOdataForCollectionBuilder()
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

        collection.put(MEMORY_MODULE, newOdataForCollectionBuilder()
            .name("Memory Collection")
            .oDataType("#MemoryCollection.MemoryCollection")
            .build());

        collection.put(ETHERNET_SWITCH, newOdataForCollectionBuilder()
            .name("Ethernet Switch Collection")
            .oDataType("#EthernetSwitchCollection.EthernetSwitchCollection")
            .build());

        collection.put(ETHERNET_SWITCH_ACLS, newOdataForCollectionBuilder()
            .name("Ethernet Switch Access Control List Collection")
            .oDataType("#EthernetSwitchACLCollection.EthernetSwitchACLCollection")
            .build());

        collection.put(ETHERNET_SWITCH_ACL_RULE, newOdataForCollectionBuilder()
            .name("Ethernet Switch Access Control List Rule Collection")
            .oDataType("#EthernetSwitchACLRuleCollection.EthernetSwitchACLRuleCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORT, newOdataForCollectionBuilder()
            .name("Switch Port Collection")
            .oDataType("#EthernetSwitchPortCollection.EthernetSwitchPortCollection")
            .build());

        collection.put(ETHERNET_SWITCH_PORT_VLAN, newOdataForCollectionBuilder()
            .name("VLAN Network Interface Collection")
            .oDataType("#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection")
            .build());

        collection.put(ETHERNET_SWITCH_STATIC_MAC, newOdataForCollectionBuilder()
            .name("Static MAC Collection")
            .oDataType("#EthernetSwitchStaticMACCollection.EthernetSwitchStaticMACCollection")
            .build());

        collection.put(DRIVE, newOdataForCollectionBuilder()
            .name("Drive Collection")
            .oDataType("#DriveCollection.DriveCollection")
            .build());

        collection.put(THERMAL_ZONE, newOdataForCollectionBuilder()
            .name("Thermal Zone Collection")
            .oDataType("#ThermalZoneCollection.ThermalZoneCollection")
            .build());

        collection.put(POWER_ZONE, newOdataForCollectionBuilder()
            .name("Power Zone Collection")
            .oDataType("#PowerZoneCollection.PowerZoneCollection")
            .build());

        collection.put(DRIVE, newOdataForCollectionBuilder()
            .name("Drive Collection")
            .oDataType("#DriveCollection.DriveCollection")
            .build());

        collection.put(STORAGE_POOL, newOdataForCollectionBuilder()
            .name("Storage Pool Collection")
            .oDataType("#StoragePoolCollection.StoragePoolCollection")
            .build());

        collection.put(VOLUME, newOdataForCollectionBuilder()
            .name("Volume Collection")
            .oDataType("#VolumeCollection.VolumeCollection")
            .build());

        collection.put(PCIE_DEVICE, newOdataForCollectionBuilder()
            .name("PCIe Device Collection")
            .oDataType("#PCIeDeviceCollection.PCIeDeviceCollection")
            .build());

        collection.put(PCIE_DEVICE_FUNCTION, newOdataForCollectionBuilder()
            .name("PCIe Function Collection")
            .oDataType("#PCIeFunctionCollection.PCIeFunctionCollection")
            .build());

        collection.put(NETWORK_INTERFACE, newOdataForCollectionBuilder()
            .name("Network Interface Collection")
            .oDataType("#NetworkInterfaceCollection.NetworkInterfaceCollection")
            .build());

        collection.put(NETWORK_DEVICE_FUNCTION, newOdataForCollectionBuilder()
            .name("Network Device Function Collection")
            .oDataType("#NetworkDeviceFunctionCollection.NetworkDeviceFunctionCollection")
            .build());

        collection.put(FABRIC, newOdataForCollectionBuilder()
            .name("Fabric Collection")
            .oDataType("#FabricCollection.FabricCollection")
            .build());

        collection.put(ZONE, newOdataForCollectionBuilder()
            .name("Zone Collection")
            .oDataType("#ZoneCollection.ZoneCollection")
            .build());

        collection.put(FABRIC_SWITCH, newOdataForCollectionBuilder()
            .name("Switch Collection")
            .oDataType("#SwitchCollection.SwitchCollection")
            .build());

        collection.put(FABRIC_SWITCH_PORT, newOdataForCollectionBuilder()
            .name("Port Collection")
            .oDataType("#PortCollection.PortCollection")
            .build());

        collection.put(ENDPOINT, newOdataForCollectionBuilder()
            .name("Endpoint Collection")
            .oDataType("#EndpointCollection.EndpointCollection")
            .build());

        collection.put(METRIC_DEFINITION, newOdataForCollectionBuilder()
            .name("Metric Definition Collection")
            .oDataType("#MetricDefinitionCollection.MetricDefinitionCollection")
            .build());

        collection.put(METRIC_REPORT_DEFINITION, newOdataForCollectionBuilder()
            .name("Metric Report Definition Collection")
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
