/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.dto.ComposedNodeDto;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.assembly.NodeDisassembler;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeRemovalTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTasksCoordinator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.synchronization.ThrowingRunnable;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class ComposedNodeRemovalServiceImpl implements RemovalService<ComposedNodeDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NodeDisassembler nodeDisassembler;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private NodeTasksCoordinator nodeTasksCoordinator;

    @Inject
    private Logger logger;

    @Inject
    @Config
    private Holder<ServiceConfig> config;

    @Override
    @Transactional(REQUIRES_NEW)
    @SuppressWarnings({"unchecked"})
    public void perform(Context target) throws BusinessApiException, TimeoutException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(target);

        taskCoordinator.run(composedNode.getAssociatedComputerSystemUuid(), (ThrowingRunnable) () -> {
            ComposedNodeState composedNodeState = composedNode.getComposedNodeState();
            Collection<NodeTask> tasks = nodeDisassembler.getDisassemblyTasks(composedNode.getId());
            for (NodeTask task : tasks) {
                taskRunnerWrapper(task, composedNodeState);
            }
            nodeTasksCoordinator.removeAllTasks(target.getId());
        });
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private void taskRunnerWrapper(NodeTask task, ComposedNodeState composedNodeState) throws TimeoutException {
        try {
            taskCoordinator.run(task.getServiceUuid(), task);
        } catch (RuntimeException e) {
            if (FAILED.equals(composedNodeState)) {
                logger.e(format("Error while running task %s", task), e);
                if (task instanceof NodeRemovalTask) {
                    logger.i(format("Invoking forced deallocate for task %s.", task));
                    ((NodeRemovalTask) task).deallocate();
                }
            } else {
                throw e;
            }
        }
    }
}
