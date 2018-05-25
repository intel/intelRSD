/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class MemoryLocationObject {
    @JsonSetter(value = "Socket", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> socket = unassigned();

    @JsonSetter(value = "MemoryController", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> memoryController = unassigned();

    @JsonSetter(value = "Channel", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> channel = unassigned();

    @JsonSetter(value = "Slot", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> slot = unassigned();

    public Ref<Integer> getSocket() {
        return socket;
    }

    public Ref<Integer> getMemoryController() {
        return memoryController;
    }

    public Ref<Integer> getChannel() {
        return channel;
    }

    public Ref<Integer> getSlot() {
        return slot;
    }
}
