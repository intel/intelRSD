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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
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
@NamedQueries({
    @NamedQuery(name = StorageService.GET_ALL_STORAGE_SERVICE_IDS,
        query = "SELECT storageService.entityId FROM StorageService storageService")
})
@Table(name = "storage_service", indexes = @Index(name = "idx_storage_service_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
@Eventable
public class StorageService extends DiscoverableEntity {
    public static final String GET_ALL_STORAGE_SERVICE_IDS = "GET_ALL_STORAGE_SERVICE_IDS";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Volume> volumes = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_service_manager",
        joinColumns = {@JoinColumn(name = "storage_service_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> storagePools = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Set<Volume> getVolumes() {
        return volumes;
    }

    public void addVolume(Volume volume) {
        requiresNonNull(volume, "volume");

        volumes.add(volume);
        if (!this.equals(volume.getStorageService())) {
            volume.setStorageService(this);
        }
    }

    public void unlinkVolume(Volume volume) {
        if (volumes.contains(volume)) {
            volumes.remove(volume);
            if (volume != null) {
                volume.unlinkStorageService(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getStorageService())) {
            drive.setStorageService(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkStorageService(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!this.equals(endpoint.getStorageService())) {
            endpoint.setStorageService(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkStorageService(this);
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

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && computerSystem.getStorageServices().contains(this)) {
                computerSystem.addStorageService(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkStorageService(this);
            }
        }
    }

    public Set<StoragePool> getStoragePools() {
        return storagePools;
    }

    public void addStoragePool(StoragePool storagePool) {
        requiresNonNull(storagePool, "storagePool");

        storagePools.add(storagePool);
        if (!this.equals(storagePool.getStorageService())) {
            storagePool.setStorageService(this);
        }
    }

    public void unlinkStoragePool(StoragePool storagePool) {
        if (storagePools.contains(storagePool)) {
            storagePools.remove(storagePool);
            if (storagePool != null) {
                storagePool.unlinkStorageService(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(volumes, this::unlinkVolume);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(managers, this::unlinkManager);
        unlinkCollection(storagePools, this::unlinkStoragePool);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
