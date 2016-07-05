/*
 * Copyright (c) 2015 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.rest.odataid.ODataId;

import java.util.UUID;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "redfishVersion", "UUID",
        "chassis", "services", "systems", "managers", "eventService", "nodes", "ethernetSwitches", "oem", "links"
})
@JsonIgnoreProperties("modified")
public final class ServiceRootJson extends BaseJson {
    public String id;
    public String name;
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
    public Oem oem = new Oem();
    public Object links = new Object();

    public ServiceRootJson() {
        super("#ServiceRoot.1.0.0.ServiceRoot");
    }

    public static final class Oem {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "apiVersion"})
        public static final class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType = "#Intel.Oem.ServiceRoot";
            public String apiVersion = "1.2.0";
        }
    }
}

