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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson.ExtendedInfoJson;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "portId", "status", "linkType",
        "operationalState", "administrativeState", "linkSpeedMbps", "neighborInfo", "neighborMac",
        "frameSize", "autosense", "fullDuplex", "macAddress", "ipv4Addresses", "ipv6Addresses",
        "portClass", "portMode", "portType", "oem", "vlans", "links"
})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:VisibilityModifier"})
public class EthernetSwitchPortJson extends BaseResourceJson {
    public String portId;
    public Status status;
    public LinkType linkType;
    public OperationalState operationalState;
    public AdministrativeState administrativeState;
    public Integer linkSpeedMbps;
    public NeighborInfo neighborInfo;
    @JsonProperty("NeighborMAC")
    public MacAddress neighborMac;
    public Integer frameSize;
    public Boolean autosense;
    public Boolean fullDuplex;
    @JsonProperty("MACAddress")
    public MacAddress macAddress;
    @JsonProperty("IPv4Addresses")
    public List<EthernetInterfaceJson.Ipv4AddressJson> ipv4Addresses = new ArrayList<>();
    @JsonProperty("IPv6Addresses")
    public List<EthernetInterfaceJson.Ipv6AddressJson> ipv6Addresses = new ArrayList<>();
    public PortClass portClass;
    public PortMode portMode;
    public PortType portType;
    @JsonProperty("VLANs")
    public ODataId vlans;
    @JsonProperty("Links")
    public Links links = new Links();

    public EthernetSwitchPortJson() {
        super("#EthernetSwitchPort.v1_0_0.EthernetSwitchPort");
    }

    @JsonPropertyOrder({"PrimaryVLAN", "Switch", "memberOfPort", "portMembers"})
    public class Links extends RedfishLinksJson {
        @JsonProperty("PrimaryVLAN")
        public ODataId primaryVlan;
        @JsonProperty("Switch")
        public ODataId switchLink;
        public ODataId memberOfPort;
        public Set<ODataId> portMembers = new HashSet<>();
        public Oem oem = new Oem();

        @JsonInclude(NON_NULL)
        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemJson {
            @JsonProperty("Intel_RackScale")
            public RackScaleOem rackScaleOem;

            public RackScaleOem createNewRackScaleOem() {
                return new RackScaleOem();
            }

            @JsonInclude(NON_NULL)
            @JsonPropertyOrder({"odataType", "neighborInterface", "neighborInterfaceExtendedInfo"})
            public class RackScaleOem {
                @JsonProperty("@odata.type")
                public String odataType = "#Intel.Oem.EthernetSwitchPort";

                @JsonProperty("NeighborInterface")
                public ODataId neighborInterface;

                @JsonProperty("NeighborInterface@Message.ExtendedInfo")
                public List<ExtendedInfoJson> neighborInterfaceExtendedInfo;
            }
        }
    }
}
