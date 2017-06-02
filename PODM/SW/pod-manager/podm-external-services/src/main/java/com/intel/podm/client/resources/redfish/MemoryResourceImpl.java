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
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.MemoryLocationObject;
import com.intel.podm.client.api.resources.redfish.MemoryRegionObject;
import com.intel.podm.client.api.resources.redfish.MemoryResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
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

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#Memory" + OdataTypes.VERSION_PATTERN + "Memory"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class MemoryResourceImpl extends ExternalServiceResourceImpl implements MemoryResource {
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
    private Ref<List<MemoryRegionObjectImpl>> regions = unassigned();

    @JsonProperty("OperatingMemoryModes")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<OperatingMemoryMode>> operatingMemoryModes = unassigned();

    @JsonProperty("MemoryLocation")
    private MemoryLocationObjectImpl memoryLocation;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public Ref<MemoryType> getMemoryType() {
        return memoryType;
    }

    @Override
    public Ref<MemoryDeviceType> getMemoryDeviceType() {
        return memoryDeviceType;
    }

    @Override
    public Ref<BaseModuleType> getBaseModuleType() {
        return baseModuleType;
    }

    @Override
    public Ref<List<MemoryMedia>> getMemoryMedia() {
        return memoryMedia;
    }

    @Override
    public Ref<Integer> getCapacityMib() {
        return capacityMib;
    }

    @Override
    public Ref<Integer> getDataWidthBits() {
        return dataWidthBits;
    }

    @Override
    public Ref<Integer> getBusWidthBits() {
        return busWidthBits;
    }

    @Override
    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    @Override
    public Ref<String> getSerialNumber() {
        return serialNumber;
    }

    @Override
    public Ref<String> getPartNumber() {
        return partNumber;
    }

    @Override
    public Ref<List<Integer>> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    @Override
    public MemoryLocationObject getMemoryLocation() {
        return memoryLocation;
    }

    @Override
    public Ref<String> getFirmwareRevision() {
        return firmwareRevision;
    }

    @Override
    public Ref<String> getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    @Override
    public Ref<List<String>> getFunctionClasses() {
        return functionClasses;
    }

    @Override
    public Ref<String> getVendorId() {
        return vendorId;
    }

    @Override
    public Ref<String> getDeviceId() {
        return deviceId;
    }

    @Override
    public Ref<Integer> getRankCount() {
        return rankCount;
    }

    @Override
    public Ref<String> getDeviceLocator() {
        return deviceLocator;
    }

    @Override
    public Ref<ErrorCorrection> getErrorCorrection() {
        return errorCorrection;
    }

    @Override
    public Ref<Status> getStatus() {
        return status;
    }

    @Override
    public Ref<Integer> getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    @Override
    public Ref<List<MemoryRegionObject>> getRegions() {
        return (Ref) regions;
    }

    @Override
    public Ref<List<OperatingMemoryMode>> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @Override
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
