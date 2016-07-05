/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.client.api.resources.redfish.AdapterResource;
import com.intel.podm.client.api.resources.redfish.LuiAdapterResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

@OdataTypes("#Adapter.1.0.0.Adapter")
public class AdapterResourceImpl extends ExternalServiceResourceImpl implements AdapterResource, LuiAdapterResource {
    @JsonProperty("Interface")
    private StorageControllerInterface scInterface;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("BusInfo")
    private String busInfo;
    @JsonProperty("Devices")
    private ODataId devices;
    @JsonProperty("Oem")
    private ODataId oem;
    @JsonProperty("Links")
    private Links links;

    @Override
    public StorageControllerInterface getInterface() {
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
    public String getBusInfo() {
        return busInfo;
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

    private static class Links {
        @JsonProperty("ContainedBy")
        private ODataId containedBy;
        @JsonProperty("Oem")
        private Object oem;
    }
}
