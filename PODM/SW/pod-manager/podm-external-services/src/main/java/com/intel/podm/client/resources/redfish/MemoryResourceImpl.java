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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.MemoryResource;
import com.intel.podm.client.api.resources.redfish.MemoryLocationObject;
import com.intel.podm.client.api.resources.redfish.MemoryRegionObject;
import com.intel.podm.client.api.resources.redfish.LuiMemoryResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.MemoryType;

import java.math.BigDecimal;
import java.util.List;

@OdataTypes("#Memory.1.0.0.Memory")
public class MemoryResourceImpl extends ExternalServiceResourceImpl implements MemoryResource, LuiMemoryResource {
    @JsonProperty("MemoryType")
    private MemoryType memoryType;

    @JsonProperty("MemoryDeviceType")
    private MemoryDeviceType memoryDeviceType;

    @JsonProperty("BaseModuleType")
    private BaseModuleType baseModuleType;

    @JsonProperty("MemoryMedia")
    private List<MemoryMedia> memoryMedia;

    @JsonProperty("CapacityMiB")
    private Integer capacityMib;

    @JsonProperty("DataWidthBits")
    private Integer dataWidthBits;

    @JsonProperty("BusWidthBits")
    private Integer busWidthBits;

    @JsonProperty("Manufacturer")
    private String manufacturer;

    @JsonProperty("SerialNumber")
    private String serialNumber;

    @JsonProperty("PartNumber")
    private String partNumber;

    @JsonProperty("AllowedSpeedsMHz")
    private List<Integer> allowedSpeedsMhz;

    @JsonProperty("FirmwareRevision")
    private String firmwareRevision;

    @JsonProperty("FirmwareApiVersion")
    private String firmwareApiVersion;

    @JsonProperty("FunctionClasses")
    private List<String> functionClasses;
    @JsonProperty("VendorId")
    private String vendorId;

    @JsonProperty("DeviceId")
    private String deviceId;

    @JsonProperty("RankCount")
    private Integer rankCount;

    @JsonProperty("DeviceLocator")
    private String deviceLocator;

    @JsonProperty("ErrorCorrection")
    private ErrorCorrection errorCorrection;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("OperatingSpeedMHz")
    private Integer operatingSpeedMhz;

    @JsonProperty("Regions")
    private Iterable<MemoryRegionObjectImpl> regions;

    @JsonProperty("OperatingMemoryModes")
    private List<OperatingMemoryMode> operatingMemoryModes;

    @JsonProperty("MemoryLocation")
    private MemoryLocationObjectImpl memoryLocation;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public MemoryType getMemoryType() {
        return memoryType;
    }

    @Override
    public MemoryDeviceType getMemoryDeviceType() {
        return memoryDeviceType;
    }

    @Override
    public BaseModuleType getBaseModuleType() {
        return baseModuleType;
    }

    @Override
    public List<MemoryMedia> getMemoryMedia() {
        return memoryMedia;
    }

    @Override
    public Integer getCapacityMib() {
        return capacityMib;
    }

    @Override
    public Integer getDataWidthBits() {
        return dataWidthBits;
    }

    @Override
    public Integer getBusWidthBits() {
        return busWidthBits;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
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
    public List<Integer> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    @Override
    public MemoryLocationObject getMemoryLocation() {
        return memoryLocation;
    }

    @Override
    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    @Override
    public String getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    @Override
    public List<String> getFunctionClasses() {
        return functionClasses;
    }

    @Override
    public String getVendorId() {
        return vendorId;
    }

    @Override
    public String getDeviceId() {
        return deviceId;
    }

    @Override
    public Integer getRankCount() {
        return rankCount;
    }

    @Override
    public String getDeviceLocator() {
        return deviceLocator;
    }

    @Override
    public ErrorCorrection getErrorCorrection() {
        return errorCorrection;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public Integer getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    @Override
    public Iterable<MemoryRegionObject> getRegions() {
        return (Iterable) regions;
    }

    @Override
    public List<OperatingMemoryMode> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @Override
    public BigDecimal getVoltageVolt() {
        return oem.rackScaleOem.voltageVolt;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonIgnoreProperties(ignoreUnknown = true)
        private static final class RackScaleOem {
            @JsonProperty("VoltageVolt")
            private BigDecimal voltageVolt;
        }
    }
}
