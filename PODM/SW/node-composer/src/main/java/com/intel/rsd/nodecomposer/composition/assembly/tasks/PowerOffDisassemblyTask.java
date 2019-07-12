/*
 * Copyright (c) 2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ComputerSystemResetActionInvoker;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class PowerOffDisassemblyTask extends NodeTask implements Serializable {
    private final ComputerSystemResetActionInvoker resetActionInvoker;
    private final EntityTreeTraverser traverser;

    @Autowired
    public PowerOffDisassemblyTask(ComputerSystemResetActionInvoker resetActionInvoker, EntityTreeTraverser traverser) {
        this.resetActionInvoker = resetActionInvoker;
        this.traverser = traverser;
    }

    @Override
    @TimeMeasured(tag = "[NodeDisassemblyTask]")
    public void run() {
        try {
            ComposedNode composedNode = traverser.traverseComposedNode(composedNodeODataId);
            validateComposedNodeState(composedNode);
            ComputerSystem computerSystem = getComputerSystemFromNode(composedNode);

            resetActionInvoker.forceOff(computerSystem.getUri());
        } catch (BusinessApiException e) {
            throw new RuntimeException(
                format("Rest action on composed node %s failed with an exception %s", composedNodeODataId, e.getMessage()), e);
        }
    }

    private void validateComposedNodeState(ComposedNode composedNode) throws ResourceStateMismatchException {
        if (!composedNode.isInAnyOfStates(ASSEMBLED, FAILED)) {
            throw new ResourceStateMismatchException(
                format("ComposedNode state (%s) is invalid for requested power action: ", composedNode.getComposedNodeState()));
        }
    }
}
