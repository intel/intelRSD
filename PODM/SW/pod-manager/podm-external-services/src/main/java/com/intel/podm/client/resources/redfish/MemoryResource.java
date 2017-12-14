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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#Memory" + VERSION_PATTERN + "Memory"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class MemoryResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("MemoryType")
    @AsUnassigned(WHEN_NULL)
    private Ref<MemoryType> memoryType = unassigned();

    @JsonProperty("MemoryDeviceType")
    @AsUnassigned(WHEN_NULL)
    private Ref<MemoryDeviceType> memoryDeviceType = unassigned();

    @JsonProperty("BaseModuleType")
    @AsUnassigned(WHEN_NULL)
    private Ref<BaseModuleType> baseModuleType = unassigned();

    @JsonProperty("MemoryMedia")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<MemoryMedia>> memoryMedia = unassigned();

    @JsonProperty("CapacityMiB")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> capacityMib = unassigned();

    @JsonProperty("DataWidthBits")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> dataWidthBits = unassigned();

    @JsonProperty("BusWidthBits")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> busWidthBits = unassigned();

    @JsonProperty("Manufacturer")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> manufacturer = unassigned();

    @JsonProperty("SerialNumber")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> serialNumber = unassigned();

    @JsonProperty("PartNumber")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> partNumber = unassigned();

    @JsonProperty("AllowedSpeedsMHz")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<Integer>> allowedSpeedsMhz = unassigned();

    @JsonProperty("FirmwareRevision")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> firmwareRevision = unassigned();

    @JsonProperty("FirmwareApiVersion")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> firmwareApiVersion = unassigned();

    @JsonProperty("FunctionClasses")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<String>> functionClasses = unassigned();

    @JsonProperty("VendorID")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> vendorId = unassigned();

    @JsonProperty("DeviceID")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> deviceId = unassigned();

    @JsonProperty("RankCount")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> rankCount = unassigned();

    @JsonProperty("DeviceLocator")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> deviceLocator = unassigned();

    @JsonProperty("ErrorCorrection")
    @AsUnassigned(WHEN_NULL)
    private Ref<ErrorCorrection> errorCorrection = unassigned();

    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @JsonProperty("OperatingSpeedMhz")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> operatingSpeedMhz = unassigned();

    @JsonProperty("Regions")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<MemoryRegionObject>> regions = unassigned();

    @JsonProperty("OperatingMemoryModes")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<OperatingMemoryMode>> operatingMemoryModes = unassigned();

    @JsonProperty("MemoryLocation")
    private MemoryLocationObject memoryLocation;

    @JsonProperty("Metrics")
    private ODataId memoryMetrics;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Ref<MemoryType> getMemoryType() {
        return memoryType;
    }

    public Ref<MemoryDeviceType> getMemoryDeviceType() {
        return memoryDeviceType;
    }

    public Ref<BaseModuleType> getBaseModuleType() {
        return baseModuleType;
    }

    public Ref<List<MemoryMedia>> getMemoryMedia() {
        return memoryMedia;
    }

    public Ref<Integer> getCapacityMib() {
        return capacityMib;
    }

    public Ref<Integer> getDataWidthBits() {
        return dataWidthBits;
    }

    public Ref<Integer> getBusWidthBits() {
        return busWidthBits;
    }

    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    public Ref<String> getSerialNumber() {
        return serialNumber;
    }

    public Ref<String> getPartNumber() {
        return partNumber;
    }

    public Ref<List<Integer>> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    public MemoryLocationObject getMemoryLocation() {
        return memoryLocation;
    }

    public Ref<String> getFirmwareRevision() {
        return firmwareRevision;
    }

    public Ref<String> getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    public Ref<List<String>> getFunctionClasses() {
        return functionClasses;
    }

    public Ref<String> getVendorId() {
        return vendorId;
    }

    public Ref<String> getDeviceId() {
        return deviceId;
    }

    public Ref<Integer> getRankCount() {
        return rankCount;
    }

    public Ref<String> getDeviceLocator() {
        return deviceLocator;
    }

    public Ref<ErrorCorrection> getErrorCorrection() {
        return errorCorrection;
    }

    public Ref<Status> getStatus() {
        return status;
    }

    public Ref<Integer> getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    public Ref<List<MemoryRegionObject>> getRegions() {
        return regions;
    }

    public Ref<List<OperatingMemoryMode>> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @LinkName("memoryMetrics")
    public ResourceSupplier getMemoryMetrics() {
        if (memoryMetrics == null) {
            return null;
        }

        return toSupplier(memoryMetrics);
    }

    public Ref<BigDecimal> getVoltageVolt() {
        return oem.rackScaleOem.voltageVolt;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("VoltageVolt")
            @AsUnassigned(WHEN_NULL)
            private Ref<BigDecimal> voltageVolt = unassigned();
        }
    }
}
