/*
 * Copyright (c) 2017-2019 Intel Corporation
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
import lombok.Getter;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Volume" + VERSION_PATTERN + "Volume"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class VolumeResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonProperty("Status")
    private Status status;

    @Getter
    @JsonProperty("CapacityBytes")
    private Number capacityBytes;

    @Getter
    @JsonProperty("Identifiers")
    private Set<IdentifierObject> identifiers;

    @JsonProperty("CapacitySources")
    private List<CapacitySourcesResource> capacitySources;

    @JsonProperty("Links")
    private Links links = new Links();

    @JsonProperty("Actions")
    private Actions actions = new Actions();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Boolean getBootable() {
        return oem.rackScaleOem.bootable;
    }

    @LinkName("capacitySourcesInVolume")
    public Iterable<ResourceSupplier> getCapacitySources() {
        return toSuppliersFromEmbeddableResourceElement(capacitySources, "capacitySources");
    }

    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        return toSuppliers(links.drives);
    }

    public Actions getActions() {
        return actions;
    }

    public final class Links {
        @JsonProperty("Drives")
        private Set<ODataId> drives = new HashSet<>();
    }

    public class Actions {
        @JsonProperty("#Volume.Initialize")
        private VolumeResource.Actions.Initialize initialize = new VolumeResource.Actions.Initialize();

        public Initialize getInitialize() {
            return initialize;
        }

        public class Initialize {
            @Getter
            @JsonSetter(value = "target", nulls = AS_EMPTY)
            private Ref<String> target = unassigned();
        }
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Bootable")
            private Boolean bootable;
        }
    }
}
