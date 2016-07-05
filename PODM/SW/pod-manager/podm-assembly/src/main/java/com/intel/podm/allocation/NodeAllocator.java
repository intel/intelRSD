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

package com.intel.podm.allocation;

import com.intel.podm.allocation.strategy.AllocationStrategy;
import com.intel.podm.allocation.strategy.AllocationStrategyFactory;
import com.intel.podm.assembly.tasks.ContinuouslyOverrideBootSourceTask;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.assembly.tasks.NodeTasksCoordinator;
import com.intel.podm.assembly.tasks.PowerOffTask;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class NodeAllocator {
    @Inject
    private AllocationStrategyFactory allocationStrategyFactory;

    @Inject
    private NodeTasksCoordinator nodeTasksCoordinator;

    @Inject
    private ComposedNodeStateChanger composedNodeStateChanger;

    @Inject
    private BeanFactory beanFactory;

    private List<NodeAssemblyTask> tasks = newArrayList();

    @Transactional(REQUIRES_NEW)
    public ComposedNode compose(RequestedNode requestedNode) throws CompositionException {
        AllocationStrategy allocationStrategy = allocationStrategyFactory.create(requestedNode);

        allocationStrategy.validate();
        ComputerSystem computerSystemResource = allocationStrategy.findComputerSystemResource();
        ComposedNode node = allocationStrategy.allocateWithComputerSystem(computerSystemResource);

        tasks.addAll(allocationStrategy.getTasks());
        tasks.addAll(getDefaultTasks());

        return node;
    }

    public void allocate(Id nodeId) throws CompositionException {
        try {
            nodeTasksCoordinator.setTasksForNode(nodeId, tasks);
        } catch (IllegalStateException e) {
            throw new CompositionException("Composed Node [" + nodeId + "] has been already allocated.", e);
        }
        composedNodeStateChanger.change(nodeId, ALLOCATED);
    }

    private List<NodeAssemblyTask> getDefaultTasks() {
        return newArrayList(
                beanFactory.create(PowerOffTask.class),
                beanFactory.create(ContinuouslyOverrideBootSourceTask.class)
        );
    }
}
