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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.StorageServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.List;

@JsonIgnoreProperties(ignoreUnknown = true)
@OdataTypes("#StorageService.1.0.0.StorageService")
public final class StorageServiceResourceImpl extends ExternalServiceResourceImpl implements StorageServiceResource {
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

    @Override
    public Status getStatus() {
        return status;
    }

    @LinkName("physicalDrives")
    @Override
    public Iterable<ResourceSupplier> getDrives() throws ExternalServiceApiReaderException {
        return processMembersListResource(drives);
    }

    @LinkName("logicalDrives")
    @Override
    public Iterable<ResourceSupplier> getLogicalDrives() throws ExternalServiceApiReaderException {
        return processMembersListResource(logicalDrives);
    }

    @LinkName("targets")
    @Override
    public Iterable<ResourceSupplier> getRemoteTargets() throws ExternalServiceApiReaderException {
        return processMembersListResource(targets);
    }

    @LinkName("managedBy")
    @Override
    public Iterable<ResourceSupplier> getManagers() throws ExternalServiceApiReaderException {
        return toSuppliers(links.managedBy);
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Links {
        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;
        @JsonProperty("Oem")
        private Object oem;
    }
}
