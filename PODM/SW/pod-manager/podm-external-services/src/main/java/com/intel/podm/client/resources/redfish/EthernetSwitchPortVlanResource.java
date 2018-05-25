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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#VLanNetworkInterface" + VERSION_PATTERN + "VLanNetworkInterface"
})
public class EthernetSwitchPortVlanResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("VLANEnable")
    private Boolean vlanEnable;
    @JsonProperty("VLANId")
    private Integer vlanId;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public Boolean getTagged() {
        return oem.rackScaleOem.tagged;
    }

    public Status getStatus() {
        return oem.rackScaleOem.status;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Tagged")
            private Boolean tagged;
            @JsonProperty("Status")
            private Status status;
        }
    }
}
