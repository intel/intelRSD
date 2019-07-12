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

import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Chassis" + VERSION_PATTERN + "Chassis"
})
public class ChassisResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonProperty("Status")
    private Status status;

    @JsonProperty("NetworkAdapters")
    private ODataId networkAdapters;

    @JsonProperty("Links")
    private Links links = new Links();

    @LinkName("containsChassis")
    public Iterable<ResourceSupplier> getContained() throws WebClientRequestException {
        return toSuppliers(links.contains);
    }

    @LinkName("computerSystems")
    public Iterable<ResourceSupplier> getComputerSystems() throws WebClientRequestException {
        return toSuppliers(links.computerSystems);
    }

    @LinkName("containsSwitches")
    public Iterable<ResourceSupplier> getSwitches() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.switches);
    }

    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return toSuppliers(links.drives);
    }

    @LinkName("storage")
    public Iterable<ResourceSupplier> getStorage() throws WebClientRequestException {
        return toSuppliers(links.storage);
    }

    @LinkName("networkAdapters")
    public Iterable<ResourceSupplier> getNetworkAdapters() throws WebClientRequestException {
        return processMembersListResource(networkAdapters);
    }

    public class Links {
        @JsonProperty("Contains")
        private List<ODataId> contains;

        @JsonProperty("ComputerSystems")
        private List<ODataId> computerSystems;

        @JsonProperty("Drives")
        private List<ODataId> drives;

        @JsonProperty("Storage")
        private List<ODataId> storage;

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public class Oem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public class RackScaleOem {
                @JsonProperty("Switches")
                private List<ODataId> switches = new ArrayList<>();
            }
        }
    }
}
