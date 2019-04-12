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

package com.intel.rsd.nodecomposer.rest.redfish.json.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import lombok.Getter;

import java.util.Map;

@Getter
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ComposedNodePartialRepresentation {
    @JsonProperty("Boot")
    public Boot boot;

    @JsonProperty("ClearTPMOnDelete")
    public Boolean clearTpmOnDelete;

    @JsonProperty("ClearOptaneDCPersistentMemoryOnDelete")
    public Boolean clearOptanePersistentMemoryOnDelete;

    @JsonProperty("Oem")
    public Oem oem;

    @Getter
    public static class Boot {
        @JsonProperty("BootSourceOverrideTarget")
        public String bootSourceOverrideTarget;

        @JsonProperty("BootSourceOverrideEnabled")
        public BootSourceState bootSourceOverrideEnabled;

        @JsonProperty("BootSourceOverrideMode")
        public BootSourceMode bootSourceOverrideMode;
    }

    @Getter
    public static class Oem {
        @JsonProperty("Intel_RackScale")
        public IntelRackScale intelRackScale;

        @Getter
        public static class IntelRackScale {
            @JsonProperty("TaggedValues")
            public Map<String, JsonNode> taggedValues;
        }
    }
}
