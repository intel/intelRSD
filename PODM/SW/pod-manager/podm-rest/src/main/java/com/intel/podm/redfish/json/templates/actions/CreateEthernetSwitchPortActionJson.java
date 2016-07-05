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
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortCreation;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortLinks;
import com.intel.podm.common.types.PortMode;

import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;

@JsonIgnoreProperties(ignoreUnknown = false)
public class CreateEthernetSwitchPortActionJson implements RequestedEthernetSwitchPortCreation {
    @JsonProperty("Name")
    public String name;

    @JsonProperty("PortId")
    public String portId;

    @JsonProperty("PortMode")
    public PortMode portMode;

    private Optional<RequestedEthernetSwitchPortLinks> links = empty();

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getPortId() {
        return portId;
    }

    @Override
    public PortMode getPortMode() {
        return portMode;
    }

    @Override
    public Optional<RequestedEthernetSwitchPortLinks> getLinks() {
        return links;
    }

    @JsonProperty("Links")
    public void setLinks(EthernetSwitchPortLinksJson links) {
        this.links = links != null ? of(links) : null;
    }
}
