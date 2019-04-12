/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation;

import com.google.common.base.MoreObjects;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import lombok.Getter;

import java.util.List;

@Getter
public class AllocationContext {

    private ODataId oDataId;
    private List<NodeTask> assemblyTasks;

    public AllocationContext(ODataId oDataId, List<NodeTask> assemblyTasks) {
        this.oDataId = oDataId;
        this.assemblyTasks = assemblyTasks;
    }

    @Override
    public String toString() {
        return MoreObjects.toStringHelper(this)
            .add("oDataId", oDataId)
            .add("assemblyTasks", assemblyTasks)
            .toString();
    }
}
