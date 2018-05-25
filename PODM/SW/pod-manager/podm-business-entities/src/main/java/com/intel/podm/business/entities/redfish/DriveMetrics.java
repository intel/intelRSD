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
import com.intel.podm.business.entities.redfish.embeddables.DriveMetricsHealthData;
import com.intel.podm.business.entities.redfish.embeddables.DriveMetricsLifeTime;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "drive_metrics", indexes = @Index(name = "idx_drive_metrics_entity_id", columnList = "entity_id", unique = true))
public class DriveMetrics extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "temperature_kelvin")
    private BigDecimal temperatureKelvin;

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "drive_id")
    private Drive drive;

    @Embedded
    private DriveMetricsLifeTime lifeTime;

    @Embedded
    private DriveMetricsHealthData healthData;

    public Drive getDrive() {
        return drive;
    }

    public void setDrive(Drive drive) {
        if (!Objects.equals(this.drive, drive)) {
            unlinkDrive(this.drive);
            this.drive = drive;
            if (drive != null && !this.equals(drive.getMetrics())) {
                drive.setMetrics(this);
            }
        }
    }

    public void unlinkDrive(Drive drive) {
        if (Objects.equals(this.drive, drive)) {
            this.drive = null;
            if (drive != null) {
                drive.unlinkMetrics(this);
            }
        }
    }

    public DriveMetricsLifeTime getLifeTime() {
        return lifeTime;
    }

    public void setLifeTime(DriveMetricsLifeTime lifeTime) {
        this.lifeTime = lifeTime;
    }

    public DriveMetricsHealthData getHealthData() {
        return healthData;
    }

    public void setHealthData(DriveMetricsHealthData healthData) {
        this.healthData = healthData;
    }

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public BigDecimal getTemperatureKelvin() {
        return temperatureKelvin;
    }

    public void setTemperatureKelvin(BigDecimal temperatureKelvin) {
        this.temperatureKelvin = temperatureKelvin;
    }

    @Override
    public void preRemove() {
        unlinkDrive(drive);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, drive);
    }
}
