/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.UUID;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "redfishVersion", "UUID",
    "chassis", "services", "systems", "managers", "eventService", "nodes", "ethernetSwitches",
    "fabrics", "oem", "links"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public final class ServiceRootJson extends BaseResourceJson {
    public String redfishVersion;
    @JsonProperty("UUID")
    public UUID uuid;
    public ODataId chassis;
    public ODataId services;
    public ODataId systems;
    public ODataId managers;
    public ODataId nodes;
    public ODataId ethernetSwitches;
    public ODataId eventService;
    public ODataId fabrics;
    public Oem oem = new Oem();
    public Links links = new Links();

    public ServiceRootJson() {
        super("#ServiceRoot.v1_1_1.ServiceRoot");
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "apiVersion"})
        public final class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType = "#Intel.Oem.ServiceRoot";
            public String apiVersion = "2.0.0";
        }
    }

    public class Links extends RedfishLinksJson {
    }
}

