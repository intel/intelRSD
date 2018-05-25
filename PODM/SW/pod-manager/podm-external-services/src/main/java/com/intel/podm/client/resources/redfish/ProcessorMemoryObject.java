/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.podm.common.types.ProcessorMemoryType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class ProcessorMemoryObject {
    @JsonSetter(value = "Type", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<ProcessorMemoryType> type;

    @JsonSetter(value = "CapacityMB", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> capacityMb;

    @JsonSetter(value = "SpeedMHz", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> speedMhz;

    public Ref<ProcessorMemoryType> getType() {
        return type;
    }

    public Ref<Integer> getCapacityMb() {
        return capacityMb;
    }

    public Ref<Integer> getSpeedMhz() {
        return speedMhz;
    }
}
