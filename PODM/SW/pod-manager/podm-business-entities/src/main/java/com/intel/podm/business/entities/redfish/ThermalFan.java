/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "thermal_fan", indexes = @Index(name = "idx_thermal_fan_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class ThermalFan extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "member_id")
    private String memberId;

    @Column(name = "physical_context")
    @Enumerated(STRING)
    private PhysicalContext physicalContext;

    @Column(name = "reading")
    private Integer reading;

    @Column(name = "reading_units")
    private String readingUnits;

    @Column(name = "upper_threshold_non_critical")
    private Integer upperThresholdNonCritical;

    @Column(name = "upper_threshold_critical")
    private Integer upperThresholdCritical;

    @Column(name = "upper_threshold_fatal")
    private Integer upperThresholdFatal;

    @Column(name = "lower_threshold_non_critical")
    private Integer lowerThresholdNonCritical;

    @Column(name = "lower_threshold_critical")
    private Integer lowerThresholdCritical;

    @Column(name = "lower_threshold_fatal")
    private Integer lowerThresholdFatal;

    @Column(name = "min_reading_range")
    private Integer minReadingRange;

    @Column(name = "max_reading_range")
    private Integer maxReadingRange;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "thermal_id")
    private Thermal thermal;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public void setPhysicalContext(PhysicalContext physicalContext) {
        this.physicalContext = physicalContext;
    }

    public Integer getReading() {
        return reading;
    }

    public void setReading(Integer reading) {
        this.reading = reading;
    }

    public String getReadingUnits() {
        return readingUnits;
    }

    public void setReadingUnits(String readingUnits) {
        this.readingUnits = readingUnits;
    }

    public Integer getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public void setUpperThresholdNonCritical(Integer upperThresholdNonCritical) {
        this.upperThresholdNonCritical = upperThresholdNonCritical;
    }

    public Integer getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public void setUpperThresholdCritical(Integer upperThresholdCritical) {
        this.upperThresholdCritical = upperThresholdCritical;
    }

    public Integer getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public void setUpperThresholdFatal(Integer upperThresholdFatal) {
        this.upperThresholdFatal = upperThresholdFatal;
    }

    public Integer getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public void setLowerThresholdNonCritical(Integer lowerThresholdNonCritical) {
        this.lowerThresholdNonCritical = lowerThresholdNonCritical;
    }

    public Integer getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public void setLowerThresholdCritical(Integer lowerThresholdCritical) {
        this.lowerThresholdCritical = lowerThresholdCritical;
    }

    public Integer getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public void setLowerThresholdFatal(Integer lowerThresholdFatal) {
        this.lowerThresholdFatal = lowerThresholdFatal;
    }

    public Integer getMinReadingRange() {
        return minReadingRange;
    }

    public void setMinReadingRange(Integer minReadingRange) {
        this.minReadingRange = minReadingRange;
    }

    public Integer getMaxReadingRange() {
        return maxReadingRange;
    }

    public void setMaxReadingRange(Integer maxReadingRange) {
        this.maxReadingRange = maxReadingRange;
    }

    @EventOriginProvider
    public Thermal getThermal() {
        return thermal;
    }

    public void setThermal(Thermal thermal) {
        if (!Objects.equals(this.thermal, thermal)) {
            unlinkThermal(this.thermal);
            this.thermal = thermal;
            if (thermal != null && !thermal.getFans().contains(this)) {
                thermal.addFan(this);
            }
        }

        this.thermal = thermal;
    }

    public void unlinkThermal(Thermal thermal) {
        if (Objects.equals(this.thermal, thermal)) {
            this.thermal = null;
            if (thermal != null) {
                thermal.unlinkFan(this);
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
