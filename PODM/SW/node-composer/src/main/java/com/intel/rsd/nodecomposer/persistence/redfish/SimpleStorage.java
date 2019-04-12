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

import com.intel.rsd.nodecomposer.persistence.listeners.SimpleStorageListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;

import javax.persistence.EntityListeners;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "simple_storage")
@EntityListeners(SimpleStorageListener.class)
public class SimpleStorage extends DiscoverableEntity {
    @OneToMany(mappedBy = "simpleStorage", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<SimpleStorageDevice> devices = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

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
}
