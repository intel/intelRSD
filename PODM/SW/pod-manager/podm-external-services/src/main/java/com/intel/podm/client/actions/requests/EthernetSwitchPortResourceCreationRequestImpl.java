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

package com.intel.podm.client.actions.requests;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceCreationRequest;
import com.intel.podm.common.types.PortMode;

import java.net.URI;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;

@JsonPropertyOrder({"Name", "PortId", "PortMode", "Links"})
@JsonInclude(NON_DEFAULT)
public class EthernetSwitchPortResourceCreationRequestImpl implements EthernetSwitchPortResourceCreationRequest {
    @JsonProperty("Name")
    private String name;

    @JsonProperty("PortId")
    private String portId;

    @JsonProperty("PortMode")
    private PortMode portMode;

    @JsonProperty("Links")
    private EthernetSwitchPortLinks links;

    @Override
    public void setName(String name) {
        this.name = name;
    }

    @Override
    public void setPortId(String portId) {
        this.portId = portId;
    }

    @Override
    public void setPortMode(PortMode portMode) {
        this.portMode = portMode;
    }

    @Override
    public void setPortMembers(Set<URI> uris) {
        EthernetSwitchPortLinks ethernetSwitchPortLinks = new EthernetSwitchPortLinks();
        ethernetSwitchPortLinks.setRequestedPortMembers(uris);

        if (ethernetSwitchPortLinks.getPortMembers() != null) {
            links = ethernetSwitchPortLinks;
        }
    }
}
