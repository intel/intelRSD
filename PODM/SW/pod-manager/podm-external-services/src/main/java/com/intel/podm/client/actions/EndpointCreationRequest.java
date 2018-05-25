/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.IpV4AddressOrigin;
import com.intel.podm.common.types.IpV6AddressOrigin;
import com.intel.podm.common.types.Protocol;

import java.util.ArrayList;
import java.util.Collection;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public class EndpointCreationRequest {
    private String name;
    private String description;
    private Protocol endpointProtocol;
    private Collection<Identifier> identifiers = new ArrayList<>();
    private Collection<ConnectedEntity> connectedEntities = new ArrayList<>();

    @JsonProperty("IPTransportDetails")
    private Collection<IpTransportDetails> ipTransportDetails = new ArrayList<>();

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Protocol getEndpointProtocol() {
        return endpointProtocol;
    }

    public void setEndpointProtocol(Protocol endpointProtocol) {
        this.endpointProtocol = endpointProtocol;
    }

    public Collection<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void setIdentifiers(Collection<Identifier> identifiers) {
        this.identifiers = identifiers;
    }

    public Collection<ConnectedEntity> getConnectedEntities() {
        return connectedEntities;
    }

    public void setConnectedEntities(Collection<ConnectedEntity> connectedEntities) {
        this.connectedEntities = connectedEntities;
    }

    public Collection<IpTransportDetails> getIpTransportDetails() {
        return ipTransportDetails;
    }

    public void setIpTransportDetails(Collection<IpTransportDetails> ipTransportDetails) {
        this.ipTransportDetails = ipTransportDetails;
    }

    public void addConnectedEntity(ConnectedEntity entity) {
        this.connectedEntities.add(entity);
    }

    @JsonInclude(NON_NULL)
    public static class Identifier {
        private DurableNameFormat durableNameFormat;
        private String durableName;

        public DurableNameFormat getDurableNameFormat() {
            return durableNameFormat;
        }

        public void setDurableNameFormat(DurableNameFormat durableNameFormat) {
            this.durableNameFormat = durableNameFormat;
        }

        public String getDurableName() {
            return durableName;
        }

        public void setDurableName(String durableName) {
            this.durableName = durableName;
        }
    }

    @JsonInclude(NON_NULL)
    public static class ConnectedEntity {
        private EntityRole entityRole;
        private ODataId entityLink;

        public EntityRole getEntityRole() {
            return entityRole;
        }

        public void setEntityRole(EntityRole entityRole) {
            this.entityRole = entityRole;
        }

        public ODataId getEntityLink() {
            return entityLink;
        }

        public void setEntityLink(ODataId entityLink) {
            this.entityLink = entityLink;
        }
    }

    @JsonInclude(NON_NULL)
    public static class IpTransportDetails {
        private Protocol transportProtocol;
        private Integer port;
        @JsonProperty("IPv4Address")
        private IpV4AddressObject ipv4Address;
        @JsonProperty("IPv6Address")
        private IpV6AddressObject ipv6Address;

        public Integer getPort() {
            return port;
        }

        public void setPort(Integer port) {
            this.port = port;
        }

        public Protocol getTransportProtocol() {
            return transportProtocol;
        }

        public void setTransportProtocol(Protocol transportProtocol) {
            this.transportProtocol = transportProtocol;
        }

        public IpV4AddressObject getIpv4Address() {
            return ipv4Address;
        }

        public void setIpv4Address(IpV4AddressObject ipv4Address) {
            this.ipv4Address = ipv4Address;
        }

        public IpV6AddressObject getIpv6Address() {
            return ipv6Address;
        }

        public void setIpv6Address(IpV6AddressObject ipv6Address) {
            this.ipv6Address = ipv6Address;
        }
    }

    @JsonInclude(NON_NULL)
    public static class IpV4AddressObject {
        private String address;
        private String subnetMask;
        private IpV4AddressOrigin addressOrigin;
        private String gateway;

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }

        public String getSubnetMask() {
            return subnetMask;
        }

        public void setSubnetMask(String subnetMask) {
            this.subnetMask = subnetMask;
        }

        public IpV4AddressOrigin getAddressOrigin() {
            return addressOrigin;
        }

        public void setAddressOrigin(IpV4AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
        }

        public String getGateway() {
            return gateway;
        }

        public void setGateway(String gateway) {
            this.gateway = gateway;
        }
    }

    @JsonInclude(NON_NULL)
    public static class IpV6AddressObject {
        private String address;
        private Integer prefixLength;
        private IpV6AddressOrigin addressOrigin;
        private AddressState addressState;

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }

        public Integer getPrefixLength() {
            return prefixLength;
        }

        public void setPrefixLength(Integer prefixLength) {
            this.prefixLength = prefixLength;
        }

        public IpV6AddressOrigin getAddressOrigin() {
            return addressOrigin;
        }

        public void setAddressOrigin(IpV6AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
        }

        public AddressState getAddressState() {
            return addressState;
        }

        public void setAddressState(AddressState addressState) {
            this.addressState = addressState;
        }
    }
}
