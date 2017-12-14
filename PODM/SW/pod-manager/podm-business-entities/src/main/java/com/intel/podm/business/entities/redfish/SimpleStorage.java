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
import com.intel.podm.business.entities.listeners.SimpleStorageListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.EntityListeners;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
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
@Table(name = "simple_storage", indexes = @Index(name = "idx_simple_storage_entity_id", columnList = "entity_id", unique = true))
@NamedQueries({
    @NamedQuery(name = SimpleStorage.GET_SIMPLE_STORAGE_MULTI_SOURCE,
        query = "SELECT simpleStorage "
            + "FROM SimpleStorage simpleStorage "
            + "WHERE simpleStorage.multiSourceDiscriminator = :multiSourceDiscriminator "
            + "AND simpleStorage.isComplementary = :isComplementary"
    )
})
@EntityListeners(SimpleStorageListener.class)
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class SimpleStorage extends DiscoverableEntity implements MultiSourceResource {
    public static final String GET_SIMPLE_STORAGE_MULTI_SOURCE = "GET_SIMPLE_STORAGE_MULTI_SOURCE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "uefi_device_path")
    private String uefiDevicePath;

    @Column(name = "multi_source_discriminator")
    private String multiSourceDiscriminator;

    @OneToMany(mappedBy = "simpleStorage", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<SimpleStorageDevice> devices = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getUefiDevicePath() {
        return uefiDevicePath;
    }

    public void setUefiDevicePath(String uefiDevicePath) {
        this.uefiDevicePath = uefiDevicePath;
    }

    @Override
    public String getMultiSourceDiscriminator() {
        return multiSourceDiscriminator;
    }

    @Override
    public void setMultiSourceDiscriminator(String multiSourceDiscriminator) {
        this.multiSourceDiscriminator = multiSourceDiscriminator;
    }

    public Set<SimpleStorageDevice> getDevices() {
        return devices;
    }

    public void addDevice(SimpleStorageDevice simpleStorageDevice) {
        requiresNonNull(simpleStorageDevice, "simpleStorageDevice");

        devices.add(simpleStorageDevice);
        if (!this.equals(simpleStorageDevice.getSimpleStorage())) {
            simpleStorageDevice.setSimpleStorage(this);
        }
    }

    public void unlinkDevice(SimpleStorageDevice simpleStorageDevice) {
        if (devices.contains(simpleStorageDevice)) {
            devices.remove(simpleStorageDevice);
            if (simpleStorageDevice != null) {
                simpleStorageDevice.unlinkSimpleStorage(this);
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
            if (computerSystem != null && !computerSystem.getSimpleStorages().contains(this)) {
                computerSystem.addSimpleStorage(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkSimpleStorage(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(devices, this::unlinkDevice);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, computerSystem);
    }
}
