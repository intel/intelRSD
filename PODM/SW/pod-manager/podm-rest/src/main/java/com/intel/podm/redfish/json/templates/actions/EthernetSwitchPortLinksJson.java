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
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortLinks;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.resources.ODataId;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.redfish.UriConverter.getContextFromUri;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@JsonIgnoreProperties(ignoreUnknown = true)
public class EthernetSwitchPortLinksJson implements RequestedEthernetSwitchPortLinks {

    private Optional<List<Context>> portMembers = empty();

    @JsonProperty("PrimaryVLAN")
    private ODataId primaryVlan;

    @JsonProperty("PortMembers")
    public void setPortMembers(List<ODataId> portMembers) {
        if (portMembers == null) {
            this.portMembers = null;
            return;
        }
        List<Context> contextList = new ArrayList<>();
        for (ODataId portMember : portMembers) {
            if (portMember == null) {
                throw new RuntimeException("Port member is not set");
            }
            Context switchContext = getContextFromUri(portMember.toUri(), ETHERNET_SWITCH_PORT);
            contextList.add(switchContext);
        }
        this.portMembers = of(contextList);
    }

    @Override
    public Context getPrimaryVlan() {
        if (primaryVlan == null) {
            return null;
        }
        return getContextFromUri(primaryVlan.toUri(), ETHERNET_SWITCH_PORT_VLAN);
    }

    @Override
    public Optional<List<Context>> getPortMembers() {
        return portMembers;
    }
}
