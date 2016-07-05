/*
 * Copyright (c) 2016 Intel Corporation
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

import com.google.common.collect.Lists;
import com.intel.podm.assembly.AssemblyException;
import com.intel.podm.common.types.Id;

import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import java.util.List;
import java.util.Map;

import static com.google.common.collect.Maps.newHashMap;
import static javax.ejb.LockType.WRITE;

@Singleton
@Lock(WRITE)
public class NodeTasksCoordinator {
    @Inject
    private NodeAssemblyTaskRunner nodeAssemblyTaskRunner;

    private final Map<Id, List<NodeAssemblyTask>> tasks = newHashMap();

    public void setTasksForNode(Id composedNodeId, List<NodeAssemblyTask> tasksToAdd) {
        if (tasks.containsKey(composedNodeId)) {
            throw new IllegalStateException("Tasks for Composed Node [" + composedNodeId + "] have already been set.");
        }

        tasks.put(composedNodeId, Lists.newArrayList(tasksToAdd));
    }

    public void runTasks(Id composedNodeId) throws AssemblyException {
        if (!tasks.containsKey(composedNodeId)) {
            throw new AssemblyException("Composed Node [" + composedNodeId + "] hasn't been previously allocated.");
        }

        List<NodeAssemblyTask> tasksToRun = tasks.remove(composedNodeId);
        nodeAssemblyTaskRunner.submitAll(composedNodeId, tasksToRun);
    }

    public void removeAllTasks(Id composedNodeId) {
        tasks.remove(composedNodeId);
    }
}
