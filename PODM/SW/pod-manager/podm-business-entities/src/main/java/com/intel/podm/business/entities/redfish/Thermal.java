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
import javax.persistence.ManyToMany;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "thermal", indexes = @Index(name = "idx_thermal_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class Thermal extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "desired_speed_pwm")
    private Integer desiredSpeedPwm;

    @Column(name = "volumetric_air_flow_cfm")
    private Integer volumetricAirflowCfm;

    @OneToMany(mappedBy = "thermal", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ThermalTemperature> temperatures = new HashSet<>();

    @OneToMany(mappedBy = "thermal", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ThermalFan> fans = new HashSet<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @ManyToMany(mappedBy = "cooledBy", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> cooledChassis = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Integer getDesiredSpeedPwm() {
        return desiredSpeedPwm;
    }

    public void setDesiredSpeedPwm(Integer desiredSpeedPwm) {
        this.desiredSpeedPwm = desiredSpeedPwm;
    }

    public Integer getVolumetricAirflowCfm() {
        return volumetricAirflowCfm;
    }

    public void setVolumetricAirflowCfm(Integer volumetricAirflowCfm) {
        this.volumetricAirflowCfm = volumetricAirflowCfm;
    }

    public Set<ThermalTemperature> getTemperatures() {
        return temperatures;
    }

    public void addTemperature(ThermalTemperature temperature) {
        requiresNonNull(temperature, "temperature");

        temperatures.add(temperature);
        if (!this.equals(temperature.getThermal())) {
            temperature.setThermal(this);
        }
    }

    public void unlinkTemperature(ThermalTemperature temperature) {
        if (temperatures.contains(temperature)) {
            temperatures.remove(temperature);
            if (temperature != null) {
                temperature.unlinkThermal(this);
            }
        }
    }

    public Set<ThermalFan> getFans() {
        return fans;
    }

    public void addFan(ThermalFan fan) {
        requiresNonNull(fan, "fan");

        fans.add(fan);
        if (!this.equals(fan.getThermal())) {
            fan.setThermal(this);
        }
    }

    public void unlinkFan(ThermalFan fan) {
        if (fans.contains(fan)) {
            fans.remove(fan);
            if (fan != null) {
                fan.unlinkThermal(this);
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
            if (chassis != null && !this.equals(chassis.getThermal())) {
                chassis.setThermal(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkThermal(this);
            }
        }
    }

    public Set<Chassis> getCooledChassis() {
        return cooledChassis;
    }

    public void addCooledChassis(Chassis chassis) {
        requiresNonNull(chassis, "chassis");

        cooledChassis.add(chassis);
        if (!chassis.getCooledBy().contains(this)) {
            chassis.addCooledBy(this);
        }
    }

    public void unlinkCooledChassis(Chassis chassis) {
        if (cooledChassis.contains(chassis)) {
            cooledChassis.remove(chassis);
            if (chassis != null) {
                chassis.unlinkCooledBy(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(temperatures, this::unlinkTemperature);
        unlinkCollection(fans, this::unlinkFan);
        unlinkChassis(chassis);
        unlinkCollection(cooledChassis, this::unlinkCooledChassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }

}
