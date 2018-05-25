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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

@Embeddable
@SuppressWarnings({"checkstyle:MethodCount"})
public class SwitchPortMetrics {
    @Column(name = "packets")
    private Long packets;
    @Column(name = "dropped_packets")
    private Long droppedPackets;
    @Column(name = "error_packets")
    private Long errorPackets;
    @Column(name = "broadcast_packets")
    private Long broadcastPackets;
    @Column(name = "multicast_packets")
    private Long multicastPackets;
    @Column(name = "bytes")
    private Long bytes;
    @Column(name = "errors")
    private Long errors;

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

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof SwitchPortMetrics)) {
            return false;
        }
        SwitchPortMetrics that = (SwitchPortMetrics) o;
        return new EqualsBuilder()
            .append(packets, that.packets)
            .append(droppedPackets, that.droppedPackets)
            .append(errorPackets, that.errorPackets)
            .append(broadcastPackets, that.broadcastPackets)
            .append(multicastPackets, that.multicastPackets)
            .append(bytes, that.bytes)
            .append(errors, that.errors)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(packets, droppedPackets, errorPackets, broadcastPackets, multicastPackets, bytes, errors);
    }
}
