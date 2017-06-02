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
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.common.types.Status;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "fan")
@Eventable
public class Fan extends Entity {
    @Column(name = "name")
    private String name;

    @Column(name = "status")
    private Status status;

    @Column(name = "reading_rpm")
    private Integer readingRpm;

    @Embedded
    private RackLocation rackLocation;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "thermal_zone_id")
    private ThermalZone thermalZone;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Integer getReadingRpm() {
        return readingRpm;
    }

    public void setReadingRpm(Integer readingRpm) {
        this.readingRpm = readingRpm;
    }

    public RackLocation getRackLocation() {
        return rackLocation;
    }

    public void setRackLocation(RackLocation rackLocation) {
        this.rackLocation = rackLocation;
    }

    @EventOriginProvider
    public ThermalZone getThermalZone() {
        return thermalZone;
    }

    public void setThermalZone(ThermalZone thermalZone) {
        if (!Objects.equals(this.thermalZone, thermalZone)) {
            unlinkThermalZone(this.thermalZone);
            this.thermalZone = thermalZone;
            if (thermalZone != null && !thermalZone.getFans().contains(this)) {
                thermalZone.addFan(this);
            }
        }
    }

    public void unlinkThermalZone(ThermalZone thermalZone) {
        if (Objects.equals(this.thermalZone, thermalZone)) {
            this.thermalZone = null;
            if (thermalZone != null) {
                thermalZone.unlinkFan(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkThermalZone(thermalZone);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, thermalZone);
    }
}
