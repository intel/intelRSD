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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Status;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "assetTag", "manufacturer",
    "model", "sku", "serialNumber", "partNumber", "deviceType", "firmwareVersion", "status",
    "links", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class PcieDeviceJson extends BaseResourceJson {
    public String assetTag;
    public String manufacturer;
    public String model;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public DeviceType deviceType;
    public String firmwareVersion;
    public Status status;
    public Links links = new Links();

    public PcieDeviceJson() {
        super("#PCIeDevice.v1_0_0.PCIeDevice");
    }

    public class Links extends RedfishLinksJson {
        public Set<ODataId> chassis = new HashSet<>();
        @JsonProperty("PCIeFunctions")
        public Set<ODataId> pcieFunctions = new HashSet<>();
    }
}
