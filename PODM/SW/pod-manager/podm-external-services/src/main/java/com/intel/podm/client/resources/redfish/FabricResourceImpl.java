/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.FabricResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

@OdataTypes({
    "#Fabric" + OdataTypes.VERSION_PATTERN + "Fabric"
})
public class FabricResourceImpl extends ExternalServiceResourceImpl implements FabricResource {
    @JsonProperty("FabricType")
    private Protocol fabricType;

    @JsonProperty("MaxZones")
    private Integer maxZones;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("Zones")
    private ODataId zones;

    @JsonProperty("Endpoints")
    private ODataId endpoints;

    @JsonProperty("Switches")
    private ODataId switches;

    @Override
    public Protocol getFabricType() {
        return fabricType;
    }

    @Override
    public Integer getMaxZones() {
        return maxZones;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("zoneInFabric")
    public Iterable<ResourceSupplier> getZones() throws ExternalServiceApiReaderException {
        return processMembersListResource(zones);
    }

    @Override
    @LinkName("endpointInFabric")
    public Iterable<ResourceSupplier> getEndpoints() throws ExternalServiceApiReaderException {
        return processMembersListResource(endpoints);
    }

    @Override
    @LinkName("switchInFabric")
    public Iterable<ResourceSupplier> getSwitches() throws ExternalServiceApiReaderException {
        return processMembersListResource(switches);
    }
}
