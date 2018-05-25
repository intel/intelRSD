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
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.PowerInputType;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;

import static com.intel.podm.client.resources.UnknownOemsHelper.convertJsonNodeToString;

public class InputRangeObject {
    @JsonProperty("InputType")
    private PowerInputType inputType;

    @JsonProperty("MinimumVoltage")
    private BigDecimal minimumVoltage;

    @JsonProperty("MaximumVoltage")
    private BigDecimal maximumVoltage;

    @JsonProperty("OutputWattage")
    private BigDecimal outputWattage;

    @JsonProperty("MinimumFrequencyHz")
    private BigDecimal minimumFrequencyHz;

    @JsonProperty("MaximumFrequencyHz")
    private BigDecimal maximumFrequencyHz;

    @IgnoreAutomaticOem
    private String oem;

    public PowerInputType getInputType() {
        return inputType;
    }

    public BigDecimal getMinimumVoltage() {
        return minimumVoltage;
    }

    public BigDecimal getMaximumVoltage() {
        return maximumVoltage;
    }

    public BigDecimal getOutputWattage() {
        return outputWattage;
    }

    public BigDecimal getMinimumFrequencyHz() {
        return minimumFrequencyHz;
    }

    public BigDecimal getMaximumFrequencyHz() {
        return maximumFrequencyHz;
    }

    public String getOem() {
        return oem;
    }

    @JsonProperty("Oem")
    public void setOem(JsonNode jsonNode) throws JsonProcessingException {
        oem = convertJsonNodeToString(jsonNode);
    }
}
