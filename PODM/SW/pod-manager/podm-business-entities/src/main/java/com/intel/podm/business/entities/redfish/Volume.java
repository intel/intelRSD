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

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.listeners.VolumeListener;
import com.intel.podm.business.entities.redfish.base.ComposableAsset;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Capacity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.entities.redfish.embeddables.Operation;
import com.intel.podm.common.types.EncryptionTypes;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.StorageAccessCapability;
import com.intel.podm.common.types.VolumeType;
import com.intel.podm.common.types.actions.InitializeType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "volume", indexes = @Index(name = "idx_volume_entity_id", columnList = "entity_id", unique = true))
@EntityListeners(VolumeListener.class)
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Volume extends DiscoverableEntity implements ComposableAsset {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "capacity_bytes")
    private BigDecimal capacityBytes;

    @Embedded
    private Capacity capacity;

    @Column(name = "volume_type")
    @Enumerated(STRING)
    private VolumeType volumeType;

    @Column(name = "encrypted")
    private Boolean encrypted;

    @Column(name = "encryption_types")
    @Enumerated(STRING)
    private EncryptionTypes encryptionTypes;

    @Column(name = "block_size_bytes")
    private BigDecimal blockSizeBytes;

    @Column(name = "optimum_io_size_bytes")
    private BigDecimal optimumIoSizeBytes;

    @Column(name = "bootable")
    private Boolean bootable;

    @Column(name = "erase_on_detach")
    private Boolean eraseOnDetach;

    @Column(name = "erased")
    private Boolean erased;

    @Column(name = "initialize_action_supported")
    private Boolean initializeActionSupported;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @ElementCollection
    @CollectionTable(name = "volume_identifier", joinColumns = @JoinColumn(name = "volume_id"))
    private Set<Identifier> identifiers = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "volume_operation", joinColumns = @JoinColumn(name = "volume_id"))
    @OrderColumn(name = "operation_order")
    private List<Operation> operations = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "volume_access_capability", joinColumns = @JoinColumn(name = "volume_id"))
    @OrderColumn(name = "access_capability_order")
    @Column(name = "access_capability")
    @Enumerated(STRING)
    private List<StorageAccessCapability> accessCapabilities = new ArrayList<>();

    @OneToMany(mappedBy = "volume", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<CapacitySource> capacitySources = new HashSet<>();

    @OneToMany(mappedBy = "volume", fetch = LAZY, cascade = {MERGE, PERSIST})
    private List<ReplicaInfo> replicaInfos = new ArrayList<>();

    @OneToMany(mappedBy = "volume", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "volume_metadata_id")
    private VolumeMetadata metadata = new VolumeMetadata();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "volume_metrics_id")
    private VolumeMetrics metrics;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_pool_id")
    private StoragePool storagePool;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "volume_allowable_initialize_type", joinColumns = @JoinColumn(name = "volume_id"))
    @Column(name = "allowable_initialize_type")
    @OrderColumn(name = "allowable_initialize_type_order")
    private List<InitializeType> allowableInitializeTypes = new ArrayList<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id entityId) {
        this.entityId = entityId;
    }

    @Override
    public boolean isDegraded() {
        return this.getStatus() == null
            || statusOf(this).isCritical().verify();
    }

    public BigDecimal getCapacityBytes() {
        return capacityBytes;
    }

    public void setCapacityBytes(BigDecimal capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public Capacity getCapacity() {
        return capacity;
    }

    public void setCapacity(Capacity capacity) {
        this.capacity = capacity;
    }

    public VolumeType getVolumeType() {
        return volumeType;
    }

    public void setVolumeType(VolumeType volumeType) {
        this.volumeType = volumeType;
    }

    public Boolean getEncrypted() {
        return encrypted;
    }

    public void setEncrypted(Boolean encrypted) {
        this.encrypted = encrypted;
    }

    public EncryptionTypes getEncryptionTypes() {
        return encryptionTypes;
    }

    public void setEncryptionTypes(EncryptionTypes encryptionTypes) {
        this.encryptionTypes = encryptionTypes;
    }

    public BigDecimal getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(BigDecimal blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public BigDecimal getOptimumIoSizeBytes() {
        return optimumIoSizeBytes;
    }

    public void setOptimumIoSizeBytes(BigDecimal optimumIoSizeBytes) {
        this.optimumIoSizeBytes = optimumIoSizeBytes;
    }

    public Boolean getBootable() {
        return bootable;
    }

    public void setBootable(Boolean bootable) {
        this.bootable = bootable;
    }

    public Boolean getEraseOnDetach() {
        return eraseOnDetach;
    }

    public void setEraseOnDetach(Boolean eraseOnDetach) {
        this.eraseOnDetach = eraseOnDetach;
    }

    public Boolean getErased() {
        return erased;
    }

    public void setErased(Boolean erased) {
        this.erased = erased;
    }

    public Boolean getInitializeActionSupported() {
        return initializeActionSupported;
    }

    public void setInitializeActionSupported(Boolean initializeActionSupported) {
        this.initializeActionSupported = initializeActionSupported;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public Set<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public VolumeMetadata getMetadata() {
        return metadata;
    }

    public List<Operation> getOperations() {
        return operations;
    }

    public void addOperations(Operation operation) {
        requiresNonNull(operation, "operations");
        operations.add(operation);
    }

    public List<InitializeType> getAllowableInitializeTypes() {
        return allowableInitializeTypes;
    }

    public void addAllowableInitializeType(InitializeType initializeType) {
        allowableInitializeTypes.add(initializeType);
    }

    public List<StorageAccessCapability> getAccessCapabilities() {
        return accessCapabilities;
    }

    public void addAccessCapability(StorageAccessCapability accessCapability) {
        accessCapabilities.add(accessCapability);
    }

    public String getFirstVolumeIdentifierDurableName() {
        return getIdentifiers()
            .stream()
            .findFirst()
            .map(Identifier::getDurableName)
            .orElse(null);
    }

    public Set<CapacitySource> getCapacitySources() {
        return capacitySources;
    }

    public void addCapacitySource(CapacitySource capacitySource) {
        requiresNonNull(capacitySource, "capacitySource");

        capacitySources.add(capacitySource);
        if (!this.equals(capacitySource.getVolume())) {
            capacitySource.setVolume(this);
        }
    }

    public void unlinkCapacitySource(CapacitySource capacitySource) {
        if (capacitySources.contains(capacitySource)) {
            capacitySources.remove(capacitySource);
            if (capacitySource != null) {
                capacitySource.unlinkVolume(this);
            }
        }
    }

    public List<ReplicaInfo> getReplicaInfos() {
        return replicaInfos;
    }

    public void addReplicaInfo(ReplicaInfo replicaInfo) {
        requiresNonNull(replicaInfo, "replicaInfo");

        replicaInfos.add(replicaInfo);
        if (!this.equals(replicaInfo.getVolume())) {
            replicaInfo.setVolume(this);
        }
    }

    public void unlinkReplicaInfo(ReplicaInfo replicaInfo) {
        if (replicaInfos.contains(replicaInfo)) {
            replicaInfos.remove(replicaInfo);
            if (replicaInfo != null) {
                replicaInfo.unlinkReplica(replicaInfo.getReplica());
                replicaInfo.unlinkVolume(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrives(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getVolume())) {
            drive.setVolume(this);
        }
    }

    public void unlinkDrives(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkVolume(this);
            }
        }
    }

    public VolumeMetrics getMetrics() {
        return metrics;
    }

    public void setMetrics(VolumeMetrics metrics) {
        if (!Objects.equals(this.metrics, metrics)) {
            unlinkMetrics(this.metrics);
            this.metrics = metrics;
            if (metrics != null && !this.equals(metrics.getVolume())) {
                metrics.setVolume(this);
            }
        }
    }

    public void unlinkMetrics(VolumeMetrics metrics) {
        if (Objects.equals(this.metrics, metrics)) {
            this.metrics = null;
            if (metrics != null) {
                metrics.unlinkVolume(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return getEntityConnections().stream().map(ConnectedEntity::getEndpoint).collect(toSet());
    }

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        if (!Objects.equals(this.storageService, storageService)) {
            unlinkStorageService(this.storageService);
            this.storageService = storageService;
            if (storageService != null && !storageService.getVolumes().contains(this)) {
                storageService.addVolume(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkVolume(this);
            }
        }
    }

    public StoragePool getStoragePool() {
        return storagePool;
    }

    public void setStoragePool(StoragePool storagePool) {
        this.storagePool = storagePool;
    }

    public void unlinkStoragePool(StoragePool storagePool) {
        if (Objects.equals(this.storagePool, storagePool)) {
            this.storagePool = null;
            if (storagePool != null) {
                storagePool.unlinkAllocatedVolume(this);
            }
        }
    }

    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !composedNode.getVolumes().contains(this)) {
                composedNode.unlinkVolume(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkVolume(this);
            }
        }
    }

    public boolean isErasePossible() {
        return Objects.equals(getInitializeActionSupported(), TRUE) && !Objects.equals(getEraseOnDetach(), FALSE);
    }

    @Override
    public void preRemove() {
        unlinkCollection(capacitySources, this::unlinkCapacitySource);
        unlinkCollection(drives, this::unlinkDrives);
        unlinkCollection(replicaInfos, this::unlinkReplicaInfo);
        unlinkStorageService(storageService);
        unlinkStoragePool(storagePool);
        unlinkComposedNode(composedNode);
        unlinkMetrics(metrics);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, storageService);
    }
}
