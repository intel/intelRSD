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

import com.intel.podm.business.dto.redfish.attributes.MemoryLocationDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryRegionDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.math.BigDecimal;
import java.util.Collection;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class MemoryDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final MemoryType memoryType;
    private final MemoryDeviceType memoryDeviceType;
    private final BaseModuleType baseModuleType;
    private final List<MemoryMedia> memoryMedia;
    private final Integer capacityMib;
    private final Integer dataWidthBits;
    private final Integer busWidthBits;
    private final String manufacturer;
    private final String serialNumber;
    private final String partNumber;
    private final Collection<Integer> allowedSpeedsMhz;
    private final String firmwareRevision;
    private final String firmwareApiVersion;
    private final Collection<String> functionClasses;
    private final String vendorId;
    private final String deviceId;
    private final Integer rankCount;
    private final String deviceLocator;
    private final ErrorCorrection errorCorrection;
    private final Status status;
    private final Integer operatingSpeedMhz;
    private final Collection<MemoryRegionDto> regions;
    private final List<OperatingMemoryMode> operatingMemoryModes;
    private final BigDecimal voltageVolt;
    private final MemoryLocationDto memoryLocation;

    private MemoryDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        memoryType = builder.memoryType;
        memoryDeviceType = builder.memoryDeviceType;
        baseModuleType = builder.baseModuleType;
        memoryMedia = builder.memoryMedia;
        capacityMib = builder.capacityMib;
        dataWidthBits = builder.dataWidthBits;
        busWidthBits = builder.busWidthBits;
        manufacturer = builder.manufacturer;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        allowedSpeedsMhz = builder.allowedSpeedsMhz;
        firmwareRevision = builder.firmwareRevision;
        firmwareApiVersion = builder.firmwareApiVersion;
        functionClasses = builder.functionClasses;
        vendorId = builder.vendorId;
        deviceId = builder.deviceId;
        rankCount = builder.rankCount;
        deviceLocator = builder.deviceLocator;
        errorCorrection = builder.errorCorrection;
        status = builder.status;
        operatingSpeedMhz = builder.operatingSpeedMhz;
        regions = builder.regions;
        operatingMemoryModes = builder.operatingMemoryModes;
        voltageVolt = builder.voltageVolt;
        memoryLocation = builder.memoryLocation;
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

    public MemoryType getMemoryType() {
        return memoryType;
    }

    public MemoryDeviceType getMemoryDeviceType() {
        return memoryDeviceType;
    }

    public BaseModuleType getBaseModuleType() {
        return baseModuleType;
    }

    public List<MemoryMedia> getMemoryMedia() {
        return memoryMedia;
    }

    public Integer getCapacityMib() {
        return capacityMib;
    }

    public Integer getDataWidthBits() {
        return dataWidthBits;
    }

    public Integer getBusWidthBits() {
        return busWidthBits;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public Collection<Integer> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    public String getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    public Collection<String> getFunctionClasses() {
        return functionClasses;
    }

    public String getVendorId() {
        return vendorId;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public Integer getRankCount() {
        return rankCount;
    }

    public String getDeviceLocator() {
        return deviceLocator;
    }

    public ErrorCorrection getErrorCorrection() {
        return errorCorrection;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    public Collection<MemoryRegionDto> getRegions() {
        return regions;
    }

    public List<OperatingMemoryMode> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    public BigDecimal getVoltageVolt() {
        return voltageVolt;
    }

    public MemoryLocationDto getMemoryLocation() {
        return memoryLocation;
    }

    @Override
    public Links getLinks() {
        return null;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private MemoryType memoryType;
        private MemoryDeviceType memoryDeviceType;
        private BaseModuleType baseModuleType;
        private List<MemoryMedia> memoryMedia;
        private Integer capacityMib;
        private Integer dataWidthBits;
        private Integer busWidthBits;
        private String manufacturer;
        private String serialNumber;
        private String partNumber;
        private Collection<Integer> allowedSpeedsMhz;
        private String firmwareRevision;
        private String firmwareApiVersion;
        private Collection<String> functionClasses;
        private String vendorId;
        private String deviceId;
        private Integer rankCount;
        private String deviceLocator;
        private ErrorCorrection errorCorrection;
        private Status status;
        private Integer operatingSpeedMhz;
        private Collection<MemoryRegionDto> regions;
        private List<OperatingMemoryMode> operatingMemoryModes;
        private BigDecimal voltageVolt;
        private MemoryLocationDto memoryLocation;

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

        public Builder memoryType(MemoryType memoryType) {
            this.memoryType = memoryType;
            return this;
        }

        public Builder memoryDeviceType(MemoryDeviceType memoryDeviceType) {
            this.memoryDeviceType = memoryDeviceType;
            return this;
        }

        public Builder baseModuleType(BaseModuleType baseModuleType) {
            this.baseModuleType = baseModuleType;
            return this;
        }

        public Builder memoryMedia(List<MemoryMedia> memoryMedia) {
            this.memoryMedia = memoryMedia;
            return this;
        }

        public Builder capacityMib(Integer capacityMib) {
            this.capacityMib = capacityMib;
            return this;
        }

        public Builder dataWidthBits(Integer dataWidthBits) {
            this.dataWidthBits = dataWidthBits;
            return this;
        }

        public Builder busWidthBits(Integer busWidthBits) {
            this.busWidthBits = busWidthBits;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
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

        public Builder allowedSpeedsMhz(Collection<Integer> allowedSpeedsMhz) {
            this.allowedSpeedsMhz = allowedSpeedsMhz;
            return this;
        }

        public Builder firmwareRevision(String firmwareRevision) {
            this.firmwareRevision = firmwareRevision;
            return this;
        }

        public Builder firmwareApiVersion(String firmwareApiVersion) {
            this.firmwareApiVersion = firmwareApiVersion;
            return this;
        }

        public Builder functionClasses(Collection<String> functionClasses) {
            this.functionClasses = functionClasses;
            return this;
        }

        public Builder vendorId(String vendorId) {
            this.vendorId = vendorId;
            return this;
        }

        public Builder deviceId(String deviceId) {
            this.deviceId = deviceId;
            return this;
        }

        public Builder rankCount(Integer rankCount) {
            this.rankCount = rankCount;
            return this;
        }

        public Builder deviceLocator(String deviceLocator) {
            this.deviceLocator = deviceLocator;
            return this;
        }

        public Builder errorCorrection(ErrorCorrection errorCorrection) {
            this.errorCorrection = errorCorrection;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder operatingSpeedMhz(Integer operatingSpeedMhz) {
            this.operatingSpeedMhz = operatingSpeedMhz;
            return this;
        }

        public Builder regions(Collection<MemoryRegionDto> regions) {
            this.regions = regions;
            return this;
        }

        public Builder operatingMemoryModes(List<OperatingMemoryMode> operatingMemoryModes) {
            this.operatingMemoryModes = operatingMemoryModes;
            return this;
        }

        public Builder voltageVolt(BigDecimal voltageVolt) {
            this.voltageVolt = voltageVolt;
            return this;
        }

        public Builder memoryLocation(MemoryLocationDto memoryLocation) {
            this.memoryLocation = memoryLocation;
            return this;
        }

        public MemoryDto build() {
            return new MemoryDto(this);
        }
    }
}
