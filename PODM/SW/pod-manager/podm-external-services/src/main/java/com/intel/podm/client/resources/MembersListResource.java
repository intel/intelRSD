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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;

import java.util.List;

@JsonIgnoreProperties(ignoreUnknown = true)
@OdataTypes({
    "#ProcessorCollection.ProcessorCollection",
    "#EthernetInterfaceCollection.EthernetInterfaceCollection",
    // TODO: to be removed - only for RMM integration
    "#EthernetInterfaceCollection.1.0.0.EthernetInterfaceCollection",
    "#ManagerCollection.ManagerCollection",
    "#ChassisCollection.ChassisCollection",
    // TODO: to be removed - only for RMM integration
    "#ChassisCollection.1.0.0.ChassisCollection",
    "#MemoryChunkCollection.MemoryChunkCollection",
    "#ComputerSystemCollection.ComputerSystemCollection",
    "#AdapterCollection.AdapterCollection",
    "#DeviceCollection.DeviceCollection",
    "#MemoryCollection.MemoryCollection",
    "#EthernetSwitchCollection.EthernetSwitchCollection",
    "#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection",
    // TODO: to be removed - only for RMM integration
    "#VLanNetworkInterfaceCollection.1.0.0.VLanNetworkInterfaceCollection",
    "#EthernetSwitchPortCollection.EthernetSwitchPortCollection",
    "#EventDestinationCollection.EventDestinationCollection",
    "#StorageServiceCollection.StorageServiceCollection",
    "#LogicalDriveCollection.LogicalDriveCollection",
    "#PhysicalDriveCollection.PhysicalDriveCollection",
    "#PowerZoneCollection.1.0.0.PowerZoneCollection",
    "#PSUCollection.1.0.0.PSUCollection",
    "#ThermalZoneCollection.1.0.0.ThermalZoneCollection",
    "#DrawerCollection.1.0.0.DrawerCollection",
    "#MBPCollection.1.0.0.MBPCollection",
    "#FanCollection.1.0.0.FanCollection",
    "#RemoteTargetCollection.RemoteTargetCollection",
    "#SimpleStorageCollection.SimpleStorageCollection",
    "#ThermalZoneCollection.ThermalZoneCollection",
    "#PowerZoneCollection.PowerZoneCollection",
    "#ManagerCollection.1.0.0.ManagerCollection",
    // TODO: to be removed - only for RMM integration
    "http://rackscale.intel.com/schema#Intel.Oem.PowerZoneCollection",
    // TODO: to be removed - only for RMM integration
    "http://rackscale.intel.com/schema#Intel.Oem.ThermalZoneCollection"
})
public class MembersListResource extends ExternalServiceResourceImpl {

    @JsonProperty("Members")
    private List<ODataId> members;

    public List<ODataId> getMembers() {
        return members;
    }
}
