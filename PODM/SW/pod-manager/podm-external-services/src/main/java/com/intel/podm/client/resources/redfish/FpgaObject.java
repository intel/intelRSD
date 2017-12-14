/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.common.types.FpgaType;
import com.intel.podm.common.types.HssiConfig;
import com.intel.podm.common.types.HssiSideband;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class FpgaObject {
    @JsonProperty("Type")
    @AsUnassigned(WHEN_NULL)
    private Ref<FpgaType> type = unassigned();
    @JsonProperty("BitStreamVersion")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> bitStreamVersion = unassigned();
    @JsonProperty("HSSIConfiguration")
    @AsUnassigned(WHEN_NULL)
    private Ref<HssiConfig> hssiConfiguration = unassigned();
    @JsonProperty("HSSISideband")
    @AsUnassigned(WHEN_NULL)
    private Ref<HssiSideband> hssiSideband = unassigned();
    @JsonProperty("ReconfigurationSlots")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> reconfigurationSlots = unassigned();

    public Ref<FpgaType> getType() {
        return type;
    }

    public Ref<String> getBitStreamVersion() {
        return bitStreamVersion;
    }

    public Ref<HssiConfig> getHssiConfiguration() {
        return hssiConfiguration;
    }

    public Ref<HssiSideband> getHssiSideband() {
        return hssiSideband;
    }

    public Ref<Integer> getReconfigurationSlots() {
        return reconfigurationSlots;
    }
}
