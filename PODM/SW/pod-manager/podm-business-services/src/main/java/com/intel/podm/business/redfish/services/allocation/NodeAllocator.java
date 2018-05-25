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

package com.intel.podm.business.redfish.services.allocation;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.allocation.strategy.AllocationStrategy;
import com.intel.podm.business.redfish.services.allocation.strategy.AllocationStrategyFactory;
import com.intel.podm.business.redfish.services.allocation.strategy.ResourceFinderException;
import com.intel.podm.business.redfish.services.assembly.tasks.ContinuouslyOverrideBootSourceTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.PowerOffTask;
import com.intel.podm.business.redfish.services.assembly.tasks.SetComposedNodeStateToAssembledTask;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static java.util.Arrays.asList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
public class NodeAllocator {
    @Inject
    private AllocationStrategyFactory allocationStrategyFactory;

    @Inject
    private BeanFactory beanFactory;

    private List<NodeTask> assemblyTasks = new ArrayList<>();

    @RetryOnRollback(50)
    @Transactional(REQUIRES_NEW)
    public ComposedNode compose(RequestedNode requestedNode) throws AllocationRequestProcessingException, ResourceFinderException {
        AllocationStrategy allocationStrategy = allocationStrategyFactory.create(requestedNode);
        allocationStrategy.validate();

        ComputerSystem computerSystemResource = allocationStrategy.findComputerSystemResource();
        ComposedNode node = allocationStrategy.allocateWithComputerSystem(computerSystemResource);

        List<NodeTask> tasks = new ArrayList<>(allocationStrategy.getTasks());
        tasks.addAll(createDefaultTasks());
        this.assemblyTasks = tasks;

        return node;
    }

    private List<NodeTask> createDefaultTasks() {
        return asList(
            beanFactory.create(PowerOffTask.class),
            beanFactory.create(ContinuouslyOverrideBootSourceTask.class),
            beanFactory.create(SetComposedNodeStateToAssembledTask.class)
        );
    }

    public List<NodeTask> getAssemblyTasks() {
        return assemblyTasks;
    }
}
