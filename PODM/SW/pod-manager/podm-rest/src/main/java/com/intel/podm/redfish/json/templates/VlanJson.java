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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name",
        "description", "status", "vlanEnable", "vlanId", "oem"
})
public class VlanJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public Status status;
    @JsonProperty("VLANEnable")
    public Boolean vlanEnable;
    @JsonProperty("VLANId")
    public Integer vlanId;
    public VlanOem oem = new VlanOem();

    public VlanJson() {
        super("#VLanNetworkInterface.1.0.0.VLanNetworkInterface");
    }

    public static class VlanOem {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "tagged"})
        public static class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType;
            public Boolean tagged;
        }
    }
}
