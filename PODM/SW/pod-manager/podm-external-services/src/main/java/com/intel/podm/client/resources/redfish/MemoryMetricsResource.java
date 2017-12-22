/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({"#MemoryMetrics" + VERSION_PATTERN + "MemoryMetrics"})
public class MemoryMetricsResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("BlockSizeBytes")
    private Long blockSizeBytes;

    @JsonProperty("CurrentPeriod")
    private CurrentPeriodObject currentPeriod;

    @JsonProperty("LifeTime")
    private LifeTimeObject lifeTime;

    @JsonProperty("HealthData")
    private HealthDataObject healthData;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Long getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public CurrentPeriodObject getCurrentPeriod() {
        return currentPeriod;
    }

    public LifeTimeObject getLifeTime() {
        return lifeTime;
    }

    public HealthDataObject getHealthData() {
        return healthData;
    }

    public BigDecimal getTemperatureCelsius() {
        return oem.rackScaleOem.temperatureCelsius;
    }

    public BigDecimal getBandwidthPercent() {
        return oem.rackScaleOem.bandwidthPercent;
    }

    public BigDecimal getThrottledCyclesPercent() {
        return oem.rackScaleOem.throttledCyclesPercent;
    }

    public BigDecimal getThermalMarginCelsius() {
        return oem.rackScaleOem.thermalMarginCelsius;
    }

    public BigDecimal getConsumedPowerWatt() {
        return oem.rackScaleOem.consumedPowerWatt;
    }

    public Long getEccErrorsCount() {
        return oem.rackScaleOem.eccErrorsCount;
    }

    public List<String> getHealthList() {
        return oem.rackScaleOem.healthList;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("TemperatureCelsius")
            private BigDecimal temperatureCelsius;

            @JsonProperty("BandwidthPercent")
            private BigDecimal bandwidthPercent;

            @JsonProperty("ThrottledCyclesPercent")
            private BigDecimal throttledCyclesPercent;

            @JsonProperty("ThermalMarginCelsius")
            private BigDecimal thermalMarginCelsius;

            @JsonProperty("ConsumedPowerWatt")
            private BigDecimal consumedPowerWatt;

            @JsonProperty("ECCErrorsCount")
            private Long eccErrorsCount;

            @JsonProperty("Health")
            private List<String> healthList;
        }
    }
}
