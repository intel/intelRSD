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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.redfish.services.allocation.ComposedNodeStateChanger;
import com.intel.podm.business.redfish.services.assembly.AssemblyException;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableList;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Singleton
public class NodeTasksCoordinator {
    private final Map<Id, List<NodeTask>> tasks = new HashMap<>();

    @Inject
    private NodeAssemblyTaskRunner nodeAssemblyTaskRunner;

    @Inject
    private Logger logger;

    @Inject
    private ComposedNodeStateChanger composedNodeStateChanger;

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (put operation)
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void setTasksForNode(Id composedNodeId, List<NodeTask> tasksToAdd) {
        if (tasks.containsKey(composedNodeId)) {
            throw new IllegalStateException("Tasks for Composed Node [" + composedNodeId + "] have already been set.");
        }

        tasks.put(composedNodeId, unmodifiableList(tasksToAdd));
    }

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (remove operation)
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void runTasks(Id composedNodeId) throws AssemblyException {
        if (!tasks.containsKey(composedNodeId)) {
            throw new AssemblyException("Composed Node [" + composedNodeId + "] hasn't been previously allocated.");
        }

        List<NodeTask> tasksToRun = tasks.remove(composedNodeId);

        NodeAssemblyTaskChainBuilder.instance()
            .prepareTaskChain(tasksToRun)
            .forComposedNode(composedNodeId)
            .useExceptionHandler(e -> {
                logger.e(format("Error when running task for ComposedNode: %s, details: %s", composedNodeId, e.getMessage()), e);
                composedNodeStateChanger.change(composedNodeId, FAILED);
            })
            .executeWith(nodeAssemblyTaskRunner);

    }

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (remove operation)
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void removeAllTasks(Id composedNodeId) {
        tasks.remove(composedNodeId);
    }
}
