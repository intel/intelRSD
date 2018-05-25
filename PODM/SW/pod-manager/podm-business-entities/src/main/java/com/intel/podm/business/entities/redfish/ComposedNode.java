/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.business.entities.converters.IdToLongConverter;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import org.hibernate.annotations.Generated;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.Convert;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.Health.CRITICAL;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.types.State.UNAVAILABLE_OFFLINE;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.Boolean.TRUE;
import static java.util.Arrays.stream;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = ComposedNode.GET_ALL_NODES_IDS,
        query = "SELECT composedNode.entityId FROM ComposedNode composedNode"),
    @NamedQuery(name = ComposedNode.GET_NODE_BY_ASSOCIATED_COMPUTER_SYSTEM_UUID,
        query = "SELECT composedNode FROM ComposedNode composedNode WHERE composedNode.associatedComputerSystemUuid = :uuid"),
    @NamedQuery(name = ComposedNode.GET_NODES_ELIGIBLE_FOR_RECOVERY,
        query = "SELECT composedNode FROM ComposedNode composedNode WHERE composedNode.eligibleForRecovery = true")
})
@Table(name = "composed_node", indexes = @Index(name = "idx_composed_node_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class ComposedNode extends Entity {
    public static final String GET_ALL_NODES_IDS = "GET_ALL_NODES_IDS";
    public static final String GET_NODES_ELIGIBLE_FOR_RECOVERY = "GET_NODES_ELIGIBLE_FOR_RECOVERY";
    public static final String GET_NODE_BY_ASSOCIATED_COMPUTER_SYSTEM_UUID = "GET_NODE_BY_ASSOCIATED_COMPUTER_SYSTEM_UUID";
    public static final Status OFFLINE_CRITICAL_STATUS = new Status(UNAVAILABLE_OFFLINE, CRITICAL, null);

    @Generated(INSERT)
    @Convert(converter = IdToLongConverter.class)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_NUMERIC_COLUMN_DEFINITION, insertable = false, nullable = false)
    private Id entityId;

    @Column(name = "name")
    private String name;

    @Column(name = "description")
    private String description;

    @Column(name = "status")
    private Status status;

    @Column(name = "composed_node_state")
    @Enumerated(STRING)
    private ComposedNodeState composedNodeState;

    @Column(name = "remote_drive_capacity_gib")
    private BigDecimal remoteDriveCapacityGib;

    @Column(name = "associated_computer_system_uuid")
    private UUID associatedComputerSystemUuid;

    @Column(name = "associated_compute_service_uuid")
    private UUID associatedComputeServiceUuid;

    @Column(name = "associated_storage_service_uuid")
    private UUID associatedStorageServiceUuid;

    @Column(name = "eligible_for_recovery")
    private boolean eligibleForRecovery;

    @Column(name = "prior_untagged_vlan_id")
    private Integer priorUntaggedVlanId;

    @Column(name = "number_of_requested_drives")
    private int numberOfRequestedDrives;

    @Column(name = "clear_tpm_on_delete")
    private boolean clearTpmOnDelete;

    @ElementCollection
    @CollectionTable(name = "associated_endpoint_identifier", joinColumns = @JoinColumn(name = "associated_endpoint_identifier_id"))
    @OrderColumn(name = "associated_endpoint_identifier_order")
    private Set<Identifier> associatedEndpointIdentifiers = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "associated_volume_identifier", joinColumns = @JoinColumn(name = "associated_volume_identifier_id"))
    @OrderColumn(name = "associated_volume_identifier")
    private Set<Identifier> associatedVolumeIdentifiers = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Volume> volumes = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @ManyToMany(mappedBy = "composedNodes", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> storagePools = new HashSet<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    public Id getId() {
        return entityId;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public ComposedNodeState getComposedNodeState() {
        return composedNodeState;
    }

    public void setComposedNodeState(ComposedNodeState composedNodeState) {
        this.composedNodeState = composedNodeState;
    }

    public BigDecimal getRemoteDriveCapacityGib() {
        return remoteDriveCapacityGib;
    }

    public void setRemoteDriveCapacityGib(BigDecimal remoteDriveCapacityGib) {
        this.remoteDriveCapacityGib = remoteDriveCapacityGib;
    }

    public UUID getAssociatedComputerSystemUuid() {
        return associatedComputerSystemUuid;
    }

    public void setAssociatedComputerSystemUuid(UUID associatedComputerSystemUuid) {
        this.associatedComputerSystemUuid = associatedComputerSystemUuid;
    }

    public UUID getAssociatedComputeServiceUuid() {
        return associatedComputeServiceUuid;
    }

    public void setAssociatedComputeServiceUuid(UUID associatedComputerServiceUuid) {
        this.associatedComputeServiceUuid = associatedComputerServiceUuid;
    }

    public UUID getAssociatedStorageServiceUuid() {
        return associatedStorageServiceUuid;
    }

    public void setAssociatedStorageServiceUuid(UUID associatedStorageServiceUuid) {
        this.associatedStorageServiceUuid = associatedStorageServiceUuid;
    }

    public boolean isEligibleForRecovery() {
        return eligibleForRecovery;
    }

    public void setEligibleForRecovery(boolean eligibleForRecovery) {
        this.eligibleForRecovery = eligibleForRecovery;
    }

    public Integer getPriorUntaggedVlanId() {
        return priorUntaggedVlanId;
    }

    public void setPriorUntaggedVlanId(Integer untaggedVlanId) {
        this.priorUntaggedVlanId = untaggedVlanId;
    }

    public int getNumberOfRequestedDrives() {
        return numberOfRequestedDrives;
    }

    public void incrementNumberOfRequestedDrives() {
        this.numberOfRequestedDrives++;
    }

    public void decrementNumberOfRequestedDrives() {
        this.numberOfRequestedDrives--;
    }

    public boolean getClearTpmOnDelete() {
        return clearTpmOnDelete;
    }

    public void setClearTpmOnDelete(boolean clearTpmOnDelete) {
        this.clearTpmOnDelete = clearTpmOnDelete;
    }

    public Set<Identifier> getAssociatedEndpointIdentifiers() {
        return associatedEndpointIdentifiers;
    }

    public void addAssociatedEndpointIdentifiers(Set<Identifier> associatedEndpointIdentifier) {
        associatedEndpointIdentifiers.addAll(associatedEndpointIdentifier);
    }

    public Set<Identifier> getAssociatedVolumeIdentifiers() {
        return associatedVolumeIdentifiers;
    }

    public void addAssociatedVolumeIdentifiers(Collection<Identifier> associatedVolumeIdentifier) {
        associatedVolumeIdentifiers.addAll(associatedVolumeIdentifier);
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!this.equals(endpoint.getComposedNode())) {
            endpoint.setComposedNode(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkComposedNode(this);
            }
        }
    }

    public Set<Volume> getVolumes() {
        return volumes;
    }

    public void addVolume(Volume volume) {
        requiresNonNull(volume, "volume");

        volumes.add(volume);
        if (!this.equals(volume.getComposedNode())) {
            volume.setComposedNode(this);
        }
    }

    public void unlinkVolume(Volume volume) {
        if (volumes.contains(volume)) {
            volumes.remove(volume);
            if (volume != null) {
                volume.unlinkComposedNode(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getComposedNode())) {
            drive.setComposedNode(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkComposedNode(this);
            }
        }
    }

    public Set<StoragePool> getStoragePools() {
        return storagePools;
    }

    public void addStoragePool(StoragePool storagePool) {
        requiresNonNull(storagePool, "storagePool");

        storagePools.add(storagePool);
        if (!storagePool.getComposedNodes().contains(this)) {
            storagePool.addComposedNode(this);
        }
    }

    public void unlinkStoragePool(StoragePool storagePool) {
        if (storagePools.contains(storagePool)) {
            storagePools.remove(storagePool);
            if (storagePool != null) {
                storagePool.unlinkComposedNode(this);
            }
        }
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !this.equals(computerSystem.getComposedNode())) {
                computerSystem.setComposedNode(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                setStatus(OFFLINE_CRITICAL_STATUS);
                computerSystem.unlinkComposedNode(this);
            }
        }
    }

    public boolean isInAnyOfStates(ComposedNodeState... states) {
        ComposedNodeState actualComposedNodeState = getComposedNodeState();
        return actualComposedNodeState != null
            && stream(states).anyMatch(expectedComposedNodeState -> actualComposedNodeState == expectedComposedNodeState);
    }

    public Set<EthernetInterface> getEthernetInterfaces() {
        return computerSystem != null ? computerSystem.getEthernetInterfaces() : emptySet();
    }

    public Set<Drive> getLocalDrives() {
        if (computerSystem == null) {
            return emptySet();
        }

        return computerSystem.getStorages().stream()
            .map(Storage::getDrives)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Set<SimpleStorage> getSimpleStorages() {
        return computerSystem != null ? computerSystem.getSimpleStorages() : emptySet();
    }

    public boolean isEnabledAndHealthy() {
        return getStatus() != null && Objects.equals(getStatus().getHealth(), OK) && Objects.equals(getStatus().getState(), ENABLED);
    }

    @Override
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(volumes, this::unlinkVolume);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(storagePools, this::unlinkStoragePool);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }

    public Optional<ConnectedEntity> findAnyConnectedEntityWithBootableVolume() {
        return getEndpoints().stream()
            .flatMap(endpoint -> endpoint.getConnectedEntities().stream())
            .filter(this::hasLinkToBootableVolume)
            .findFirst();
    }

    private boolean hasLinkToBootableVolume(ConnectedEntity connectedEntity) {
        DiscoverableEntity discoverableEntity = connectedEntity.getEntityLink();

        if (discoverableEntity instanceof Volume) {
            Volume volume = (Volume) discoverableEntity;

            return Objects.equals(volume.getBootable(), TRUE);
        }

        return false;
    }
}
