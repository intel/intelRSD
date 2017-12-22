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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.actions.ResetActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.PciePortType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "portId", "portProtocol",
    "portType", "currentSpeedGbps", "width", "maxSpeedGbps", "actions", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PortDto extends RedfishDto {
    private Status status;
    private String portId;
    private PciePortType portType;
    private Protocol portProtocol;
    private Long currentSpeedGbps;
    private Long width;
    private Long maxSpeedGbps;
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private final Oem oem = new Oem();

    public PortDto() {
        super("#Port.v1_0_0.Port");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getPortId() {
        return portId;
    }

    public void setPortId(String portId) {
        this.portId = portId;
    }

    public PciePortType getPortType() {
        return portType;
    }

    public void setPortType(PciePortType portType) {
        this.portType = portType;
    }

    public Protocol getPortProtocol() {
        return portProtocol;
    }

    public void setPortProtocol(Protocol portProtocol) {
        this.portProtocol = portProtocol;
    }

    public Long getCurrentSpeedGbps() {
        return currentSpeedGbps;
    }

    public void setCurrentSpeedGbps(Long currentSpeedGbps) {
        this.currentSpeedGbps = currentSpeedGbps;
    }

    public Long getWidth() {
        return width;
    }

    public void setWidth(Long width) {
        this.width = width;
    }

    public Long getMaxSpeedGbps() {
        return maxSpeedGbps;
    }

    public void setMaxSpeedGbps(Long maxSpeedGbps) {
        this.maxSpeedGbps = maxSpeedGbps;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"associatedEndpoints", "ConnectedSwitches", "connectedSwitchPorts", "oem"})
    public final class Links extends RedfishLinksDto {
        private final Collection<Context> associatedEndpoints = new HashSet<>();
        private final Collection<Context> connectedSwitches = new HashSet<>();
        private final Collection<Context> connectedSwitchPorts = new HashSet<>();

        public Collection<Context> getAssociatedEndpoints() {
            return associatedEndpoints;
        }

        public Collection<Context> getConnectedSwitches() {
            return connectedSwitches;
        }

        public Collection<Context> getConnectedSwitchPorts() {
            return connectedSwitchPorts;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScale = new RackScaleOem();

        public RackScaleOem getRackScale() {
            return rackScale;
        }

        @JsonPropertyOrder({"@odata.type", "PCIeConnectionId", "Metrics"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Port";
            @JsonProperty("PCIeConnectionId")
            private final List<String> cableIds = new ArrayList<>();
            @JsonProperty("Metrics")
            private SingletonContext metrics;

            public SingletonContext getMetrics() {
                return metrics;
            }

            public void setMetrics(SingletonContext metrics) {
                this.metrics = metrics;
            }

            public String getoDataType() {
                return oDataType;
            }

            public List<String> getCableIds() {
                return cableIds;
            }
        }
    }

    @JsonPropertyOrder({"reset"})
    public class Actions extends RedfishActionsDto {
        @JsonProperty("#Port.Reset")
        private final ResetActionDto reset = new ResetActionDto();

        public ResetActionDto getReset() {
            return reset;
        }
    }
}
