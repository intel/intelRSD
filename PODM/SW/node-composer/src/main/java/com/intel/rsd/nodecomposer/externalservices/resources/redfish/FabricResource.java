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
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;

import java.util.List;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Fabric" + VERSION_PATTERN + "Fabric"
})
public class FabricResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonProperty("Status")
    private Status status;

    @Getter
    @JsonProperty("FabricType")
    private Protocol fabricType;

    @JsonProperty("Zones")
    private ODataId zones;

    @JsonProperty("Switches")
    private ODataId switches;

    @JsonProperty("Endpoints")
    private ODataId endpoints;

    @JsonProperty("Links")
    private Links links = new Links();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @LinkName("zoneInFabric")
    public Iterable<ResourceSupplier> getZones() throws WebClientRequestException {
        return processMembersListResource(zones);
    }

    @LinkName("switchInFabric")
    public Iterable<ResourceSupplier> getSwitches() throws WebClientRequestException {
        return processMembersListResource(switches);
    }

    @LinkName("endpointInFabric")
    public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
        return processMembersListResource(endpoints);
    }

    @LinkName("managedByFabric")
    public Iterable<ResourceSupplier> getManagedBy() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.managedBy);
    }

    public Protocol getFabricTypeFromOem() {
        return oem.rackScaleOem.fabricType;
    }

    public class Links {
        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public class Oem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public class RackScaleOem {
                @JsonProperty("ManagedBy")
                private List<ODataId> managedBy;
            }
        }
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("FabricType")
            private Protocol fabricType;
        }
    }
}
