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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.PowerInputType;

import java.math.BigDecimal;

public final class InputRangeDto {
    private final PowerInputType inputType;
    private final BigDecimal minimumVoltage;
    private final BigDecimal maximumVoltage;
    private final BigDecimal minimumFrequencyHz;
    private final BigDecimal maximumFrequencyHz;
    private final BigDecimal outputWattage;
    private final UnknownOemDto oem;

    private InputRangeDto(Builder builder) {
        this.inputType = builder.inputType;
        this.minimumVoltage = builder.minimumVoltage;
        this.maximumVoltage = builder.maximumVoltage;
        this.minimumFrequencyHz = builder.minimumFrequencyHz;
        this.maximumFrequencyHz = builder.maximumFrequencyHz;
        this.outputWattage = builder.outputWattage;
        this.oem = builder.oem;
    }

    public PowerInputType getInputType() {
        return inputType;
    }

    public BigDecimal getMinimumVoltage() {
        return minimumVoltage;
    }

    public BigDecimal getMaximumVoltage() {
        return maximumVoltage;
    }

    public BigDecimal getMinimumFrequencyHz() {
        return minimumFrequencyHz;
    }

    public BigDecimal getMaximumFrequencyHz() {
        return maximumFrequencyHz;
    }

    public BigDecimal getOutputWattage() {
        return outputWattage;
    }

    public UnknownOemDto getOem() {
        return oem;
    }

    public static Builder newPowerSupplyDto() {
        return new Builder();
    }

    public static final class Builder {
        private PowerInputType inputType;
        private BigDecimal minimumVoltage;
        private BigDecimal maximumVoltage;
        private BigDecimal minimumFrequencyHz;
        private BigDecimal maximumFrequencyHz;
        private BigDecimal outputWattage;
        private UnknownOemDto oem;

        private Builder() { }

        public InputRangeDto build() {
            return new InputRangeDto(this);
        }

        public Builder inputType(PowerInputType inputType) {
            this.inputType = inputType;
            return this;
        }

        public Builder minimumVoltage(BigDecimal minimumVoltage) {
            this.minimumVoltage = minimumVoltage;
            return this;
        }

        public Builder maximumVoltage(BigDecimal maximumVoltage) {
            this.maximumVoltage = maximumVoltage;
            return this;
        }

        public Builder minimumFrequencyHz(BigDecimal minimumFrequencyHz) {
            this.minimumFrequencyHz = minimumFrequencyHz;
            return this;
        }

        public Builder maximumFrequencyHz(BigDecimal maximumFrequencyHz) {
            this.maximumFrequencyHz = maximumFrequencyHz;
            return this;
        }

        public Builder outputWattage(BigDecimal outputWattage) {
            this.outputWattage = outputWattage;
            return this;
        }

        public Builder oem(UnknownOemDto oem) {
            this.oem = oem;
            return this;
        }
    }
}
