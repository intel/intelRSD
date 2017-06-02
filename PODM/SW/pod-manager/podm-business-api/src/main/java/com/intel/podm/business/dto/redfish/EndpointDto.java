/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.dto.redfish.attributes.ConnectedEntityDto;
import com.intel.podm.business.dto.redfish.attributes.IdentifierDto;
import com.intel.podm.business.dto.redfish.attributes.PciIdDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class EndpointDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final Protocol endpointProtocol;
    private final PciIdDto pciId;
    private final Set<IdentifierDto> identifiers;
    private final Set<ConnectedEntityDto> connectedEntities;
    private final Set<RedundancyDto> redundancies;
    private final Integer hostReservationMemoryBytes;
    private final Links links;

    private EndpointDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        endpointProtocol = builder.endpointProtocol;
        pciId = builder.pciId;
        identifiers = builder.identifiers;
        connectedEntities = builder.connectedEntities;
        hostReservationMemoryBytes = builder.hostReservationMemoryBytes;
        redundancies = builder.redundancies;
        links = new Links(builder.ports);
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

    public Set<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public Protocol getEndpointProtocol() {
        return endpointProtocol;
    }

    public PciIdDto getPciId() {
        return pciId;
    }

    public Set<IdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public Set<ConnectedEntityDto> getConnectedEntities() {
        return connectedEntities;
    }

    public Integer getHostReservationMemoryBytes() {
        return hostReservationMemoryBytes;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> ports;

        public Links(Set<Context> ports) {
            this.ports = ports;
        }

        public Set<Context> getPorts() {
            return ports;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private Protocol endpointProtocol;
        private PciIdDto pciId;
        private Set<IdentifierDto> identifiers;
        private Set<Context> ports;
        private Set<ConnectedEntityDto> connectedEntities;
        private Set<RedundancyDto> redundancies;
        private Integer hostReservationMemoryBytes;

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

        public Builder endpointProtocol(Protocol endpointProtocol) {
            this.endpointProtocol = endpointProtocol;
            return this;
        }

        public Builder pciId(PciIdDto pciId) {
            this.pciId = pciId;
            return this;
        }

        public Builder identifiers(Set<IdentifierDto> identifiers) {
            this.identifiers = identifiers;
            return this;
        }

        public Builder connectedEntities(Set<ConnectedEntityDto> connectedEntities) {
            this.connectedEntities = connectedEntities;
            return this;
        }

        public Builder ports(Set<Context> ports) {
            this.ports = ports;
            return this;
        }

        public Builder hostReservationMemoryBytes(Integer hostReservationMemoryBytes) {
            this.hostReservationMemoryBytes = hostReservationMemoryBytes;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder redundancies(Set<RedundancyDto> redundancies) {
            this.redundancies = redundancies;
            return this;
        }

        public EndpointDto build() {
            return new EndpointDto(this);
        }
    }
}
