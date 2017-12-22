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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;

import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#ProcessorCollection\\.ProcessorCollection",
    "#EthernetInterfaceCollection\\.EthernetInterfaceCollection",
    "#ManagerCollection\\.ManagerCollection",
    "#ChassisCollection\\.ChassisCollection",
    "#ComputerSystemCollection\\.ComputerSystemCollection",
    "#MemoryCollection\\.MemoryCollection",
    "#EthernetSwitchCollection\\.EthernetSwitchCollection",
    "#VLanNetworkInterfaceCollection\\.VLanNetworkInterfaceCollection",
    "#EthernetSwitchPortCollection\\.EthernetSwitchPortCollection",
    "#EventDestinationCollection\\.EventDestinationCollection",
    "#StorageServiceCollection\\.StorageServiceCollection",
    "#LogicalDriveCollection\\.LogicalDriveCollection",
    "#PhysicalDriveCollection\\.PhysicalDriveCollection",
    "#PSUCollection" + VERSION_PATTERN + "PSUCollection",
    "#DrawerCollection" + VERSION_PATTERN + "DrawerCollection",
    "#MBPCollection" + VERSION_PATTERN + "MBPCollection",
    "#FanCollection" + VERSION_PATTERN + "FanCollection",
    "#RemoteTargetCollection\\.RemoteTargetCollection",
    "#SimpleStorageCollection\\.SimpleStorageCollection",
    "#StorageCollection\\.StorageCollection",
    "#ManagerCollection" + VERSION_PATTERN + "ManagerCollection",
    "#Intel\\.Oem\\.RmmPowerZoneCollection",
    "#Intel\\.Oem\\.RmmThermalZoneCollection",
    "#StorageDriveCollection\\.StorageDriveCollection",
    "#PCIeDeviceCollection\\.PCIeDeviceCollection",
    "#PCIeFunctionCollection\\.PCIeFunctionCollection",
    "#FabricCollection\\.FabricCollection",
    "#ZoneCollection\\.ZoneCollection",
    "#SwitchCollection\\.SwitchCollection",
    "#PortCollection\\.PortCollection",
    "#EndpointCollection\\.EndpointCollection",
    "#NetworkInterfaceCollection\\.NetworkInterfaceCollection",
    "#NetworkDeviceFunctionCollection\\.NetworkDeviceFunctionCollection",
    "#EthernetSwitchACLCollection\\.EthernetSwitchACLCollection",
    "#EthernetSwitchACLRuleCollection\\.EthernetSwitchACLRuleCollection",
    "#MetricDefinitionCollection\\.MetricDefinitionCollection",
    "#MetricReportDefinitionCollection\\.MetricReportDefinitionCollection",
    "#EthernetSwitchStaticMACCollection\\.EthernetSwitchStaticMACCollection",

    // legacy collections
    "#AdapterCollection\\.AdapterCollection",
    "#DeviceCollection\\.DeviceCollection",

    // for RMM, should stay for backward compatibility
    "#EthernetInterfaceCollection" + VERSION_PATTERN + "EthernetInterfaceCollection",
    "#ChassisCollection" + VERSION_PATTERN + "ChassisCollection",
    "#VLanNetworkInterfaceCollection" + VERSION_PATTERN + "VLanNetworkInterfaceCollection",
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.PowerZoneCollection",
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.ThermalZoneCollection",
})
public class MembersListResource extends ExternalServiceResourceImpl {
    @JsonProperty("Members")
    private List<ODataId> members;

    public List<ODataId> getMembers() {
        return members;
    }
}
