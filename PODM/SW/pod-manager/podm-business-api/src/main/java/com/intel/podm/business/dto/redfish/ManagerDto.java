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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.GraphicalConsoleDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.Status;

import java.util.List;
import java.util.UUID;

import static java.util.Collections.unmodifiableList;

public final class ManagerDto {
    private final Id id;
    private final String name;
    private final ManagerType managerType;
    private final String description;
    private final UUID serviceEntryPointUuid;
    private final UUID uuid;
    private final String model;
    private final Status status;
    private final GraphicalConsoleDto graphicalConsole;
    private final ConsoleDto serialConsole;
    private final ConsoleDto commandShell;
    private final String firmwareVersion;
    private final boolean hasNetworkProtocol;

    private final List<Context> managedComputerSystems;
    private final List<Context> managedChassisCollection;
    private final Context managerInChassis;
    private final List<Context> managedEthernetSwitches;
    private final List<Context> managedServices;
    private final Context context;

    private ManagerDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        managerType = builder.managerType;
        description = builder.description;
        serviceEntryPointUuid = builder.serviceEntryPointUuid;
        uuid = builder.uuid;
        model = builder.model;
        status = builder.status;
        graphicalConsole = builder.graphicalConsole;
        serialConsole = builder.serialConsole;
        commandShell = builder.commandShell;
        firmwareVersion = builder.firmwareVersion;
        hasNetworkProtocol = builder.hasNetworkProtocol;
        managedComputerSystems = unmodifiableList(builder.managedComputerSystems);
        managedChassisCollection = unmodifiableList(builder.managedChassisCollection);
        managerInChassis = builder.managerInChassis;
        managedEthernetSwitches = unmodifiableList(builder.managedEthernetSwitches);
        managedServices = unmodifiableList(builder.managedServices);
        context = builder.context;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public ManagerType getManagerType() {
        return managerType;
    }

    public String getDescription() {
        return description;
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

    public boolean hasNetworkProtocol() {
        return hasNetworkProtocol;
    }

    public List<Context> getManagedComputerSystems() {
        return managedComputerSystems;
    }

    public List<Context> getManagedChassisCollection() {
        return managedChassisCollection;
    }

    public Context getManagerInChassis() {
        return managerInChassis;
    }

    public List<Context> getManagedEthernetSwitches() {
        return managedEthernetSwitches;
    }

    public List<Context> getManagedServices() {
        return managedServices;
    }

    public Context getContext() {
        return context;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private ManagerType managerType;
        private String description;
        private UUID serviceEntryPointUuid;
        private UUID uuid;
        private String model;
        private Status status;
        private GraphicalConsoleDto graphicalConsole;
        private ConsoleDto serialConsole;
        private ConsoleDto commandShell;
        private String firmwareVersion;
        private boolean hasNetworkProtocol;
        private List<Context> managedComputerSystems;
        private List<Context> managedChassisCollection;
        private Context managerInChassis;
        private List<Context> managedEthernetSwitches;
        private List<Context> managedServices;
        private Context context;

        private Builder() {
        }

        public Builder id(Id id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder managerType(ManagerType managerType) {
            this.managerType = managerType;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
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

        public Builder hasNetworkProtocol(boolean hasNetworkProtocol) {
            this.hasNetworkProtocol = hasNetworkProtocol;
            return this;
        }

        public Builder managedComputerSystems(List<Context> managedComputerSystems) {
            this.managedComputerSystems = managedComputerSystems;
            return this;
        }

        public Builder managedChassisCollection(List<Context> managedChassisCollection) {
            this.managedChassisCollection = managedChassisCollection;
            return this;
        }

        public Builder managerInChassis(Context managerInChassis) {
            this.managerInChassis = managerInChassis;
            return this;
        }

        public Builder managedEthernetSwitches(List<Context> managedEthernetSwitches) {
            this.managedEthernetSwitches = managedEthernetSwitches;
            return this;
        }

        public Builder managedServices(List<Context> managedServices) {
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
