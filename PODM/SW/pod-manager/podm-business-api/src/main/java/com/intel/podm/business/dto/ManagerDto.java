/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.GeneralConnectType;
import com.intel.podm.common.types.GraphicalConnectType;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

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
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "managerType", "description", "serviceEntryPointUuid", "uuid", "model", "dateTime",
    "dateTimeLocalOffset", "powerState", "status", "graphicalConsole", "serialConsole", "commandShell", "firmwareVersion", "networkProtocol",
    "ethernetInterfaces", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ManagerDto extends RedfishDto {
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private ManagerType managerType;
    @JsonProperty("ServiceEntryPointUUID")
    @JsonInclude(NON_NULL)
    private UUID serviceEntryPointUuid;
    @JsonProperty("UUID")
    private UUID uuid;
    private String model;
    private String dateTime;
    private String dateTimeLocalOffset;
    private PowerState powerState;
    private Status status;
    @JsonInclude(NON_DEFAULT)
    private GraphicalConsoleDto graphicalConsole = new GraphicalConsoleDto();
    @JsonInclude(NON_DEFAULT)
    private ConsoleDto serialConsole = new ConsoleDto();
    @JsonInclude(NON_DEFAULT)
    private ConsoleDto commandShell = new ConsoleDto();
    private String firmwareVersion;
    @JsonInclude(NON_NULL)
    private SingletonContext networkProtocol;
    private SingletonContext ethernetInterfaces;

    public ManagerDto() {
        super("#Manager.v1_2_0.Manager");
    }

    public ManagerType getManagerType() {
        return managerType;
    }

    public void setManagerType(ManagerType managerType) {
        this.managerType = managerType;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public UUID getServiceEntryPointUuid() {
        return serviceEntryPointUuid;
    }

    public void setServiceEntryPointUuid(UUID serviceEntryPointUuid) {
        this.serviceEntryPointUuid = serviceEntryPointUuid;
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public String getDateTime() {
        return dateTime;
    }

    public void setDateTime(String dateTime) {
        this.dateTime = dateTime;
    }

    public String getDateTimeLocalOffset() {
        return dateTimeLocalOffset;
    }

    public void setDateTimeLocalOffset(String dateTimeLocalOffset) {
        this.dateTimeLocalOffset = dateTimeLocalOffset;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public GraphicalConsoleDto getGraphicalConsole() {
        return graphicalConsole;
    }

    public void setGraphicalConsole(GraphicalConsoleDto graphicalConsole) {
        this.graphicalConsole = graphicalConsole;
    }

    public ConsoleDto getSerialConsole() {
        return serialConsole;
    }

    public void setSerialConsole(ConsoleDto serialConsole) {
        this.serialConsole = serialConsole;
    }

    public ConsoleDto getCommandShell() {
        return commandShell;
    }

    public void setCommandShell(ConsoleDto commandShell) {
        this.commandShell = commandShell;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public SingletonContext getNetworkProtocol() {
        return networkProtocol;
    }

    public void setNetworkProtocol(SingletonContext networkProtocol) {
        this.networkProtocol = networkProtocol;
    }

    public SingletonContext getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void setEthernetInterfaces(SingletonContext ethernetInterfaces) {
        this.ethernetInterfaces = ethernetInterfaces;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class ConsoleDto {
        @JsonInclude(NON_NULL)
        private Boolean serviceEnabled;
        @JsonInclude(NON_NULL)
        private Integer maxConcurrentSessions;
        @JsonInclude(NON_EMPTY)
        private List<GeneralConnectType> connectTypesSupported = new ArrayList<>();

        public Boolean getServiceEnabled() {
            return serviceEnabled;
        }

        public void setServiceEnabled(Boolean serviceEnabled) {
            this.serviceEnabled = serviceEnabled;
        }

        public Integer getMaxConcurrentSessions() {
            return maxConcurrentSessions;
        }

        public void setMaxConcurrentSessions(Integer maxConcurrentSessions) {
            this.maxConcurrentSessions = maxConcurrentSessions;
        }

        public List<GeneralConnectType> getConnectTypesSupported() {
            return connectTypesSupported;
        }

        public void setConnectTypesSupported(List<GeneralConnectType> connectTypesSupported) {
            this.connectTypesSupported = connectTypesSupported;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (o == null || getClass() != o.getClass()) {
                return false;
            }

            ConsoleDto that = (ConsoleDto) o;

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

    @JsonPropertyOrder({"serviceEnabled", "maxConcurrentSessions", "connectTypesSupported"})
    public static final class GraphicalConsoleDto {
        @JsonInclude(NON_NULL)
        private Boolean serviceEnabled;
        @JsonInclude(NON_NULL)
        private Integer maxConcurrentSessions;
        @JsonInclude(NON_EMPTY)
        private List<GraphicalConnectType> connectTypesSupported = new ArrayList<>();

        public Boolean getServiceEnabled() {
            return serviceEnabled;
        }

        public void setServiceEnabled(Boolean serviceEnabled) {
            this.serviceEnabled = serviceEnabled;
        }

        public Integer getMaxConcurrentSessions() {
            return maxConcurrentSessions;
        }

        public void setMaxConcurrentSessions(Integer maxConcurrentSessions) {
            this.maxConcurrentSessions = maxConcurrentSessions;
        }

        public List<GraphicalConnectType> getConnectTypesSupported() {
            return connectTypesSupported;
        }

        public void setConnectTypesSupported(List<GraphicalConnectType> connectTypesSupported) {
            this.connectTypesSupported = connectTypesSupported;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (o == null || getClass() != o.getClass()) {
                return false;
            }

            GraphicalConsoleDto that = (GraphicalConsoleDto) o;

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

    public class Actions extends RedfishActionsDto {
    }

    @JsonPropertyOrder({"@odata.type", "managedComputerSystems", "managedChassisCollection", "managerInChassis", "oem"})
    public final class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Manager.v1_1_0.Links";
        @JsonProperty("ManagerForServers")
        private final Set<Context> managedComputerSystems = new HashSet<>();
        @JsonProperty("ManagerForChassis")
        private final Set<Context> managedChassisCollection = new HashSet<>();
        private final Oem oem = new Oem();
        private Context managerInChassis;

        public String getoDataType() {
            return oDataType;
        }

        public Set<Context> getManagedComputerSystems() {
            return managedComputerSystems;
        }

        public Set<Context> getManagedChassisCollection() {
            return managedChassisCollection;
        }

        public Context getManagerInChassis() {
            return managerInChassis;
        }

        public void setManagerInChassis(Context managerInChassis) {
            this.managerInChassis = managerInChassis;
        }

        public Oem getOem() {
            return oem;
        }

        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOemDto {
            @JsonProperty("Intel_RackScale")
            private final RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonPropertyOrder({"@odata.type", "managerForServices", "managedEthernetSwitches"})
            @JsonIgnoreProperties(ignoreUnknown = true)
            public final class RackScaleOem {
                @JsonProperty("@odata.type")
                private final String oDataType = "#Intel.Oem.ManagerLinks";
                private final Set<Context> managerForServices = new HashSet<>();
                @JsonProperty("ManagerForSwitches")
                private final Set<Context> managedEthernetSwitches = new HashSet<>();

                public String getoDataType() {
                    return oDataType;
                }

                public Set<Context> getManagerForServices() {
                    return managerForServices;
                }

                public Set<Context> getManagedEthernetSwitches() {
                    return managedEthernetSwitches;
                }
            }
        }
    }
}
