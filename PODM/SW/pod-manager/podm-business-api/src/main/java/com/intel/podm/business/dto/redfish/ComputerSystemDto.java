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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.BootDto;
import com.intel.podm.business.dto.redfish.attributes.SystemDeviceDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;

import java.util.List;
import java.util.UUID;

public final class ComputerSystemDto {
    private final Id id;
    private final String name;
    private final String assetTag;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final String description;
    private final UUID uuid;
    private final String hostName;
    private final SystemType systemType;
    private final Status status;
    private final IndicatorLed indicatorLed;
    private final PowerState powerState;
    private final String biosVersion;
    private final BootDto boot;
    private final List<Context> chassis;
    private final Integer totalSystemMemoryGiB;
    private final Status memoryStatus;
    private final Integer processorsCount;
    private final String processorsModel;
    private final Status processorsStatus;
    private final List<SystemDeviceDto> pciDevices;
    private final List<ResetType> allowableResetTypes;
    private final DiscoveryState discoveryState;
    private final Integer processorSockets;
    private final Integer memorySockets;
    private final List<Context> managedBy;
    private ComputerSystemDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        assetTag = builder.assetTag;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        description = builder.description;
        uuid = builder.uuid;
        hostName = builder.hostName;
        status = builder.status;
        indicatorLed = builder.indicatorLed;
        powerState = builder.powerState;
        biosVersion = builder.biosVersion;
        boot = builder.boot;
        chassis = builder.chassis;
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
        managedBy = builder.managedBy;
        systemType = builder.systemType;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public SystemType getSystemType() {
        return systemType;
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
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

    public String getDescription() {
        return description;
    }

    public UUID getUuid() {
        return uuid;
    }

    public String getHostName() {
        return hostName;
    }

    public Status getStatus() {
        return status;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public String getBiosVersion() {
        return biosVersion;
    }

    public BootDto getBoot() {
        return boot;
    }

    public List<Context> getChassis() {
        return chassis;
    }

    public Integer getTotalSystemMemoryGiB() {
        return totalSystemMemoryGiB;
    }

    public Status getMemoryStatus() {
        return memoryStatus;
    }

    public Integer getProcessorsCount() {
        return processorsCount;
    }

    public String getProcessorsModel() {
        return processorsModel;
    }

    public Status getProcessorsStatus() {
        return processorsStatus;
    }

    public List<SystemDeviceDto> getPciDevices() {
        return pciDevices;
    }

    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public DiscoveryState getDiscoveryState() {
        return discoveryState;
    }

    public Integer getProcessorSockets() {
        return processorSockets;
    }

    public Integer getMemorySockets() {
        return memorySockets;
    }

    public List<Context> getManagedBy() {
        return managedBy;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String assetTag;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private String description;
        private UUID uuid;
        private String hostName;
        private Status status;
        private IndicatorLed indicatorLed;
        private PowerState powerState;
        private String biosVersion;
        private BootDto boot;
        private List<Context> chassis;
        private Integer totalSystemMemoryGiB;
        private Status memoryStatus;
        private Integer processorsCount;
        private String processorsModel;
        private Status processorsStatus;
        private List<SystemDeviceDto> pciDevices;
        private List<ResetType> allowableResetTypes;
        private DiscoveryState discoveryState;
        private Integer processorSockets;
        private Integer memorySockets;
        private List<Context> managedBy;
        private SystemType systemType;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder assetTag(String val) {
            assetTag = val;
            return this;
        }

        public Builder manufacturer(String val) {
            manufacturer = val;
            return this;
        }

        public Builder model(String val) {
            model = val;
            return this;
        }

        public Builder sku(String val) {
            sku = val;
            return this;
        }

        public Builder serialNumber(String val) {
            serialNumber = val;
            return this;
        }

        public Builder partNumber(String val) {
            partNumber = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder uuid(UUID val) {
            uuid = val;
            return this;
        }

        public Builder hostName(String val) {
            hostName = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder indicatorLed(IndicatorLed val) {
            indicatorLed = val;
            return this;
        }

        public Builder powerState(PowerState val) {
            powerState = val;
            return this;
        }

        public Builder biosVersion(String val) {
            biosVersion = val;
            return this;
        }

        public Builder boot(BootDto val) {
            boot = val;
            return this;
        }

        public Builder chassis(List<Context> val) {
            chassis = val;
            return this;
        }

        public Builder totalSystemMemoryGiB(Integer val) {
            totalSystemMemoryGiB = val;
            return this;
        }

        public Builder memoryStatus(Status val) {
            memoryStatus = val;
            return this;
        }

        public Builder processorsCount(Integer val) {
            processorsCount = val;
            return this;
        }

        public Builder processorsModel(String val) {
            processorsModel = val;
            return this;
        }

        public Builder processorsStatus(Status val) {
            processorsStatus = val;
            return this;
        }

        public Builder pciDevices(List<SystemDeviceDto> val) {
            pciDevices = val;
            return this;
        }

        public Builder allowableResetTypes(List<ResetType> resetTypes) {
            allowableResetTypes = resetTypes;
            return this;
        }

        public Builder discoveryState(DiscoveryState val) {
            discoveryState = val;
            return this;
        }

        public Builder processorSockets(Integer val) {
            processorSockets = val;
            return this;
        }

        public Builder memorySockets(Integer val) {
            memorySockets = val;
            return this;
        }

        public Builder managedBy(List<Context> val) {
            managedBy = val;
            return this;
        }

        public Builder systemType(SystemType val) {
            systemType = val;
            return this;
        }

        public ComputerSystemDto build() {
            return new ComputerSystemDto(this);
        }
    }
}
