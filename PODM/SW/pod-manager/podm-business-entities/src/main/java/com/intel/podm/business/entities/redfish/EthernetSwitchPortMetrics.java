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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.SwitchPortMetrics;
import com.intel.podm.common.types.Id;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "ethernet_switch_port_metrics", indexes = @Index(name = "idx_ethernet_switch_port_metrics_entity_id", columnList = "entity_id", unique = true))
public class EthernetSwitchPortMetrics extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "packets", column = @Column(name = "received_packets")),
        @AttributeOverride(name = "droppedPackets", column = @Column(name = "received_dropped_packets")),
        @AttributeOverride(name = "errorPackets", column = @Column(name = "received_error_packets")),
        @AttributeOverride(name = "broadcastPackets", column = @Column(name = "received_broadcast_packets")),
        @AttributeOverride(name = "multicastPackets", column = @Column(name = "received_multicast_packets")),
        @AttributeOverride(name = "bytes", column = @Column(name = "received_bytes")),
        @AttributeOverride(name = "errors", column = @Column(name = "received_errors"))
    })
    private SwitchPortMetrics received = new SwitchPortMetrics();

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "packets", column = @Column(name = "transmitted_packets")),
        @AttributeOverride(name = "droppedPackets", column = @Column(name = "transmitted_dropped_packets")),
        @AttributeOverride(name = "errorPackets", column = @Column(name = "transmitted_error_packets")),
        @AttributeOverride(name = "broadcastPackets", column = @Column(name = "transmitted_broadcast_packets")),
        @AttributeOverride(name = "multicastPackets", column = @Column(name = "transmitted_multicast_packets")),
        @AttributeOverride(name = "bytes", column = @Column(name = "transmitted_bytes")),
        @AttributeOverride(name = "errors", column = @Column(name = "transmitted_errors"))
    })
    private SwitchPortMetrics transmitted = new SwitchPortMetrics();

    @Column(name = "collisions")
    private Long collisions;

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_port_id")
    private EthernetSwitchPort ethernetSwitchPort;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public SwitchPortMetrics getReceived() {
        return received;
    }

    public void setReceived(SwitchPortMetrics received) {
        this.received = received;
    }

    public SwitchPortMetrics getTransmitted() {
        return transmitted;
    }

    public void setTransmitted(SwitchPortMetrics transmitted) {
        this.transmitted = transmitted;
    }

    public Long getCollisions() {
        return collisions;
    }

    public void setCollisions(Long collisions) {
        this.collisions = collisions;
    }

    public EthernetSwitchPort getEthernetSwitchPort() {
        return ethernetSwitchPort;
    }

    public void setEthernetSwitchPort(EthernetSwitchPort ethernetSwitchPort) {
        if (!Objects.equals(this.ethernetSwitchPort, ethernetSwitchPort)) {
            unlinkEthernetSwitchPort(this.ethernetSwitchPort);
            this.ethernetSwitchPort = ethernetSwitchPort;
            if (ethernetSwitchPort != null && !this.equals(ethernetSwitchPort.getMetrics())) {
                ethernetSwitchPort.setMetrics(this);
            }
        }
    }

    public void unlinkEthernetSwitchPort(EthernetSwitchPort ethernetSwitchPort) {
        if (Objects.equals(this.ethernetSwitchPort, ethernetSwitchPort)) {
            this.ethernetSwitchPort = null;
            if (ethernetSwitchPort != null) {
                ethernetSwitchPort.unlinkMetrics(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkEthernetSwitchPort(ethernetSwitchPort);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, ethernetSwitchPort);
    }
}
