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
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.ConnectedEntityJson;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.PciIdJson;
import com.intel.podm.redfish.json.templates.attributes.RedundancyJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status",
    "endpointProtocol", "pciId", "identifiers", "connectedEntities", "redundancy", "hostReservationMemoryBytes", "links", "oem", "actions"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class EndpointJson extends BaseResourceJson {
    public Status status;
    public Links links = new Links();
    public Actions actions = new Actions();
    public Protocol endpointProtocol;
    public Set<ConnectedEntityJson> connectedEntities;
    public Set<IdentifierJson> identifiers;
    public Set<RedundancyJson> redundancy = new HashSet<>();
    public PciIdJson pciId;
    public Integer hostReservationMemoryBytes;

    public EndpointJson() {
        super("#Endpoint.v1_0_0.Endpoint");
    }

    @JsonPropertyOrder({"ports", "oem"})
    public class Links extends RedfishLinksJson {
        @JsonProperty("Ports")
        public Set<ODataId> ports = new HashSet<>();
    }

    public class Actions extends RedfishActionsJson {
    }
}
