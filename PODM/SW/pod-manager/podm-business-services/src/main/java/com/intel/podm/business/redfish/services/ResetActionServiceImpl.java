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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ResetActionInvoker;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.InvalidPayloadException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.Resettable;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.ResetRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.actions.ResetType;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ResetActionServiceImpl implements ActionService<ResetRequest> {
    @Inject
    private ResetActionInvoker invoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private Logger logger;

    @Override
    @Transactional(REQUIRES_NEW)
    public void perform(Context target, ResetRequest request) throws BusinessApiException, TimeoutException {
        ResetType resetType = validateResetRequest(request);

        Entity entity = traverser.traverse(target);
        Resettable resettableEntity = null;

        if (entity instanceof Resettable) {
            resettableEntity = (Resettable) entity;
        } else if (entity instanceof ComposedNode) {
            ComposedNode composedNode = (ComposedNode) entity;
            validateComposedNodeState(composedNode);
            resettableEntity = getComputerSystem(composedNode);
        }

        if (resettableEntity != null) {
            validateResettableStatus(resettableEntity);
            validateResetType(resettableEntity, resetType);
            performReset(resettableEntity, resetType);
        } else {
            throw new BusinessApiException("ResetAction not applicable to entity " + entity.getClass().getSimpleName());
        }
    }

    private ResetType validateResetRequest(ResetRequest request) throws InvalidPayloadException {
        ResetType resetType = request.getResetType();
        if (resetType == null) {
            throw new InvalidPayloadException("Mandatory ResetType is missing.");
        }
        return resetType;
    }

    private ComputerSystem getComputerSystem(ComposedNode composedNode) throws ResourceStateMismatchException {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
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

    private void performReset(Resettable resettableEntity, ResetType resetType) throws BusinessApiException, TimeoutException {
        try {
            taskCoordinator.runThrowing(resettableEntity.getService().getUuid(), () -> invoker.reset(resettableEntity, resetType));
        } catch (ActionException e) {
            logger.d("Reset action failed: error: {}, details: {}", e.getErrorResponse(), e.getMessage());
            throw new EntityOperationException("Reset action failed: " + e.getMessage(), e);
        }
    }

    private void validateResetType(Resettable resettable, ResetType resetType) throws BusinessApiException {
        List<ResetType> supportedResetTypes = resettable.getAllowableResetTypes();

        if (isEmpty(supportedResetTypes)) {
            throw new InvalidPayloadException("Reset action not allowed on resource." + resettable.getSourceUri()
                + ". There are no allowable reset types.");
        }

        if (!supportedResetTypes.contains(resetType)) {
            throw new InvalidPayloadException(format("Provided ResetType should be in allowable ResetType's %s", supportedResetTypes));
        }
    }

    private void validateResettableStatus(Resettable entity) throws ResourceStateMismatchException {
        if (!entity.isPresent()) {
            throw new ResourceStateMismatchException("ComputerSystem should be enabled and healthy in order to invoke actions on it.");
        }
    }
}
