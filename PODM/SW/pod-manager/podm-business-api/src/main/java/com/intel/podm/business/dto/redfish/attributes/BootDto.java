/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;

import java.util.LinkedList;
import java.util.List;

public final class BootDto {
    private BootSourceState bootSourceOverrideEnabled;
    private BootSourceType bootSourceOverrideTarget;
    private List<BootSourceType> bootSourceOverrideTargetAllowableValues = new LinkedList<>();

    private BootDto(Builder builder) {
        bootSourceOverrideEnabled = builder.bootSourceOverrideEnabled;
        bootSourceOverrideTarget = builder.bootSourceOverrideTarget;
        bootSourceOverrideTargetAllowableValues = builder.bootSourceOverrideTargetAllowableValues;
    }

    public BootSourceState getBootSourceOverrideEnabled() {
        return bootSourceOverrideEnabled;
    }

    public BootSourceType getBootSourceOverrideTarget() {
        return bootSourceOverrideTarget;
    }

    public List<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
        return bootSourceOverrideTargetAllowableValues;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private BootSourceState bootSourceOverrideEnabled;
        private BootSourceType bootSourceOverrideTarget;
        private List<BootSourceType> bootSourceOverrideTargetAllowableValues = new LinkedList<>();

        private Builder() {
        }

        public Builder bootSourceOverrideEnabled(BootSourceState bootSourceOverrideEnabled) {
            this.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
            return this;
        }

        public Builder bootSourceOverrideTarget(BootSourceType bootSourceOverrideTarget) {
            this.bootSourceOverrideTarget = bootSourceOverrideTarget;
            return this;
        }

        public Builder bootSourceOverrideTargetAllowableValues(List<BootSourceType> bootSourceOverrideTargetAllowableValues) {
            if (bootSourceOverrideTargetAllowableValues != null) {
                this.bootSourceOverrideTargetAllowableValues = bootSourceOverrideTargetAllowableValues;
            }
            return this;
        }

        public BootDto build() {
            return new BootDto(this);
        }
    }
}
