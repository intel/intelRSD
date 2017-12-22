/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.entities.redfish.embeddables.CurrentPeriod;
import com.intel.podm.business.entities.redfish.embeddables.HealthData;
import com.intel.podm.business.entities.redfish.embeddables.LifeTime;
import com.intel.podm.common.types.Id;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
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
@Table(name = "memory_metrics", indexes = @Index(name = "idx_memory_metrics_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class MemoryMetrics extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "temperature_celsius")
    private BigDecimal temperatureCelsius;

    @Column(name = "block_size_bytes")
    private Long blockSizeBytes;

    @Column(name = "bandwidth_percent")
    private BigDecimal bandwidthPercent;

    @Column(name = "throttled_cycles_percent")
    private BigDecimal throttledCyclesPercent;

    @Column(name = "thermal_margin_celsius")
    private BigDecimal thermalMarginCelsius;

    @Column(name = "consumed_power_watt")
    private BigDecimal consumedPowerWatt;

    @Column(name = "ecc_errors_count")
    private Long eccErrorsCount;

    @Embedded
    private HealthData healthData;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "blocksRead", column = @Column(name = "current_period_blocks_read")),
        @AttributeOverride(name = "blocksWritten", column = @Column(name = "current_period_blocks_written"))
    })
    private CurrentPeriod currentPeriod;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "blocksRead", column = @Column(name = "life_time_blocks_read")),
        @AttributeOverride(name = "blocksWritten", column = @Column(name = "life_time_blocks_written"))
    })
    private LifeTime lifeTime;

    @ElementCollection
    @CollectionTable(name = "memory_metrics_health", joinColumns = @JoinColumn(name = "memory_metrics_id"))
    @Column(name = "health")
    @OrderColumn(name = "health_order")
    private List<String> healthList = new ArrayList<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "memory_id")
    private Memory memory;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id entityId) {
        this.entityId = entityId;
    }

    public BigDecimal getTemperatureCelsius() {
        return temperatureCelsius;
    }

    public void setTemperatureCelsius(BigDecimal temperatureCelsius) {
        this.temperatureCelsius = temperatureCelsius;
    }

    public Long getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(Long blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public BigDecimal getBandwidthPercent() {
        return bandwidthPercent;
    }

    public void setBandwidthPercent(BigDecimal bandwidthPercent) {
        this.bandwidthPercent = bandwidthPercent;
    }

    public BigDecimal getThrottledCyclesPercent() {
        return throttledCyclesPercent;
    }

    public void setThrottledCyclesPercent(BigDecimal throttledCyclesPercent) {
        this.throttledCyclesPercent = throttledCyclesPercent;
    }

    public BigDecimal getThermalMarginCelsius() {
        return thermalMarginCelsius;
    }

    public void setThermalMarginCelsius(BigDecimal thermalMarginCelsius) {
        this.thermalMarginCelsius = thermalMarginCelsius;
    }

    public BigDecimal getConsumedPowerWatt() {
        return consumedPowerWatt;
    }

    public void setConsumedPowerWatt(BigDecimal consumedPowerWatt) {
        this.consumedPowerWatt = consumedPowerWatt;
    }

    public Long getEccErrorsCount() {
        return eccErrorsCount;
    }

    public void setEccErrorsCount(Long eccErrorsCount) {
        this.eccErrorsCount = eccErrorsCount;
    }

    public HealthData getHealthData() {
        return healthData;
    }

    public void setHealthData(HealthData healthData) {
        this.healthData = healthData;
    }

    public CurrentPeriod getCurrentPeriod() {
        return currentPeriod;
    }

    public void setCurrentPeriod(CurrentPeriod currentPeriod) {
        this.currentPeriod = currentPeriod;
    }

    public LifeTime getLifeTime() {
        return lifeTime;
    }

    public void setLifeTime(LifeTime lifeTime) {
        this.lifeTime = lifeTime;
    }

    public List<String> getHealthList() {
        return healthList;
    }

    public void addHealth(String health) {
        this.healthList.add(health);
    }

    public Memory getMemory() {
        return memory;
    }

    public void setMemory(Memory memory) {
        if (!Objects.equals(this.memory, memory)) {
            unlinkMemory(this.memory);
            this.memory = memory;
            if (memory != null && !this.equals(memory.getMemoryMetrics())) {
                memory.setMemoryMetrics(this);
            }
        }
    }

    public void unlinkMemory(Memory memory) {
        if (Objects.equals(this.memory, memory)) {
            this.memory = null;
            if (memory != null) {
                memory.unlinkMemoryMetrics(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkMemory(memory);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, memory);
    }
}
