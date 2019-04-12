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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import lombok.Getter;
import lombok.Setter;

import java.util.List;

public abstract class ResourceAllocationStrategy {
    @Getter
    @Setter
    private ODataId oDataId;

    public abstract Violations validate();
    public abstract List<NodeTask> allocate(ComposedNode composedNode);
}
