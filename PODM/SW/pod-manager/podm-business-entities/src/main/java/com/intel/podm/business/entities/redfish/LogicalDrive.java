/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.VolumeMode;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.types.ComposedNodeState.ALLOCATED;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATING;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLING;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.math.BigDecimal.ZERO;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "logical_drive", indexes = @Index(name = "idx_logical_drive_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class LogicalDrive extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "type")
    @Enumerated(STRING)
    private LogicalDriveType type;

    @Column(name = "mode")
    @Enumerated(STRING)
    private VolumeMode mode;

    @Column(name = "write_protected")
    private Boolean writeProtected;

    @Column(name = "capacity_gib")
    private BigDecimal capacityGib;

    @Column(name = "image")
    private String image;

    @Column(name = "bootable")
    private Boolean bootable;

    @Column(name = "snapshot")
    private Boolean snapshot;

    @ManyToMany(mappedBy = "logicalDrives", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<RemoteTarget> remoteTargets = new HashSet<>();

    @ManyToMany(mappedBy = "logicalDrives", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PhysicalDrive> physicalDrives = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "logical_drive_logical_drive",
        joinColumns = {@JoinColumn(name = "used_by_logical_drive_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "used_logical_drive_id", referencedColumnName = "id")})
    private Set<LogicalDrive> usedLogicalDrives = new HashSet<>();

    @ManyToMany(mappedBy = "usedLogicalDrives", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<LogicalDrive> usedByLogicalDrives = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "master_drive_id")
    private LogicalDrive masterDrive;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public LogicalDriveType getType() {
        return type;
    }

    public void setType(LogicalDriveType type) {
        this.type = type;
    }

    public VolumeMode getMode() {
        return mode;
    }

    public void setMode(VolumeMode volumeMode) {
        this.mode = volumeMode;
    }

    public Boolean getWriteProtected() {
        return writeProtected;
    }

    public void setWriteProtected(Boolean writeProtected) {
        this.writeProtected = writeProtected;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    public String getImage() {
        return image;
    }

    public void setImage(String image) {
        this.image = image;
    }

    public Boolean getBootable() {
        return bootable;
    }

    public void setBootable(Boolean bootable) {
        this.bootable = bootable;
    }

    public Boolean getSnapshot() {
        return snapshot;
    }

    public void setSnapshot(Boolean snapshot) {
        this.snapshot = snapshot;
    }

    public Set<RemoteTarget> getRemoteTargets() {
        return remoteTargets;
    }

    public void addRemoteTarget(RemoteTarget remoteTarget) {
        requiresNonNull(remoteTarget, "remoteTarget");

        remoteTargets.add(remoteTarget);
        if (!remoteTarget.getLogicalDrives().contains(this)) {
            remoteTarget.addLogicalDrive(this);
        }
    }

    public void unlinkRemoteTarget(RemoteTarget remoteTarget) {
        if (remoteTargets.contains(remoteTarget)) {
            remoteTargets.remove(remoteTarget);
            if (remoteTarget != null) {
                remoteTarget.unlinkLogicalDrive(this);
            }
        }
    }

    public Set<PhysicalDrive> getPhysicalDrives() {
        return physicalDrives;
    }

    public void addPhysicalDrive(PhysicalDrive physicalDrive) {
        requiresNonNull(physicalDrive, "physicalDrive");

        physicalDrives.add(physicalDrive);
        if (!physicalDrive.getLogicalDrives().contains(this)) {
            physicalDrive.addLogicalDrive(this);
        }
    }

    public void unlinkPhysicalDrive(PhysicalDrive physicalDrive) {
        if (physicalDrives.contains(physicalDrive)) {
            physicalDrives.remove(physicalDrive);
            if (physicalDrive != null) {
                physicalDrive.unlinkLogicalDrive(this);
            }
        }
    }

    public Set<LogicalDrive> getUsedLogicalDrives() {
        return usedLogicalDrives;
    }

    public void addUsedLogicalDrive(LogicalDrive logicalDrive) {
        requiresNonNull(logicalDrive, "logicalDrive");

        usedLogicalDrives.add(logicalDrive);
        if (!logicalDrive.getUsedByLogicalDrives().contains(this)) {
            logicalDrive.addUsedByLogicalDrive(this);
        }
    }

    public void unlinkUsedLogicalDrive(LogicalDrive logicalDrive) {
        if (usedLogicalDrives.contains(logicalDrive)) {
            usedLogicalDrives.remove(logicalDrive);
            if (logicalDrive != null) {
                logicalDrive.unlinkUsedByLogicalDrive(this);
            }
        }
    }

    public Set<LogicalDrive> getUsedByLogicalDrives() {
        return usedByLogicalDrives;
    }

    public void addUsedByLogicalDrive(LogicalDrive logicalDrive) {
        requiresNonNull(logicalDrive, "logicalDrive");

        usedByLogicalDrives.add(logicalDrive);
        if (!logicalDrive.getUsedLogicalDrives().contains(this)) {
            logicalDrive.addUsedLogicalDrive(this);
        }
    }

    public void unlinkUsedByLogicalDrive(LogicalDrive logicalDrive) {
        if (usedByLogicalDrives.contains(logicalDrive)) {
            usedByLogicalDrives.remove(logicalDrive);
            if (logicalDrive != null) {
                logicalDrive.unlinkUsedLogicalDrive(this);
            }
        }
    }

    public LogicalDrive getMasterDrive() {
        return masterDrive;
    }

    public void setMasterDrive(LogicalDrive masterDrive) {
        if (!Objects.equals(this.masterDrive, masterDrive)) {
            unlinkMasterDrive(this.masterDrive);
            this.masterDrive = masterDrive;
        }
    }

    public void unlinkMasterDrive(LogicalDrive masterDrive) {
        if (Objects.equals(this.masterDrive, masterDrive)) {
            this.masterDrive = null;
        }
    }

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        if (!Objects.equals(this.storageService, storageService)) {
            unlinkStorageService(this.storageService);
            this.storageService = storageService;
            if (storageService != null && !storageService.getLogicalDrives().contains(this)) {
                storageService.addLogicalDrive(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkLogicalDrive(this);
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
            if (composedNode != null && !composedNode.getLogicalDrives().contains(this)) {
                composedNode.addLogicalDrive(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkLogicalDrive(this);
            }
        }
    }

    public BigDecimal getFreeSpaceGib() {
        Collection<LogicalDrive> usedBy = getUsedByLogicalDrives();
        BigDecimal freeSpaceGib = getCapacityGib();

        for (LogicalDrive logicalDrive : usedBy) {
            freeSpaceGib = freeSpaceGib.subtract(logicalDrive.getCapacityGib());
        }

        freeSpaceGib = freeSpaceGib.subtract(getSpaceReservedByComposedNodes());
        return freeSpaceGib;
    }

    private BigDecimal getSpaceReservedByComposedNodes() {
        if (composedNode != null
            && composedNode.isInAnyOfStates(ALLOCATING, ALLOCATED, ASSEMBLING)
            && composedNode.getRemoteDriveCapacityGib() != null) {
            return composedNode.getRemoteDriveCapacityGib();
        }

        return ZERO;
    }

    @Override
    public void preRemove() {
        unlinkCollection(remoteTargets, this::unlinkRemoteTarget);
        unlinkCollection(physicalDrives, this::unlinkPhysicalDrive);
        unlinkCollection(usedLogicalDrives, this::unlinkUsedLogicalDrive);
        unlinkCollection(usedByLogicalDrives, this::unlinkUsedByLogicalDrive);
        unlinkMasterDrive(masterDrive);
        unlinkStorageService(storageService);
        unlinkComposedNode(composedNode);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, storageService);
    }
}
