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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import java.util.HashSet;
import java.util.Set;

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ComputerSystemPartialRepresentation implements RedfishComputerSystem {

    @JsonProperty("AssetTag")
    public String assetTag;

    @JsonProperty("Boot")
    public Boot boot;

    @JsonProperty("Oem")
    public Oem oem = new Oem();

    @JsonProperty("TrustedModules")
    public Set<TrustedModule> trustedModules = new HashSet<>();

    @Override
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public Boolean getUserModeEnabled() {
        return oem.rackScaleOem.userModeEnabled;
    }

    @Override
    public RedfishComputerSystem.Boot getBoot() {
        return boot;
    }

    public static class Boot implements RedfishComputerSystem.Boot {
        @JsonProperty("BootSourceOverrideTarget")
        public BootSourceType bootSourceOverrideTarget;

        @JsonProperty("BootSourceOverrideEnabled")
        public BootSourceState bootSourceOverrideEnabled;

        @JsonProperty("BootSourceOverrideMode")
        public BootSourceMode bootSourceOverrideMode;

        @Override
        public BootSourceType getBootSourceOverrideTarget() {
            return bootSourceOverrideTarget;
        }

        @Override
        public BootSourceState getBootSourceOverrideEnabled() {
            return bootSourceOverrideEnabled;
        }

        @Override
        public BootSourceMode getBootSourceOverrideMode() {
            return bootSourceOverrideMode;
        }

    }

    public static class Oem {

        @JsonProperty("Intel_RackScale")
        public ComputerSystemPartialRepresentation.Oem.RackScaleOem rackScaleOem = new ComputerSystemPartialRepresentation.Oem.RackScaleOem();

        public static class RackScaleOem {
            @JsonProperty("UserModeEnabled")
            public Boolean userModeEnabled;
        }

    }
}
