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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.math.BigDecimal;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "temperatureKelvin", "lifeTime", "healthData", "oem"})
@JsonIgnoreProperties({"Oem"})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class DriveMetricsDto extends RedfishDto {
    private BigDecimal temperatureKelvin;

    @JsonInclude(NON_NULL)
    private LifeTimeDto lifeTime;

    @JsonInclude(NON_NULL)
    private HealthDataDto healthData;

    public DriveMetricsDto() {
        super("#DriveMetrics.v1_0_0.DriveMetrics");
    }

    public BigDecimal getTemperatureKelvin() {
        return temperatureKelvin;
    }

    public void setTemperatureKelvin(BigDecimal temperatureKelvin) {
        this.temperatureKelvin = temperatureKelvin;
    }

    public LifeTimeDto getLifeTime() {
        return lifeTime;
    }

    public void setLifeTime(LifeTimeDto lifeTime) {
        this.lifeTime = lifeTime;
    }

    public HealthDataDto getHealthData() {
        return healthData;
    }

    public void setHealthData(HealthDataDto healthData) {
        this.healthData = healthData;
    }

    @JsonPropertyOrder({"unitSizeBytes", "unitsRead", "unitsWritten", "hostReadCommands", "hostWriteCommands", "powerCycles",
        "powerOnHours", "controllerBusyTimeMinutes"})
    public static class LifeTimeDto {
        private Long unitSizeBytes;

        private BigDecimal unitsRead;

        private BigDecimal unitsWritten;

        private BigDecimal hostReadCommands;

        private BigDecimal hostWriteCommands;

        private BigDecimal powerCycles;

        private BigDecimal powerOnHours;

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
    }

    @JsonPropertyOrder({"availableSparePercentage", "predictedMediaLifeUsedPercent", "unsafeShutdowns", "mediaErrors"})
    private static class HealthDataDto {
        private BigDecimal availableSparePercentage;

        private BigDecimal predictedMediaLifeUsedPercent;

        private BigDecimal unsafeShutdowns;

        private BigDecimal mediaErrors;

        public BigDecimal getAvailableSparePercentage() {
            return availableSparePercentage;
        }

        public void setAvailableSparePercentage(BigDecimal availableSparePercentage) {
            this.availableSparePercentage = availableSparePercentage;
        }

        public BigDecimal getPredictedMediaLifeUsedPercent() {
            return predictedMediaLifeUsedPercent;
        }

        public void setPredictedMediaLifeUsedPercent(BigDecimal predictedMediaLifeUsedPercent) {
            this.predictedMediaLifeUsedPercent = predictedMediaLifeUsedPercent;
        }

        public BigDecimal getUnsafeShutdowns() {
            return unsafeShutdowns;
        }

        public void setUnsafeShutdowns(BigDecimal unsafeShutdowns) {
            this.unsafeShutdowns = unsafeShutdowns;
        }

        public BigDecimal getMediaErrors() {
            return mediaErrors;
        }

        public void setMediaErrors(BigDecimal mediaErrors) {
            this.mediaErrors = mediaErrors;
        }
    }
}
