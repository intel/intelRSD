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
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

@OdataTypes({
    "#Storage" + VERSION_PATTERN + "Storage"
})
public class StorageResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status;
    @JsonProperty("StorageControllers")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<StorageControllerResource>> storageControllers = Ref.unassigned();
    @JsonProperty("Drives")
    private List<ODataId> drives;
    @JsonProperty("Volumes")
    private Object volumes;
    @JsonProperty("Links")
    private Links links;

    public Ref<Status> getStatus() {
        return status;
    }

    @LinkName("storageControllers")
    public Iterable<ResourceSupplier> getStorageControllers() throws WebClientRequestException {
        if (storageControllers.isAssigned()) {
            return toSuppliersFromResources(storageControllers.get());
        } else {
            return null;
        }
    }

    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return toSuppliers(drives);
    }

    public ResourceSupplier getChassis() {
        return toSupplier(links.enclosures.stream().findFirst().orElseThrow(
            () -> new IllegalStateException("There should be exact one enclosure Chassis within Enclosures collection."))
        );
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Enclosures")
        private List<ODataId> enclosures = new ArrayList<>();
    }
}
