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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.redfish.json.templates.attributes.MemoryLocationJson;
import com.intel.podm.redfish.json.templates.attributes.MemoryRegionJson;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.ALWAYS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "name", "id", "description", "memoryType", "memoryDeviceType", "baseModuleType",
    "memoryMedia", "capacityMib", "dataWidthBits", "busWidthBits", "manufacturer", "serialNumber", "partNumber",
    "allowedSpeedsMhz", "firmwareRevision", "firmwareApiVersion", "functionClasses", "vendorId", "deviceId",
    "rankCount", "deviceLocator", "memoryLocation", "errorCorrection", "status", "operatingSpeedMhz", "regions",
    "operatingMemoryModes", "metrics", "oem"})
@JsonInclude(ALWAYS)
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class MemoryJson extends BaseResourceJson {
    @JsonProperty("MemoryType")
    public MemoryType memoryType;
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
    public MemoryLocationJson memoryLocation = new MemoryLocationJson();
    public ErrorCorrection errorCorrection;
    public Status status;
    public Integer operatingSpeedMhz;
    public List<MemoryRegionJson> regions = new ArrayList<>();
    public List<OperatingMemoryMode> operatingMemoryModes = new ArrayList<>();
    public Oem oem = new Oem();
    public Actions actions = new Actions();

    public MemoryJson() {
        super("#Memory.v1_1_0.Memory");
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"odataType", "voltageVolt"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            public String odataType;
            public BigDecimal voltageVolt;
        }
    }

    public class Actions extends RedfishActionsJson {
    }
}
