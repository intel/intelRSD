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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
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
@Table(name = "power", indexes = @Index(name = "idx_power_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class Power extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @OneToMany(mappedBy = "power", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PowerControl> powerControls = new HashSet<>();

    @OneToMany(mappedBy = "power", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PowerVoltage> voltages = new HashSet<>();

    @OneToMany(mappedBy = "power", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PowerSupply> powerSupplies = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Set<PowerControl> getPowerControls() {
        return powerControls;
    }

    public void addPowerControl(PowerControl powerControl) {
        requiresNonNull(powerControl, "powerControl");

        powerControls.add(powerControl);
        if (!this.equals(powerControl.getPower())) {
            powerControl.setPower(this);
        }
    }

    public void unlinkPowerControl(PowerControl powerControl) {
        if (powerControls.contains(powerControl)) {
            powerControls.remove(powerControl);
            if (powerControl != null) {
                powerControl.unlinkPower(this);
            }
        }
    }

    public Set<PowerVoltage> getVoltages() {
        return voltages;
    }

    public void addVoltage(PowerVoltage voltage) {
        requiresNonNull(voltage, "voltage");

        voltages.add(voltage);
        if (!this.equals(voltage.getPower())) {
            voltage.setPower(this);
        }
    }

    public void unlinkVoltage(PowerVoltage voltage) {
        if (voltages.contains(voltage)) {
            voltages.remove(voltage);
            if (voltage != null) {
                voltage.unlinkPower(this);
            }
        }
    }

    public Set<PowerSupply> getPowerSupplies() {
        return powerSupplies;
    }

    public void addPowerSupply(PowerSupply powerSupply) {
        requiresNonNull(powerSupply, "powerSupply");

        powerSupplies.add(powerSupply);
        if (!this.equals(powerSupply.getPower())) {
            powerSupply.setPower(this);
        }
    }

    public void unlinkPowerSupply(PowerSupply powerSupply) {
        if (powerSupplies.contains(powerSupply)) {
            powerSupplies.remove(powerSupply);
            if (powerSupply != null) {
                powerSupply.unlinkPower(this);
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
            if (chassis != null && !this.equals(chassis.getPower())) {
                chassis.setPower(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkPower(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(powerControls, this::unlinkPowerControl);
        unlinkCollection(voltages, this::unlinkVoltage);
        unlinkCollection(powerSupplies, this::unlinkPowerSupply);
        unlinkChassis(chassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
