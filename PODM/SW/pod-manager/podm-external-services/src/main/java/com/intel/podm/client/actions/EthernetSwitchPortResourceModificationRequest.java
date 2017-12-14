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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.AdministrativeState;

import java.net.URI;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;

@JsonPropertyOrder({"AdministrativeState", "LinkSpeedMbps", "FrameSize", "Autosense", "Links"})
@JsonInclude(NON_DEFAULT)
public class EthernetSwitchPortResourceModificationRequest {
    @JsonProperty("AdministrativeState")
    private AdministrativeState administrativeState;

    @JsonProperty("LinkSpeedMbps")
    private Integer linkSpeed;

    @JsonProperty("FrameSize")
    private Integer frameSize;

    @JsonProperty("Autosense")
    private Boolean autosense;

    @JsonProperty("Links")
    private EthernetSwitchPortLinks links;

    public void setAdministrativeState(AdministrativeState administrativeState) {
        this.administrativeState = administrativeState;
    }

    public void setLinkSpeed(Integer linkSpeed) {
        this.linkSpeed = linkSpeed;
    }

    public void setFrameSize(Integer frameSize) {
        this.frameSize = frameSize;
    }

    public void setAutosense(Boolean autosense) {
        this.autosense = autosense;
    }

    public void setLinks(Set<URI> uris, URI primaryVlanUri) {
        EthernetSwitchPortLinks portLinks = new EthernetSwitchPortLinks();
        portLinks.setRequestedPortMembers(uris);
        portLinks.setPrimaryVlan(primaryVlanUri);

        if (portLinks.getPortMembers() != null || portLinks.getPrimaryVlan() != null) {
            links = portLinks;
        }
    }
}
