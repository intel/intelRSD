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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryClassification;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.ALWAYS;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "name", "id", "description", "memoryType", "memoryDeviceType", "baseModuleType", "memoryMedia",
    "capacityMib", "dataWidthBits", "busWidthBits", "manufacturer", "serialNumber", "partNumber", "allowedSpeedsMhz", "firmwareRevision",
    "firmwareApiVersion", "functionClasses", "vendorId", "deviceId", "rankCount", "deviceLocator", "memoryLocation", "errorCorrection", "status",
    "operatingSpeedMhz", "regions", "operatingMemoryModes", "memoryMetrics", "actions", "oem"})
@JsonInclude(ALWAYS)
@SuppressWarnings({"checkstyle:MethodCount"})
public final class MemoryDto extends RedfishDto {
    private final Oem oem = new Oem();
    private final Actions actions = new Actions();
    private MemoryType memoryType;
    private MemoryDeviceType memoryDeviceType;
    private BaseModuleType baseModuleType;
    private List<MemoryMedia> memoryMedia = new ArrayList<>();
    @JsonProperty("CapacityMiB")
    private Integer capacityMib;
    private Integer dataWidthBits;
    private Integer busWidthBits;
    private String manufacturer;
    private String serialNumber;
    private String partNumber;
    @JsonProperty("AllowedSpeedsMHz")
    private Collection<Integer> allowedSpeedsMhz = new ArrayList<>();
    private String firmwareRevision;
    private String firmwareApiVersion;
    private Collection<String> functionClasses = new ArrayList<>();
    @JsonProperty("VendorID")
    private String vendorId;
    @JsonProperty("DeviceID")
    private String deviceId;
    private Integer rankCount;
    private String deviceLocator;
    private ErrorCorrection errorCorrection;
    private Status status;
    private Integer operatingSpeedMhz;
    private Collection<MemoryRegionDto> regions = new ArrayList<>();
    private List<OperatingMemoryMode> operatingMemoryModes = new ArrayList<>();
    private MemoryLocationDto memoryLocation = new MemoryLocationDto();
    @JsonProperty("Metrics")
    @JsonInclude(NON_NULL)
    private SingletonContext memoryMetrics;

    public MemoryDto() {
        super("#Memory.v1_1_0.Memory");
    }

    public MemoryType getMemoryType() {
        return memoryType;
    }

    public void setMemoryType(MemoryType memoryType) {
        this.memoryType = memoryType;
    }

    public MemoryDeviceType getMemoryDeviceType() {
        return memoryDeviceType;
    }

    public void setMemoryDeviceType(MemoryDeviceType memoryDeviceType) {
        this.memoryDeviceType = memoryDeviceType;
    }

    public BaseModuleType getBaseModuleType() {
        return baseModuleType;
    }

    public void setBaseModuleType(BaseModuleType baseModuleType) {
        this.baseModuleType = baseModuleType;
    }

    public List<MemoryMedia> getMemoryMedia() {
        return memoryMedia;
    }

    public void setMemoryMedia(List<MemoryMedia> memoryMedia) {
        this.memoryMedia = memoryMedia;
    }

    public Integer getCapacityMib() {
        return capacityMib;
    }

    public void setCapacityMib(Integer capacityMib) {
        this.capacityMib = capacityMib;
    }

    public Integer getDataWidthBits() {
        return dataWidthBits;
    }

    public void setDataWidthBits(Integer dataWidthBits) {
        this.dataWidthBits = dataWidthBits;
    }

    public Integer getBusWidthBits() {
        return busWidthBits;
    }

    public void setBusWidthBits(Integer busWidthBits) {
        this.busWidthBits = busWidthBits;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public Collection<Integer> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    public void setAllowedSpeedsMhz(Collection<Integer> allowedSpeedsMhz) {
        this.allowedSpeedsMhz = allowedSpeedsMhz;
    }

    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    public void setFirmwareRevision(String firmwareRevision) {
        this.firmwareRevision = firmwareRevision;
    }

    public String getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    public void setFirmwareApiVersion(String firmwareApiVersion) {
        this.firmwareApiVersion = firmwareApiVersion;
    }

    public Collection<String> getFunctionClasses() {
        return functionClasses;
    }

    public void setFunctionClasses(Collection<String> functionClasses) {
        this.functionClasses = functionClasses;
    }

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public Integer getRankCount() {
        return rankCount;
    }

    public void setRankCount(Integer rankCount) {
        this.rankCount = rankCount;
    }

    public String getDeviceLocator() {
        return deviceLocator;
    }

    public void setDeviceLocator(String deviceLocator) {
        this.deviceLocator = deviceLocator;
    }

    public ErrorCorrection getErrorCorrection() {
        return errorCorrection;
    }

    public void setErrorCorrection(ErrorCorrection errorCorrection) {
        this.errorCorrection = errorCorrection;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Integer getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    public void setOperatingSpeedMhz(Integer operatingSpeedMhz) {
        this.operatingSpeedMhz = operatingSpeedMhz;
    }

    public Collection<MemoryRegionDto> getRegions() {
        return regions;
    }

    public void setRegions(Collection<MemoryRegionDto> regions) {
        this.regions = regions;
    }

    public List<OperatingMemoryMode> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    public void setOperatingMemoryModes(List<OperatingMemoryMode> operatingMemoryModes) {
        this.operatingMemoryModes = operatingMemoryModes;
    }

    public MemoryLocationDto getMemoryLocation() {
        return memoryLocation;
    }

    public void setMemoryLocation(MemoryLocationDto memoryLocation) {
        this.memoryLocation = memoryLocation;
    }

    public Oem getOem() {
        return oem;
    }

    public Actions getActions() {
        return actions;
    }

    public SingletonContext getMemoryMetrics() {
        return memoryMetrics;
    }

    public void setMemoryMetrics(SingletonContext memoryMetrics) {
        this.memoryMetrics = memoryMetrics;
    }

    @JsonPropertyOrder({"regionId", "memoryClassification", "offsetMib", "sizeMib"})
    public static final class MemoryRegionDto {
        private String regionId;
        private MemoryClassification memoryClassification;
        @JsonProperty("OffsetMiB")
        private Integer offsetMib;
        @JsonProperty("SizeMiB")
        private Integer sizeMib;

        public String getRegionId() {
            return regionId;
        }

        public void setRegionId(String regionId) {
            this.regionId = regionId;
        }

        public MemoryClassification getMemoryClassification() {
            return memoryClassification;
        }

        public void setMemoryClassification(MemoryClassification memoryClassification) {
            this.memoryClassification = memoryClassification;
        }

        public Integer getOffsetMib() {
            return offsetMib;
        }

        public void setOffsetMib(Integer offsetMib) {
            this.offsetMib = offsetMib;
        }

        public Integer getSizeMib() {
            return sizeMib;
        }

        public void setSizeMib(Integer sizeMib) {
            this.sizeMib = sizeMib;
        }
    }

    @JsonPropertyOrder({"socket", "memoryController", "channel", "slot"})
    @JsonInclude(ALWAYS)
    public static final class MemoryLocationDto {
        private Integer socket;
        private Integer memoryController;
        private Integer channel;
        private Integer slot;

        public Integer getSocket() {
            return socket;
        }

        public void setSocket(Integer socket) {
            this.socket = socket;
        }

        public Integer getMemoryController() {
            return memoryController;
        }

        public void setMemoryController(Integer memoryController) {
            this.memoryController = memoryController;
        }

        public Integer getChannel() {
            return channel;
        }

        public void setChannel(Integer channel) {
            this.channel = channel;
        }

        public Integer getSlot() {
            return slot;
        }

        public void setSlot(Integer slot) {
            this.slot = slot;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "voltageVolt"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.Memory";
            private BigDecimal voltageVolt;

            public BigDecimal getVoltageVolt() {
                return voltageVolt;
            }

            public void setVoltageVolt(BigDecimal voltageVolt) {
                this.voltageVolt = voltageVolt;
            }
        }
    }

    public class Actions extends RedfishActionsDto {
    }
}
