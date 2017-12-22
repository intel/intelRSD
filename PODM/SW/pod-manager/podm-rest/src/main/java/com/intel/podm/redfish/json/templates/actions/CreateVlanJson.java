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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class CreateVlanJson implements RedfishVlanNetworkInterface {
    @JsonProperty("VLANId")
    public Integer vlanId;

    @JsonProperty("VLANEnable")
    public Boolean vlanEnable;

    @JsonProperty("Oem")
    public Oem oem;

    @Override
    public Integer getVlanId() {
        return vlanId;
    }

    @Override
    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    @Override
    public Boolean getTagged() {
        if (oem == null) {
            return null;
        }

        if (oem.intelRackScale == null) {
            return null;
        }

        return oem.intelRackScale.tagged;
    }

    public static class Oem {
        @JsonProperty("Intel_RackScale")
        public IntelRackScale intelRackScale;

        public static class IntelRackScale {
            @JsonProperty("Tagged")
            public Boolean tagged;
        }
    }
}
