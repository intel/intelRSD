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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.MemoryLocationJson;
import com.intel.podm.redfish.json.templates.attributes.MemoryOemJson;
import com.intel.podm.redfish.json.templates.attributes.MemoryRegionJson;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.ALWAYS;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "name", "id", "description", "memoryType", "memoryDeviceType", "baseModuleType",
        "memoryMedia", "capacityMib", "dataWidthBits", "busWidthBits", "manufacturer", "serialNumber", "partNumber",
        "allowedSpeedsMhz", "firmwareRevision", "firmwareApiVersion", "functionClasses", "vendorId", "deviceId",
        "rankCount", "deviceLocator", "memoryLocation", "errorCorrection", "status", "operatingSpeedMhz", "regions",
        "operatingMemoryModes", "metrics", "oem"})
@JsonInclude(ALWAYS)
public class MemoryJson extends BaseJson {
    public String name;
    public String id;
    @JsonProperty("MemoryType")
    public MemoryType memoryType;
    public String description;
    public MemoryDeviceType memoryDeviceType;
    public BaseModuleType baseModuleType;
    public List<MemoryMedia> memoryMedia = new ArrayList<>();
    @JsonProperty("CapacityMiB")
    public Integer capacityMib;
    public Integer dataWidthBits;
    public Integer busWidthBits;
    public String manufacturer;
    public String serialNumber;
    public String partNumber;
    @JsonProperty("AllowedSpeedsMHz")
    public Collection<Integer> allowedSpeedsMhz = new ArrayList<>();
    public String firmwareRevision;
    public String firmwareApiVersion;
    public Collection<String> functionClasses = new ArrayList<>();
    @JsonProperty("VendorID")
    public String vendorId;
    @JsonProperty("DeviceID")
    public String deviceId;
    public Integer rankCount;
    public String deviceLocator;
    public MemoryLocationJson memoryLocation;
    public ErrorCorrection errorCorrection;
    public Status status;
    @JsonProperty("OperatingSpeedMhz")
    public Integer operatingSpeedMhz;
    public List<MemoryRegionJson> regions = new ArrayList<>();
    public List<OperatingMemoryMode> operatingMemoryModes = new ArrayList<>();
    public MemoryOemJson oem = new MemoryOemJson();

    public MemoryJson() {
        super("#Memory.1.0.0.Memory");
    }
}
