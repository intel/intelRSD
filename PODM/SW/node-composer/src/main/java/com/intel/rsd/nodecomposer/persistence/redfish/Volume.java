/*
 * Copyright (c) 2017-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.persistence.listeners.VolumeListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.AttachableType;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.EntityListeners;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.types.AttachableType.VOLUME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "volume")
@EntityListeners(VolumeListener.class)
@SuppressWarnings({"checkstyle:MethodCount"})
public class Volume extends DiscoverableEntity implements ComposableAsset, AttachableAsset {
    @Getter
    @Setter
    @Column(name = "capacity_bytes")
    private BigDecimal capacityBytes;

    @Getter
    @Setter
    @Column(name = "bootable")
    private Boolean bootable;

    @Getter
    @Setter
    @Column(name = "initialize_action_supported")
    private Boolean initializeActionSupported;

    @Column(name = "achievable")
    private Boolean achievable = true;

    @Getter
    @ElementCollection
    @CollectionTable(name = "volume_identifier", joinColumns = @JoinColumn(name = "volume_id"))
    private Set<Identifier> identifiers = new HashSet<>();

    @OneToMany(mappedBy = "volume", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<CapacitySource> capacitySources = new HashSet<>();

    @OneToMany(mappedBy = "volume", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "volume_metadata_id")
    private VolumeMetadata metadata = new VolumeMetadata();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_pool_id")
    private StoragePool storagePool;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @Override
    public boolean isDegraded() {
        return this.getStatus() == null
            || statusOf(this).isCritical().verify();
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public VolumeMetadata getMetadata() {
        return metadata;
    }

    //todo: remove capacity sources?

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

    @Override
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

    @Override
    public void preRemove() {
        unlinkCollection(capacitySources, this::unlinkCapacitySource);
        unlinkCollection(drives, this::unlinkDrives);
        unlinkStorageService(storageService);
        unlinkStoragePool(storagePool);
        unlinkComposedNode(composedNode);
    }

    @Override
    public AttachableType getAttachableType() {
        return VOLUME;
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
    public boolean isAchievable() {
        return achievable;
    }

    @Override
    public void setAchievable(boolean achievable) {
        this.achievable = achievable;
    }
}
