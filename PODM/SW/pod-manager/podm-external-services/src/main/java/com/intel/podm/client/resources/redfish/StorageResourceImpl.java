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
import com.intel.podm.client.api.resources.redfish.StorageResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

@OdataTypes({
    "#Storage" + OdataTypes.VERSION_PATTERN + "Storage"
})
public class StorageResourceImpl extends ExternalServiceResourceImpl implements StorageResource {
    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status;
    @JsonProperty("StorageControllers")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<StorageControllerResourceImpl>> storageControllers = Ref.unassigned();
    @JsonProperty("Drives")
    private List<ODataId> drives;
    @JsonProperty("Volumes")
    private Object volumes;
    @JsonProperty("Links")
    private Links links;

    @Override
    public Ref<Status> getStatus() {
        return status;
    }

    @Override
    public Ref<List<? extends StorageControllerResource>> getStorageControllers() {
        return (Ref) storageControllers;
    }

    @Override
    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws ExternalServiceApiReaderException {
        return toSuppliers(drives);
    }

    @Override
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
