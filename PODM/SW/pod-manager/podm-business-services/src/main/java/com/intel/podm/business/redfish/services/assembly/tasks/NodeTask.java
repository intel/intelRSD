/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasuredClassIdentityProvider;
import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.synchronization.ThrowingRunnable;
import com.intel.podm.common.types.Id;

import java.util.UUID;

import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static org.apache.commons.lang3.StringUtils.substringBefore;

@TimeMeasuredClassIdentityProvider(NodeTaskClassIdentityProvider.class)
// Child classes are obligated to rethrow RuntimeException on exception to ensure transaction will be rolled back
public abstract class NodeTask extends DefaultManagedTask implements ThrowingRunnable<RuntimeException> {
    protected Id nodeId;

    public void setNodeId(Id nodeId) {
        this.nodeId = nodeId;
    }

    protected ComputerSystem getComputerSystemFromNode(ComposedNode node) {
        return ofNullable(node.getComputerSystem()).orElseThrow(() ->
            new RuntimeException(format("Unknown Computer system for Node: %s", node.getId())));
    }

    protected UUID getAssociatedComputeServiceUuid(ComposedNode node) {
        return ofNullable(node.getAssociatedComputeServiceUuid()).orElseThrow(() ->
            new RuntimeException(format("Unknown Computer system for Node: %s", node.getId())));
    }

    public abstract UUID getServiceUuid();

    @Override
    public String toString() {
        return format("%s(#%s)", substringBefore(getClass().getSimpleName(), "$"), nodeId);
    }
}
