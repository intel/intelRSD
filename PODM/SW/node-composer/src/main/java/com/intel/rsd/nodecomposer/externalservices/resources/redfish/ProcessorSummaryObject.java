/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

public class ProcessorSummaryObject {
    @JsonSetter(value = "Count", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> count = unassigned();
    @JsonSetter(value = "Model", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> model = unassigned();
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    public Ref<Integer> getCount() {
        return count;
    }

    public Ref<String> getModel() {
        return model;
    }

    public Ref<Status> getStatus() {
        return status;
    }
}
