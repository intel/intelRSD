/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.allocation.strategy.ResourceFinderException;
import com.intel.podm.allocation.validation.RemoteDriveValidationException;
import com.intel.podm.assembly.tasks.ContinuouslyOverrideBootSourceTask;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.assembly.tasks.PowerOffTask;
import com.intel.podm.assembly.tasks.SetComposedNodeStateToAssembledTask;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static java.util.Arrays.asList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class NodeAllocator {
    @Inject
    private AllocationStrategyFactory allocationStrategyFactory;

    @Inject
    private BeanFactory beanFactory;

    private List<NodeAssemblyTask> assemblyTasks = new ArrayList<>();

    @Transactional(REQUIRES_NEW)
    public ComposedNode compose(RequestedNode requestedNode) throws RemoteDriveValidationException, AllocationRequestProcessingException,
                                                                    ResourceFinderException {
        AllocationStrategy allocationStrategy = allocationStrategyFactory.create(requestedNode);

        allocationStrategy.validate();
        ComputerSystem computerSystemResource = allocationStrategy.findComputerSystemResource();
        ComposedNode node = allocationStrategy.allocateWithComputerSystem(computerSystemResource);

        assemblyTasks.addAll(allocationStrategy.getTasks());
        assemblyTasks.addAll(createDefaultTasks());

        return node;
    }

    private List<NodeAssemblyTask> createDefaultTasks() {
        return asList(
            beanFactory.create(PowerOffTask.class),
            beanFactory.create(ContinuouslyOverrideBootSourceTask.class),
            beanFactory.create(SetComposedNodeStateToAssembledTask.class)
        );
    }

    public List<NodeAssemblyTask> getAssemblyTasks() {
        return assemblyTasks;
    }
}
