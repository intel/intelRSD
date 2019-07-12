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
import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ComputerSystemResetActionInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.ResetRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ResetType;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.business.Violations.ofValueNotAllowedViolation;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

@Slf4j
@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeResetActionService {
    private final ComputerSystemResetActionInvoker invoker;
    private final EntityTreeTraverser traverser;

    @Autowired
    public ComposedNodeResetActionService(ComputerSystemResetActionInvoker invoker, EntityTreeTraverser traverser) {
        this.invoker = invoker;
        this.traverser = traverser;
    }

    public void perform(ODataId composedNodeODataId, ResetRequest request) throws BusinessApiException {
        val resetType = validateResetRequest(request);
        val composedNode = traverser.traverseComposedNode(composedNodeODataId);
        validateComposedNodeState(composedNode);
        val computerSystem = getComputerSystem(composedNode);

        validateResettableStatus(computerSystem);
        validateResetType(computerSystem, resetType);

        invoker.reset(computerSystem.getUri(), resetType);
    }

    private ResetType validateResetRequest(ResetRequest request) throws RequestValidationException {
        val resetType = request.getResetType();
        if (resetType == null) {
            Violations violations = new Violations();
            violations.addMissingPropertyViolation("ResetType");
            throw new RequestValidationException(violations);
        }
        return resetType;
    }

    private ComputerSystem getComputerSystem(ComposedNode composedNode) throws ResourceStateMismatchException {
        val computerSystem = composedNode.getComputerSystem();
        if (computerSystem == null) {
            throw new ResourceStateMismatchException("No ComputerSystem is associated with ComposedNode. Action aborted!");
        }
        return computerSystem;
    }

    private void validateComposedNodeState(ComposedNode composedNode) throws ResourceStateMismatchException {
        if (!composedNode.isInAnyOfStates(ASSEMBLED, FAILED)) {
            throw new ResourceStateMismatchException("ComposedNode state is invalid for requested power action: " + composedNode.getComposedNodeState());
        }
    }

    private void validateResetType(ComputerSystem computerSystem, ResetType resetType) throws BusinessApiException {
        val supportedResetTypes = computerSystem.getAllowableResetTypes();

        if (isEmpty(supportedResetTypes)) {
            val violation = format("Reset action not allowed on resource %s. There are no allowable reset types.", computerSystem.getUri());
            throw new RequestValidationException(createWithViolations(violation));
        }

        if (!supportedResetTypes.contains(resetType)) {
            throw new RequestValidationException(ofValueNotAllowedViolation("ResetType", supportedResetTypes));
        }
    }

    private void validateResettableStatus(ComputerSystem computerSystem) throws ResourceStateMismatchException {
        if (!computerSystem.isPresent()) {
            throw new ResourceStateMismatchException("ComputerSystem should be enabled and healthy in order to invoke actions on it.");
        }
    }
}
