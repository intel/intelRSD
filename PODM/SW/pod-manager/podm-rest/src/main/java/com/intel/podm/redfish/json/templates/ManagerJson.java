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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.GeneralConnectType;
import com.intel.podm.common.types.GraphicalConnectType;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_EMPTY;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "managerType", "description",
    "serviceEntryPointUuid", "uuid", "model", "status", "powerState", "graphicalConsole", "serialConsole",
    "commandShell", "firmwareVersion", "networkProtocol", "ethernetInterfaces", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ManagerJson extends BaseResourceJson {
    public ManagerType managerType;
    @JsonProperty("ServiceEntryPointUUID")
    @JsonInclude(NON_NULL)
    public UUID serviceEntryPointUuid;
    @JsonProperty("UUID")
    public UUID uuid;
    public String model;
    public Status status;
    public PowerState powerState;
    @JsonInclude(NON_DEFAULT)
    public GraphicalConsole graphicalConsole;
    @JsonInclude(NON_DEFAULT)
    public Console serialConsole;
    @JsonInclude(NON_DEFAULT)
    public Console commandShell;
    public String firmwareVersion;
    @JsonInclude(NON_NULL)
    public ODataId networkProtocol;
    public ODataId ethernetInterfaces;
    public Links links = new Links();

    public ManagerJson() {
        super("#Manager.v1_2_0.Manager");
    }

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class Console {
        @JsonInclude(NON_NULL)
        public Boolean serviceEnabled;

        @JsonInclude(NON_NULL)
        public Integer maxConcurrentSessions;

        @JsonInclude(NON_EMPTY)
        public final List<GeneralConnectType> connectTypesSupported = new ArrayList<>();

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (o == null || getClass() != o.getClass()) {
                return false;
            }

            Console console = (Console) o;

            return new EqualsBuilder()
                .append(serviceEnabled, console.serviceEnabled)
                .append(maxConcurrentSessions, console.maxConcurrentSessions)
                .append(connectTypesSupported, console.connectTypesSupported)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(serviceEnabled)
                .append(maxConcurrentSessions)
                .append(connectTypesSupported)
                .toHashCode();
        }
    }

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class GraphicalConsole {
        @JsonInclude(NON_NULL)
        public Boolean serviceEnabled;

        @JsonInclude(NON_NULL)
        public Integer maxConcurrentSessions;

        @JsonInclude(NON_EMPTY)
        public final List<GraphicalConnectType> connectTypesSupported = new ArrayList<>();

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (o == null || getClass() != o.getClass()) {
                return false;
            }

            GraphicalConsole that = (GraphicalConsole) o;

            return new EqualsBuilder()
                .append(serviceEnabled, that.serviceEnabled)
                .append(maxConcurrentSessions, that.maxConcurrentSessions)
                .append(connectTypesSupported, that.connectTypesSupported)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(serviceEnabled)
                .append(maxConcurrentSessions)
                .append(connectTypesSupported)
                .toHashCode();
        }
    }

    @JsonPropertyOrder({"@odata.type", "managedComputerSystems", "managedChassisCollection", "managerInChassis", "oem"})
    public final class Links extends RedfishLinksJson {
        @JsonProperty("@odata.type")
        public String oDataType;

        @JsonProperty("ManagerForServers")
        public Set<ODataId> managedComputerSystems = new HashSet<>();

        @JsonProperty("ManagerForChassis")
        public Set<ODataId> managedChassisCollection = new HashSet<>();

        @JsonInclude(NON_NULL)
        public ODataId managerInChassis;

        @JsonProperty("Oem")
        public Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOemJson {
            @JsonProperty("Intel_RackScale")
            public RackScaleOem rackScaleOem = new RackScaleOem();

            @JsonPropertyOrder({"@odata.type", "managerForServices", "managedEthernetSwitches", "managerForPcieSwitches", "managerForPcieDevices"})
            public final class RackScaleOem {
                @JsonProperty("@odata.type")
                public String oDataType;

                @JsonProperty("ManagerForServices")
                public Set<ODataId> managerForServices = new HashSet<>();

                @JsonProperty("ManagerForSwitches")
                public Set<ODataId> managedEthernetSwitches = new HashSet<>();
            }
        }
    }
}
