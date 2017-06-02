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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.redfish.json.templates.attributes.ResetActionJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "portId", "portProtocol",
    "portType", "currentSpeedGbps", "width", "maxSpeedGbps", "actions", "links", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class PortJson extends BaseResourceJson {
    public Status status;
    public String portId;
    public PortType portType;
    public Protocol portProtocol;
    public Long currentSpeedGbps;
    public Long width;
    public Long maxSpeedGbps;
    public Oem oem = new Oem();
    public Actions actions = new Actions();
    public Links links = new Links();

    public PortJson() {
        super("#Port.v1_0_0.Port");
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScale = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType = "#Intel.Oem.Port";
            @JsonProperty("PCIeConnectionId")
            public List<String> cableIds = new ArrayList<>();
        }
    }

    @JsonPropertyOrder({"associatedEndpoints", "ConnectedSwitches", "connectedSwitchPorts", "oem"})
    public class Links extends RedfishLinksJson {
        public Set<ODataId> associatedEndpoints = new HashSet<>();
        public Set<ODataId> connectedSwitches = new HashSet<>();
        public Set<ODataId> connectedSwitchPorts = new HashSet<>();
    }

    public class Actions extends RedfishActionsJson {
        @JsonProperty("#Port.Reset")
        public ResetActionJson reset = new ResetActionJson();
    }
}
