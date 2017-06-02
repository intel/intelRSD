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

@OdataTypes({
    "#ProcessorCollection\\.ProcessorCollection",
    "#EthernetInterfaceCollection\\.EthernetInterfaceCollection",
    // TODO: to be removed - only for RMM integration
    "#EthernetInterfaceCollection" + OdataTypes.VERSION_PATTERN + "EthernetInterfaceCollection",
    "#ManagerCollection\\.ManagerCollection",
    "#ChassisCollection\\.ChassisCollection",
    // TODO: to be removed - only for RMM integration
    "#ChassisCollection" + OdataTypes.VERSION_PATTERN + "ChassisCollection",
    "#ComputerSystemCollection\\.ComputerSystemCollection",
    "#MemoryCollection\\.MemoryCollection",
    "#EthernetSwitchCollection\\.EthernetSwitchCollection",
    "#VLanNetworkInterfaceCollection\\.VLanNetworkInterfaceCollection",
    // TODO: to be removed - only for RMM integration
    "#VLanNetworkInterfaceCollection" + OdataTypes.VERSION_PATTERN + "VLanNetworkInterfaceCollection",
    "#EthernetSwitchPortCollection\\.EthernetSwitchPortCollection",
    "#EventDestinationCollection\\.EventDestinationCollection",
    "#StorageServiceCollection\\.StorageServiceCollection",
    "#LogicalDriveCollection\\.LogicalDriveCollection",
    "#PhysicalDriveCollection\\.PhysicalDriveCollection",
    "#PowerZoneCollection" + OdataTypes.VERSION_PATTERN + "PowerZoneCollection",
    "#PSUCollection" + OdataTypes.VERSION_PATTERN + "PSUCollection",
    "#ThermalZoneCollection" + OdataTypes.VERSION_PATTERN + "ThermalZoneCollection",
    "#DrawerCollection" + OdataTypes.VERSION_PATTERN + "DrawerCollection",
    "#MBPCollection" + OdataTypes.VERSION_PATTERN + "MBPCollection",
    "#FanCollection" + OdataTypes.VERSION_PATTERN + "FanCollection",
    "#RemoteTargetCollection\\.RemoteTargetCollection",
    "#SimpleStorageCollection\\.SimpleStorageCollection",
    "#StorageCollection\\.StorageCollection",
    "#ThermalZoneCollection\\.ThermalZoneCollection",
    "#PowerZoneCollection\\.PowerZoneCollection",
    "#ManagerCollection" + OdataTypes.VERSION_PATTERN + "ManagerCollection",
    "#Intel\\.Oem\\.RmmPowerZoneCollection",
    "#Intel\\.Oem\\.RmmThermalZoneCollection",
    // TODO: to be removed - only for RMM integration
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.PowerZoneCollection",
    // TODO: to be removed - only for RMM integration
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.ThermalZoneCollection",
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

    //legacy collections
    "#AdapterCollection\\.AdapterCollection",
    "#DeviceCollection\\.DeviceCollection"
})
public class MembersListResource extends ExternalServiceResourceImpl {
    @JsonProperty("Members")
    private List<ODataId> members;

    public List<ODataId> getMembers() {
        return members;
    }
}
