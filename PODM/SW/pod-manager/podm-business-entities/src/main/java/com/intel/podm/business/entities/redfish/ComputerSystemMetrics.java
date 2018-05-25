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
import com.intel.podm.common.types.Id;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "computer_system_metrics", indexes = @Index(name = "idx_computer_system_metrics_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class ComputerSystemMetrics extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "io_bandwidth_gbps")
    private BigDecimal ioBandwidthGbps;

    @Column(name = "processor_bandwidth_percent")
    private BigDecimal processorBandwidthPercent;

    @Column(name = "memory_bandwidth_percent")
    private BigDecimal memoryBandwidthPercent;

    @Column(name = "memory_throttled_cycles_percent")
    private BigDecimal memoryThrottledCyclesPercent;

    @Column(name = "processor_power_watt")
    private BigDecimal processorPowerWatt;

    @Column(name = "memory_power_watt")
    private BigDecimal memoryPowerWatt;

    @ElementCollection
    @CollectionTable(name = "computer_system_metrics_health", joinColumns = @JoinColumn(name = "computer_system_metrics_id"))
    @Column(name = "health")
    @OrderColumn(name = "health_order")
    private List<String> healthList = new ArrayList<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id entityId) {
        this.entityId = entityId;
    }

    public BigDecimal getIoBandwidthGbps() {
        return ioBandwidthGbps;
    }

    public void setIoBandwidthGbps(BigDecimal ioBandwidthGbps) {
        this.ioBandwidthGbps = ioBandwidthGbps;
    }

    public BigDecimal getProcessorBandwidthPercent() {
        return processorBandwidthPercent;
    }

    public void setProcessorBandwidthPercent(BigDecimal processorBandwidthPercent) {
        this.processorBandwidthPercent = processorBandwidthPercent;
    }

    public BigDecimal getMemoryBandwidthPercent() {
        return memoryBandwidthPercent;
    }

    public void setMemoryBandwidthPercent(BigDecimal memoryBandwidthPercent) {
        this.memoryBandwidthPercent = memoryBandwidthPercent;
    }

    public BigDecimal getMemoryThrottledCyclesPercent() {
        return memoryThrottledCyclesPercent;
    }

    public void setMemoryThrottledCyclesPercent(BigDecimal memoryThrottledCyclesPercent) {
        this.memoryThrottledCyclesPercent = memoryThrottledCyclesPercent;
    }

    public BigDecimal getProcessorPowerWatt() {
        return processorPowerWatt;
    }

    public void setProcessorPowerWatt(BigDecimal processorPowerWatt) {
        this.processorPowerWatt = processorPowerWatt;
    }

    public BigDecimal getMemoryPowerWatt() {
        return memoryPowerWatt;
    }

    public void setMemoryPowerWatt(BigDecimal memoryPowerWatt) {
        this.memoryPowerWatt = memoryPowerWatt;
    }

    public List<String> getHealthList() {
        return healthList;
    }

    public void addHealth(String health) {
        this.healthList.add(health);
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !this.equals(computerSystem.getComputerSystemMetrics())) {
                computerSystem.setComputerSystemMetrics(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkComputerSystemMetrics(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, computerSystem);
    }
}
