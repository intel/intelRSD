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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;

@Embeddable
@SuppressWarnings("checkstyle:MethodCount")
public class DriveMetricsLifeTime {
    @Column(name = "unit_size_bytes")
    private Long unitSizeBytes;

    @Column(name = "units_read")
    private BigDecimal unitsRead;

    @Column(name = "units_written")
    private BigDecimal unitsWritten;

    @Column(name = "host_read_commands")
    private BigDecimal hostReadCommands;

    @Column(name = "host_write_commands")
    private BigDecimal hostWriteCommands;

    @Column(name = "power_cycles")
    private BigDecimal powerCycles;

    @Column(name = "power_on_hours")
    private BigDecimal powerOnHours;

    @Column(name = "controller_busy_time_minutes")
    private BigDecimal controllerBusyTimeMinutes;

    public Long getUnitSizeBytes() {
        return unitSizeBytes;
    }

    public void setUnitSizeBytes(Long unitSizeBytes) {
        this.unitSizeBytes = unitSizeBytes;
    }

    public BigDecimal getUnitsRead() {
        return unitsRead;
    }

    public void setUnitsRead(BigDecimal unitsRead) {
        this.unitsRead = unitsRead;
    }

    public BigDecimal getUnitsWritten() {
        return unitsWritten;
    }

    public void setUnitsWritten(BigDecimal unitsWritten) {
        this.unitsWritten = unitsWritten;
    }

    public BigDecimal getHostReadCommands() {
        return hostReadCommands;
    }

    public void setHostReadCommands(BigDecimal hostReadCommands) {
        this.hostReadCommands = hostReadCommands;
    }

    public BigDecimal getHostWriteCommands() {
        return hostWriteCommands;
    }

    public void setHostWriteCommands(BigDecimal hostWriteCommands) {
        this.hostWriteCommands = hostWriteCommands;
    }

    public BigDecimal getPowerCycles() {
        return powerCycles;
    }

    public void setPowerCycles(BigDecimal powerCycles) {
        this.powerCycles = powerCycles;
    }

    public BigDecimal getPowerOnHours() {
        return powerOnHours;
    }

    public void setPowerOnHours(BigDecimal powerOnHours) {
        this.powerOnHours = powerOnHours;
    }

    public BigDecimal getControllerBusyTimeMinutes() {
        return controllerBusyTimeMinutes;
    }

    public void setControllerBusyTimeMinutes(BigDecimal controllerBusyTimeMinutes) {
        this.controllerBusyTimeMinutes = controllerBusyTimeMinutes;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        DriveMetricsLifeTime that = (DriveMetricsLifeTime) o;
        return new EqualsBuilder()
            .append(unitSizeBytes, that.unitSizeBytes)
            .append(unitsRead, that.unitsRead)
            .append(unitsWritten, that.unitsWritten)
            .append(hostReadCommands, that.hostReadCommands)
            .append(hostWriteCommands, that.hostWriteCommands)
            .append(powerCycles, that.powerCycles)
            .append(powerOnHours, that.powerOnHours)
            .append(controllerBusyTimeMinutes, that.controllerBusyTimeMinutes)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(unitSizeBytes)
            .append(unitsRead)
            .append(unitsWritten)
            .append(hostReadCommands)
            .append(hostWriteCommands)
            .append(powerCycles)
            .append(powerOnHours)
            .append(controllerBusyTimeMinutes)
            .toHashCode();
    }
}
