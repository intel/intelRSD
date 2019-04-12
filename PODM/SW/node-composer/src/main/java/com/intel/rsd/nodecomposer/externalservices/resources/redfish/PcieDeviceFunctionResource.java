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
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;

import java.util.LinkedHashSet;
import java.util.Set;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#PCIeFunction" + VERSION_PATTERN + "PCIeFunction"
})
public class PcieDeviceFunctionResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonProperty("Status")
    private Status status;

    @JsonProperty("Links")
    private Links links = new Links();

    @LinkName("functionOfDrives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        if (links.drives == null) {
            return null;
        }
        return toSuppliers(links.drives);
    }

    @LinkName("pcieDevice")
    public ResourceSupplier getPcieDevice() {
        if (links.pcieDevice == null) {
            return null;
        }
        return toSupplier(links.pcieDevice);
    }

    public class Links {
        @JsonProperty("Drives")
        private Set<ODataId> drives = new LinkedHashSet<>();
        @JsonProperty("PCIeDevice")
        private ODataId pcieDevice;
    }
}
