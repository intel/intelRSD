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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ExtendedInfoDto;
import com.intel.podm.business.dto.redfish.attributes.IpV4AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressPolicyDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public final class EthernetInterfaceDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final Boolean interfaceEnabled;
    private final MacAddress permanentMacAddress;
    private final MacAddress macAddress;
    private final Integer speedMbps;
    private final Boolean autoNeg;
    private final Boolean fullDuplex;
    private final Integer mtuSize;
    private final String fqdn;
    private final String hostname;
    private final List<String> nameServers;
    private final Boolean vlanEnable;
    private final Integer vlanId;
    private final List<IpV4AddressDto> ipv4Addresses;
    private final Integer maxIPv6StaticAddresses;
    private final String ipv6DefaultGateway;
    private final List<IpV6AddressDto> ipv6Addresses;
    private final List<IpV6AddressDto> ipv6StaticAddresses;
    private final List<IpV6AddressPolicyDto> ipV6AddressesPolicyTable;
    private final Links links;

    private EthernetInterfaceDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        interfaceEnabled = builder.interfaceEnabled;
        permanentMacAddress = builder.permanentMacAddress;
        macAddress = builder.macAddress;
        speedMbps = builder.speedMbps;
        autoNeg = builder.autoNeg;
        fullDuplex = builder.fullDuplex;
        mtuSize = builder.mtuSize;
        fqdn = builder.fqdn;
        hostname = builder.hostname;
        nameServers = builder.nameServers;
        vlanEnable = builder.vlanEnable;
        vlanId = builder.vlanId;
        ipv4Addresses = builder.ipv4Addresses;
        maxIPv6StaticAddresses = builder.maxIPv6StaticAddresses;
        ipv6DefaultGateway = builder.ipv6DefaultGateway;
        ipv6Addresses = builder.ipv6Addresses;
        ipv6StaticAddresses = builder.ipv6StaticAddresses;
        ipV6AddressesPolicyTable = builder.ipV6AddressesPolicyTable;
        links = new Links(builder.neighborPort, builder.neighborPortExtendedInfo);
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public Status getStatus() {
        return status;
    }

    public Boolean getInterfaceEnabled() {
        return interfaceEnabled;
    }

    public MacAddress getPermanentMacAddress() {
        return permanentMacAddress;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public Integer getSpeedMbps() {
        return speedMbps;
    }

    public Boolean getAutoNeg() {
        return autoNeg;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public Integer getMtuSize() {
        return mtuSize;
    }

    public String getFqdn() {
        return fqdn;
    }

    public String getHostname() {
        return hostname;
    }

    public List<String> getNameServers() {
        return nameServers;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public List<IpV4AddressDto> getIpv4Addresses() {
        return ipv4Addresses;
    }

    public Integer getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    public String getIpv6DefaultGateway() {
        return ipv6DefaultGateway;
    }

    public List<IpV6AddressDto> getIpv6Addresses() {
        return ipv6Addresses;
    }

    public List<IpV6AddressDto> getIpv6StaticAddresses() {
        return ipv6StaticAddresses;
    }

    public List<IpV6AddressPolicyDto> getIpV6AddressesPolicyTable() {
        return ipV6AddressesPolicyTable;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private Context neighborPort;
        private List<ExtendedInfoDto> neighborPortExtendedInfo;

        public Links(Context neighborPort, List<ExtendedInfoDto> neighborPortExtendedInfo) {
            this.neighborPort = neighborPort;
            this.neighborPortExtendedInfo = neighborPortExtendedInfo;
        }

        public Context getNeighborPort() {
            return neighborPort;
        }

        public List<ExtendedInfoDto> getNeighborPortExtendedInfo() {
            return neighborPortExtendedInfo;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private Boolean interfaceEnabled;
        private MacAddress permanentMacAddress;
        private MacAddress macAddress;
        private Integer speedMbps;
        private Boolean autoNeg;
        private Boolean fullDuplex;
        private Integer mtuSize;
        private String fqdn;
        private String hostname;
        private List<String> nameServers;
        private Boolean vlanEnable;
        private Integer vlanId;
        private List<IpV4AddressDto> ipv4Addresses;
        private Integer maxIPv6StaticAddresses;
        private String ipv6DefaultGateway;
        private List<IpV6AddressDto> ipv6Addresses;
        private List<IpV6AddressDto> ipv6StaticAddresses;
        private List<IpV6AddressPolicyDto> ipV6AddressesPolicyTable;
        private Context neighborPort;
        private List<ExtendedInfoDto> neighborPortExtendedInfo;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder interfaceEnabled(Boolean interfaceEnabled) {
            this.interfaceEnabled = interfaceEnabled;
            return this;
        }

        public Builder permanentMacAddress(MacAddress permanentMacAddress) {
            this.permanentMacAddress = permanentMacAddress;
            return this;
        }

        public Builder macAddress(MacAddress macAddress) {
            this.macAddress = macAddress;
            return this;
        }

        public Builder speedMbps(Integer speedMbps) {
            this.speedMbps = speedMbps;
            return this;
        }

        public Builder autoNeg(Boolean autoNeg) {
            this.autoNeg = autoNeg;
            return this;
        }

        public Builder fullDuplex(Boolean fullDuplex) {
            this.fullDuplex = fullDuplex;
            return this;
        }

        public Builder mtuSize(Integer mtuSize) {
            this.mtuSize = mtuSize;
            return this;
        }

        public Builder fqdn(String fqdn) {
            this.fqdn = fqdn;
            return this;
        }

        public Builder hostname(String hostname) {
            this.hostname = hostname;
            return this;
        }

        public Builder nameServers(List<String> nameServers) {
            this.nameServers = nameServers;
            return this;
        }

        public Builder vlanEnable(Boolean vlanEnable) {
            this.vlanEnable = vlanEnable;
            return this;
        }

        public Builder vlanId(Integer vlanId) {
            this.vlanId = vlanId;
            return this;
        }

        public Builder ipv4Addresses(List<IpV4AddressDto> ipv4Addresses) {
            this.ipv4Addresses = ipv4Addresses;
            return this;
        }

        public Builder maxIPv6StaticAddresses(Integer maxIPv6StaticAddresses) {
            this.maxIPv6StaticAddresses = maxIPv6StaticAddresses;
            return this;
        }

        public Builder ipv6DefaultGateway(String ipv6DefaultGateway) {
            this.ipv6DefaultGateway = ipv6DefaultGateway;
            return this;
        }

        public Builder ipv6Addresses(List<IpV6AddressDto> ipv6Addresses) {
            this.ipv6Addresses = ipv6Addresses;
            return this;
        }

        public Builder ipv6StaticAddresses(List<IpV6AddressDto> ipv6StaticAddresses) {
            this.ipv6StaticAddresses = ipv6StaticAddresses;
            return this;
        }

        public Builder ipV6AddressesPolicyTable(List<IpV6AddressPolicyDto> ipV6AddressesPolicyTable) {
            this.ipV6AddressesPolicyTable = ipV6AddressesPolicyTable;
            return this;
        }

        public Builder neighborPort(Context neighborPort) {
            this.neighborPort = neighborPort;
            return this;
        }

        public Builder neighborPortExtendedInfo(List<ExtendedInfoDto> neighborPortExtendedInfo) {
            this.neighborPortExtendedInfo = neighborPortExtendedInfo;
            return this;
        }

        public EthernetInterfaceDto build() {
            return new EthernetInterfaceDto(this);
        }
    }
}
