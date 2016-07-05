/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.NodeSystemType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;

import java.util.List;
import java.util.UUID;

import static com.google.common.collect.Lists.newArrayList;

public final class ComposedNodeDto {
    private final Id id;
    private final String name;
    private final String description;
    private final NodeSystemType systemType;
    private final String assetTag;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final UUID uuid;
    private final String hostName;
    private final PowerState powerState;
    private final String biosVersion;
    private final Status status;
    private final ComposedNodeProcessorsDto composedNodeProcessors;
    private final ComposedNodeMemoryDto composedNodeMemory;
    private final ComposedNodeState composedNodeState;
    private final BootDto boot;

    private final Context computerSystem;
    private final List<ResetType> allowableResetTypes;
    private final List<Context> processors;
    private final List<Context> memoryModules;
    private final List<Context> ethernetInterfaces;
    private final List<Context> localDrives;
    private final List<Context> simpleStorage;
    private final List<Context> remoteDrives;
    private final List<Context> managedBy;

    private ComposedNodeDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        systemType = builder.systemType;
        assetTag = builder.assetTag;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        uuid = builder.uuid;
        hostName = builder.hostName;
        powerState = builder.powerState;
        biosVersion = builder.biosVersion;
        status = builder.status;
        composedNodeProcessors = builder.composedNodeProcessors;
        composedNodeMemory = builder.composedNodeMemory;
        composedNodeState = builder.composedNodeState;
        boot = builder.boot;
        computerSystem = builder.computerSystem;
        allowableResetTypes = builder.allowableResetTypes;
        processors = builder.processors;
        memoryModules = builder.memoryModules;
        ethernetInterfaces = builder.ethernetInterfaces;
        localDrives = builder.localDrives;
        simpleStorage = builder.simpleStorage;
        remoteDrives = builder.remoteDrives;
        managedBy = builder.managedBy;
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

    public String getDescription() {
        return description;
    }

    public NodeSystemType getSystemType() {
        return systemType;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getSku() {
        return sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public UUID getUuid() {
        return uuid;
    }

    public String getHostName() {
        return hostName;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public String getBiosVersion() {
        return biosVersion;
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

    public Context getComputerSystem() {
        return computerSystem;
    }

    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public List<Context> getProcessors() {
        return processors;
    }

    public List<Context> getMemoryModules() {
        return memoryModules;
    }

    public List<Context> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public List<Context> getLocalDrives() {
        return localDrives;
    }

    public List<Context> getSimpleStorage() {
        return simpleStorage;
    }

    public List<Context> getRemoteDrives() {
        return remoteDrives;
    }

    public List<Context> getManagedBy() {
        return managedBy;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private NodeSystemType systemType;
        private String assetTag;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private UUID uuid;
        private String hostName;
        private PowerState powerState;
        private String biosVersion;
        private Status status;
        private ComposedNodeProcessorsDto composedNodeProcessors;
        private ComposedNodeMemoryDto composedNodeMemory;
        private ComposedNodeState composedNodeState;
        private BootDto boot;
        private Context computerSystem;
        private List<ResetType> allowableResetTypes = newArrayList();
        private List<Context> processors = newArrayList();
        private List<Context> memoryModules = newArrayList();
        private List<Context> ethernetInterfaces = newArrayList();
        private List<Context> localDrives = newArrayList();
        private List<Context> simpleStorage = newArrayList();
        private List<Context> remoteDrives = newArrayList();
        private List<Context> managedBy = newArrayList();

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

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder systemType(NodeSystemType systemType) {
            this.systemType = systemType;
            return this;
        }

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder sku(String sku) {
            this.sku = sku;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public Builder hostName(String hostName) {
            this.hostName = hostName;
            return this;
        }

        public Builder powerState(PowerState powerState) {
            this.powerState = powerState;
            return this;
        }

        public Builder biosVersion(String biosVersion) {
            this.biosVersion = biosVersion;
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

        public Builder allowableResetTypes(List<ResetType> allowableResetTypes) {
            this.allowableResetTypes = allowableResetTypes;
            return this;
        }

        public Builder processors(List<Context> processors) {
            this.processors = processors;
            return this;
        }

        public Builder memoryModules(List<Context> memoryModules) {
            this.memoryModules = memoryModules;
            return this;
        }

        public Builder ethernetInterfaces(List<Context> ethernetInterfaces) {
            this.ethernetInterfaces = ethernetInterfaces;
            return this;
        }

        public Builder localDrives(List<Context> localDrives) {
            this.localDrives = localDrives;
            return this;
        }

        public Builder simpleStorage(List<Context> simpleStorage) {
            this.simpleStorage = simpleStorage;
            return this;
        }

        public Builder remoteDrives(List<Context> remoteDrives) {
            this.remoteDrives = remoteDrives;
            return this;
        }

        public Builder managedBy(List<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public ComposedNodeDto build() {
            return new ComposedNodeDto(this);
        }
    }
}
