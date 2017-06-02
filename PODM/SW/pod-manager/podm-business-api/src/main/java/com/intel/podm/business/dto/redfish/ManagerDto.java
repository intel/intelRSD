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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.GraphicalConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;
import java.util.UUID;

import static java.util.Collections.unmodifiableSet;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class ManagerDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final ManagerType managerType;
    private final UUID serviceEntryPointUuid;
    private final UUID uuid;
    private final String model;
    private final Status status;
    private final PowerState powerState;
    private final GraphicalConsoleDto graphicalConsole;
    private final ConsoleDto serialConsole;
    private final ConsoleDto commandShell;
    private final String firmwareVersion;
    private final Context context;
    private final Links links;
    private final NetworkProtocolDto networkProtocol;

    private ManagerDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        managerType = builder.managerType;
        serviceEntryPointUuid = builder.serviceEntryPointUuid;
        uuid = builder.uuid;
        model = builder.model;
        status = builder.status;
        powerState = builder.powerState;
        graphicalConsole = builder.graphicalConsole;
        serialConsole = builder.serialConsole;
        commandShell = builder.commandShell;
        firmwareVersion = builder.firmwareVersion;
        context = builder.context;
        links = createLinks(builder);
        networkProtocol = builder.networkProtocol;
    }

    private Links createLinks(Builder builder) {
        return Links.newBuilder()
                .managedComputerSystems(unmodifiableSet(builder.managedComputerSystems))
                .managedChassisCollection(unmodifiableSet(builder.managedChassisCollection))
                .managerInChassis(builder.managerInChassis)
                .managedEthernetSwitches(unmodifiableSet(builder.managedEthernetSwitches))
                .managedServices(unmodifiableSet(builder.managedServices))
                .build();
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    public ManagerType getManagerType() {
        return managerType;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public UUID getServiceEntryPointUuid() {
        return serviceEntryPointUuid;
    }

    public UUID getUuid() {
        return uuid;
    }

    public String getModel() {
        return model;
    }

    public Status getStatus() {
        return status;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public GraphicalConsoleDto getGraphicalConsole() {
        return graphicalConsole;
    }

    public ConsoleDto getSerialConsole() {
        return serialConsole;
    }

    public ConsoleDto getCommandShell() {
        return commandShell;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public NetworkProtocolDto getNetworkProtocol() {
        return networkProtocol;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public Context getContext() {
        return context;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> managedComputerSystems;
        private final Set<Context> managedChassisCollection;
        private final Context managerInChassis;
        private final Set<Context> managedEthernetSwitches;
        private final Set<Context> managedServices;
        private final Context context;

        private Links(Builder builder) {
            managedComputerSystems = builder.managedComputerSystems;
            managedChassisCollection = builder.managedChassisCollection;
            managerInChassis = builder.managerInChassis;
            managedEthernetSwitches = builder.managedEthernetSwitches;
            managedServices = builder.managedServices;
            context = builder.context;
        }

        public static Builder newBuilder() {
            return new Builder();
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

        public Set<Context> getManagedEthernetSwitches() {
            return managedEthernetSwitches;
        }

        public Set<Context> getManagedServices() {
            return managedServices;
        }

        public Context getContext() {
            return context;
        }

        public static final class Builder {
            private Set<Context> managedComputerSystems;
            private Set<Context> managedChassisCollection;
            private Context managerInChassis;
            private Set<Context> managedEthernetSwitches;
            private Set<Context> managedServices;
            private Context context;

            private Builder() {
            }

            public Builder managedComputerSystems(Set<Context> managedComputerSystems) {
                this.managedComputerSystems = managedComputerSystems;
                return this;
            }

            public Builder managedChassisCollection(Set<Context> managedChassisCollection) {
                this.managedChassisCollection = managedChassisCollection;
                return this;
            }

            public Builder managerInChassis(Context managerInChassis) {
                this.managerInChassis = managerInChassis;
                return this;
            }

            public Builder managedEthernetSwitches(Set<Context> managedEthernetSwitches) {
                this.managedEthernetSwitches = managedEthernetSwitches;
                return this;
            }

            public Builder managedServices(Set<Context> managedServices) {
                this.managedServices = managedServices;
                return this;
            }

            public Builder context(Context context) {
                this.context = context;
                return this;
            }

            public Links build() {
                return new Links(this);
            }
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private ManagerType managerType;
        private UUID serviceEntryPointUuid;
        private UUID uuid;
        private String model;
        private Status status;
        private PowerState powerState;
        private GraphicalConsoleDto graphicalConsole;
        private ConsoleDto serialConsole;
        private ConsoleDto commandShell;
        private String firmwareVersion;
        private Set<Context> managedComputerSystems;
        private Set<Context> managedChassisCollection;
        private Context managerInChassis;
        private Set<Context> managedEthernetSwitches;
        private Set<Context> managedServices;
        private Context context;
        private NetworkProtocolDto networkProtocol;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder managerType(ManagerType managerType) {
            this.managerType = managerType;
            return this;
        }

        public Builder serviceEntryPointUuid(UUID serviceEntryPointUuid) {
            this.serviceEntryPointUuid = serviceEntryPointUuid;
            return this;
        }

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder powerState(PowerState powerState) {
            this.powerState = powerState;
            return this;
        }

        public Builder graphicalConsole(GraphicalConsoleDto graphicalConsole) {
            this.graphicalConsole = graphicalConsole;
            return this;
        }

        public Builder serialConsole(ConsoleDto serialConsole) {
            this.serialConsole = serialConsole;
            return this;
        }

        public Builder commandShell(ConsoleDto commandShell) {
            this.commandShell = commandShell;
            return this;
        }

        public Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
            return this;
        }

        public Builder networkProtocol(NetworkProtocolDto networkProtocol) {
            this.networkProtocol = networkProtocol;
            return this;
        }

        public Builder managedComputerSystems(Set<Context> managedComputerSystems) {
            this.managedComputerSystems = managedComputerSystems;
            return this;
        }

        public Builder managedChassisCollection(Set<Context> managedChassisCollection) {
            this.managedChassisCollection = managedChassisCollection;
            return this;
        }

        public Builder managerInChassis(Context managerInChassis) {
            this.managerInChassis = managerInChassis;
            return this;
        }

        public Builder managedEthernetSwitches(Set<Context> managedEthernetSwitches) {
            this.managedEthernetSwitches = managedEthernetSwitches;
            return this;
        }

        public Builder managedServices(Set<Context> managedServices) {
            this.managedServices = managedServices;
            return this;
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }

        public ManagerDto build() {
            return new ManagerDto(this);
        }
    }
}
