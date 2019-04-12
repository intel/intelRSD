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

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;

import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "storage_service")
@SuppressWarnings({"checkstyle:MethodCount"})
public class StorageService extends DiscoverableEntity {
    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Volume> volumes = new HashSet<>();

    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToMany(mappedBy = "storageService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> storagePools = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_service_manager",
        joinColumns = {@JoinColumn(name = "storage_service_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

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

    public Fabric getFabric() {
        return fabric;
    }

    public void setFabric(Fabric fabric) {
        if (!Objects.equals(this.fabric, fabric)) {
            unlinkFabric(this.fabric);
            this.fabric = fabric;
            if (fabric != null && !this.equals(fabric.getStorageService())) {
                fabric.setStorageService(this);
            }
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (Objects.equals(this.fabric, fabric)) {
            this.fabric = null;
            if (fabric != null) {
                fabric.unlinkStorageService(this);
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
        unlinkCollection(volumes, this::unlinkVolume);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(storagePools, this::unlinkStoragePool);
        unlinkCollection(managers, this::unlinkManager);
        unlinkComputerSystem(computerSystem);
        unlinkFabric(fabric);
    }
}
