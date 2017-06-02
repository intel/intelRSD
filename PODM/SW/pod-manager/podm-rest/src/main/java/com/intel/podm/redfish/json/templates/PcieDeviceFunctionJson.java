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
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.StorageControllerJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.LinkedHashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "functionId", "deviceClass",
    "functionType", "deviceId", "vendorId", "classCode", "revisionId", "subsystemId", "subsystemVendorId", "status",
    "functionalDevice", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class PcieDeviceFunctionJson extends BaseResourceJson {
    public Integer functionId;
    public DeviceClass deviceClass;
    public FunctionType functionType;
    public String deviceId;
    public String vendorId;
    public String classCode;
    public String revisionId;
    public String subsystemId;
    public String subsystemVendorId;
    public Status status;
    public Links links = new Links();

    public PcieDeviceFunctionJson() {
        super("#PCIeFunction.v1_0_0.PCIeFunction");
    }

    @JsonPropertyOrder({"drives", "pcieDevice", "storageControllers", "ethernetInterfaces", "oem"})
    public class Links extends RedfishLinksJson {
        public Set<ODataId> drives = new LinkedHashSet<>();
        @JsonProperty("PCIeDevice")
        public ODataId pcieDevice;
        public Set<StorageControllerJson> storageControllers = new LinkedHashSet<>();
        public Set<ODataId> ethernetInterfaces = new LinkedHashSet<>();
    }
}
