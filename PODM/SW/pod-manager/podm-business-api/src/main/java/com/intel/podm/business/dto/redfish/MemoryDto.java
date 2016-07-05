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

import com.intel.podm.business.dto.redfish.attributes.MemoryLocationDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryRegionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.MemoryType;

import java.math.BigDecimal;
import java.util.Collection;
import java.util.List;

public final class MemoryDto {
    private final String name;
    private final String id;
    private final String description;
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
    private final Context context;

    private MemoryDto(Builder builder) {
        name = builder.name;
        id = builder.id;
        description = builder.description;
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
        context = builder.context;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getId() {
        return id;
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

    public Context getContext() {
        return context;
    }

    public static final class Builder {
        private String name;
        private String id;
        private String description;
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
        private Context context;

        private Builder() {
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder id(String val) {
            id = val;
            return this;
        }

        public Builder memoryType(MemoryType val) {
            memoryType = val;
            return this;
        }

        public Builder memoryDeviceType(MemoryDeviceType val) {
            memoryDeviceType = val;
            return this;
        }

        public Builder baseModuleType(BaseModuleType val) {
            baseModuleType = val;
            return this;
        }

        public Builder memoryMedia(List<MemoryMedia> val) {
            memoryMedia = val;
            return this;
        }

        public Builder capacityMib(Integer val) {
            capacityMib = val;
            return this;
        }

        public Builder dataWidthBits(Integer val) {
            dataWidthBits = val;
            return this;
        }

        public Builder busWidthBits(Integer val) {
            busWidthBits = val;
            return this;
        }

        public Builder manufacturer(String val) {
            manufacturer = val;
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

        public Builder allowedSpeedsMhz(Collection<Integer> val) {
            allowedSpeedsMhz = val;
            return this;
        }

        public Builder firmwareRevision(String val) {
            firmwareRevision = val;
            return this;
        }

        public Builder firmwareApiVersion(String val) {
            firmwareApiVersion = val;
            return this;
        }

        public Builder functionClasses(Collection<String> val) {
            functionClasses = val;
            return this;
        }

        public Builder vendorId(String val) {
            vendorId = val;
            return this;
        }

        public Builder deviceId(String val) {
            deviceId = val;
            return this;
        }

        public Builder rankCount(Integer val) {
            rankCount = val;
            return this;
        }

        public Builder deviceLocator(String val) {
            deviceLocator = val;
            return this;
        }

        public Builder errorCorrection(ErrorCorrection val) {
            errorCorrection = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder operatingSpeedMhz(Integer val) {
            operatingSpeedMhz = val;
            return this;
        }

        public Builder regions(Collection<MemoryRegionDto> val) {
            regions = val;
            return this;
        }

        public Builder operatingMemoryModes(List<OperatingMemoryMode> val) {
            operatingMemoryModes = val;
            return this;
        }

        public Builder voltageVolt(BigDecimal val) {
            voltageVolt = val;
            return this;
        }

        public Builder memoryLocation(MemoryLocationDto val) {
            memoryLocation = val;
            return this;
        }

        public Builder context(Context val) {
            context = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public MemoryDto build() {
            return new MemoryDto(this);
        }
    }
}
