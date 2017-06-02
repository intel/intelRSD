/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License;
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

import com.intel.podm.business.dto.redfish.attributes.BootDto;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeMemoryDto;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeProcessorsDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class ComposedNodeDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final UUID uuid;
    private final PowerState powerState;
    private final Status status;
    private final ComposedNodeProcessorsDto composedNodeProcessors;
    private final ComposedNodeMemoryDto composedNodeMemory;
    private final ComposedNodeState composedNodeState;
    private final BootDto boot;
    private final Actions actions;
    private final Links links;

    private ComposedNodeDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        uuid = builder.uuid;
        powerState = builder.powerState;
        status = builder.status;
        composedNodeProcessors = builder.composedNodeProcessors;
        composedNodeMemory = builder.composedNodeMemory;
        composedNodeState = builder.composedNodeState;
        boot = builder.boot;
        actions = new Actions(builder.allowableResetTypes);
        Links builderLinks = new Links(builder.computerSystem);
        builderLinks.setProcessors(builder.processors);
        builderLinks.setMemoryModules(builder.memoryModules);
        builderLinks.setEthernetInterfaces(builder.ethernetInterfaces);
        builderLinks.setLocalDrives(builder.localDrives);
        builderLinks.setSimpleStorage(builder.simpleStorage);
        builderLinks.setRemoteDrives(builder.remoteDrives);
        builderLinks.setManagedBy(builder.managedBy);
        builderLinks.setPcieDrives(builder.pcieDrives);
        builderLinks.setAvailablePcieDrives(builder.availablePcieDrives);
        links = builderLinks;
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

    @Override
    public String getDescription() {
        return description;
    }

    public UUID getUuid() {
        return uuid;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public Status getStatus() {
        return status;
    }

    public ComposedNodeProcessorsDto getComposedNodeProcessors() {
        return composedNodeProcessors;
    }

    public ComposedNodeMemoryDto getComposedNodeMemory() {
        return composedNodeMemory;
    }

    public ComposedNodeState getComposedNodeState() {
        return composedNodeState;
    }

    public BootDto getBoot() {
        return boot;
    }

    public Actions getActions() {
        return actions;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Actions {
        private final Set<ResetType> allowableResetTypes;

        public Actions(Set<ResetType> allowableResetTypes) {
            this.allowableResetTypes = allowableResetTypes;
        }

        public Set<ResetType> getAllowableResetTypes() {
            return allowableResetTypes;
        }
    }

    public static final class Links implements RedfishResource.Links {
        private final Context computerSystem;

        private Set<Context> processors;
        private Set<Context> memoryModules;
        private Set<Context> ethernetInterfaces;
        private Set<Context> localDrives;
        private Set<Context> simpleStorage;
        private Set<Context> remoteDrives;
        private Set<Context> managedBy;
        private Set<Context> pcieDrives;
        private Set<Context> availablePcieDrives;

        public Links(Context computerSystem) {
            this.computerSystem = computerSystem;
        }

        public Context getComputerSystem() {
            return computerSystem;
        }

        public Set<Context> getProcessors() {
            return processors;
        }

        public void setProcessors(Set<Context> processors) {
            this.processors = processors;
        }

        public Set<Context> getMemoryModules() {
            return memoryModules;
        }

        public void setMemoryModules(Set<Context> memoryModules) {
            this.memoryModules = memoryModules;
        }

        public Set<Context> getEthernetInterfaces() {
            return ethernetInterfaces;
        }

        public void setEthernetInterfaces(Set<Context> ethernetInterfaces) {
            this.ethernetInterfaces = ethernetInterfaces;
        }

        public Set<Context> getLocalDrives() {
            return localDrives;
        }

        public void setLocalDrives(Set<Context> localDrives) {
            this.localDrives = localDrives;
        }

        public Set<Context> getSimpleStorage() {
            return simpleStorage;
        }

        public void setSimpleStorage(Set<Context> simpleStorage) {
            this.simpleStorage = simpleStorage;
        }

        public Set<Context> getRemoteDrives() {
            return remoteDrives;
        }

        public void setRemoteDrives(Set<Context> remoteDrives) {
            this.remoteDrives = remoteDrives;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }

        public Set<Context> getPcieDrives() {
            return pcieDrives;
        }

        public void setPcieDrives(Set<Context> pcieDrives) {
            this.pcieDrives = pcieDrives;
        }

        public Set<Context> getAvailablePcieDrives() {
            return availablePcieDrives;
        }

        public void setAvailablePcieDrives(Set<Context> availablePcieDrives) {
            this.availablePcieDrives = availablePcieDrives;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private UUID uuid;
        private PowerState powerState;
        private Status status;
        private ComposedNodeProcessorsDto composedNodeProcessors;
        private ComposedNodeMemoryDto composedNodeMemory;
        private ComposedNodeState composedNodeState;
        private BootDto boot;
        private Context computerSystem;
        private Set<ResetType> allowableResetTypes = new HashSet<>();
        private Set<Context> processors = new HashSet<>();
        private Set<Context> memoryModules = new HashSet<>();
        private Set<Context> ethernetInterfaces = new HashSet<>();
        private Set<Context> localDrives = new HashSet<>();
        private Set<Context> simpleStorage = new HashSet<>();
        private Set<Context> remoteDrives = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();
        private Set<Context> pcieDrives = new HashSet<>();
        private Set<Context> availablePcieDrives = new HashSet<>();

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

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public Builder powerState(PowerState powerState) {
            this.powerState = powerState;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder composedNodeProcessors(ComposedNodeProcessorsDto composedNodeProcessors) {
            this.composedNodeProcessors = composedNodeProcessors;
            return this;
        }

        public Builder composedNodeMemory(ComposedNodeMemoryDto composedNodeMemory) {
            this.composedNodeMemory = composedNodeMemory;
            return this;
        }

        public Builder composedNodeState(ComposedNodeState composedNodeState) {
            this.composedNodeState = composedNodeState;
            return this;
        }

        public Builder boot(BootDto boot) {
            this.boot = boot;
            return this;
        }

        public Builder computerSystem(Context computerSystem) {
            this.computerSystem = computerSystem;
            return this;
        }

        public Builder allowableResetTypes(Collection<ResetType> allowableResetTypes) {
            this.allowableResetTypes.addAll(allowableResetTypes);
            return this;
        }

        public Builder processors(Set<Context> processors) {
            this.processors = processors;
            return this;
        }

        public Builder memoryModules(Set<Context> memoryModules) {
            this.memoryModules = memoryModules;
            return this;
        }

        public Builder ethernetInterfaces(Set<Context> ethernetInterfaces) {
            this.ethernetInterfaces = ethernetInterfaces;
            return this;
        }

        public Builder localDrives(Set<Context> localDrives) {
            this.localDrives = localDrives;
            return this;
        }

        public Builder simpleStorage(Set<Context> simpleStorage) {
            this.simpleStorage = simpleStorage;
            return this;
        }

        public Builder remoteDrives(Set<Context> remoteDrives) {
            this.remoteDrives = remoteDrives;
            return this;
        }

        public Builder managedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public Builder pcieDrives(Set<Context> pcieDrives) {
            this.pcieDrives = pcieDrives;
            return this;
        }

        public Builder availablePcieDrives(Set<Context> availablePcieDrives) {
            this.availablePcieDrives = availablePcieDrives;
            return this;
        }

        public ComposedNodeDto build() {
            return new ComposedNodeDto(this);
        }
    }
}
