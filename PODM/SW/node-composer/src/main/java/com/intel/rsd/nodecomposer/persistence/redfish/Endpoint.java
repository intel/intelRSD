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

import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.listeners.EndpointListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.HasProtocol;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.EndpointAuthentication;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.AttachableType;
import com.intel.rsd.nodecomposer.types.EntityRole;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.types.AttachableType.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.DurableNameFormat.IQN;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.utils.Collections.firstByPredicate;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toList;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(
        name = GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL,
        query = "SELECT e FROM Endpoint e WHERE e.authentication.username IS NULL AND e.protocol = :protocol"
    ),
    @NamedQuery(
        name = GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL,
        query = "SELECT e FROM Endpoint e WHERE e.computerSystem IS NOT NULL AND e.protocol = :protocol"
    ),
    @NamedQuery(
        name = Endpoint.GET_INITIATOR_ENDPOINT_BY_FABRIC_AND_SYSTEM,
        query = "SELECT ce.endpoint FROM ConnectedEntity ce "
            + "WHERE ce.entityRole =:role AND ce.endpoint.fabric.uri = :fabric AND ce.endpoint.computerSystem.uri = :system"
    ),
    @NamedQuery(
        name = Endpoint.GET_INITIATOR_ENDPOINT_BY_STORAGE_SERVICE_AND_SYSTEM,
        query = "SELECT ce.endpoint FROM ConnectedEntity ce "
            + "WHERE ce.entityRole =:role AND ce.endpoint.fabric.storageService.uri = :ss AND ce.endpoint.computerSystem.uri = :system"
    )
})
@Table(name = "endpoint")
@EntityListeners(EndpointListener.class)
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Endpoint extends DiscoverableEntity implements HasProtocol, ComposableAsset, AttachableAsset {
    public static final String GET_INITIATOR_ENDPOINT_BY_FABRIC_AND_SYSTEM = "GET_INITIATOR_ENDPOINT_BY_FABRIC_AND_SYSTEM";
    public static final String GET_INITIATOR_ENDPOINT_BY_STORAGE_SERVICE_AND_SYSTEM = "GET_INITIATOR_ENDPOINT_BY_STORAGE_SERVICE_AND_SYSTEM";
    public static final String GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL = "GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL";
    public static final String GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL = "GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL";

    @Getter
    @Setter
    @Column(name = "endpoint_protocol")
    @Enumerated(STRING)
    private Protocol protocol;

    @Column(name = "achievable")
    private Boolean achievable = true;

    @Getter
    @Setter
    @Embedded
    private EndpointAuthentication authentication;

    @ElementCollection
    @CollectionTable(name = "endpoint_identifier", joinColumns = @JoinColumn(name = "endpoint_id"))
    @OrderColumn(name = "endpoint_identifier_order")
    private Set<Identifier> identifiers = new HashSet<>();

    @OneToMany(mappedBy = "endpoint", fetch = EAGER, cascade = {MERGE, PERSIST})
    private Set<ConnectedEntity> connectedEntities = new HashSet<>();

    @OneToMany(mappedBy = "endpoint", fetch = EAGER, cascade = {MERGE, PERSIST})
    private Set<IpTransportDetails> ipTransportDetails = new HashSet<>();

    @ManyToMany(mappedBy = "endpoints", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Port> ports = new HashSet<>();

    @ManyToMany(mappedBy = "endpoints", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetInterface> ethernetInterfaces = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "zone_id")
    private Zone zone;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "endpoint_metadata_id")
    private EndpointMetadata metadata = new EndpointMetadata();

    @Override
    public boolean isDegraded() {
        return this.getStatus() == null
            || statusOf(this).isCritical().verify();
    }

    public Set<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
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

    public Set<IpTransportDetails> getIpTransportDetails() {
        return ipTransportDetails;
    }

    public void addIpTransportDetails(IpTransportDetails ipTransportDetails) {
        requiresNonNull(ipTransportDetails, "ipTransportDetails");
        this.ipTransportDetails.add(ipTransportDetails);
        if (!this.equals(ipTransportDetails.getEndpoint())) {
            ipTransportDetails.setEndpoint(this);
        }
    }

    public void unlinkIpTransportDetails(IpTransportDetails ipTransportDetails) {
        if (this.ipTransportDetails.contains(ipTransportDetails)) {
            this.ipTransportDetails.remove(ipTransportDetails);
            if (ipTransportDetails != null) {
                ipTransportDetails.unlinkEndpoint(this);
            }
        }
    }

    public Set<Port> getPorts() {
        return ports;
    }

    public void addPort(Port port) {
        requiresNonNull(port, "port");
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

    public Set<EthernetInterface> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        requiresNonNull(ethernetInterface, "ethernetInterface");
        ethernetInterfaces.add(ethernetInterface);
        if (!ethernetInterface.getEndpoints().contains(this)) {
            ethernetInterface.addEndpoint(this);
        }
    }

    public void unlinkEthernetInterface(EthernetInterface ethernetInterface) {
        if (ethernetInterfaces.contains(ethernetInterface)) {
            ethernetInterfaces.remove(ethernetInterface);
            if (ethernetInterface != null) {
                ethernetInterface.unlinkEndpoint(this);
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

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        if (!Objects.equals(this.storageService, storageService)) {
            unlinkStorageService(this.storageService);
            this.storageService = storageService;
            if (storageService != null && !storageService.getEndpoints().contains(this)) {
                storageService.addEndpoint(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkEndpoint(this);
            }
        }
    }

    public Collection<Processor> getProcessors() {
        return getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Processor.class::isInstance)
            .map(Processor.class::cast)
            .collect(toList());
    }

    public Collection<Volume> getVolumes() {
        return getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Volume.class::isInstance)
            .map(Volume.class::cast)
            .collect(toList());
    }

    public Collection<Drive> getDrives() {
        return getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Drive.class::isInstance)
            .map(Drive.class::cast)
            .collect(toList());
    }

    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !composedNode.getEndpoints().contains(this)) {
                composedNode.unlinkEndpoint(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkEndpoint(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(connectedEntities, this::unlinkConnectedEntities);
        unlinkCollection(ipTransportDetails, this::unlinkIpTransportDetails);
        unlinkCollection(ports, this::unlinkPort);
        unlinkCollection(ethernetInterfaces, this::unlinkEthernetInterface);
        unlinkStorageService(storageService);
        unlinkFabric(fabric);
        unlinkZone(zone);
        unlinkComputerSystem(computerSystem);
        unlinkComposedNode(composedNode);
    }

    public Optional<Identifier> findIqnIdentifier() {
        return firstByPredicate(getIdentifiers(), identifier -> IQN.equals(identifier.getDurableNameFormat()));
    }

    public boolean isAttachable() {
        return !this.isAllocated() && this.getZone() == null;
    }

    public boolean hasRole(EntityRole entityRole) {
        return this.getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityRole)
            .anyMatch(role -> Objects.equals(entityRole, role));
    }

    public Optional<IpTransportDetails> findIscsiTransportDetailsWithDefinedIp() {
        return firstByPredicate(getIpTransportDetails(), e ->
            ISCSI.equals(e.getTransportProtocol()) && e.getIp().isPresent()
        );
    }

    @Override
    public AttachableType getAttachableType() {
        return ENDPOINT;
    }

    @Override
    public boolean isAllocated() {
        return metadata.isAllocated();
    }

    @Override
    public void setAllocated(boolean value) {
        metadata.setAllocated(value);
    }

    @Override
    public Set<Endpoint> getEndpoints() {
        return emptySet();
    }

    @Override
    public boolean isAchievable() {
        return achievable;
    }

    @Override
    public void setAchievable(boolean achievable) {
        this.achievable = achievable;
    }
}
