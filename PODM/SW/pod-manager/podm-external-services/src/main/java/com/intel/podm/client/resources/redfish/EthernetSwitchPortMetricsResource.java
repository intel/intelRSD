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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({"#EthernetSwitchPortMetrics" + VERSION_PATTERN + "EthernetSwitchPortMetrics"})
public class EthernetSwitchPortMetricsResource extends ExternalServiceResourceImpl {
    @JsonProperty("Received")
    private Metrics received;
    @JsonProperty("Transmitted")
    private Metrics transmitted;
    @JsonProperty("Collisions")
    private Long collisions;

    public Metrics getReceived() {
        return received;
    }

    public Metrics getTransmitted() {
        return transmitted;
    }

    public Long getCollisions() {
        return collisions;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class Metrics {
        @JsonProperty("@odata.type")
        private String oDataType;
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

        public Long getDroppedPackets() {
            return droppedPackets;
        }

        public Long getErrorPackets() {
            return errorPackets;
        }

        public Long getBroadcastPackets() {
            return broadcastPackets;
        }

        public Long getMulticastPackets() {
            return multicastPackets;
        }

        public Long getBytes() {
            return bytes;
        }

        public Long getErrors() {
            return errors;
        }
    }
}
