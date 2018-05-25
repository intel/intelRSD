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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;
import java.util.Optional;

import static java.util.Optional.ofNullable;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.apache.commons.lang3.ObjectUtils.firstNonNull;

@javax.persistence.Entity
@Table(name = "ip_transport_details", indexes = @Index(name = "idx_transport_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class IpTransportDetails extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "transport_protocol")
    @Enumerated(STRING)
    private Protocol transportProtocol;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "endpoint_id")
    private Endpoint endpoint;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "ipv4_address")),
        @AttributeOverride(name = "subnetMask", column = @Column(name = "ipv4_subnet_mask")),
        @AttributeOverride(name = "addressOrigin", column = @Column(name = "ipv4_address_origin")),
        @AttributeOverride(name = "gateway", column = @Column(name = "ipv4_gateway")),
        @AttributeOverride(name = "oem", column = @Column(name = "ipv4_oem")),
    })
    private IpV4Address ipV4Address;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "ipv6_address")),
        @AttributeOverride(name = "subnetMask", column = @Column(name = "ipv6_subnet_mask")),
        @AttributeOverride(name = "addressOrigin", column = @Column(name = "ipv6_address_origin")),
        @AttributeOverride(name = "gateway", column = @Column(name = "ipv6_gateway")),
        @AttributeOverride(name = "oem", column = @Column(name = "ipv6_oem")),
    })
    private IpV6Address ipV6Address;

    @Column(name = "port")
    private Integer port;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Protocol getTransportProtocol() {
        return transportProtocol;
    }

    public void setTransportProtocol(Protocol protocol) {
        this.transportProtocol = protocol;
    }


    public Endpoint getEndpoint() {
        return endpoint;
    }

    public void setEndpoint(Endpoint endpoint) {
        if (endpoint == null) {
            unlinkEndpoint(this.endpoint);
        } else {
            this.endpoint = endpoint;
            if (!endpoint.getIpTransportsDetails().contains(this)) {
                endpoint.addTransport(this);
            }
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (Objects.equals(this.endpoint, endpoint)) {
            this.endpoint = null;
            if (endpoint != null) {
                endpoint.unlinkTransport(this);
            }
        }
    }

    public IpV4Address getIpv4Address() {
        return ipV4Address;
    }

    public void setIpv4Address(IpV4Address ipV4Address) {
        this.ipV4Address = ipV4Address;
    }

    public IpV6Address getIpv6Address() {
        return ipV6Address;
    }

    public void setIpv6Address(IpV6Address ipV6Address) {
        this.ipV6Address = ipV6Address;
    }

    public Integer getPort() {
        return port;
    }

    public void setPort(Integer port) {
        this.port = port;
    }

    @Override
    public void preRemove() {
        unlinkEndpoint(endpoint);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, endpoint);
    }

    public Optional<String> getIp() {
        String ip4Address = getIpv4Address() != null ? getIpv4Address().getAddress() : null;
        String ip6Address = getIpv6Address() != null ? getIpv6Address().getAddress() : null;
        return ofNullable(firstNonNull(ip4Address, ip6Address));
    }
}
