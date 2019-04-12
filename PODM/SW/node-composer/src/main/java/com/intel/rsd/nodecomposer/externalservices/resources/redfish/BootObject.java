/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;

import java.util.LinkedHashSet;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class BootObject {
    @JsonSetter(value = "BootSourceOverrideEnabled", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<BootSourceState> bootSourceOverrideEnabled = unassigned();

    @JsonSetter(value = "BootSourceOverrideTarget", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> bootSourceOverrideTarget = unassigned();

    @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
    private LinkedHashSet<String> bootSourceOverrideTargetAllowableValues;

    @JsonSetter(value = "BootSourceOverrideMode", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<BootSourceMode> bootSourceOverrideMode = unassigned();

    @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
    private LinkedHashSet<BootSourceMode> bootSourceOverrideModeAllowableValues;

    public Ref<BootSourceState> getBootSourceOverrideEnabled() {
        return bootSourceOverrideEnabled;
    }

    public Ref<String> getBootSourceOverrideTarget() {
        return bootSourceOverrideTarget;
    }

    public LinkedHashSet<String> getBootSourceOverrideTargetAllowableValues() {
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
