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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

@JsonIgnoreProperties(ignoreUnknown = false)
public class UpdateEthernetSwitchPortActionJson implements RedfishEthernetSwitchPort {
    @JsonProperty("AdministrativeState")
    private AdministrativeState administrativeState;

    @JsonProperty("LinkSpeedMbps")
    private Integer linkSpeedMbps;

    @JsonProperty("FrameSize")
    private Integer frameSize;

    @JsonProperty("Autosense")
    private Boolean autosense;

    @JsonProperty("Links")
    private EthernetSwitchPortLinksJson links = new EthernetSwitchPortLinksJson();

    @Override
    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    @Override
    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    @Override
    public Integer getFrameSize() {
        return frameSize;
    }

    @Override
    public Boolean getAutosense() {
        return autosense;
    }

    @Override
    public RedfishEthernetSwitchPort.Links getLinks() {
        return links;
    }
}
