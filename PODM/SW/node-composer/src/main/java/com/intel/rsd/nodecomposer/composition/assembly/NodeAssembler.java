/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.ComposedNodeStateChanger;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTasksCoordinator;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLING;
import static javax.transaction.Transactional.TxType.NOT_SUPPORTED;

@Component
public class NodeAssembler {
    private final ComposedNodeStateChanger composedNodeStateChanger;
    private final NodeTasksCoordinator nodeTasksCoordinator;

    @Autowired
    public NodeAssembler(ComposedNodeStateChanger composedNodeStateChanger, NodeTasksCoordinator nodeTasksCoordinator) {
        this.composedNodeStateChanger = composedNodeStateChanger;
        this.nodeTasksCoordinator = nodeTasksCoordinator;
    }

    @Transactional(NOT_SUPPORTED)
    public void assemble(ODataId nodeOdataId) throws AssemblyException {
        composedNodeStateChanger.change(nodeOdataId, ASSEMBLING);
        nodeTasksCoordinator.runTasks(nodeOdataId);
    }
}
