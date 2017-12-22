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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUriWithPossibleParent;

public class EthernetSwitchPortLinksJson implements RedfishEthernetSwitchPort.Links {

    private Set<Context> portMembers;

    private Context primaryVlan;

    @JsonProperty("PrimaryVLAN")
    public void setPrimaryVlan(ODataId primaryVlanOdataId) {
        if (primaryVlanOdataId == null) {
            return;
        }

        primaryVlan = getContextFromUriWithPossibleParent(primaryVlanOdataId.toUri(), ETHERNET_SWITCH, ETHERNET_SWITCH_PORT_VLAN);
    }

    @Override
    public Context getPrimaryVlan() {
        return primaryVlan;
    }

    @JsonProperty("PortMembers")
    public void setPortMembers(Set<ODataId> portMembersOdataIds) {
        if (portMembersOdataIds == null) {
            return;
        }
        Set<Context> contextList = new HashSet<>();
        for (ODataId portMember : portMembersOdataIds) {
            if (portMember == null) {
                throw new RuntimeException("Port member is not set");
            }

            Context switchContext = getContextFromUri(portMember.toUri(), ETHERNET_SWITCH_PORT);
            contextList.add(switchContext);
        }
        this.portMembers = contextList;
    }

    @Override
    public Set<Context> getPortMembers() {
        return portMembers;
    }
}
