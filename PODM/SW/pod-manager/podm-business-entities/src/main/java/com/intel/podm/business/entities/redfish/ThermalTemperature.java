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

import com.intel.podm.business.entities.EventOriginProvider;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.PhysicalContext;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "thermal_temperature", indexes = @Index(name = "idx_thermal_temperature_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class ThermalTemperature extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "member_id")
    private String memberId;

    @Column(name = "sensor_number")
    private Integer sensorNumber;

    @Column(name = "reading_celsius")
    private BigDecimal readingCelsius;

    @Column(name = "upper_threshold_non_critical")
    private BigDecimal upperThresholdNonCritical;

    @Column(name = "upper_threshold_critical")
    private BigDecimal upperThresholdCritical;

    @Column(name = "upper_threshold_fatal")
    private BigDecimal upperThresholdFatal;

    @Column(name = "lower_threshold_non_critical")
    private BigDecimal lowerThresholdNonCritical;

    @Column(name = "lower_threshold_critical")
    private BigDecimal lowerThresholdCritical;

    @Column(name = "lower_threshold_fatal")
    private BigDecimal lowerThresholdFatal;

    @Column(name = "min_reading_range")
    private BigDecimal minReadingRange;

    @Column(name = "max_reading_range")
    private BigDecimal maxReadingRange;

    @Column(name = "physical_context")
    private PhysicalContext physicalContext;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "thermal_id")
    private Thermal thermal;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public Integer getSensorNumber() {
        return sensorNumber;
    }

    public void setSensorNumber(Integer sensorNumber) {
        this.sensorNumber = sensorNumber;
    }

    public BigDecimal getReadingCelsius() {
        return readingCelsius;
    }

    public void setReadingCelsius(BigDecimal readingCelsius) {
        this.readingCelsius = readingCelsius;
    }

    public BigDecimal getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public void setUpperThresholdNonCritical(BigDecimal upperThresholdNonCritical) {
        this.upperThresholdNonCritical = upperThresholdNonCritical;
    }

    public BigDecimal getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public void setUpperThresholdCritical(BigDecimal upperThresholdCritical) {
        this.upperThresholdCritical = upperThresholdCritical;
    }

    public BigDecimal getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public void setUpperThresholdFatal(BigDecimal upperThresholdFatal) {
        this.upperThresholdFatal = upperThresholdFatal;
    }

    public BigDecimal getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public void setLowerThresholdNonCritical(BigDecimal lowerThresholdNonCritical) {
        this.lowerThresholdNonCritical = lowerThresholdNonCritical;
    }

    public BigDecimal getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public void setLowerThresholdCritical(BigDecimal lowerThresholdCritical) {
        this.lowerThresholdCritical = lowerThresholdCritical;
    }

    public BigDecimal getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public void setLowerThresholdFatal(BigDecimal lowerThresholdFatal) {
        this.lowerThresholdFatal = lowerThresholdFatal;
    }

    public BigDecimal getMinReadingRange() {
        return minReadingRange;
    }

    public void setMinReadingRange(BigDecimal minReadingRange) {
        this.minReadingRange = minReadingRange;
    }

    public BigDecimal getMaxReadingRange() {
        return maxReadingRange;
    }

    public void setMaxReadingRange(BigDecimal maxReadingRange) {
        this.maxReadingRange = maxReadingRange;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public void setPhysicalContext(PhysicalContext physicalContext) {
        this.physicalContext = physicalContext;
    }

    @EventOriginProvider
    public Thermal getThermal() {
        return thermal;
    }

    public void setThermal(Thermal thermal) {
        if (!Objects.equals(this.thermal, thermal)) {
            unlinkThermal(this.thermal);
            this.thermal = thermal;
            if (thermal != null && !thermal.getTemperatures().contains(this)) {
                thermal.addTemperature(this);
            }
        }
    }

    public void unlinkThermal(Thermal thermal) {
        if (Objects.equals(this.thermal, thermal)) {
            this.thermal = null;
            if (thermal != null) {
                thermal.unlinkTemperature(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkThermal(thermal);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, thermal);
    }
}
