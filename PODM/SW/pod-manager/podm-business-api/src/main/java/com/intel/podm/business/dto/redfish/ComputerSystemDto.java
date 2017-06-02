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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class ComputerSystemDto extends BaseDto implements RedfishComputerSystem {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String assetTag;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final UUID uuid;
    private final String hostName;
    private final SystemType systemType;
    private final Status status;
    private final IndicatorLed indicatorLed;
    private final PowerState powerState;
    private final String biosVersion;
    private final Boot boot;
    private final BigDecimal totalSystemMemoryGiB;
    private final Status memoryStatus;
    private final Integer processorsCount;
    private final String processorsModel;
    private final Status processorsStatus;
    private final Set<Device> pciDevices;
    private final Set<ResetType> allowableResetTypes;
    private final DiscoveryState discoveryState;
    private final Integer processorSockets;
    private final Integer memorySockets;
    private final List<String> pcieConnectionId;
    private final Set<Context> pcieDevices;
    private final Set<Context> pcieDeviceFunctions;
    private final Set<Context> endpoints;
    private final Links links;

    private ComputerSystemDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        assetTag = builder.assetTag;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        uuid = builder.uuid;
        hostName = builder.hostName;
        status = builder.status;
        indicatorLed = builder.indicatorLed;
        powerState = builder.powerState;
        biosVersion = builder.biosVersion;
        boot = builder.boot;
        totalSystemMemoryGiB = builder.totalSystemMemoryGiB;
        memoryStatus = builder.memoryStatus;
        processorsCount = builder.processorsCount;
        processorsModel = builder.processorsModel;
        processorsStatus = builder.processorsStatus;
        pciDevices = builder.pciDevices;
        allowableResetTypes = builder.allowableResetTypes;
        discoveryState = builder.discoveryState;
        processorSockets = builder.processorSockets;
        memorySockets = builder.memorySockets;
        pcieConnectionId = builder.pcieConnectionId;
        systemType = builder.systemType;
        endpoints = builder.endpoints;
        pcieDevices = builder.pcieDevices;
        pcieDeviceFunctions = builder.pcieDeviceFunctions;
        links = new Links(builder.chassis, builder.managedBy, builder.endpoints);
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

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    @Override
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getSku() {
        return sku;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public String getPartNumber() {
        return partNumber;
    }

    @Override
    public UUID getUuid() {
        return uuid;
    }

    @Override
    public String getHostName() {
        return hostName;
    }

    @Override
    public SystemType getSystemType() {
        return systemType;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public PowerState getPowerState() {
        return powerState;
    }

    @Override
    public String getBiosVersion() {
        return biosVersion;
    }

    @Override
    public Boot getBoot() {
        return boot;
    }

    @Override
    public BigDecimal getTotalSystemMemoryGiB() {
        return totalSystemMemoryGiB;
    }

    @Override
    public Status getMemoryStatus() {
        return memoryStatus;
    }

    @Override
    public Integer getProcessorsCount() {
        return processorsCount;
    }

    @Override
    public String getProcessorsModel() {
        return processorsModel;
    }

    @Override
    public Status getProcessorsStatus() {
        return processorsStatus;
    }

    @Override
    public Set<Device> getPciDevices() {
        return pciDevices;
    }

    public Set<Context> getPciDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    @Override
    public Set<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    @Override
    public DiscoveryState getDiscoveryState() {
        return discoveryState;
    }

    @Override
    public Integer getProcessorSockets() {
        return processorSockets;
    }

    @Override
    public Integer getMemorySockets() {
        return memorySockets;
    }

    public Set<Context> getPcieDevices() {
        return pcieDevices;
    }

    public List<String> getPcieConnectionId() {
        return pcieConnectionId;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> chassis;
        private final Set<Context> managedBy;
        private final Set<Context> endpoints;

        public Links(Set<Context> chassis, Set<Context> managedBy, Set<Context> endpoints) {
            this.chassis = chassis;
            this.managedBy = managedBy;
            this.endpoints = endpoints;
        }

        public Set<Context> getChassis() {
            return chassis;
        }

        public Set<Context> getEndpoints() {
            return endpoints;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String assetTag;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private UUID uuid;
        private String hostName;
        private Status status;
        private IndicatorLed indicatorLed;
        private PowerState powerState;
        private String biosVersion;
        private Boot boot;
        private Set<Context> chassis;
        private BigDecimal totalSystemMemoryGiB;
        private Status memoryStatus;
        private Integer processorsCount;
        private String processorsModel;
        private List<String> pcieConnectionId = new ArrayList<>();
        private Status processorsStatus;
        private Set<Device> pciDevices = new HashSet<>();
        private Set<ResetType> allowableResetTypes = new HashSet<>();
        private DiscoveryState discoveryState;
        private Integer processorSockets;
        private Integer memorySockets;
        private Set<Context> managedBy;
        private SystemType systemType;
        private Set<Context> pcieDevices = new HashSet<>();
        private Set<Context> endpoints = new HashSet<>();
        private Set<Context> pcieDeviceFunctions = new HashSet<>();

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

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public Builder pcieConnectionId(List<String> pcieConnectionId) {
            this.pcieConnectionId = pcieConnectionId;
            return this;
        }

        public Builder endpoints(Set<Context> endpoints) {
            this.endpoints = endpoints;
            return this;
        }

        public Builder pcieDeviceFunctions(Set<Context> pcieDeviceFunctions) {
            this.pcieDeviceFunctions = pcieDeviceFunctions;
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

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder indicatorLed(IndicatorLed indicatorLed) {
            this.indicatorLed = indicatorLed;
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

        public Builder boot(Boot boot) {
            this.boot = boot;
            return this;
        }

        public Builder chassis(Set<Context> chassis) {
            this.chassis = chassis;
            return this;
        }

        public Builder totalSystemMemoryGiB(BigDecimal totalSystemMemoryGiB) {
            this.totalSystemMemoryGiB = totalSystemMemoryGiB;
            return this;
        }

        public Builder memoryStatus(Status memoryStatus) {
            this.memoryStatus = memoryStatus;
            return this;
        }

        public Builder processorsCount(Integer processorsCount) {
            this.processorsCount = processorsCount;
            return this;
        }

        public Builder processorsModel(String processorsModel) {
            this.processorsModel = processorsModel;
            return this;
        }

        public Builder processorsStatus(Status processorsStatus) {
            this.processorsStatus = processorsStatus;
            return this;
        }

        public Builder pciDevices(Set<Device> pciDevices) {
            this.pciDevices = pciDevices;
            return this;
        }

        public Builder allowableResetTypes(Collection<ResetType> allowableResetTypes) {
            this.allowableResetTypes.addAll(allowableResetTypes);
            return this;
        }

        public Builder discoveryState(DiscoveryState discoveryState) {
            this.discoveryState = discoveryState;
            return this;
        }

        public Builder processorSockets(Integer processorSockets) {
            this.processorSockets = processorSockets;
            return this;
        }

        public Builder memorySockets(Integer memorySockets) {
            this.memorySockets = memorySockets;
            return this;
        }

        public Builder managedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public Builder systemType(SystemType systemType) {
            this.systemType = systemType;
            return this;
        }

        public Builder pcieDevices(Set<Context> pcieDevices) {
            this.pcieDevices = pcieDevices;
            return this;
        }

        public ComputerSystemDto build() {
            return new ComputerSystemDto(this);
        }
    }
}
