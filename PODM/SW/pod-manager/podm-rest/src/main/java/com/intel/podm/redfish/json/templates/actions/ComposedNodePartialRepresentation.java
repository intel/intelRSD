/*
 * Copyright (c) 2015-2018 Intel Corporation
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

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ComposedNodePartialRepresentation implements RedfishComputerSystem {

    @JsonProperty("Boot")
    public Boot boot;

    @JsonProperty("ClearTPMOnDelete")
    public Boolean clearTpmOnDelete;

    public RedfishComputerSystem.Boot getBoot() {
        return boot;
    }

    public Boolean getClearTpmOnDelete() {
        return clearTpmOnDelete;
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
}
