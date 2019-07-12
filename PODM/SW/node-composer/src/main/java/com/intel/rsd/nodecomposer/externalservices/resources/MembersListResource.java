/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;

import java.util.List;

@OdataTypes({
    "#ProcessorCollection\\.ProcessorCollection",
    "#EthernetInterfaceCollection\\.EthernetInterfaceCollection",
    "#ChassisCollection\\.ChassisCollection",
    "#ComputerSystemCollection\\.ComputerSystemCollection",
    "#MemoryCollection\\.MemoryCollection",
    "#EthernetSwitchCollection\\.EthernetSwitchCollection",
    "#VLanNetworkInterfaceCollection\\.VLanNetworkInterfaceCollection",
    "#EthernetSwitchPortCollection\\.EthernetSwitchPortCollection",
    "#StorageServiceCollection\\.StorageServiceCollection",
    "#VolumeCollection\\.VolumeCollection",
    "#DriveCollection\\.DriveCollection",
    "#SimpleStorageCollection\\.SimpleStorageCollection",
    "#StorageCollection\\.StorageCollection",
    "#PCIeDeviceCollection\\.PCIeDeviceCollection",
    "#PCIeFunctionCollection\\.PCIeFunctionCollection",
    "#FabricCollection\\.FabricCollection",
    "#ZoneCollection\\.ZoneCollection",
    "#SwitchCollection\\.SwitchCollection",
    "#PortCollection\\.PortCollection",
    "#EndpointCollection\\.EndpointCollection",
    "#NetworkAdapterCollection\\.NetworkAdapterCollection",
    "#NetworkInterfaceCollection\\.NetworkInterfaceCollection",
    "#NetworkDeviceFunctionCollection\\.NetworkDeviceFunctionCollection",
    "#StoragePoolCollection\\.StoragePoolCollection",
    "#StorageDriveCollection\\.StorageDriveCollection",
    "#EventDestinationCollection\\.EventDestinationCollection",
    "#ManagerCollection\\.ManagerCollection"
})

public class MembersListResource extends ExternalServiceResourceImpl {
    @JsonProperty("Members")
    private List<ODataId> members;

    public List<ODataId> getMembers() {
        return members;
    }
}
