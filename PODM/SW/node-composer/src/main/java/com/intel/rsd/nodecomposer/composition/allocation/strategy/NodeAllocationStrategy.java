/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationContext;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationStrategy;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.SetComposedNodeStateToAssembledTask;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.val;

import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATING;
import static com.intel.rsd.nodecomposer.types.Health.OK;
import static com.intel.rsd.nodecomposer.types.State.ENABLED;
import static java.util.Arrays.asList;

public class NodeAllocationStrategy {

    private final ComposedNodeDao composedNodeDao;
    private final SetComposedNodeStateToAssembledTask setComposedNodeStateToAssembledTask;
    private final ComputerSystemAllocationStrategy computerSystemAllocationStrategy;
    private final AllocationStrategy driveAllocationStrategy;

    public NodeAllocationStrategy(ComposedNodeDao composedNodeDao,
                                  SetComposedNodeStateToAssembledTask setComposedNodeStateToAssembledTask,
                                  ComputerSystemAllocationStrategy computerSystemAllocationStrategy,
                                  AllocationStrategy driveAllocationStrategy) {

        this.composedNodeDao = composedNodeDao;
        this.setComposedNodeStateToAssembledTask = setComposedNodeStateToAssembledTask;
        this.computerSystemAllocationStrategy = computerSystemAllocationStrategy;
        this.driveAllocationStrategy = driveAllocationStrategy;
    }

    public AllocationContext allocate(RequestedNode requestedNode) throws ResourceFinderException {
        val createdNode = createComposedNode(requestedNode);
        for (AllocationStrategy allocationStrategy : asList(computerSystemAllocationStrategy, driveAllocationStrategy)) {
            allocationStrategy.allocate(createdNode);
        }

        composedNodeDao.persist(createdNode);
        return new AllocationContext(createdNode.getUri(), getTasks());
    }

    private List<NodeTask> getTasks() {
        List<NodeTask> nodeTasks = new ArrayList<>();
        nodeTasks.addAll(computerSystemAllocationStrategy.getTasks());
        nodeTasks.addAll(driveAllocationStrategy.getTasks());
        nodeTasks.add(setComposedNodeStateToAssembledTask);
        return nodeTasks;
    }

    private ComposedNode createComposedNode(RequestedNode requestedNode) {
        ComposedNode composedNode = new ComposedNode();
        composedNode.setName(requestedNode.getName());
        composedNode.setDescription(requestedNode.getDescription());
        composedNode.setComposedNodeState(ALLOCATING);
        composedNode.setStatus(new Status(ENABLED, OK, OK));
        return composedNode;
    }

}
