/*
 * Copyright (c) 2015-2017 Intel Corporation
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
@Table(name = "power_zone_power_supply")
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class PowerZonePowerSupply extends Entity {
    @Column(name = "name")
    private String name;

    @Column(name = "status")
    private Status status;

    @Column(name = "power_capacity_watts")
    private Integer powerCapacityWatts;

    @Column(name = "last_power_output_watts")
    private Integer lastPowerOutputWatts;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model_number")
    private String modelNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "firmware_revision")
    private String firmwareRevision;

    @Embedded
    private RackLocation rackLocation;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "power_zone_id")
    private PowerZone powerZone;

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

    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public void setPowerCapacityWatts(Integer powerCapacityWatts) {
        this.powerCapacityWatts = powerCapacityWatts;
    }

    public Integer getLastPowerOutputWatt() {
        return lastPowerOutputWatts;
    }

    public void setLastPowerOutputWatt(Integer lastPowerOutputWatt) {
        this.lastPowerOutputWatts = lastPowerOutputWatt;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModelNumber() {
        return modelNumber;
    }

    public void setModelNumber(String model) {
        this.modelNumber = model;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    public void setFirmwareRevision(String firmwareRevision) {
        this.firmwareRevision = firmwareRevision;
    }

    public RackLocation getRackLocation() {
        return rackLocation;
    }

    public void setRackLocation(RackLocation rackLocation) {
        this.rackLocation = rackLocation;
    }

    @EventOriginProvider
    public PowerZone getPowerZone() {
        return powerZone;
    }

    public void setPowerZone(PowerZone powerZone) {
        if (!Objects.equals(this.powerZone, powerZone)) {
            unlinkPowerZone(this.powerZone);
            this.powerZone = powerZone;
            if (powerZone != null && !powerZone.getPowerSupplies().contains(this)) {
                powerZone.addPowerSupply(this);
            }
        }
    }

    public void unlinkPowerZone(PowerZone powerZone) {
        if (Objects.equals(this.powerZone, powerZone)) {
            this.powerZone = null;
            if (powerZone != null) {
                powerZone.unlinkPowerSupply(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkPowerZone(powerZone);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, powerZone);
    }
}
