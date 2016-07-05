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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.rest.odataid.ODataId;

import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "portId", "status", "linkType",
        "operationalState", "administrativeState", "linkSpeedMbps", "neighborInfo", "neighborMac",
        "frameSize", "autosense", "fullDuplex", "macAddress", "ipv4Addresses", "ipv6Addresses",
        "portClass", "portMode", "portType", "oem", "vlans", "links"
})
public class EthernetSwitchPortJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
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
    public List<EthernetInterfaceJson.Ipv4AddressJson> ipv4Addresses = newArrayList();
    @JsonProperty("IPv6Addresses")
    public List<EthernetInterfaceJson.Ipv6AddressJson> ipv6Addresses = newArrayList();
    public PortClass portClass;
    public PortMode portMode;
    public PortType portType;
    @JsonProperty("VLANs")
    public ODataId vlans;
    @JsonProperty("Links")
    public Links links = new Links();
    @JsonProperty("Oem")
    public final Object oem = new Object();

    public EthernetSwitchPortJson() {
        super("#EthernetSwitchPort.1.0.0.EthernetSwitchPort");
    }

    @JsonPropertyOrder({"PrimaryVLAN", "Switch", "memberOfPort", "portMembers"})
    public static class Links {
        @JsonProperty("PrimaryVLAN")
        public ODataId primaryVlan;
        @JsonProperty("Switch")
        public ODataId switchLink;
        public ODataId memberOfPort;
        public List<ODataId> portMembers = newArrayList();
        public Oem oem = new Oem();
    }

    @JsonInclude(NON_NULL)
    public static class Oem {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem;

        public static class RackScaleOem {
            @JsonProperty("@odata.type")
            public String odataType;

            @JsonProperty("NeighborInterface")
            public ODataId neighborInterface;

            public RackScaleOem(ODataId neighborInterface) {
                this.odataType = "#Intel.Oem.EthernetSwitchPort";
                this.neighborInterface = neighborInterface;
            }
        }
    }
}
