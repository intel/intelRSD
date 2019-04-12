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
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.PciePortType;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;

import java.util.Set;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Port" + VERSION_PATTERN + "Port"
})
@SuppressWarnings("checkstyle:AbbreviationAsWordInName")
@Getter
public class PortResource extends ExternalServiceResourceImpl {
    @JsonProperty("Status")
    private Status status;

    @JsonProperty("PortType")
    private PciePortType portType;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Set<String> getPCIeConnectionId() {
        return oem.rackScaleOem.pcieConnectionId;
    }

    @Getter
    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("PCIeConnectionId")
            private Set<String> pcieConnectionId;
        }
    }
}
