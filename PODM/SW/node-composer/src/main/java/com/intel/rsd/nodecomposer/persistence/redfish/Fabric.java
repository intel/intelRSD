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
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "fabric")
@SuppressWarnings({"checkstyle:MethodCount"})
public class Fabric extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "fabric_type")
    @Enumerated(STRING)
    private Protocol fabricType;

    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Zone> zones = new HashSet<>();

    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Switch> switches = new HashSet<>();

    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToOne(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private StorageService storageService;

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "fabric_manager",
        joinColumns = {@JoinColumn(name = "fabric_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    public Set<Zone> getZones() {
        return zones;
    }

    public void addZone(Zone zone) {
        requiresNonNull(zone, "zone");
        zones.add(zone);
        if (!this.equals(zone.getFabric())) {
            zone.setFabric(this);
        }
    }

    public void unlinkZone(Zone zone) {
        if (zones.contains(zone)) {
            zones.remove(zone);
            if (zone != null) {
                zone.unlinkFabric(this);
            }
        }
    }

    public Set<Switch> getSwitches() {
        return switches;
    }

    public void addSwitch(Switch fabricSwitch) {
        requiresNonNull(fabricSwitch, "fabricSwitch");
        switches.add(fabricSwitch);
        if (!this.equals(fabricSwitch.getFabric())) {
            fabricSwitch.setFabric(this);
        }
    }

    public void unlinkSwitch(Switch fabricSwitch) {
        if (switches.contains(fabricSwitch)) {
            switches.remove(fabricSwitch);
            if (fabricSwitch != null) {
                fabricSwitch.unlinkFabric(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");
        endpoints.add(endpoint);
        if (!this.equals(endpoint.getFabric())) {
            endpoint.setFabric(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkFabric(this);
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
            if (storageService != null && !this.equals(storageService.getFabric())) {
                storageService.setFabric(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkFabric(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getFabrics().contains(this)) {
            manager.addFabric(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkFabric(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(zones, this::unlinkZone);
        unlinkCollection(switches, this::unlinkSwitch);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkStorageService(storageService);
        unlinkCollection(managers, this::unlinkManager);
    }
}
