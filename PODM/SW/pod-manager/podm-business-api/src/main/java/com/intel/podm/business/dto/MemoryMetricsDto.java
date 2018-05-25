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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "blockSizeBytes", "currentPeriod",
    "lifeTime", "healthData", "oem"})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class MemoryMetricsDto extends RedfishDto {
    private Long blockSizeBytes;
    private CurrentPeriodDto currentPeriod = new CurrentPeriodDto();
    private LifeTimeDto lifeTime = new LifeTimeDto();
    private HealthDataDto healthData = new HealthDataDto();

    private Oem oem = new Oem();

    public MemoryMetricsDto() {
        super("#MemoryMetrics.v1_1_0.MemoryMetrics");
    }

    public Long getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(Long blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public CurrentPeriodDto getCurrentPeriod() {
        return currentPeriod;
    }

    public void setCurrentPeriod(CurrentPeriodDto currentPeriod) {
        this.currentPeriod = currentPeriod;
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

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"remainingSpareBlockPercentage", "lastShutdownSuccess", "dataLossDetected", "performanceDegraded", "alarmTrips"})
    public static final class HealthDataDto {
        private BigDecimal remainingSpareBlockPercentage;
        private Boolean lastShutdownSuccess;
        private Boolean dataLossDetected;
        private Boolean performanceDegraded;
        private AlarmTripsDto alarmTrips = new AlarmTripsDto();

        public BigDecimal getRemainingSpareBlockPercentage() {
            return remainingSpareBlockPercentage;
        }

        public void setRemainingSpareBlockPercentage(BigDecimal remainingSpareBlockPercentage) {
            this.remainingSpareBlockPercentage = remainingSpareBlockPercentage;
        }

        public Boolean getLastShutdownSuccess() {
            return lastShutdownSuccess;
        }

        public void setLastShutdownSuccess(Boolean lastShutdownSuccess) {
            this.lastShutdownSuccess = lastShutdownSuccess;
        }

        public Boolean getDataLossDetected() {
            return dataLossDetected;
        }

        public void setDataLossDetected(Boolean dataLossDetected) {
            this.dataLossDetected = dataLossDetected;
        }

        public Boolean getPerformanceDegraded() {
            return performanceDegraded;
        }

        public void setPerformanceDegraded(Boolean performanceDegraded) {
            this.performanceDegraded = performanceDegraded;
        }

        public AlarmTripsDto getAlarmTrips() {
            return alarmTrips;
        }

        public void setAlarmTrips(AlarmTripsDto alarmTrips) {
            this.alarmTrips = alarmTrips;
        }
    }

    @JsonPropertyOrder({"temperature", "spareBlock", "uncorrectableEccError", "correctableEccError", "addressParityError"})
    public static final class AlarmTripsDto {
        private Boolean temperature;
        private Boolean spareBlock;
        @JsonProperty("UncorrectableECCError")
        private Boolean uncorrectableEccError;
        @JsonProperty("CorrectableECCError")
        private Boolean correctableEccError;
        private Boolean addressParityError;

        public Boolean getTemperature() {
            return temperature;
        }

        public void setTemperature(Boolean temperature) {
            this.temperature = temperature;
        }

        public Boolean getSpareBlock() {
            return spareBlock;
        }

        public void setSpareBlock(Boolean spareBlock) {
            this.spareBlock = spareBlock;
        }

        public Boolean getUncorrectableEccError() {
            return uncorrectableEccError;
        }

        public void setUncorrectableEccError(Boolean uncorrectableEccError) {
            this.uncorrectableEccError = uncorrectableEccError;
        }

        public Boolean getCorrectableEccError() {
            return correctableEccError;
        }

        public void setCorrectableEccError(Boolean correctableEccError) {
            this.correctableEccError = correctableEccError;
        }

        public Boolean getAddressParityError() {
            return addressParityError;
        }

        public void setAddressParityError(Boolean addressParityError) {
            this.addressParityError = addressParityError;
        }
    }

    @JsonPropertyOrder({"blocksRead", "blocksWritten"})
    public static final class LifeTimeDto {
        private Long blocksRead;
        private Long blocksWritten;

        public Long getBlocksRead() {
            return blocksRead;
        }

        public void setBlocksRead(Long blocksRead) {
            this.blocksRead = blocksRead;
        }

        public Long getBlocksWritten() {
            return blocksWritten;
        }

        public void setBlocksWritten(Long blocksWritten) {
            this.blocksWritten = blocksWritten;
        }
    }

    @JsonPropertyOrder({"blocksRead", "blocksWritten"})
    public static final class CurrentPeriodDto {
        private Long blocksRead;
        private Long blocksWritten;

        public Long getBlocksRead() {
            return blocksRead;
        }

        public void setBlocksRead(Long blocksRead) {
            this.blocksRead = blocksRead;
        }

        public Long getBlocksWritten() {
            return blocksWritten;
        }

        public void setBlocksWritten(Long blocksWritten) {
            this.blocksWritten = blocksWritten;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "temperatureCelsius", "bandwidthPercent", "throttledCyclesPercent", "thermalMarginCelsius", "consumedPowerWatt",
            "eccErrorsCount", "healthList"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.MemoryMetrics";
            private BigDecimal temperatureCelsius;
            private BigDecimal bandwidthPercent;
            private BigDecimal throttledCyclesPercent;
            private BigDecimal thermalMarginCelsius;
            private BigDecimal consumedPowerWatt;

            @JsonProperty("ECCErrorsCount")
            private Long eccErrorsCount;

            @JsonProperty("Health")
            private List<String> healthList;

            public String getOdataType() {
                return odataType;
            }

            public BigDecimal getTemperatureCelsius() {
                return temperatureCelsius;
            }

            public void setTemperatureCelsius(BigDecimal temperatureCelsius) {
                this.temperatureCelsius = temperatureCelsius;
            }

            public BigDecimal getBandwidthPercent() {
                return bandwidthPercent;
            }

            public void setBandwidthPercent(BigDecimal bandwidthPercent) {
                this.bandwidthPercent = bandwidthPercent;
            }

            public BigDecimal getThrottledCyclesPercent() {
                return throttledCyclesPercent;
            }

            public void setThrottledCyclesPercent(BigDecimal throttledCyclesPercent) {
                this.throttledCyclesPercent = throttledCyclesPercent;
            }

            public BigDecimal getThermalMarginCelsius() {
                return thermalMarginCelsius;
            }

            public void setThermalMarginCelsius(BigDecimal thermalMarginCelsius) {
                this.thermalMarginCelsius = thermalMarginCelsius;
            }

            public BigDecimal getConsumedPowerWatt() {
                return consumedPowerWatt;
            }

            public void setConsumedPowerWatt(BigDecimal consumedPowerWatt) {
                this.consumedPowerWatt = consumedPowerWatt;
            }

            public Long getEccErrorsCount() {
                return eccErrorsCount;
            }

            public void setEccErrorsCount(Long eccErrorsCount) {
                this.eccErrorsCount = eccErrorsCount;
            }

            public List<String> getHealthList() {
                return healthList;
            }

            public void setHealthList(List<String> healthList) {
                this.healthList = healthList;
            }
        }
    }
}
