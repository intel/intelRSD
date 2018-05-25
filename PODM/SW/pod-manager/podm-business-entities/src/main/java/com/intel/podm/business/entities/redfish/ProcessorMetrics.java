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
@Table(name = "processor_metrics", indexes = @Index(name = "idx_processor_metrics_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class ProcessorMetrics extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "bandwidth_percent")
    private BigDecimal bandwidthPercent;

    @Column(name = "average_frequency_mhz")
    private Integer averageFrequencyMhz;

    @Column(name = "throttling_celsius")
    private Integer throttlingCelsius;

    @Column(name = "temperature_celsius")
    private Integer temperatureCelsius;

    @Column(name = "consumed_power_watt")
    private Integer consumedPowerWatt;

    @ElementCollection
    @CollectionTable(name = "processor_metrics_health", joinColumns = @JoinColumn(name = "processor_metrics_id"))
    @Column(name = "health")
    @OrderColumn(name = "health_order")
    private List<String> healthList = new ArrayList<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "processor_id")
    private Processor processor;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public BigDecimal getBandwidthPercent() {
        return bandwidthPercent;
    }

    public void setBandwidthPercent(BigDecimal bandwidthPercent) {
        this.bandwidthPercent = bandwidthPercent;
    }

    public Integer getAverageFrequencyMhz() {
        return averageFrequencyMhz;
    }

    public void setAverageFrequencyMhz(Integer averageFrequencyMhz) {
        this.averageFrequencyMhz = averageFrequencyMhz;
    }

    public Integer getThrottlingCelsius() {
        return throttlingCelsius;
    }

    public void setThrottlingCelsius(Integer throttlingCelsius) {
        this.throttlingCelsius = throttlingCelsius;
    }

    public Integer getTemperatureCelsius() {
        return temperatureCelsius;
    }

    public void setTemperatureCelsius(Integer temperatureCelsius) {
        this.temperatureCelsius = temperatureCelsius;
    }

    public Integer getConsumedPowerWatt() {
        return consumedPowerWatt;
    }

    public void setConsumedPowerWatt(Integer consumedPowerWatt) {
        this.consumedPowerWatt = consumedPowerWatt;
    }

    public List<String> getHealthList() {
        return healthList;
    }

    public void addHealth(String health) {
        this.healthList.add(health);
    }

    public Processor getProcessor() {
        return processor;
    }

    public void setProcessor(Processor processor) {
        if (!Objects.equals(this.processor, processor)) {
            unlinkProcessor(this.processor);
            this.processor = processor;
            if (processor != null && !this.equals(processor.getProcessorMetrics())) {
                processor.setProcessorMetrics(this);
            }
        }
    }

    public void unlinkProcessor(Processor processor) {
        if (Objects.equals(this.processor, processor)) {
            this.processor = null;
            if (processor != null) {
                processor.unlinkProcessorMetrics(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkProcessor(processor);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, processor);
    }
}
