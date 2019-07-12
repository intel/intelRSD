/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationContext;
import com.intel.rsd.nodecomposer.composition.allocation.ComposedNodeStateChanger;
import com.intel.rsd.nodecomposer.composition.allocation.ContextAlreadyRegisteredException;
import com.intel.rsd.nodecomposer.composition.assembly.AssemblyException;
import com.intel.rsd.nodecomposer.hazelcast.HazelcastHelper;
import com.intel.rsd.nodecomposer.utils.lock.Lock;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.List;
import java.util.Map;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;
import static com.intel.rsd.nodecomposer.utils.proxy.Unproxier.unproxy;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.SUPPORTS;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class NodeTasksCoordinator {

    private final NodeAssemblyTaskRunner nodeAssemblyTaskRunner;
    private final ComposedNodeStateChanger composedNodeStateChanger;
    private final HazelcastHelper hazelcastHelper;

    @Autowired
    public NodeTasksCoordinator(NodeAssemblyTaskRunner nodeAssemblyTaskRunner,
                                ComposedNodeStateChanger composedNodeStateChanger,
                                HazelcastHelper hazelcastHelper) {

        this.nodeAssemblyTaskRunner = nodeAssemblyTaskRunner;
        this.composedNodeStateChanger = composedNodeStateChanger;
        this.hazelcastHelper = hazelcastHelper;
    }

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (put operation)
     */
    @Lock
    @Transactional(SUPPORTS)
    public void register(AllocationContext allocationContext) throws ContextAlreadyRegisteredException {
        val contextId = allocationContext.getODataId();
        if (getTasks().containsKey(contextId)) {
            log.error("Allocation Context is already registered: {}", allocationContext);
            throw new ContextAlreadyRegisteredException("Internal server error");
        }

        getTasks().put(contextId, unproxy(allocationContext.getAssemblyTasks()));
        composedNodeStateChanger.change(contextId, ALLOCATED);
    }

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (remove operation)
     */
    @Lock
    @Transactional(SUPPORTS)
    public void runTasks(ODataId composedNodeODataId) throws AssemblyException {
        if (!getTasks().containsKey(composedNodeODataId)) {
            throw new AssemblyException("Composed Node [" + composedNodeODataId + "] hasn't been previously allocated.");
        }

        List<NodeTask> tasksToRun = getTasks().remove(composedNodeODataId);

        NodeAssemblyTaskChainBuilder.instance()
            .prepareTaskChain(tasksToRun)
            .forComposedNode(composedNodeODataId)
            .useExceptionHandler(e -> {
                log.error(format("Error when running task for ComposedNode: %s, details: %s", composedNodeODataId, e.getMessage()), e);
                composedNodeStateChanger.change(composedNodeODataId, FAILED);
            })
            .executeWith(nodeAssemblyTaskRunner);
    }

    /**
     * LockType.WRITE used due to concurrent access to assembly tasks map that modifies it (remove operation)
     */
    @Lock
    public void removeAllTasks(ODataId composedNodeODataId) {
        getTasks().remove(composedNodeODataId);
    }

    private Map<ODataId, List<NodeTask>> getTasks() {
        return hazelcastHelper.assemblyTaskMap();
    }
}
