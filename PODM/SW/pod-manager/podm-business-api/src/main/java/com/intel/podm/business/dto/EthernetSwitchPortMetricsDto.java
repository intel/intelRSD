/*
 * Copyright (c) 2017 Intel Corporation
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

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "received", "transmitted", "collisions", "actions", "oem"
})
public final class EthernetSwitchPortMetricsDto extends RedfishDto {
    @JsonProperty("Received")
    private Metrics received = new Metrics();
    @JsonProperty("Transmitted")
    private Metrics transmitted = new Metrics();
    @JsonProperty("Collisions")
    private Long collisions;
    @JsonProperty("Actions")
    private Actions actions = new Actions();

    public EthernetSwitchPortMetricsDto() {
        super("#EthernetSwitchPortMetrics.v1_0_0.EthernetSwitchPortMetrics");
    }

    public Metrics getReceived() {
        return received;
    }

    public void setReceived(Metrics received) {
        this.received = received;
    }

    public Metrics getTransmitted() {
        return transmitted;
    }

    public void setTransmitted(Metrics transmitted) {
        this.transmitted = transmitted;
    }

    public Long getCollisions() {
        return collisions;
    }

    public void setCollisions(Long collisions) {
        this.collisions = collisions;
    }

    public Actions getActions() {
        return actions;
    }

    public void setActions(Actions actions) {
        this.actions = actions;
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    @JsonPropertyOrder({"oDataType", "packets", "droppedPackets", "errorPackets", "broadcastPackets", "multicastPackets", "bytes", "errors"})
    public static class Metrics {
        @JsonProperty("@odata.type")
        private final String oDataType = "#EthernetSwitchPortMetrics.v1_0_0.Metrics";
        @JsonProperty("Packets")
        private Long packets;
        @JsonProperty("DroppedPackets")
        private Long droppedPackets;
        @JsonProperty("ErrorPackets")
        private Long errorPackets;
        @JsonProperty("BroadcastPackets")
        private Long broadcastPackets;
        @JsonProperty("MulticastPackets")
        private Long multicastPackets;
        @JsonProperty("Bytes")
        private Long bytes;
        @JsonProperty("Errors")
        private Long errors;

        public String getoDataType() {
            return oDataType;
        }

        public Long getPackets() {
            return packets;
        }

        public void setPackets(Long packets) {
            this.packets = packets;
        }

        public Long getDroppedPackets() {
            return droppedPackets;
        }

        public void setDroppedPackets(Long droppedPackets) {
            this.droppedPackets = droppedPackets;
        }

        public Long getErrorPackets() {
            return errorPackets;
        }

        public void setErrorPackets(Long errorPackets) {
            this.errorPackets = errorPackets;
        }

        public Long getBroadcastPackets() {
            return broadcastPackets;
        }

        public void setBroadcastPackets(Long broadcastPackets) {
            this.broadcastPackets = broadcastPackets;
        }

        public Long getMulticastPackets() {
            return multicastPackets;
        }

        public void setMulticastPackets(Long multicastPackets) {
            this.multicastPackets = multicastPackets;
        }

        public Long getBytes() {
            return bytes;
        }

        public void setBytes(Long bytes) {
            this.bytes = bytes;
        }

        public Long getErrors() {
            return errors;
        }

        public void setErrors(Long errors) {
            this.errors = errors;
        }
    }

    public class Actions extends RedfishActionsDto {
    }
}
