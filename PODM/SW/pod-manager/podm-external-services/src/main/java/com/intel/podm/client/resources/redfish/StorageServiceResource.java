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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#StorageService" + VERSION_PATTERN + "StorageService"
})
public final class StorageServiceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Drives")
    private ODataId drives;
    @JsonProperty("LogicalDrives")
    private ODataId logicalDrives;
    @JsonProperty("RemoteTargets")
    private ODataId targets;
    @JsonProperty("Links")
    private Links links = new Links();

    public Status getStatus() {
        return status;
    }

    @LinkName("physicalDrives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return processMembersListResource(drives);
    }

    @LinkName("logicalDrives")
    public Iterable<ResourceSupplier> getLogicalDrives() throws WebClientRequestException {
        return processMembersListResource(logicalDrives);
    }

    @LinkName("targets")
    public Iterable<ResourceSupplier> getRemoteTargets() throws WebClientRequestException {
        return processMembersListResource(targets);
    }

    @LinkName("managedBy")
    public Iterable<ResourceSupplier> getManagers() throws WebClientRequestException {
        return toSuppliers(links.managedBy);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;
    }
}
