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
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;

@OdataTypes("#VLanNetworkInterface.1.0.0.VLanNetworkInterface")
public class EthernetSwitchPortVlanResourceImpl extends ExternalServiceResourceImpl implements EthernetSwitchPortVlanResource {
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("VLANEnable")
    private Boolean vlanEnable;
    @JsonProperty("VLANId")
    private Integer vlanId;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    @Override
    public Integer getVlanId() {
        return vlanId;
    }

    @Override
    public Boolean getTagged() {
        return oem.rackScaleOem.tagged;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonIgnoreProperties(ignoreUnknown = true)
        private static class RackScaleOem {
            @JsonProperty("Tagged")
            private Boolean tagged;
        }
    }
}
