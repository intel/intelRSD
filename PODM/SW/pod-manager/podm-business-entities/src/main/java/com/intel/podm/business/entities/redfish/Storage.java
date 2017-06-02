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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "storage", indexes = @Index(name = "idx_storage_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class Storage extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @OneToMany(mappedBy = "storage", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageController> storageControllers = new HashSet<>();

    @OneToMany(mappedBy = "storageAdapter", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageController> adapters = new HashSet<>();

    @OneToMany(mappedBy = "storage", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Set<StorageController> getStorageControllers() {
        return storageControllers;
    }

    public void addStorageController(StorageController storageController) {
        requiresNonNull(storageController, "storageController");

        storageControllers.add(storageController);
        if (!this.equals(storageController.getStorage())) {
            storageController.setStorage(this);
        }
    }

    public void unlinkStorageController(StorageController storageController) {
        if (storageControllers.contains(storageController)) {
            storageControllers.remove(storageController);
            if (storageController != null) {
                storageController.unlinkStorage(this);
            }
        }
    }

    public Set<StorageController> getAdapters() {
        return adapters;
    }

    public void addAdapter(StorageController storageController) {
        requiresNonNull(storageController, "storageController");

        adapters.add(storageController);
        if (!this.equals(storageController.getStorageAdapter())) {
            storageController.setStorageAdapter(this);
        }
    }

    public void unlinkAdapter(StorageController storageController) {
        if (adapters.contains(storageController)) {
            adapters.remove(storageController);
            if (storageController != null) {
                storageController.unlinkStorageAdapter(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getStorage())) {
            drive.setStorage(this);
            if (chassis != null) {
                drive.setChassis(chassis);
            }
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkStorage(this);
                drive.setChassis(null);
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
            if (computerSystem != null && !computerSystem.getStorages().contains(this)) {
                computerSystem.addStorage(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkStorage(this);
            }
        }
    }

    public Chassis getChassis() {
        return chassis;
    }

    public void setChassis(Chassis chassis) {
        if (!Objects.equals(this.chassis, chassis)) {
            unlinkChassis(this.chassis);
            this.chassis = chassis;
            if (chassis != null && !chassis.getStorages().contains(this)) {
                chassis.addStorage(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkStorage(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(storageControllers, this::unlinkStorageController);
        unlinkCollection(adapters, this::unlinkAdapter);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkComputerSystem(computerSystem);
        unlinkChassis(chassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, computerSystem);
    }
}
