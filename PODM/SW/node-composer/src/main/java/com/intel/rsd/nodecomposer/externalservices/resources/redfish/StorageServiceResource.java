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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.common.base.Joiner;
import com.google.common.base.Splitter;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.val;

import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#StorageService" + VERSION_PATTERN + "StorageService"
})
public final class StorageServiceResource extends ExternalServiceResourceImpl {
    @Getter
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

    @LinkName("volumes")
    public Iterable<ResourceSupplier> getVolumes() throws WebClientRequestException {
        return processMembersListResource(volumes);
    }

    @LinkName("physicalDrives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return processMembersListResource(drives);
    }

    @LinkName("fabric")
    public ResourceSupplier getFabric() {
        if (endpoints == null) {
            return null;
        }

        List<String> pathSegments = new ArrayList<>(Splitter.on("/").splitToList(endpoints.toString()));
        pathSegments.remove(pathSegments.size() - 1);
        val fabricOdataId = ODataId.oDataIdFromString(Joiner.on("/").join(pathSegments));
        return toSupplier(fabricOdataId);
    }

    @LinkName("endpoints")
    public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
        return processMembersListResource(endpoints);
    }

    @LinkName("storagePools")
    public Iterable<ResourceSupplier> getStoragePools() throws WebClientRequestException {
        return processMembersListResource(storagePools);
    }
}
