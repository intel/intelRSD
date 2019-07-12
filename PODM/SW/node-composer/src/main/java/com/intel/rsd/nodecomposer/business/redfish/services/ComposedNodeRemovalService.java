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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.NodeDisassembler;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeRemovalTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTasksCoordinator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.List;

import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class ComposedNodeRemovalService {
    private final NodeDisassembler nodeDisassembler;
    private final NodeTasksCoordinator nodeTasksCoordinator;

    @Autowired
    public ComposedNodeRemovalService(NodeDisassembler nodeDisassembler, NodeTasksCoordinator nodeTasksCoordinator) {
        this.nodeDisassembler = nodeDisassembler;
        this.nodeTasksCoordinator = nodeTasksCoordinator;
    }

    public void delete(ODataId composedNodeODataId) throws BusinessApiException {
        List<NodeTask> tasks = nodeDisassembler.getDisassemblyTasks(composedNodeODataId);
        for (NodeTask task : tasks) {
            task.run();
        }
        nodeTasksCoordinator.removeAllTasks(composedNodeODataId);
    }

    @Transactional(value = REQUIRES_NEW, rollbackOn = BusinessApiException.class)
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    public void forceDelete(ODataId composedNodeODataId) throws BusinessApiException {
        List<NodeTask> tasks = nodeDisassembler.getDisassemblyTasks(composedNodeODataId);
        for (NodeTask task : tasks) {
            try {
                task.run();
            } catch (RuntimeException e) {
                log.error(format("Error while running task %s", task), e);
                if (task instanceof NodeRemovalTask) {
                    log.info(format("Invoking forced deallocate for task %s.", task));
                    ((NodeRemovalTask) task).deallocate();
                }
            }
        }
        nodeTasksCoordinator.removeAllTasks(composedNodeODataId);
    }
}
