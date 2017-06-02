/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.assembly.NodeDisassembler;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.assembly.tasks.NodeTasksCoordinator;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.synchronization.ThrowingRunnable;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.concurrent.TimeoutException;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class ComposedNodeRemovalServiceImpl implements RemovalService<ComposedNodeDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NodeDisassembler nodeDisassembler;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private NodeTasksCoordinator nodeTasksCoordinator;

    @Override
    @Transactional(REQUIRES_NEW)
    public void perform(Context target) throws BusinessApiException, TimeoutException {
        taskCoordinator.runThrowing(target, (ThrowingRunnable) () -> {
            ComposedNode composedNode = (ComposedNode) traverser.traverse(target);
            Collection<NodeAssemblyTask> tasks = nodeDisassembler.getDisassemblyTasks(composedNode.getId());

            for (NodeAssemblyTask task : tasks) {
                taskCoordinator.run(task.getServiceUuid(), task);
            }
            nodeTasksCoordinator.removeAllTasks(target.getId());
        });
    }
}
