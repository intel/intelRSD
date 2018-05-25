/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.dto.actions.actionInfo.ActionInfoDto;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.DetachResourceInfoService;
import com.intel.podm.business.redfish.services.detach.DetachResourceStrategy;
import com.intel.podm.business.redfish.services.detach.DetachResourceStrategyFactory;
import com.intel.podm.business.redfish.services.helpers.NodeActionsValidator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.DetachResourceRequest;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.Violations.createWithViolations;
import static javax.transaction.Transactional.TxType.NEVER;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class DetachResourceActionServiceImpl implements ActionService<DetachResourceRequest> {
    @Inject
    private DetachResourceStrategyFactory detachResourceStrategyFactory;

    @Inject
    private NodeActionsValidator nodeActionHelper;

    @Inject
    private DetachResourceInfoService detachResourceInfoService;

    @Inject
    private EntityTreeTraverser traverser;

    @Override
    @Transactional(NEVER)
    public void perform(Context composedNodeContext, DetachResourceRequest request) throws BusinessApiException, TimeoutException {
        validateRequest(request);
        nodeActionHelper.validateIfActionCanBePerformedOnNode(composedNodeContext);

        Context resourceToDetach = request.getResourceContext();
        DetachResourceStrategy detachResourceStrategy = detachResourceStrategyFactory.getStrategyForResource(resourceToDetach.getType());
        detachResourceStrategy.detach(composedNodeContext, resourceToDetach);
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public ActionInfoDto getActionInfo(Context target) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(target);
        return detachResourceInfoService.getActionInfo(composedNode);
    }

    private void validateRequest(DetachResourceRequest request) throws RequestValidationException {
        if (request.getResourceContext() == null) {
            throw new RequestValidationException(createWithViolations("Resource needs to be specified"));
        }
    }
}
