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

package com.intel.podm.client.actions.requests;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public class ComputerSystemUpdateRequest {

    @JsonProperty("AssetTag")
    private String assetTag;

    @JsonProperty("Boot")
    private BootSourceOverride bootSourceOverride;

    public ComputerSystemUpdateRequest(ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
        this.assetTag = computerSystemUpdateDefinition.getAssetTag();
        this.bootSourceOverride = new BootSourceOverride(computerSystemUpdateDefinition);
    }

    @JsonInclude(NON_NULL)
    private class BootSourceOverride {

        @JsonProperty("BootSourceOverrideEnabled")
        private BootSourceState bootSourceState;

        @JsonProperty("BootSourceOverrideTarget")
        private BootSourceType bootSourceType;

        @JsonProperty("BootSourceOverrideMode")
        private BootSourceMode bootSourceMode;

        BootSourceOverride(ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
            this.bootSourceState = computerSystemUpdateDefinition.getBootSourceState();
            this.bootSourceType = computerSystemUpdateDefinition.getBootSourceType();
            this.bootSourceMode = computerSystemUpdateDefinition.getBootSourceMode();
        }
    }
}
