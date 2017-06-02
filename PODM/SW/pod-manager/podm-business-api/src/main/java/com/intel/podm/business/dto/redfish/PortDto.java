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

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.Collection;
import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PortDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final String portId;
    private final PortType portType;
    private final Protocol portProtocol;
    private final Long currentSpeedGbps;
    private final Long width;
    private final Long maxSpeedGbps;
    private final List<String> pcieConnectionId;
    private final Collection<ResetType> allowableResetValues;
    private final Links links;

    private PortDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        portId = builder.portId;
        portType = builder.portType;
        currentSpeedGbps = builder.currentSpeedGbps;
        width = builder.width;
        maxSpeedGbps = builder.maxSpeedGbps;
        pcieConnectionId = builder.pcieConnectionId;
        portProtocol = builder.portProtocol;
        allowableResetValues = builder.allowableResetValues;
        links = new Links(builder.associatedEndpoint,
            builder.connectedSwitch,
            builder.connectedSwitchPort);
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

    public String getPortId() {
        return portId;
    }

    public PortType getPortType() {
        return portType;
    }

    public Long getCurrentSpeedGbps() {
        return currentSpeedGbps;
    }

    public Long getWidth() {
        return width;
    }

    public Long getMaxSpeedGbps() {
        return maxSpeedGbps;
    }

    public List<String> getPcieConnectionId() {
        return pcieConnectionId;
    }

    public Collection<ResetType> getAllowableResetValues() {
        return allowableResetValues;
    }

    public Protocol getPortProtocol() {
        return portProtocol;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private Set<Context> associatedEndpoints;
        private Set<Context> connectedSwitches;
        private Set<Context> connectedSwitchPort;
        private Object oem;

        public Links(Set<Context> associatedEndpoints, Set<Context> connectedSwitches,
                     Set<Context> connectedSwitchPort) {
            this.associatedEndpoints = associatedEndpoints;
            this.connectedSwitches = connectedSwitches;
            this.connectedSwitchPort = connectedSwitchPort;
        }

        public Set<Context> getAssociatedEndpoints() {
            return associatedEndpoints;
        }

        public Set<Context> getConnectedSwitches() {
            return connectedSwitches;
        }

        public Set<Context> getConnectedSwitchPorts() {
            return connectedSwitchPort;
        }
    }

    public static class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private String portId;
        private PortType portType;
        private Protocol portProtocol;
        private Long currentSpeedGbps;
        private Long width;
        private Long maxSpeedGbps;
        private List<String> pcieConnectionId;
        private Collection<ResetType> allowableResetValues;
        private Set<Context> associatedEndpoint;
        private Set<Context> connectedSwitch;
        private Set<Context> connectedSwitchPort;

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

        public Builder associatedEndpoint(Set<Context> associatedEndpoint) {
            this.associatedEndpoint = associatedEndpoint;
            return this;
        }

        public Builder connectedSwitch(Set<Context> connectedSwitch) {
            this.connectedSwitch = connectedSwitch;
            return this;
        }

        public Builder connectedSwitchPort(Set<Context> connectedSwitchPort) {
            this.connectedSwitchPort = connectedSwitchPort;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder portId(String portId) {
            this.portId = portId;
            return this;
        }

        public Builder portType(PortType portType) {
            this.portType = portType;
            return this;
        }

        public Builder portProtocol(Protocol portProtocol) {
            this.portProtocol = portProtocol;
            return this;
        }

        public Builder currentSpeedGbps(Long currentSpeedGbps) {
            this.currentSpeedGbps = currentSpeedGbps;
            return this;
        }

        public Builder width(Long width) {
            this.width = width;
            return this;
        }

        public Builder maxSpeedGbps(Long maxSpeedGbps) {
            this.maxSpeedGbps = maxSpeedGbps;
            return this;
        }

        public Builder pcieConnectionId(List<String> pcieConnectionId) {
            this.pcieConnectionId = pcieConnectionId;
            return this;
        }

        public Builder allowableResetValues(Collection<ResetType> allowableResetValues) {
            this.allowableResetValues = allowableResetValues;
            return this;
        }

        public PortDto build() {
            return new PortDto(this);
        }
    }
}
