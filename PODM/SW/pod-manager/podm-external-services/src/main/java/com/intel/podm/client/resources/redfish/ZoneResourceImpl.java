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
import com.intel.podm.client.api.resources.redfish.ZoneResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.List;

@OdataTypes({
    "#Zone" + OdataTypes.VERSION_PATTERN + "Zone"
})
public class ZoneResourceImpl extends ExternalServiceResourceImpl implements ZoneResource {
    @JsonProperty("Status")
    private Status status;

    @JsonProperty("Links")
    private Links links = new Links();

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("endpointInZone")
    public Iterable<ResourceSupplier> getEndpoints() throws ExternalServiceApiReaderException {
        return toSuppliers(links.endpoints);
    }

    @Override
    @LinkName("switchInZone")
    public Iterable<ResourceSupplier> getInvolvedSwitches() throws ExternalServiceApiReaderException {
        return toSuppliers(links.involvedSwitches);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Endpoints")
        private List<ODataId> endpoints;
        @JsonProperty("InvolvedSwitches")
        private List<ODataId> involvedSwitches;
    }
}
