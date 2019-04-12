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

import com.intel.rsd.nodecomposer.persistence.listeners.ManagerListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.EntityListeners;
import javax.persistence.ManyToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "manager")
@EntityListeners(ManagerListener.class)
public class Manager extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "manager_type")
    private String managerType;

    @Getter
    @Setter
    @Column(name = "service_entry_point_uuid")
    private String serviceEntryPointUuid;

    @Getter
    @Setter
    @Column(name = "remote_redfish_service_uri")
    private String remoteRedfishServiceUri;

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Fabric> fabrics = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ComputerSystem> computerSystems = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageService> storageServices = new HashSet<>();

    public Set<Fabric> getFabrics() {
        return fabrics;
    }

    public void addFabric(Fabric fabric) {
        requiresNonNull(fabric, "fabric");

        fabrics.add(fabric);
        if (!fabric.getManagers().contains(this)) {
            fabric.addManager(this);
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (fabrics.contains(fabric)) {
            fabrics.remove(fabric);
            if (fabric != null) {
                fabric.unlinkManager(this);
            }
        }
    }

    public Set<ComputerSystem> getComputerSystems() {
        return computerSystems;
    }

    public void addComputerSystem(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "computerSystem");

        computerSystems.add(computerSystem);
        if (!computerSystem.getManagers().contains(this)) {
            computerSystem.addManager(this);
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (computerSystems.contains(computerSystem)) {
            computerSystems.remove(computerSystem);
            if (computerSystem != null) {
                computerSystem.unlinkManager(this);
            }
        }
    }

    public Set<StorageService> getStorageServices() {
        return storageServices;
    }

    public void addStorageService(StorageService storageService) {
        requiresNonNull(storageService, "storageService");

        storageServices.add(storageService);
        if (!storageService.getManagers().contains(this)) {
            storageService.addManager(this);
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (storageServices.contains(storageService)) {
            storageServices.remove(storageService);
            if (storageService != null) {
                storageService.unlinkManager(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(fabrics, this::unlinkFabric);
        unlinkCollection(computerSystems, this::unlinkComputerSystem);
        unlinkCollection(storageServices, this::unlinkStorageService);
    }

    public boolean isOffline() {
        return statusOf(this)
            .isUnavailableOffline()
            .verify();
    }
}
