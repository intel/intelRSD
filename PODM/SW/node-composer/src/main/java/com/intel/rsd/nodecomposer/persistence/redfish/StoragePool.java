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
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Capacity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;
import lombok.val;

import javax.persistence.Embedded;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATING;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLING;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static com.intel.rsd.nodecomposer.utils.Converters.convertGibToBytes;
import static java.math.BigDecimal.ZERO;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "storage_pool")
@SuppressWarnings({"checkstyle:MethodCount"})
public class StoragePool extends DiscoverableEntity {
    @Getter
    @Setter
    @Embedded
    private Capacity capacity;

    @OneToMany(mappedBy = "storagePool", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Volume> allocatedVolumes = new HashSet<>();

    @OneToMany(mappedBy = "storagePool", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> allocatedPools = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_providing_pool_capacity_source",
        joinColumns = {@JoinColumn(name = "storage_pool_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "capacity_source_id", referencedColumnName = "id")})
    private Set<CapacitySource> providingPoolCapacitySources = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_pool_composed_node",
        joinColumns = {@JoinColumn(name = "storage_pool_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "composed_node_id", referencedColumnName = "id")})
    private Set<ComposedNode> composedNodes = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_pool_id")
    private StoragePool storagePool;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    public Set<Volume> getAllocatedVolumes() {
        return allocatedVolumes;
    }

    public void addAllocatedVolume(Volume volume) {
        requiresNonNull(volume, "volume");

        allocatedVolumes.add(volume);
        if (!this.equals(volume.getStoragePool())) {
            volume.setStoragePool(this);
        }
    }

    public void unlinkAllocatedVolume(Volume volume) {
        if (allocatedVolumes.contains(volume)) {
            allocatedVolumes.remove(volume);
            if (volume != null) {
                volume.unlinkStoragePool(this);
            }
        }
    }

    public Set<StoragePool> getAllocatedPools() {
        return allocatedPools;
    }

    public void addAllocatedPool(StoragePool storagePool) {
        requiresNonNull(storagePool, "storagePool");

        allocatedPools.add(storagePool);
        if (!this.equals(storagePool.getStoragePool())) {
            storagePool.setStoragePool(this);
        }
    }

    public void unlinkAllocatedPool(StoragePool storagePool) {
        if (allocatedPools.contains(storagePool)) {
            allocatedPools.remove(storagePool);
            if (storagePool != null) {
                storagePool.unlinkStoragePool(this);
            }
        }
    }

    public Set<CapacitySource> getProvidingPoolCapacitySources() {
        return providingPoolCapacitySources;
    }

    public void addProvidingPoolCapacitySource(CapacitySource providingPoolCapacitySource) {
        requiresNonNull(providingPoolCapacitySource, "providingPoolCapacitySource");

        providingPoolCapacitySources.add(providingPoolCapacitySource);
        if (!providingPoolCapacitySource.getProvidingPools().contains(this)) {
            providingPoolCapacitySource.addProvidingPool(this);
        }
    }

    public void unlinkProvidingPoolCapacitySource(CapacitySource providingPoolCapacitySource) {
        if (providingPoolCapacitySources.contains(providingPoolCapacitySource)) {
            providingPoolCapacitySources.remove(providingPoolCapacitySource);
            if (providingPoolCapacitySource != null) {
                providingPoolCapacitySource.unlinkProvidingPool(this);
            }
        }
    }

    public Set<ComposedNode> getComposedNodes() {
        return composedNodes;
    }

    public void addComposedNode(ComposedNode composedNode) {
        requiresNonNull(composedNode, "composedNode");

        composedNodes.add(composedNode);
        if (!composedNode.getStoragePools().contains(this)) {
            composedNode.addStoragePool(this);
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (composedNodes.contains(composedNode)) {
            composedNodes.remove(composedNode);
            if (composedNode != null) {
                composedNode.unlinkStoragePool(this);
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
                storagePool.unlinkAllocatedPool(this);
            }
        }
    }

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        this.storageService = storageService;
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkStoragePool(this);
            }
        }
    }

    public BigDecimal getFreeSpace() {
        return getCapacity().getData().getAllocatedBytes()
            .subtract(getCapacity().getData().getConsumedBytes())
            .subtract(capacityOfVolumesFromNotAssembledComposedNodes());
    }

    public Protocol getProtocol() {
        val storageService = getStorageService();
        if (storageService == null || storageService.getFabric() == null) {
            return null;
        }

        return storageService.getFabric().getFabricType();
    }

    @Override
    public void preRemove() {
        unlinkCollection(allocatedVolumes, this::unlinkAllocatedVolume);
        unlinkCollection(allocatedPools, this::unlinkAllocatedPool);
        unlinkCollection(providingPoolCapacitySources, this::unlinkProvidingPoolCapacitySource);
        unlinkCollection(composedNodes, this::unlinkComposedNode);
        unlinkStoragePool(storagePool);
        unlinkStorageService(storageService);
    }

    private BigDecimal capacityOfVolumesFromNotAssembledComposedNodes() {
        BigDecimal capacityOfComposedNodesInGiB = composedNodes.stream()
            .filter(composedNode -> composedNode.isInAnyOfStates(ALLOCATING, ALLOCATED, ASSEMBLING))
            .map(ComposedNode::getRemoteDriveCapacityGib)
            .reduce(ZERO, BigDecimal::add);
        return convertGibToBytes(capacityOfComposedNodesInGiB);
    }
}
