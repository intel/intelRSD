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

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.listeners.PowerZoneListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.EntityListeners;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "power_zone", indexes = @Index(name = "idx_power_zone_entity_id", columnList = "entity_id", unique = true))
@EntityListeners(PowerZoneListener.class)
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class PowerZone extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "presence")
    private String presence;

    @Column(name = "power_capacity_watts")
    private Integer powerCapacityWatts;

    @Column(name = "power_consumed_watts")
    private Integer powerConsumedWatts;

    @Column(name = "power_output_watts")
    private Integer powerOutputWatts;

    @Column(name = "max_psus_supported")
    private Integer maxPsusSupported;

    @Column(name = "number_of_psus_present")
    private Integer numberOfPsusPresent;

    @Embedded
    private RackLocation rackLocation;

    @OneToMany(mappedBy = "powerZone", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PowerZonePowerSupply> powerSupplies = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getPresence() {
        return presence;
    }

    public void setPresence(String presence) {
        this.presence = presence;
    }

    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public void setPowerCapacityWatts(Integer powerCapacityWatts) {
        this.powerCapacityWatts = powerCapacityWatts;
    }

    public Integer getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    public void setPowerConsumedWatts(Integer powerConsumedWatts) {
        this.powerConsumedWatts = powerConsumedWatts;
    }

    public Integer getPowerOutputWatts() {
        return powerOutputWatts;
    }

    public void setPowerOutputWatts(Integer powerOutputWatts) {
        this.powerOutputWatts = powerOutputWatts;
    }

    public Integer getMaxPsusSupported() {
        return maxPsusSupported;
    }

    public void setMaxPsusSupported(Integer maxPsusSupported) {
        this.maxPsusSupported = maxPsusSupported;
    }

    public Integer getNumberOfPsusPresent() {
        return numberOfPsusPresent;
    }

    public void setNumberOfPsusPresent(Integer numberOfPsusPresent) {
        this.numberOfPsusPresent = numberOfPsusPresent;
    }

    public RackLocation getRackLocation() {
        return rackLocation;
    }

    public void setRackLocation(RackLocation rackLocation) {
        this.rackLocation = rackLocation;
    }

    public Set<PowerZonePowerSupply> getPowerSupplies() {
        return powerSupplies;
    }

    public void addPowerSupply(PowerZonePowerSupply powerZonePowerSupply) {
        requiresNonNull(powerZonePowerSupply, "powerZonePowerSupply");

        powerSupplies.add(powerZonePowerSupply);
        if (!this.equals(powerZonePowerSupply.getPowerZone())) {
            powerZonePowerSupply.setPowerZone(this);
        }
    }

    public void unlinkPowerSupply(PowerZonePowerSupply powerZonePowerSupply) {
        if (powerSupplies.contains(powerZonePowerSupply)) {
            powerSupplies.remove(powerZonePowerSupply);
            if (powerZonePowerSupply != null) {
                powerZonePowerSupply.unlinkPowerZone(this);
            }
        }
    }

    public Chassis getChassis() {
        return chassis;
    }

    public void setChassis(Chassis chassis) {
        if (!Objects.equals(this.chassis, chassis)) {
            unlinkChassis(this.chassis);
            this.chassis = chassis;
            if (chassis != null && !chassis.getPowerZones().contains(this)) {
                chassis.addPowerZone(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkPowerZone(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(powerSupplies, this::unlinkPowerSupply);
        unlinkChassis(chassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
