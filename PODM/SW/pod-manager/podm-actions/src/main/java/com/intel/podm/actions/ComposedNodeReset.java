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

package com.intel.podm.actions;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.actions.ResetType;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static com.intel.podm.common.types.ComposedNodeState.POWERED_OFF;
import static com.intel.podm.common.types.ComposedNodeState.POWERED_ON;
import static com.intel.podm.common.types.actions.ResetType.FORCE_OFF;
import static com.intel.podm.common.types.actions.ResetType.FORCE_ON;
import static com.intel.podm.common.types.actions.ResetType.FORCE_RESTART;
import static com.intel.podm.common.types.actions.ResetType.GRACEFUL_RESTART;
import static com.intel.podm.common.types.actions.ResetType.GRACEFUL_SHUTDOWN;
import static com.intel.podm.common.types.actions.ResetType.NMI;
import static com.intel.podm.common.types.actions.ResetType.ON;
import static com.intel.podm.common.types.actions.ResetType.PUSH_POWER_BUTTON;
import static com.intel.podm.common.types.actions.ResetType.isAnyOfTypes;
import static java.util.Collections.emptySet;
import static java.util.EnumSet.copyOf;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

/**
 * Performs reset on node. Delegates actions to node components.
 */
@RequestScoped
public class ComposedNodeReset {
    @Inject
    private ResetActionInvoker resetActionInvoker;

    @Transactional(value = REQUIRED, rollbackOn = ActionException.class)
    public void reset(ComposedNode composedNode, ResetType resetType) throws ActionException, IllegalStateException, IllegalArgumentException {
        ComputerSystem computerSystem = getComposedNodeComputerSystems(composedNode);
        validateComposedNodeState(composedNode);
        validateResetType(computerSystem, resetType);
        ComposedNodeState nodeStateAfterReset = nodeStateAfterReset(composedNode, resetType);

        resetActionInvoker.reset(computerSystem, resetType);
        composedNode.setComposedNodeState(nodeStateAfterReset);
    }

    private ComputerSystem getComposedNodeComputerSystems(ComposedNode composedNode) throws ActionException {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem == null) {
            throw new IllegalStateException("No ComputerSystem is associated with ComposedNode. Action aborted!");
        }
        return computerSystem;
    }

    private void validateComposedNodeState(ComposedNode composedNode) throws ActionException {
        if (!composedNode.isInAnyOfStates(POWERED_ON, POWERED_OFF)) {
            throw new IllegalStateException("ComposedNode state is invalid for requested power action: " + composedNode.getComposedNodeState());
        }
    }

    private void validateResetType(ComputerSystem computerSystem, ResetType resetType) throws ActionException {
        Set<ResetType> supportedResetTypes = copyOf(isEmpty(computerSystem.getAllowableResetTypes()) ? emptySet() : computerSystem.getAllowableResetTypes());

        if (!supportedResetTypes.contains(resetType)) {
            throw new IllegalArgumentException("Unsupported reset type: " + resetType);
        }
    }

    private ComposedNodeState nodeStateAfterReset(ComposedNode composedNode, ResetType resetType) throws ActionException {
        if (isAnyOfTypes(resetType, ON, FORCE_ON, GRACEFUL_RESTART, FORCE_RESTART)) {
            return POWERED_ON;
        } else if (isAnyOfTypes(resetType, FORCE_OFF, GRACEFUL_SHUTDOWN, NMI)) {
            return POWERED_OFF;
        } else if (isAnyOfTypes(resetType, PUSH_POWER_BUTTON)) {
            return oppositePowerState(composedNode.getComposedNodeState());
        } else {
            throw new IllegalArgumentException("Unsupported reset type: " + resetType);
        }
    }

    private ComposedNodeState oppositePowerState(ComposedNodeState composedNodeState) {
        return composedNodeState == POWERED_ON ? POWERED_OFF : POWERED_ON;
    }
}
