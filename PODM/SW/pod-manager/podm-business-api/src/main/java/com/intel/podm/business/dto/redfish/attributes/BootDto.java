/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.Set;

public final class BootDto implements RedfishComputerSystem.Boot {
    private BootSourceState bootSourceOverrideEnabled;
    private BootSourceType bootSourceOverrideTarget;
    private Set<BootSourceType> bootSourceOverrideTargetAllowableValues = new LinkedHashSet<>();
    private BootSourceMode bootSourceOverrideMode;
    private Set<BootSourceMode> bootSourceOverrideModeAllowableValues = new LinkedHashSet<>();

    private BootDto(Builder builder) {
        bootSourceOverrideEnabled = builder.bootSourceOverrideEnabled;
        bootSourceOverrideTarget = builder.bootSourceOverrideTarget;
        bootSourceOverrideTargetAllowableValues = builder.bootSourceOverrideTargetAllowableValues;
        bootSourceOverrideMode = builder.bootSourceOverrideMode;
        bootSourceOverrideModeAllowableValues = builder.bootSourceOverrideModeAllowableValues;
    }

    @Override
    public BootSourceState getBootSourceOverrideEnabled() {
        return bootSourceOverrideEnabled;
    }

    @Override
    public BootSourceType getBootSourceOverrideTarget() {
        return bootSourceOverrideTarget;
    }

    @Override
    public Set<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
        return bootSourceOverrideTargetAllowableValues;
    }

    @Override
    public BootSourceMode getBootSourceOverrideMode() {
        return bootSourceOverrideMode;
    }

    @Override
    public Set<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
        return bootSourceOverrideModeAllowableValues;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private BootSourceState bootSourceOverrideEnabled;
        private BootSourceType bootSourceOverrideTarget;
        private Set<BootSourceType> bootSourceOverrideTargetAllowableValues = new LinkedHashSet<>();
        private BootSourceMode bootSourceOverrideMode;
        private Set<BootSourceMode> bootSourceOverrideModeAllowableValues = new LinkedHashSet<>();

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

        public Builder bootSourceOverrideTargetAllowableValues(Collection<BootSourceType> bootSourceOverrideTargetAllowableValues) {
            if (bootSourceOverrideTargetAllowableValues != null) {
                this.bootSourceOverrideTargetAllowableValues.addAll(bootSourceOverrideTargetAllowableValues);
            }
            return this;
        }

        public Builder bootSourceOverrideMode(BootSourceMode bootSourceOverrideMode) {
            this.bootSourceOverrideMode = bootSourceOverrideMode;
            return this;
        }

        public Builder bootSourceOverrideModeAllowableValues(Collection<BootSourceMode> bootSourceOverrideModeAllowableValues) {
            if (bootSourceOverrideModeAllowableValues != null) {
                this.bootSourceOverrideModeAllowableValues.addAll(bootSourceOverrideModeAllowableValues);
            }
            return this;
        }

        public BootDto build() {
            return new BootDto(this);
        }
    }
}
