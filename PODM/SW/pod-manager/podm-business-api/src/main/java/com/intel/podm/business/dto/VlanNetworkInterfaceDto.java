/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "vlanEnable", "vlanId", "oem"})
public final class VlanNetworkInterfaceDto extends RedfishDto {
    private final Oem oem = new Oem();
    @JsonProperty("VLANEnable")
    private Boolean vlanEnable;
    @JsonProperty("VLANId")
    private Integer vlanId;

    public VlanNetworkInterfaceDto() {
        super("#VLanNetworkInterface.v1_0_1.VLanNetworkInterface");
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public void setVlanEnable(Boolean vlanEnable) {
        this.vlanEnable = vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public void setVlanId(Integer vlanId) {
        this.vlanId = vlanId;
    }

    public Oem getOem() {
        return oem;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "tagged", "status"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.VLanNetworkInterface";
            private Boolean tagged;
            private Status status;

            public String getoDataType() {
                return oDataType;
            }

            public Boolean getTagged() {
                return tagged;
            }

            public void setTagged(Boolean tagged) {
                this.tagged = tagged;
            }

            public Status getStatus() {
                return status;
            }

            public void setStatus(Status status) {
                this.status = status;
            }
        }
    }
}
