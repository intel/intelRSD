/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.assembly.tasks;


import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.types.Id;

import static java.lang.String.format;
import static java.util.Optional.of;


public abstract class NodeAssemblyTask extends DefaultManagedTask implements Runnable {
    protected Id nodeId;

    public void setNodeId(Id nodeId) {
        this.nodeId = nodeId;
    }

    @Override
    public abstract void run();

    protected ComputerSystem getComputerSystemFromNode(ComposedNode node) {
        return of(node.getComputerSystem()).orElseThrow(() ->
                new RuntimeException(format("Unknown Computer system for Node: %s", node.getId())));
    }
}
