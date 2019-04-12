/*
 * Copyright (c) 2017-2019 Intel Corporation
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
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "capacity_source")
@SuppressWarnings("checkstyle:MethodCount")
public class CapacitySource extends DiscoverableEntity {
    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "volume_id")
    private Volume volume;

    @ManyToMany(mappedBy = "providingPoolCapacitySources", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> providingPools = new HashSet<>();

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

    @Override
    public void preRemove() {
        unlinkVolume(volume);
        unlinkCollection(providingPools, this::unlinkProvidingPool);
    }
}
