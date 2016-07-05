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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortLinks;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortModification;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.AdministrativeState;

import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;

@JsonIgnoreProperties(ignoreUnknown = false)
public class UpdateEthernetSwitchPortActionJson implements RequestedEthernetSwitchPortModification {
    @JsonProperty("AdministrativeState")
    public AdministrativeState administrativeState;

    @JsonProperty("LinkSpeedMbps")
    public Integer linkSpeed;

    @JsonProperty("FrameSize")
    public Integer frameSize;

    @JsonProperty("Autosense")
    public Boolean autosense;

    @JsonProperty("Links")
    public Links links = new Links();

    @Override
    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    @Override
    public Integer getLinkSpeed() {
        return linkSpeed;
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
    public Context getPrimaryVlan() {
        return links.primaryVlan;
    }

    @Override
    public Optional<RequestedEthernetSwitchPortLinks> getLinks() {
        return links.portMembers;
    }

    @JsonProperty("Links")
    public void setLinks(EthernetSwitchPortLinksJson portMemberLinks) {
        if (portMemberLinks != null) {
            links.portMembers = of(portMemberLinks);
            links.primaryVlan = portMemberLinks.getPrimaryVlan();
        } else {
            links.portMembers = null;
            links.primaryVlan = null;
        }
    }

    private static final class Links {
        @JsonProperty("PrimaryVLAN")
        public Context primaryVlan;

        @JsonUnwrapped
        public Optional<RequestedEthernetSwitchPortLinks> portMembers = empty();
    }
}
