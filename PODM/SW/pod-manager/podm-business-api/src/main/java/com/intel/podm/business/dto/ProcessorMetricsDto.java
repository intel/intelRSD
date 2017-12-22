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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.math.BigDecimal;
import java.util.List;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "bandwidthPercent", "averageFrequencyMhz",
    "throttlingCelsius", "temperatureCelsius", "consumedPowerWatt", "healthList", "oem"})
public final class ProcessorMetricsDto extends RedfishDto {

    @JsonProperty("BandwidthPercent")
    private BigDecimal bandwidthPercent;

    @JsonProperty("AverageFrequencyMHz")
    private Integer averageFrequencyMhz;

    @JsonProperty("ThrottlingCelsius")
    private Integer throttlingCelsius;

    @JsonProperty("TemperatureCelsius")
    private Integer temperatureCelsius;

    @JsonProperty("ConsumedPowerWatt")
    private Integer consumedPowerWatt;

    @JsonProperty("Health")
    private List<String> healthList;

    public ProcessorMetricsDto() {
        super("#ProcessorMetrics.v1_0_0.ProcessorMetrics");
    }

    public BigDecimal getBandwidthPercent() {
        return bandwidthPercent;
    }

    public void setBandwidthPercent(BigDecimal bandwidthPercent) {
        this.bandwidthPercent = bandwidthPercent;
    }

    public Integer getAverageFrequencyMhz() {
        return averageFrequencyMhz;
    }

    public void setAverageFrequencyMhz(Integer averageFrequencyMhz) {
        this.averageFrequencyMhz = averageFrequencyMhz;
    }

    public Integer getThrottlingCelsius() {
        return throttlingCelsius;
    }

    public void setThrottlingCelsius(Integer throttlingCelsius) {
        this.throttlingCelsius = throttlingCelsius;
    }

    public Integer getTemperatureCelsius() {
        return temperatureCelsius;
    }

    public void setTemperatureCelsius(Integer temperatureCelsius) {
        this.temperatureCelsius = temperatureCelsius;
    }

    public Integer getConsumedPowerWatt() {
        return consumedPowerWatt;
    }

    public void setConsumedPowerWatt(Integer consumedPowerWatt) {
        this.consumedPowerWatt = consumedPowerWatt;
    }

    public List<String> getHealthList() {
        return healthList;
    }

    public void setHealthList(List<String> healthList) {
        this.healthList = healthList;
    }
}
