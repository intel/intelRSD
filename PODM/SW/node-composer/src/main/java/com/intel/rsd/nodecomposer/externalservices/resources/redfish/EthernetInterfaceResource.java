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
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import lombok.Getter;

import java.util.List;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#EthernetInterface" + VERSION_PATTERN + "EthernetInterface"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class EthernetInterfaceResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @Getter
    @JsonSetter(value = "MACAddress", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<MacAddress> macAddress = unassigned();

    @Getter
    @JsonSetter(value = "SpeedMbps", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> speedMbps = unassigned();

    @JsonProperty("VLANs")
    private ODataId vlanCollection;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Ref<List<Protocol>> getSupportedProtocols() {
        return oem.rackScaleOem.supportedProtocols;
    }

    @LinkName("ethernetInterfaceVlans")
    public Iterable<ResourceSupplier> getVlans() throws WebClientRequestException {
        return processMembersListResource(this.vlanCollection);
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonSetter(value = "SupportedProtocols", nulls = AS_EMPTY)
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<Protocol>> supportedProtocols = unassigned();
        }
    }
}
