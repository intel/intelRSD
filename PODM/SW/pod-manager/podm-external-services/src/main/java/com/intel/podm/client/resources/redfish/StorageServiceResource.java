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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;

@OdataTypes({
    "#StorageService" + VERSION_PATTERN + "StorageService"
})
@JsonIgnoreProperties(ignoreUnknown = true)
public final class StorageServiceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Volumes")
    private ODataId volumes;
    @JsonProperty("Drives")
    private ODataId drives;
    @JsonProperty("Endpoints")
    private ODataId endpoints;
    @JsonProperty("StoragePools")
    private ODataId storagePools;

    @JsonProperty("Links")
    private Links links = new Links();

    public Status getStatus() {
        return status;
    }

    @LinkName("physicalDrives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return processMembersListResource(drives);
    }

    @LinkName("endpoints")
    public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
        return processMembersListResource(endpoints);
    }

    @LinkName("storagePools")
    public Iterable<ResourceSupplier> getStoragePools() throws WebClientRequestException {
        return processMembersListResource(storagePools);
    }

    @LinkName("volumes")
    public Iterable<ResourceSupplier> getVolumes() throws WebClientRequestException {
        return processMembersListResource(volumes);
    }

    @LinkName("managersInStorage")
    public Iterable<ResourceSupplier> getManagedBy() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.managedBy);
    }

    @LinkName("hostingSystemInStorage")
    public ResourceSupplier getHostingSystem() {
        if (links.hostingSystem == null) {
            return null;
        }
        return toSupplier(links.hostingSystem);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("HostingSystem")
        private ODataId hostingSystem;

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            @JsonPropertyOrder({"managedBy"})
            public class RackScaleOem {
                private Set<ODataId> managedBy = new HashSet<>();
            }
        }
    }
}
