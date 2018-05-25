/*
 * Copyright (c) 2016-2018 Intel Corporation
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

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "ioBandwidthGbps", "processorBandwidthPercent", "memoryBandwidthPercent",
    "memoryThrottledCyclesPercent", "processorPowerWatt", "memoryPowerWatt", "healthList", "oem"})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ComputerSystemMetricsDto extends RedfishDto {
    @JsonProperty("IOBandwidthGBps")
    private BigDecimal ioBandwidthGbps;
    private BigDecimal processorBandwidthPercent;
    private BigDecimal memoryBandwidthPercent;
    private BigDecimal processorPowerWatt;
    private BigDecimal memoryPowerWatt;
    private BigDecimal memoryThrottledCyclesPercent;
    @JsonProperty("Health")
    private List<String> healthList;

    public ComputerSystemMetricsDto() {
        super("#ComputerSystemMetrics.v1_0_0.ComputerSystemMetrics");
    }

    public BigDecimal getIoBandwidthGbps() {
        return ioBandwidthGbps;
    }

    public void setIoBandwidthGbps(BigDecimal ioBandwidthGbps) {
        this.ioBandwidthGbps = ioBandwidthGbps;
    }

    public BigDecimal getProcessorBandwidthPercent() {
        return processorBandwidthPercent;
    }

    public void setProcessorBandwidthPercent(BigDecimal processorBandwidthPercent) {
        this.processorBandwidthPercent = processorBandwidthPercent;
    }

    public BigDecimal getMemoryBandwidthPercent() {
        return memoryBandwidthPercent;
    }

    public void setMemoryBandwidthPercent(BigDecimal memoryBandwidthPercent) {
        this.memoryBandwidthPercent = memoryBandwidthPercent;
    }

    public BigDecimal getProcessorPowerWatt() {
        return processorPowerWatt;
    }

    public void setProcessorPowerWatt(BigDecimal processorPowerWatt) {
        this.processorPowerWatt = processorPowerWatt;
    }

    public BigDecimal getMemoryPowerWatt() {
        return memoryPowerWatt;
    }

    public void setMemoryPowerWatt(BigDecimal memoryPowerWatt) {
        this.memoryPowerWatt = memoryPowerWatt;
    }

    public BigDecimal getMemoryThrottledCyclesPercent() {
        return memoryThrottledCyclesPercent;
    }

    public void setMemoryThrottledCyclesPercent(BigDecimal memoryThrottledCyclesPercent) {
        this.memoryThrottledCyclesPercent = memoryThrottledCyclesPercent;
    }

    public List<String> getHealthList() {
        return healthList;
    }

    public void setHealthList(List<String> healthList) {
        this.healthList = healthList;
    }
}
