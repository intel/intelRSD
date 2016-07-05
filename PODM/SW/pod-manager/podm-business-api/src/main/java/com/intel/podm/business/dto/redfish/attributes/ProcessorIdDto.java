/*
 * Copyright (c) 2016 Intel Corporation
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

public final class ProcessorIdDto {
    private final String vendorId;
    private final String identificationRegisters;
    private final String effectiveFamily;
    private final String effectiveModel;
    private final String step;
    private final String microcodeInfo;

    private ProcessorIdDto(Builder builder) {
        vendorId = builder.vendorId;
        identificationRegisters = builder.identificationRegisters;
        effectiveFamily = builder.effectiveFamily;
        effectiveModel = builder.effectiveModel;
        step = builder.step;
        microcodeInfo = builder.microcodeInfo;
    }

    public String getVendorId() {
        return vendorId;
    }

    public String getIdentificationRegisters() {
        return identificationRegisters;
    }

    public String getEffectiveFamily() {
        return effectiveFamily;
    }

    public String getEffectiveModel() {
        return effectiveModel;
    }

    public String getStep() {
        return step;
    }

    public String getMicrocodeInfo() {
        return microcodeInfo;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String vendorId;
        private String identificationRegisters;
        private String effectiveFamily;
        private String effectiveModel;
        private String step;
        private String microcodeInfo;

        public Builder vendorId(String val) {
            vendorId = val;
            return this;
        }

        public Builder identificationRegisters(String val) {
            identificationRegisters = val;
            return this;
        }

        public Builder effectiveFamily(String val) {
            effectiveFamily = val;
            return this;
        }

        public Builder effectiveModel(String val) {
            effectiveModel = val;
            return this;
        }

        public Builder step(String val) {
            step = val;
            return this;
        }

        public Builder microcodeInfo(String val) {
            microcodeInfo = val;
            return this;
        }

        public ProcessorIdDto build() {
            return new ProcessorIdDto(this);
        }
    }
}
