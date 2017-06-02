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
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class EthernetSwitchPortDto extends BaseDto implements RedfishEthernetSwitchPort {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String portId;
    private final Status status;
    private final LinkType linkType;
    private final OperationalState operationalState;
    private final AdministrativeState administrativeState;
    private final Integer linkSpeedMbps;
    private final NeighborInfo neighborInfo;
    private final MacAddress neighborMac;
    private final Integer frameSize;
    private final Boolean autosense;
    private final Boolean fullDuplex;
    private final MacAddress macAddress;
    private final List<IpV4AddressDto> ipv4Addresses;
    private final List<IpV6AddressDto> ipv6Addresses;
    private final PortClass portClass;
    private final PortMode portMode;
    private final PortType portType;
    private final Links links;

    private EthernetSwitchPortDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        portId = builder.portId;
        status = builder.status;
        linkType = builder.linkType;
        operationalState = builder.operationalState;
        administrativeState = builder.administrativeState;
        linkSpeedMbps = builder.linkSpeedMbps;
        neighborInfo = builder.neighborInfo;
        neighborMac = builder.neighborMac;
        frameSize = builder.frameSize;
        autosense = builder.autosense;
        fullDuplex = builder.fullDuplex;
        macAddress = builder.macAddress;
        ipv4Addresses = builder.ipv4Addresses;
        ipv6Addresses = builder.ipv6Addresses;
        portClass = builder.portClass;
        portMode = builder.portMode;
        portType = builder.portType;
        links = Links.newBuilder()
                .primaryVlan(builder.primaryVlan)
                .switchContext(builder.switchContext)
                .memberOfPort(builder.memberOfPort)
                .portMembers(builder.portMembers)
                .neighborInterface(builder.neighborInterface)
                .neighborInterfaceExtendedInfo(builder.neighborInterfaceExtendedInfo)
                .build();
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

    @Override
    public String getPortId() {
        return portId;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public LinkType getLinkType() {
        return linkType;
    }

    @Override
    public OperationalState getOperationalState() {
        return operationalState;
    }

    @Override
    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    @Override
    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    @Override
    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    @Override
    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    @Override
    public Integer getFrameSize() {
        return frameSize;
    }

    @Override
    public Boolean getAutosense() {
        return autosense;
    }

    @Override
    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    @Override
    public MacAddress getMacAddress() {
        return macAddress;
    }

    public List<IpV4AddressDto> getIpv4Addresses() {
        return ipv4Addresses;
    }

    public List<IpV6AddressDto> getIpv6Addresses() {
        return ipv6Addresses;
    }

    @Override
    public PortClass getPortClass() {
        return portClass;
    }

    @Override
    public PortMode getPortMode() {
        return portMode;
    }

    @Override
    public PortType getPortType() {
        return portType;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishEthernetSwitchPort.Links {
        private final Context primaryVlan;
        private final Context switchContext;
        private final Context memberOfPort;
        private final Set<Context> portMembers;
        private final Context neighborInterface;
        private final List<ExtendedInfoDto> neighborInterfaceExtendedInfo;

        private Links(Builder builder) {
            primaryVlan = builder.primaryVlan;
            switchContext = builder.switchContext;
            memberOfPort = builder.memberOfPort;
            portMembers = builder.portMembers;
            neighborInterface = builder.neighborInterface;
            neighborInterfaceExtendedInfo = builder.neighborInterfaceExtendedInfo;
        }

        public static Builder newBuilder() {
            return new Builder();
        }

        @Override
        public Context getPrimaryVlan() {
            return primaryVlan;
        }

        @Override
        public Context getSwitchContext() {
            return switchContext;
        }

        @Override
        public Context getMemberOfPort() {
            return memberOfPort;
        }

        @Override
        public Set<Context> getPortMembers() {
            return portMembers;
        }

        @Override
        public Context getNeighborInterface() {
            return neighborInterface;
        }

        public List<ExtendedInfoDto> getNeighborInterfaceExtendedInfo() {
            return neighborInterfaceExtendedInfo;
        }

        public static final class Builder {
            private Context primaryVlan;
            private Context switchContext;
            private Context memberOfPort;
            private Set<Context> portMembers;
            private Context neighborInterface;
            private List<ExtendedInfoDto> neighborInterfaceExtendedInfo;

            private Builder() {
            }

            public Builder primaryVlan(Context primaryVlan) {
                this.primaryVlan = primaryVlan;
                return this;
            }

            public Builder switchContext(Context switchContext) {
                this.switchContext = switchContext;
                return this;
            }

            public Builder memberOfPort(Context memberOfPort) {
                this.memberOfPort = memberOfPort;
                return this;
            }

            public Builder portMembers(Set<Context> portMembers) {
                this.portMembers = portMembers;
                return this;
            }

            public Builder neighborInterface(Context neighborInterface) {
                this.neighborInterface = neighborInterface;
                return this;
            }

            public Builder neighborInterfaceExtendedInfo(List<ExtendedInfoDto> neighborInterfaceExtendedInfo) {
                this.neighborInterfaceExtendedInfo = neighborInterfaceExtendedInfo;
                return this;
            }

            public Links build() {
                return new Links(this);
            }
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String portId;
        private Status status;
        private LinkType linkType;
        private OperationalState operationalState;
        private AdministrativeState administrativeState;
        private Integer linkSpeedMbps;
        private NeighborInfo neighborInfo;
        private MacAddress neighborMac;
        private Integer frameSize;
        private Boolean autosense;
        private Boolean fullDuplex;
        private MacAddress macAddress;
        private List<IpV4AddressDto> ipv4Addresses;
        private List<IpV6AddressDto> ipv6Addresses;
        private PortClass portClass;
        private PortMode portMode;
        private PortType portType;
        private Context primaryVlan;
        private Context memberOfPort;
        private Set<Context> portMembers;
        private Context neighborInterface;
        private List<ExtendedInfoDto> neighborInterfaceExtendedInfo;
        private Context switchContext;

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

        public Builder portId(String portId) {
            this.portId = portId;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder linkType(LinkType linkType) {
            this.linkType = linkType;
            return this;
        }

        public Builder operationalState(OperationalState operationalState) {
            this.operationalState = operationalState;
            return this;
        }

        public Builder administrativeState(AdministrativeState administrativeState) {
            this.administrativeState = administrativeState;
            return this;
        }

        public Builder linkSpeedMbps(Integer linkSpeedMbps) {
            this.linkSpeedMbps = linkSpeedMbps;
            return this;
        }

        public Builder neighborInfo(NeighborInfo neighborInfo) {
            this.neighborInfo = neighborInfo;
            return this;
        }

        public Builder neighborMac(MacAddress neighborMac) {
            this.neighborMac = neighborMac;
            return this;
        }

        public Builder frameSize(Integer frameSize) {
            this.frameSize = frameSize;
            return this;
        }

        public Builder autosense(Boolean autosense) {
            this.autosense = autosense;
            return this;
        }

        public Builder fullDuplex(Boolean fullDuplex) {
            this.fullDuplex = fullDuplex;
            return this;
        }

        public Builder macAddress(MacAddress macAddress) {
            this.macAddress = macAddress;
            return this;
        }

        public Builder ipv4Addresses(List<IpV4AddressDto> ipv4Addresses) {
            this.ipv4Addresses = ipv4Addresses;
            return this;
        }

        public Builder ipv6Addresses(List<IpV6AddressDto> ipv6Addresses) {
            this.ipv6Addresses = ipv6Addresses;
            return this;
        }

        public Builder portClass(PortClass portClass) {
            this.portClass = portClass;
            return this;
        }

        public Builder portMode(PortMode portMode) {
            this.portMode = portMode;
            return this;
        }

        public Builder portType(PortType portType) {
            this.portType = portType;
            return this;
        }

        public Builder primaryVlan(Context primaryVlan) {
            this.primaryVlan = primaryVlan;
            return this;
        }

        public Builder memberOfPort(Context memberOfPort) {
            this.memberOfPort = memberOfPort;
            return this;
        }

        public Builder portMembers(Set<Context> portMembers) {
            this.portMembers = portMembers;
            return this;
        }

        public Builder neighborInterface(Context neighborInterface) {
            this.neighborInterface = neighborInterface;
            return this;
        }

        public Builder neighborInterfaceExtendedInfo(List<ExtendedInfoDto> neighborInterfaceExtendedInfo) {
            this.neighborInterfaceExtendedInfo = neighborInterfaceExtendedInfo;
            return this;
        }

        public Builder switchContext(Context switchContext) {
            this.switchContext = switchContext;
            return this;
        }

        public EthernetSwitchPortDto build() {
            return new EthernetSwitchPortDto(this);
        }
    }
}
