/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.AddressState;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import com.intel.rsd.nodecomposer.types.EntityRole;
import com.intel.rsd.nodecomposer.types.IpV4AddressOrigin;
import com.intel.rsd.nodecomposer.types.IpV6AddressOrigin;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.Collection;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_EMPTY;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@Getter
@Setter
@JsonInclude(NON_NULL)
public class EndpointCreationRequest {
    private String name;
    private String description;
    private Protocol endpointProtocol;
    private Collection<Identifier> identifiers = new ArrayList<>();
    private Collection<ConnectedEntity> connectedEntities = new ArrayList<>();
    @JsonProperty("IPTransportDetails")
    private Collection<IpTransportDetails> ipTransportDetails = new ArrayList<>();
    @JsonInclude(NON_EMPTY)
    private Links links = new Links();

    public void addConnectedEntity(ConnectedEntity entity) {
        this.connectedEntities.add(entity);
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    public static class Identifier {
        private DurableNameFormat durableNameFormat;
        private String durableName;
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    public static class ConnectedEntity {
        private EntityRole entityRole;
        private ODataId entityLink;
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    public static class IpTransportDetails {
        private Protocol transportProtocol;
        private Integer port;
        @JsonProperty("IPv4Address")
        private IpV4AddressObject ipv4Address;
        @JsonProperty("IPv6Address")
        private IpV6AddressObject ipv6Address;
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    public static class IpV4AddressObject {
        private String address;
        private String subnetMask;
        private IpV4AddressOrigin addressOrigin;
        private String gateway;
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    public static class IpV6AddressObject {
        private String address;
        private Integer prefixLength;
        private IpV6AddressOrigin addressOrigin;
        private AddressState addressState;
    }

    @Getter
    @Setter
    public static class Links {
        @JsonInclude(NON_EMPTY)
        private Collection<ODataId> ports = new ArrayList<>();
    }
}
