/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonGetter;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.IpAddresses.IpV4AddressDto;
import com.intel.podm.business.dto.IpAddresses.IpV6AddressDto;
import com.intel.podm.business.dto.IpAddresses.IpV6AddressPolicyDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.LinkStatus;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;

import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.Collections.nullOrEmpty;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "linkStatus", "interfaceEnabled", "permanentMacAddress",
    "macAddress", "speedMbps", "autoNeg", "fullDuplex", "mtuSize", "hostName", "fqdn", "vlans", "ipV4Addresses", "ipV6AddressPolicyTable",
    "ipV6StaticAddresses", "maxIPv6StaticAddresses", "ipV6DefaultGateway", "ipV6Addresses", "nameServers", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class EthernetInterfaceDto extends RedfishDto {
    private final Links links = new Links();

    private Status status;
    private LinkStatus linkStatus;
    private Boolean interfaceEnabled;
    @JsonProperty("PermanentMACAddress")
    private MacAddress permanentMacAddress;
    @JsonProperty("MACAddress")
    private MacAddress macAddress;
    private Integer speedMbps;
    private Boolean autoNeg;
    private Boolean fullDuplex;
    @JsonProperty("MTUSize")
    private Integer mtuSize;
    @JsonProperty("FQDN")
    private String fqdn;
    private String hostName;
    private List<String> nameServers = new ArrayList<>();
    @JsonProperty("VLANs")
    private SingletonContext vlans;
    @JsonIgnore
    private Boolean vlanEnable;
    @JsonIgnore
    private Integer vlanId;
    @JsonProperty("IPv4Addresses")
    private List<IpV4AddressDto> ipV4Addresses = new ArrayList<>();
    private Integer maxIPv6StaticAddresses;
    @JsonProperty("IPv6DefaultGateway")
    private String ipV6DefaultGateway;
    @JsonProperty("IPv6Addresses")
    private List<IpV6AddressDto> ipV6Addresses = new ArrayList<>();
    @JsonProperty("IPv6StaticAddresses")
    private List<IpV6AddressDto> ipV6StaticAddresses = new ArrayList<>();
    @JsonProperty("IPv6AddressPolicyTable")
    private List<IpV6AddressPolicyDto> ipV6AddressPolicyTable = new ArrayList<>();
    private final Oem oem = new Oem();

    public EthernetInterfaceDto() {
        super("#EthernetInterface.v1_1_0.EthernetInterface");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public LinkStatus getLinkStatus() {
        return linkStatus;
    }

    public void setLinkStatus(LinkStatus linkStatus) {
        this.linkStatus = linkStatus;
    }

    public Boolean getInterfaceEnabled() {
        return interfaceEnabled;
    }

    public void setInterfaceEnabled(Boolean interfaceEnabled) {
        this.interfaceEnabled = interfaceEnabled;
    }

    public MacAddress getPermanentMacAddress() {
        return permanentMacAddress;
    }

    public void setPermanentMacAddress(MacAddress permanentMacAddress) {
        this.permanentMacAddress = permanentMacAddress;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(MacAddress macAddress) {
        this.macAddress = macAddress;
    }

    public Integer getSpeedMbps() {
        return speedMbps;
    }

    public void setSpeedMbps(Integer speedMbps) {
        this.speedMbps = speedMbps;
    }

    public Boolean getAutoNeg() {
        return autoNeg;
    }

    public void setAutoNeg(Boolean autoNeg) {
        this.autoNeg = autoNeg;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public void setFullDuplex(Boolean fullDuplex) {
        this.fullDuplex = fullDuplex;
    }

    public Integer getMtuSize() {
        return mtuSize;
    }

    public void setMtuSize(Integer mtuSize) {
        this.mtuSize = mtuSize;
    }

    public String getFqdn() {
        return fqdn;
    }

    public void setFqdn(String fqdn) {
        this.fqdn = fqdn;
    }

    public String getHostName() {
        return hostName;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public List<String> getNameServers() {
        return nameServers;
    }

    public void setNameServers(List<String> nameServers) {
        this.nameServers = nameServers;
    }

    public SingletonContext getVlans() {
        return vlans;
    }

    public void setVlans(SingletonContext vlans) {
        this.vlans = vlans;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public void setVlanEnable(Boolean vlanEnable) {
        this.vlanEnable = vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public void setVlanId(Integer vlanId) {
        this.vlanId = vlanId;
    }

    public List<IpV4AddressDto> getIpV4Addresses() {
        return ipV4Addresses;
    }

    public void setIpV4Addresses(List<IpV4AddressDto> ipV4Addresses) {
        this.ipV4Addresses = ipV4Addresses;
    }

    public Integer getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    public void setMaxIPv6StaticAddresses(Integer maxIPv6StaticAddresses) {
        this.maxIPv6StaticAddresses = maxIPv6StaticAddresses;
    }

    public String getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    public void setIpV6DefaultGateway(String ipV6DefaultGateway) {
        this.ipV6DefaultGateway = ipV6DefaultGateway;
    }

    public List<IpV6AddressDto> getIpV6Addresses() {
        return ipV6Addresses;
    }

    public void setIpV6Addresses(List<IpV6AddressDto> ipV6Addresses) {
        this.ipV6Addresses = ipV6Addresses;
    }

    public List<IpV6AddressDto> getIpV6StaticAddresses() {
        return ipV6StaticAddresses;
    }

    public void setIpV6StaticAddresses(List<IpV6AddressDto> ipV6StaticAddresses) {
        this.ipV6StaticAddresses = ipV6StaticAddresses;
    }

    public List<IpV6AddressPolicyDto> getIpV6AddressPolicyTable() {
        return ipV6AddressPolicyTable;
    }

    public void setIpV6AddressPolicyTable(List<IpV6AddressPolicyDto> ipV6AddressPolicyTable) {
        this.ipV6AddressPolicyTable = ipV6AddressPolicyTable;
    }

    public Links getLinks() {
        return links;
    }

    public Oem getOem() {
        return oem;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        public class RackScaleOem {
            private List<Protocol> supportedProtocols = new ArrayList<>();

            public List<Protocol> getSupportedProtocols() {
                return supportedProtocols;
            }
            public void setSupportedProtocols(List<Protocol> supportedProtocols) {
                this.supportedProtocols = supportedProtocols;
            }
        }
    }

    @JsonInclude(NON_NULL)
    public final class Links extends RedfishLinksDto {
        private final Oem oem = new Oem();

        public EthernetInterfaceDto.Links.Oem getOem() {
            return oem;
        }

        @JsonInclude(NON_NULL)
        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOemDto {
            private EthernetInterfaceDto.Links.Oem.RackScaleOem rackScaleOem = new EthernetInterfaceDto.Links.Oem.RackScaleOem();

            @JsonIgnore
            public EthernetInterfaceDto.Links.Oem.RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonGetter("Intel_RackScale")
            public EthernetInterfaceDto.Links.Oem.RackScaleOem getRackScaleOemJson() {
                if (rackScaleOem.getNeighborPort() == null && nullOrEmpty(rackScaleOem.getNeighborPortExtendedInfo())) {
                    return null;
                }
                return rackScaleOem;
            }

            @JsonInclude(NON_NULL)
            @JsonPropertyOrder({"@odata.type", "neighborPort", "neighborPortExtendedInfo"})
            public final class RackScaleOem {
                @JsonProperty("@odata.type")
                private final String odataType = "#Intel.Oem.EthernetInterfaceLinks";
                private Context neighborPort;

                @JsonProperty("NeighborPort@Message.ExtendedInfo")
                private List<ExtendedInfo> neighborPortExtendedInfo;

                public String getOdataType() {
                    return odataType;
                }

                public Context getNeighborPort() {
                    return neighborPort;
                }

                public void setNeighborPort(Context neighborPort) {
                    this.neighborPort = neighborPort;
                }

                public List<ExtendedInfo> getNeighborPortExtendedInfo() {
                    return neighborPortExtendedInfo;
                }

                public void setNeighborPortExtendedInfo(List<ExtendedInfo> neighborPortExtendedInfo) {
                    this.neighborPortExtendedInfo = neighborPortExtendedInfo;
                }
            }
        }
    }
}
