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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.VolumeMode.LVG;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toList;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "storage_service", indexes = @Index(name = "idx_storage_service_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class StorageService extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<RemoteTarget> remoteTargets = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<LogicalDrive> logicalDrives = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PhysicalDrive> physicalDrives = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_service_manager",
        joinColumns = {@JoinColumn(name = "storage_service_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Set<RemoteTarget> getRemoteTargets() {
        return remoteTargets;
    }

    public void addRemoteTarget(RemoteTarget remoteTarget) {
        requiresNonNull(remoteTarget, "remoteTarget");

        remoteTargets.add(remoteTarget);
        if (!this.equals(remoteTarget.getStorageService())) {
            remoteTarget.setStorageService(this);
        }
    }

    public void unlinkRemoteTarget(RemoteTarget remoteTarget) {
        if (remoteTargets.contains(remoteTarget)) {
            remoteTargets.remove(remoteTarget);
            if (remoteTarget != null) {
                remoteTarget.unlinkStorageService(this);
            }
        }
    }

    public Set<LogicalDrive> getLogicalDrives() {
        return logicalDrives;
    }

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        requiresNonNull(logicalDrive, "logicalDrive");

        logicalDrives.add(logicalDrive);
        if (!this.equals(logicalDrive.getStorageService())) {
            logicalDrive.setStorageService(this);
        }
    }

    public void unlinkLogicalDrive(LogicalDrive logicalDrive) {
        if (logicalDrives.contains(logicalDrive)) {
            logicalDrives.remove(logicalDrive);
            if (logicalDrive != null) {
                logicalDrive.unlinkStorageService(this);
            }
        }
    }

    public List<LogicalDrive> getLogicalVolumeGroups() {
        return getLogicalDrives()
            .stream()
            .filter(ld -> ld.getMode().equals(LVG))
            .collect(toList());
    }

    public Set<PhysicalDrive> getPhysicalDrives() {
        return physicalDrives;
    }

    public void addPhysicalDrive(PhysicalDrive physicalDrive) {
        requiresNonNull(physicalDrive, "physicalDrive");

        physicalDrives.add(physicalDrive);
        if (!this.equals(physicalDrive.getStorageService())) {
            physicalDrive.setStorageService(this);
        }
    }

    public void unlinkPhysicalDrive(PhysicalDrive physicalDrive) {
        if (physicalDrives.contains(physicalDrive)) {
            physicalDrives.remove(physicalDrive);
            if (physicalDrive != null) {
                physicalDrive.unlinkStorageService(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getStorageServices().contains(this)) {
            manager.addStorageService(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkStorageService(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(remoteTargets, this::unlinkRemoteTarget);
        unlinkCollection(logicalDrives, this::unlinkLogicalDrive);
        unlinkCollection(physicalDrives, this::unlinkPhysicalDrive);
        unlinkCollection(managers, this::unlinkManager);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
