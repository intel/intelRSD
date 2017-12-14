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

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.PowerInputType;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;

@JsonPropertyOrder({
    "inputType", "minimumVoltage", "maximumVoltage", "minimumFrequencyHz",
    "maximumFrequencyHz", "outputWattage", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class InputRangeDto {
    private PowerInputType inputType;
    private BigDecimal minimumVoltage;
    private BigDecimal maximumVoltage;
    private BigDecimal minimumFrequencyHz;
    private BigDecimal maximumFrequencyHz;
    private BigDecimal outputWattage;
    @IgnoreAutomaticOem
    private JsonNode oem;

    public PowerInputType getInputType() {
        return inputType;
    }

    public void setInputType(PowerInputType inputType) {
        this.inputType = inputType;
    }

    public BigDecimal getMinimumVoltage() {
        return minimumVoltage;
    }

    public void setMinimumVoltage(BigDecimal minimumVoltage) {
        this.minimumVoltage = minimumVoltage;
    }

    public BigDecimal getMaximumVoltage() {
        return maximumVoltage;
    }

    public void setMaximumVoltage(BigDecimal maximumVoltage) {
        this.maximumVoltage = maximumVoltage;
    }

    public BigDecimal getMinimumFrequencyHz() {
        return minimumFrequencyHz;
    }

    public void setMinimumFrequencyHz(BigDecimal minimumFrequencyHz) {
        this.minimumFrequencyHz = minimumFrequencyHz;
    }

    public BigDecimal getMaximumFrequencyHz() {
        return maximumFrequencyHz;
    }

    public void setMaximumFrequencyHz(BigDecimal maximumFrequencyHz) {
        this.maximumFrequencyHz = maximumFrequencyHz;
    }

    public BigDecimal getOutputWattage() {
        return outputWattage;
    }

    public void setOutputWattage(BigDecimal outputWattage) {
        this.outputWattage = outputWattage;
    }

    public JsonNode getOem() {
        return oem;
    }

    public void setOem(JsonNode oem) {
        this.oem = oem;
    }
}
