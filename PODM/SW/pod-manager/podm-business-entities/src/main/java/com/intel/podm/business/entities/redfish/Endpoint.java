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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.entities.redfish.embeddables.PciId;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;
import org.hibernate.annotations.Generated;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@Table(name = "endpoint", indexes = @Index(name = "idx_endpoint_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Endpoint extends DiscoverableEntity {
    @Generated(INSERT)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "endpoint_protocol")
    @Enumerated(STRING)
    private Protocol protocol;

    @Embedded
    @Column(name = "pci_id")
    private PciId pciId;

    @OneToMany(mappedBy = "endpoint", fetch = EAGER, cascade = {MERGE, PERSIST})
    private Set<ConnectedEntity> connectedEntities = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "endpoint_identifiers", joinColumns = @JoinColumn(name = "identifiers_id"))
    @OrderColumn(name = "endpoint_identifiers_order")
    private Set<Identifier> identifiers = new HashSet<>();

    @Column(name = "host_reservation_memory_bytes")
    private Integer hostReservationMemoryBytes;

    @ManyToMany(mappedBy = "endpoints", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Port> ports = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "zone_id")
    private Zone zone;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    public void setEntityId(Id entityId) {
        this.entityId = entityId;
    }

    public Protocol getProtocol() {
        return protocol;
    }

    public void setProtocol(Protocol protocol) {
        this.protocol = protocol;
    }

    public PciId getPciId() {
        return pciId;
    }

    public void setPciId(PciId pciId) {
        this.pciId = pciId;
    }

    public Integer getHostReservationMemoryBytes() {
        return hostReservationMemoryBytes;
    }

    public void setHostReservationMemoryBytes(Integer hostReservationMemoryBytes) {
        this.hostReservationMemoryBytes = hostReservationMemoryBytes;
    }

    public Set<ConnectedEntity> getConnectedEntities() {
        return connectedEntities;
    }

    public void addConnectedEntity(ConnectedEntity connectedEntity) {
        requiresNonNull(connectedEntity, "connectedEntity");

        connectedEntities.add(connectedEntity);
        if (!this.equals(connectedEntity.getEndpoint())) {
            connectedEntity.setEndpoint(this);
        }
    }

    public void unlinkConnectedEntities(ConnectedEntity connectedEntity) {
        if (this.connectedEntities.contains(connectedEntity)) {
            this.connectedEntities.remove(connectedEntity);
            if (connectedEntity != null) {
                connectedEntity.unlinkEndpoint(this);
            }
        }
    }

    public Set<Port> getPorts() {
        return ports;
    }

    public void addPort(Port port) {
        requiresNonNull(port, "zone");
        ports.add(port);
        if (!port.getEndpoints().contains(this)) {
            port.addEndpoint(this);
        }
    }

    public void unlinkPort(Port port) {
        if (ports.contains(port)) {
            ports.remove(port);
            if (port != null) {
                port.unlinkEndpoint(this);
            }
        }
    }

    public Set<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public Zone getZone() {
        return zone;
    }

    public void setZone(Zone zone) {
        if (zone == null) {
            unlinkZone(this.zone);
        } else {
            this.zone = zone;
            if (!zone.getEndpoints().contains(this)) {
                zone.addEndpoint(this);
            }
        }
    }

    public void unlinkZone(Zone zone) {
        if (Objects.equals(this.zone, zone)) {
            this.zone = null;
            if (zone != null) {
                zone.unlinkEndpoint(this);
            }
        }
    }

    public Fabric getFabric() {
        return fabric;
    }

    public void setFabric(Fabric fabric) {
        if (fabric == null) {
            unlinkFabric(this.fabric);
        } else {
            this.fabric = fabric;
            if (!fabric.getEndpoints().contains(this)) {
                fabric.addEndpoint(this);
            }
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (Objects.equals(this.fabric, fabric)) {
            this.fabric = null;
            if (fabric != null) {
                fabric.unlinkEndpoint(this);
            }
        }
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (computerSystem == null) {
            unlinkComputerSystem(this.computerSystem);
        } else {
            this.computerSystem = computerSystem;
            if (!computerSystem.getEndpoints().contains(this)) {
                computerSystem.addEndpoint(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkEndpoint(this);
            }
        }
    }

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    @Override
    public void preRemove() {
        unlinkCollection(connectedEntities, this::unlinkConnectedEntities);
        unlinkZone(zone);
        unlinkCollection(ports, this::unlinkPort);
        unlinkComputerSystem(computerSystem);
        unlinkFabric(fabric);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, fabric);
    }

}
