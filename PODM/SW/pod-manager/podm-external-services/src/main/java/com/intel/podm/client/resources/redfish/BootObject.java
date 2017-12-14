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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import java.util.LinkedHashSet;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class BootObject {
    @JsonProperty("BootSourceOverrideEnabled")
    @AsUnassigned(WHEN_NULL)
    private Ref<BootSourceState> bootSourceOverrideEnabled = unassigned();

    @JsonProperty("BootSourceOverrideTarget")
    @AsUnassigned(WHEN_NULL)
    private Ref<BootSourceType> bootSourceOverrideTarget = unassigned();

    @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
    private LinkedHashSet<BootSourceType> bootSourceOverrideTargetAllowableValues;

    @JsonProperty("BootSourceOverrideMode")
    @AsUnassigned(WHEN_NULL)
    private Ref<BootSourceMode> bootSourceOverrideMode = unassigned();

    @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
    private LinkedHashSet<BootSourceMode> bootSourceOverrideModeAllowableValues;

    public Ref<BootSourceState> getBootSourceOverrideEnabled() {
        return bootSourceOverrideEnabled;
    }

    public Ref<BootSourceType> getBootSourceOverrideTarget() {
        return bootSourceOverrideTarget;
    }

    public LinkedHashSet<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
        if (bootSourceOverrideTargetAllowableValues == null) {
            return new LinkedHashSet<>();
        }
        return bootSourceOverrideTargetAllowableValues;
    }

    public Ref<BootSourceMode> getBootSourceOverrideMode() {
        return bootSourceOverrideMode;
    }

    public LinkedHashSet<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
        if (bootSourceOverrideModeAllowableValues == null) {
            return new LinkedHashSet<>();
        }
        return bootSourceOverrideModeAllowableValues;
    }
}
