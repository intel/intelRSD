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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;

import java.math.BigDecimal;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({"#DriveMetrics" + VERSION_PATTERN + "DriveMetrics"})
public class DriveMetricsResource extends ExternalServiceResourceImpl {
    @JsonProperty("TemperatureKelvin")
    private BigDecimal temperatureKelvin;

    @JsonProperty("LifeTime")
    private LifeTime lifeTime;

    @JsonProperty("HealthData")
    private HealthData healthData;

    public BigDecimal getTemperatureKelvin() {
        return temperatureKelvin;
    }

    public LifeTime getLifeTime() {
        return lifeTime;
    }

    public HealthData getHealthData() {
        return healthData;
    }

    public static class LifeTime {
        @JsonProperty("UnitSizeBytes")
        private Long unitSizeBytes;
        @JsonProperty("UnitsRead")
        private BigDecimal unitsRead;
        @JsonProperty("UnitsWritten")
        private BigDecimal unitsWritten;
        @JsonProperty("HostReadCommands")
        private BigDecimal hostReadCommands;
        @JsonProperty("HostWriteCommands")
        private BigDecimal hostWriteCommands;
        @JsonProperty("PowerCycles")
        private BigDecimal powerCycles;
        @JsonProperty("PowerOnHours")
        private BigDecimal powerOnHours;
        @JsonProperty("ControllerBusyTimeMinutes")
        private BigDecimal controllerBusyTimeMinutes;

        public Long getUnitSizeBytes() {
            return unitSizeBytes;
        }

        public BigDecimal getUnitsRead() {
            return unitsRead;
        }

        public BigDecimal getUnitsWritten() {
            return unitsWritten;
        }

        public BigDecimal getHostReadCommands() {
            return hostReadCommands;
        }

        public BigDecimal getHostWriteCommands() {
            return hostWriteCommands;
        }

        public BigDecimal getPowerCycles() {
            return powerCycles;
        }

        public BigDecimal getPowerOnHours() {
            return powerOnHours;
        }

        public BigDecimal getControllerBusyTimeMinutes() {
            return controllerBusyTimeMinutes;
        }
    }

    public static class HealthData {
        @JsonProperty("AvailableSparePercentage")
        private BigDecimal availableSparePercentage;
        @JsonProperty("PredictedMediaLifeUsedPercent")
        private BigDecimal predictedMediaLifeUsedPercent;
        @JsonProperty("UnsafeShutdowns")
        private BigDecimal unsafeShutdowns;
        @JsonProperty("MediaErrors")
        private BigDecimal mediaErrors;

        public BigDecimal getAvailableSparePercentage() {
            return availableSparePercentage;
        }

        public BigDecimal getPredictedMediaLifeUsedPercent() {
            return predictedMediaLifeUsedPercent;
        }

        public BigDecimal getUnsafeShutdowns() {
            return unsafeShutdowns;
        }

        public BigDecimal getMediaErrors() {
            return mediaErrors;
        }
    }
}
