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

package com.intel.podm.assembly.tasks;


import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedExecutorService;
import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import java.util.List;
import java.util.concurrent.RejectedExecutionException;

@RequestScoped
public class NodeAssemblyTaskRunner {
    @Inject
    private Logger logger;

    @Resource
    private ManagedExecutorService executorService;

    @Inject
    private BeanFactory beanFactory;

    public void submitAll(Id nodeId, List<NodeAssemblyTask> tasks) {
        NodeAssemblyTaskGroup nodeAssemblyTaskGroup = beanFactory.create(NodeAssemblyTaskGroup.class);
        nodeAssemblyTaskGroup.setNodeId(nodeId);
        nodeAssemblyTaskGroup.addTasks(tasks);

        try {
            executorService.submit(nodeAssemblyTaskGroup);
        } catch (RejectedExecutionException e) {
            logger.w("Tasks was rejected by the executor for node" + nodeId, e);
            throw e;
        }
    }
}
