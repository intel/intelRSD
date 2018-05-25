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

package com.intel.podm.business.redfish.services.assembly;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.redfish.services.allocation.ComposedNodeStateChanger;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTasksCoordinator;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.ComposedNodeState.ALLOCATED;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLING;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class NodeAssembler {
    @Inject
    private ComposedNodeStateChanger composedNodeStateChanger;

    @Inject
    private NodeTasksCoordinator nodeTasksCoordinator;

    @Transactional(REQUIRED)
    public void assemble(ComposedNode composedNode) throws AssemblyException, BusinessApiException {
        if (!composedNode.isInAnyOfStates(ALLOCATED)) {
            throw new ResourceStateMismatchException(format("Only composed node in %s state can be assembled", ALLOCATED));
        }
        composedNodeStateChanger.change(composedNode.getId(), ASSEMBLING);
        nodeTasksCoordinator.runTasks(composedNode.getId());
    }
}
