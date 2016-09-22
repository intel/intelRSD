/*
 * Copyright (c) 2016 Intel Corporation
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
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.GeneralConnectType;
import com.intel.podm.common.types.GraphicalConnectType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.Status;
import com.intel.podm.rest.odataid.ODataId;

import java.util.Collection;
import java.util.List;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_EMPTY;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "managerType", "description",
        "serviceEntryPointUuid", "uuid", "model", "status", "graphicalConsole", "serialConsole",
        "commandShell", "firmwareVersion", "networkProtocol", "ethernetInterfaces", "links", "actions", "oem"
})
public class ManagerJson extends BaseJson {
    public Id id;
    public String name;
    public ManagerType managerType;
    public String description;
    @JsonProperty("ServiceEntryPointUUID")
    @JsonInclude(NON_NULL)
    public UUID serviceEntryPointUuid;
    @JsonProperty("UUID")
    public UUID uuid;
    public String model;
    public Status status;
    @JsonInclude(NON_NULL)
    public GraphicalConsole graphicalConsole;
    @JsonInclude(NON_NULL)
    public Console serialConsole;
    @JsonInclude(NON_NULL)
    public Console commandShell;
    public String firmwareVersion;
    public ODataId networkProtocol;
    public ODataId ethernetInterfaces;
    public final Links links = new Links();
    public final Object oem = new Object();

    public ManagerJson() {
        super("#Manager.1.1.0.Manager");
    }

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class Console {
        @JsonInclude(NON_NULL)
        public Boolean serviceEnabled;

        @JsonInclude(NON_NULL)
        public Integer maxConcurrentSessions;

        @JsonInclude(NON_EMPTY)
        public final List<GeneralConnectType> connectTypesSupported = newArrayList();
    }

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class GraphicalConsole {
        @JsonInclude(NON_NULL)
        public Boolean serviceEnabled;

        @JsonInclude(NON_NULL)
        public Integer maxConcurrentSessions;

        @JsonInclude(NON_EMPTY)
        public final List<GraphicalConnectType> connectTypesSupported = newArrayList();
    }

    @JsonPropertyOrder({"@odata.type", "managedComputerSystems", "managedChassisCollection", "managerInChassis", "managedEthernetSwitches", "oem"})
    public static final class Links {
        @JsonProperty("@odata.type")
        public String oDataType = "#Manager.1.1.0.Links";

        @JsonProperty("ManagerForServers")
        public Collection<ODataId> managedComputerSystems = newArrayList();

        @JsonProperty("ManagerForChassis")
        public Collection<ODataId> managedChassisCollection = newArrayList();

        public ODataId managerInChassis;

        @JsonProperty("ManagerForSwitches")
        public Collection<ODataId> managedEthernetSwitches = newArrayList();

        @JsonProperty("Oem")
        public Oem oem = new Oem();

        public static final class Oem {
            @JsonProperty("Intel_RackScale")
            public RackScaleOem rackScaleOem = new RackScaleOem();

            @JsonIgnoreProperties(ignoreUnknown = true)
            public static final class RackScaleOem {
                @JsonProperty("@odata.type")
                public String oDataType;

                @JsonProperty("ManagerForServices")
                public Collection<ODataId> managerForServices = newArrayList();
            }
        }
    }
}
