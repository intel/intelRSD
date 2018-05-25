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

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Capacity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "capacity_source", indexes = @Index(name = "idx_capacity_source_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings("checkstyle:MethodCount")
public class CapacitySource extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Embedded
    private Capacity providedCapacity;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "volume_id")
    private Volume volume;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_pool_id")
    private StoragePool storagePool;

    @ManyToMany(mappedBy = "providingPoolCapacitySources", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> providingPools = new HashSet<>();

    @ManyToMany(mappedBy = "capacitySources", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id entityId) {
        this.entityId = entityId;
    }

    public Capacity getProvidedCapacity() {
        return providedCapacity;
    }

    public void setProvidedCapacity(Capacity providedCapacity) {
        this.providedCapacity = providedCapacity;
    }

    public Volume getVolume() {
        return volume;
    }

    public void setVolume(Volume volume) {
        this.volume = volume;
    }

    public void unlinkVolume(Volume volume) {
        if (Objects.equals(this.volume, volume)) {
            this.volume = null;
            if (volume != null) {
                volume.unlinkCapacitySource(this);
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
                storagePool.unlinkCapacitySource(this);
            }
        }
    }

    public Set<StoragePool> getProvidingPools() {
        return providingPools;
    }

    public void addProvidingPool(StoragePool providingPool) {
        requiresNonNull(providingPool, "providingPool");

        providingPools.add(providingPool);
        if (!providingPool.getProvidingPoolCapacitySources().contains(this)) {
            providingPool.addProvidingPoolCapacitySource(this);
        }
    }

    public void unlinkProvidingPool(StoragePool providingPool) {
        if (providingPools.contains(providingPool)) {
            providingPools.remove(providingPool);
            if (providingPool != null) {
                providingPool.unlinkProvidingPoolCapacitySource(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!drive.getCapacitySource().contains(this)) {
            drive.addCapacitySource(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkCapacitySource(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkVolume(volume);
        unlinkStoragePool(storagePool);
        unlinkCollection(providingPools, this::unlinkProvidingPool);
        unlinkCollection(drives, this::unlinkDrive);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, volume);
    }
}
