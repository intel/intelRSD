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
import com.intel.podm.business.entities.listeners.ThermalZoneListener;
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
@Table(name = "thermal_zone", indexes = @Index(name = "idx_thermal_zone_entity_id", columnList = "entity_id", unique = true))
@EntityListeners(ThermalZoneListener.class)
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class ThermalZone extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "volumetric_airflow")
    private Integer volumetricAirflow;

    @Column(name = "number_of_fans_present")
    private Integer numberOfFansPresent;

    @Column(name = "presence")
    private String presence;

    @Column(name = "desired_speed_rpm")
    private Integer desiredSpeedRpm;

    @Column(name = "desired_speed_pwm")
    private Integer desiredSpeedPwm;

    @Column(name = "max_fans_supported")
    private Integer maxFansSupported;

    @Embedded
    private RackLocation rackLocation;

    @OneToMany(mappedBy = "thermalZone", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Fan> fans = new HashSet<>();

    @OneToMany(mappedBy = "thermalZone", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Temperature> temperatures = new HashSet<>();

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

    public Integer getVolumetricAirflow() {
        return volumetricAirflow;
    }

    public void setVolumetricAirflow(Integer volumetricAirflow) {
        this.volumetricAirflow = volumetricAirflow;
    }

    public Integer getNumberOfFansPresent() {
        return numberOfFansPresent;
    }

    public void setNumberOfFansPresent(Integer numberOfFansPresent) {
        this.numberOfFansPresent = numberOfFansPresent;
    }

    public String getPresence() {
        return presence;
    }

    public void setPresence(String presence) {
        this.presence = presence;
    }

    public Integer getDesiredSpeedRpm() {
        return desiredSpeedRpm;
    }

    public void setDesiredSpeedRpm(Integer desiredSpeedRpm) {
        this.desiredSpeedRpm = desiredSpeedRpm;
    }

    public Integer getDesiredSpeedPwm() {
        return desiredSpeedPwm;
    }

    public void setDesiredSpeedPwm(Integer desiredSpeedPwm) {
        this.desiredSpeedPwm = desiredSpeedPwm;
    }

    public Integer getMaxFansSupported() {
        return maxFansSupported;
    }

    public void setMaxFansSupported(Integer maxFansSupported) {
        this.maxFansSupported = maxFansSupported;
    }

    public RackLocation getRackLocation() {
        return rackLocation;
    }

    public void setRackLocation(RackLocation rackLocation) {
        this.rackLocation = rackLocation;
    }

    public Set<Fan> getFans() {
        return fans;
    }

    public void addFan(Fan fan) {
        requiresNonNull(fan, "fan");

        fans.add(fan);
        if (!this.equals(fan.getThermalZone())) {
            fan.setThermalZone(this);
        }
    }

    public void unlinkFan(Fan fan) {
        if (fans.contains(fan)) {
            fans.remove(fan);
            if (fan != null) {
                fan.unlinkThermalZone(this);
            }
        }
    }

    public Set<Temperature> getTemperatures() {
        return temperatures;
    }

    public void addTemperature(Temperature temperature) {
        requiresNonNull(temperature, "temperature");

        temperatures.add(temperature);
        if (!this.equals(temperature.getThermalZone())) {
            temperature.setThermalZone(this);
        }
    }

    public void unlinkTemperature(Temperature temperature) {
        if (temperatures.contains(temperature)) {
            temperatures.remove(temperature);
            if (temperature != null) {
                temperature.unlinkThermalZone(this);
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
            if (chassis != null && !chassis.getThermalZones().contains(this)) {
                chassis.addThermalZone(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkThermalZone(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(fans, this::unlinkFan);
        unlinkCollection(temperatures, this::unlinkTemperature);
        unlinkChassis(chassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
