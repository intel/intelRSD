/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
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
@Table(name = "ip_transport_details")
@SuppressWarnings({"checkstyle:MethodCount"})
public class IpTransportDetails extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "transport_protocol")
    @Enumerated(STRING)
    private Protocol transportProtocol;

    @Getter
    @Setter
    @Column(name = "port")
    private Integer port;

    @Getter
    @Setter
    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "ipv4_address")),
        @AttributeOverride(name = "subnetMask", column = @Column(name = "ipv4_subnet_mask")),
        @AttributeOverride(name = "addressOrigin", column = @Column(name = "ipv4_address_origin")),
        @AttributeOverride(name = "gateway", column = @Column(name = "ipv4_gateway")),
        @AttributeOverride(name = "oem", column = @Column(name = "ipv4_oem")),
    })
    private IpV4Address ipV4Address;

    @Getter
    @Setter
    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "ipv6_address")),
        @AttributeOverride(name = "subnetMask", column = @Column(name = "ipv6_subnet_mask")),
        @AttributeOverride(name = "addressOrigin", column = @Column(name = "ipv6_address_origin")),
        @AttributeOverride(name = "gateway", column = @Column(name = "ipv6_gateway")),
        @AttributeOverride(name = "oem", column = @Column(name = "ipv6_oem")),
    })
    private IpV6Address ipV6Address;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "endpoint_id")
    private Endpoint endpoint;

    public Endpoint getEndpoint() {
        return endpoint;
    }

    public void setEndpoint(Endpoint endpoint) {
        if (endpoint == null) {
            unlinkEndpoint(this.endpoint);
        } else {
            this.endpoint = endpoint;
            if (!endpoint.getIpTransportDetails().contains(this)) {
                endpoint.addIpTransportDetails(this);
            }
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (Objects.equals(this.endpoint, endpoint)) {
            this.endpoint = null;
            if (endpoint != null) {
                endpoint.unlinkIpTransportDetails(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkEndpoint(endpoint);
    }

    public Optional<String> getIp() {
        String ip4Address = getIpV4Address() != null ? getIpV4Address().getAddress() : null;
        String ip6Address = getIpV6Address() != null ? getIpV6Address().getAddress() : null;
        return ofNullable(firstNonNull(ip4Address, ip6Address));
    }
}
