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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.legacy.AdapterResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

@OdataTypes({
    "#Adapter" + OdataTypes.VERSION_PATTERN + "Adapter"
})
public class AdapterResourceImpl extends ExternalServiceResourceImpl implements AdapterResource {
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

    @Override
    public Protocol getInterface() {
        return scInterface;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("devices")
    public Iterable<ResourceSupplier> getDevices() throws ExternalServiceApiReaderException {
        return processMembersListResource(devices);
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        return toSupplier(links.containedBy);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("ContainedBy")
        private ODataId containedBy;
    }
}
