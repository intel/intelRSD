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
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV4AddressOrigin;
import com.intel.podm.common.types.IpV6AddressOrigin;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson.ExtendedInfoJson;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "interfaceEnabled",
    "permanentMacAddress", "macAddress", "speedMbps", "autoNeg", "fullDuplex", "mtuSize", "hostname", "fqdn",
    "vlans", "ipv4Addresses", "ipV6AddressPolicyTable", "ipv6StaticAddresses", "maxIPv6StaticAddresses",
    "ipv6DefaultGateway", "ipv6Addresses", "nameServers", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class EthernetInterfaceJson extends BaseResourceJson {
    public Status status;
    @JsonProperty("InterfaceEnabled")
    public Boolean interfaceEnabled;
    @JsonProperty("PermanentMACAddress")
    public MacAddress permanentMacAddress;
    @JsonProperty("MACAddress")
    public MacAddress macAddress;
    @JsonProperty("SpeedMbps")
    public Integer speedMbps;
    @JsonProperty("AutoNeg")
    public Boolean autoNeg;
    @JsonProperty("FullDuplex")
    public Boolean fullDuplex;
    @JsonProperty("MTUSize")
    public Integer mtuSize;
    @JsonProperty("FQDN")
    public String fqdn;
    @JsonProperty("HostName")
    public String hostname;
    @JsonProperty("IPv6DefaultGateway")
    public String ipv6DefaultGateway;
    @JsonProperty("NameServers")
    public List<String> nameServers;
    @JsonProperty("VLANs")
    public ODataId vlans;
    @JsonProperty("IPv4Addresses")
    public List<Ipv4AddressJson> ipv4Addresses = new ArrayList<>();
    @JsonProperty("IPv6Addresses")
    public List<Ipv6AddressJson> ipv6Addresses = new ArrayList<>();
    @JsonProperty("MaxIPv6StaticAddresses")
    public Integer maxIPv6StaticAddresses;
    @JsonProperty("IPv6StaticAddresses")
    public List<Ipv6AddressJson> ipv6StaticAddresses = new ArrayList<>();
    @JsonProperty("IPv6AddressPolicyTable")
    public final List<IpV6AddressPolicyJson> ipV6AddressPolicyTable = new LinkedList<>();
    @JsonProperty("Links")
    public Links links = new Links();

    public EthernetInterfaceJson() {
        super("#EthernetInterface.v1_1_0.EthernetInterface");
    }

    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "subnetMask", "addressOrigin", "gateway", "oem"})
    public static class Ipv4AddressJson {
        public String address;
        public String subnetMask;
        public IpV4AddressOrigin addressOrigin;
        public String gateway;
        @IgnoreAutomaticOem
        public JsonNode oem;
    }

    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "prefixLength", "addressOrigin", "addressState", "oem"})
    public static class Ipv6AddressJson {
        public String address;
        public Integer prefixLength;
        public IpV6AddressOrigin addressOrigin;
        public AddressState addressState;
        @IgnoreAutomaticOem
        public JsonNode oem;
    }

    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"prefix", "precedence", "label"})
    public static class IpV6AddressPolicyJson {
        public String prefix;
        public Integer precedence;
        public Integer label;
    }

    public class Links extends RedfishLinksJson {
        @JsonProperty("Oem")
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
            @JsonPropertyOrder({"odataType", "neighborPort", "neighborPortExtendedInfo"})
            public class RackScaleOem {
                @JsonProperty("@odata.type")
                public String odataType = "#Intel.Oem.EthernetInterface";

                @JsonProperty("NeighborPort")
                public ODataId neighborPort;

                @JsonProperty("NeighborPort@Message.ExtendedInfo")
                public List<ExtendedInfoJson> neighborPortExtendedInfo;
            }
        }
    }
}
