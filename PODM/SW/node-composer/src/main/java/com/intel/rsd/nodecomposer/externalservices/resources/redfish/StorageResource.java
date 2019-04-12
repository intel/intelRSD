/*
 * Copyright (c) 2016-2019 Intel Corporation
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
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;
import lombok.Getter;

import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Storage" + VERSION_PATTERN + "Storage"
})
public class StorageResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status;

    @JsonProperty("Drives")
    private List<ODataId> drives;

    @JsonProperty("Links")
    private Links links = new Links();

    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return toSuppliers(drives);
    }

    public ResourceSupplier getChassis() {
        return toSupplier(links.enclosures.stream().findFirst().orElseThrow(
            () -> new IllegalStateException("There should be exact one enclosure Chassis within Enclosures collection."))
        );
    }

    public class Links {
        @JsonProperty("Enclosures")
        private List<ODataId> enclosures = new ArrayList<>();
    }
}
