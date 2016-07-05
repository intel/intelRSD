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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.IpV4AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressDto;
import com.intel.podm.business.services.context.Context;
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

import java.util.List;

public final class EthernetSwitchPortDto {
    private final Id id;
    private final String name;
    private final String description;
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
    private final Context primaryVlan;
    private final Context switchContext;
    private final Context memberOfPort;
    private final List<Context> portMembers;
    private final Context neighborInterface;

    private EthernetSwitchPortDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
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
        primaryVlan = builder.primaryVlan;
        switchContext = builder.switchContext;
        memberOfPort = builder.memberOfPort;
        portMembers = builder.portMembers;
        neighborInterface = builder.neighborInterface;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public Context getSwitchContext() {
        return switchContext;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getPortId() {
        return portId;
    }

    public Status getStatus() {
        return status;
    }

    public LinkType getLinkType() {
        return linkType;
    }

    public OperationalState getOperationalState() {
        return operationalState;
    }

    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    public Integer getFrameSize() {
        return frameSize;
    }

    public Boolean getAutosense() {
        return autosense;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public List<IpV4AddressDto> getIpv4Addresses() {
        return ipv4Addresses;
    }

    public List<IpV6AddressDto> getIpv6Addresses() {
        return ipv6Addresses;
    }

    public PortClass getPortClass() {
        return portClass;
    }

    public PortMode getPortMode() {
        return portMode;
    }

    public PortType getPortType() {
        return portType;
    }

    public Context getPrimaryVlan() {
        return primaryVlan;
    }

    public Context getMemberOfPort() {
        return memberOfPort;
    }

    public List<Context> getPortMembers() {
        return portMembers;
    }

    public Context getNeighborInterface() {
        return neighborInterface;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
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
        private List<Context> portMembers;
        private Context neighborInterface;
        private Context switchContext;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder portId(String val) {
            portId = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder linkType(LinkType val) {
            linkType = val;
            return this;
        }

        public Builder operationalState(OperationalState val) {
            operationalState = val;
            return this;
        }

        public Builder administrativeState(AdministrativeState val) {
            administrativeState = val;
            return this;
        }

        public Builder linkSpeedMbps(Integer val) {
            linkSpeedMbps = val;
            return this;
        }

        public Builder neighborInfo(NeighborInfo val) {
            neighborInfo = val;
            return this;
        }

        public Builder neighborMac(MacAddress val) {
            neighborMac = val;
            return this;
        }

        public Builder frameSize(Integer val) {
            frameSize = val;
            return this;
        }

        public Builder autosense(Boolean val) {
            autosense = val;
            return this;
        }

        public Builder fullDuplex(Boolean val) {
            fullDuplex = val;
            return this;
        }

        public Builder macAddress(MacAddress val) {
            macAddress = val;
            return this;
        }

        public Builder ipv4Addresses(List<IpV4AddressDto> val) {
            ipv4Addresses = val;
            return this;
        }

        public Builder ipv6Addresses(List<IpV6AddressDto> val) {
            ipv6Addresses = val;
            return this;
        }

        public Builder portClass(PortClass val) {
            portClass = val;
            return this;
        }

        public Builder portMode(PortMode val) {
            portMode = val;
            return this;
        }

        public Builder portType(PortType val) {
            portType = val;
            return this;
        }

        public Builder primaryVlan(Context val) {
            primaryVlan = val;
            return this;
        }

        public Builder memberOfPort(Context val) {
            memberOfPort = val;
            return this;
        }

        public Builder portMembers(List<Context> val) {
            portMembers = val;
            return this;
        }

        public Builder neighborInterface(Context val) {
            neighborInterface = val;
            return this;
        }

        public Builder switchContext(Context val) {
            switchContext = val;
            return this;
        }

        public EthernetSwitchPortDto build() {
            return new EthernetSwitchPortDto(this);
        }
    }
}
