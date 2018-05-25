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
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({"#ComputerSystemMetrics" + VERSION_PATTERN + "ComputerSystemMetrics"})
public class ComputerSystemMetricsResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("IOBandwidthGBps")
    private BigDecimal ioBandwidthGbps;
    @JsonProperty("ProcessorBandwidthPercent")
    private BigDecimal processorBandwidthPercent;
    @JsonProperty("MemoryBandwidthPercent")
    private BigDecimal memoryBandwidthPercent;
    @JsonProperty("ProcessorPowerWatt")
    private BigDecimal processorPowerWatt;
    @JsonProperty("MemoryPowerWatt")
    private BigDecimal memoryPowerWatt;
    @JsonProperty("MemoryThrottledCyclesPercent")
    private BigDecimal memoryThrottledCyclesPercent;
    @JsonProperty("Health")
    private List<String> healthList;

    public BigDecimal getIoBandwidthGbps() {
        return ioBandwidthGbps;
    }

    public BigDecimal getProcessorBandwidthPercent() {
        return processorBandwidthPercent;
    }

    public BigDecimal getMemoryBandwidthPercent() {
        return memoryBandwidthPercent;
    }

    public BigDecimal getProcessorPowerWatt() {
        return processorPowerWatt;
    }

    public BigDecimal getMemoryPowerWatt() {
        return memoryPowerWatt;
    }

    public BigDecimal getMemoryThrottledCyclesPercent() {
        return memoryThrottledCyclesPercent;
    }

    public List<String> getHealthList() {
        return healthList;
    }
}
