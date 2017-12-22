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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class ProcessorIdObject {
    @JsonProperty("VendorId")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> vendorId = Ref.unassigned();
    @JsonProperty("IdentificationRegisters")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> identificationRegisters = Ref.unassigned();
    @JsonProperty("EffectiveFamily")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> effectiveFamily = Ref.unassigned();
    @JsonProperty("EffectiveModel")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> effectiveModel = Ref.unassigned();
    @JsonProperty("Step")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> step = Ref.unassigned();
    @JsonProperty("MicrocodeInfo")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> microcodeInfo = Ref.unassigned();

    public Ref<String> getVendorId() {
        return vendorId;
    }

    public Ref<String> getIdentificationRegisters() {
        return identificationRegisters;
    }

    public Ref<String> getEffectiveFamily() {
        return effectiveFamily;
    }

    public Ref<String> getEffectiveModel() {
        return effectiveModel;
    }

    public Ref<String> getStep() {
        return step;
    }

    public Ref<String> getMicrocodeInfo() {
        return microcodeInfo;
    }
}
