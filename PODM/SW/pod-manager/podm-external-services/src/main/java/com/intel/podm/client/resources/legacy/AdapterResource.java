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

package com.intel.podm.client.resources.legacy;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Adapter" + VERSION_PATTERN + "Adapter"
})
public class AdapterResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Interface")
    private Protocol scInterface;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Devices")
    private ODataId devices;
    @JsonProperty("Links")
    private Links links = new Links();

    public Protocol getInterface() {
        return scInterface;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("devices")
    public Iterable<ResourceSupplier> getDevices() throws WebClientRequestException {
        return processMembersListResource(devices);
    }

    public ResourceSupplier getComputerSystem() {
        return toSupplier(links.containedBy);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("ContainedBy")
        private ODataId containedBy;
    }
}
