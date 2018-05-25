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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.DcbxState;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import java.util.List;

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class UpdateEthernetSwitchPortActionJson implements RedfishEthernetSwitchPort {
    @JsonProperty("AdministrativeState")
    public AdministrativeState administrativeState;

    @JsonProperty("LinkSpeedMbps")
    public Integer linkSpeedMbps;

    @JsonProperty("FrameSize")
    public Integer frameSize;

    @JsonProperty("Autosense")
    public Boolean autosense;

    @JsonProperty("DCBXState")
    public DcbxState dcbxState;

    @JsonProperty("LLDPEnabled")
    public Boolean lldpEnabled;

    @JsonProperty("PriorityFlowControl")
    public PriorityFlowControl priorityFlowControl;

    @JsonProperty("Links")
    public EthernetSwitchPortLinksJson links = new EthernetSwitchPortLinksJson();

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
    public DcbxState getDcbxState() {
        return dcbxState;
    }

    @Override
    public Boolean getLldpEnabled() {
        return lldpEnabled;
    }

    @Override
    public PriorityFlowControl getPriorityFlowControl() {
        return priorityFlowControl;
    }

    @Override
    public RedfishEthernetSwitchPort.Links getLinks() {
        return links;
    }

    public static class PriorityFlowControl implements RedfishEthernetSwitchPort.PriorityFlowControl {
        @JsonProperty("Enabled")
        public Boolean enabled;

        @JsonProperty("EnabledPriorities")
        public List<Integer> enabledPriorities;

        @Override
        public Boolean getEnabled() {
            return enabled;
        }

        @Override
        public List<Integer> getEnabledPriorities() {
            return enabledPriorities;
        }
    }
}
