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
import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ActionInfoDto;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.DetachResourceInfoService;
import com.intel.rsd.nodecomposer.business.redfish.services.detach.Detacher;
import com.intel.rsd.nodecomposer.business.redfish.services.detach.ResourceDetacherFactory;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ComposedNodeActionsValidator;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.DetachResourceRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class DetachResourceActionService {
    private final ResourceDetacherFactory resourceDetacherFactory;
    private final ComposedNodeActionsValidator composedNodeActionsValidator;
    private final DetachResourceInfoService detachResourceInfoService;
    private final EntityTreeTraverser traverser;

    @Autowired
    public DetachResourceActionService(ResourceDetacherFactory resourceDetacherFactory, ComposedNodeActionsValidator composedNodeActionsValidator,
                                       DetachResourceInfoService detachResourceInfoService, EntityTreeTraverser traverser) {
        this.resourceDetacherFactory = resourceDetacherFactory;
        this.composedNodeActionsValidator = composedNodeActionsValidator;
        this.detachResourceInfoService = detachResourceInfoService;
        this.traverser = traverser;
    }

    @Transactional(value = REQUIRED, rollbackOn = BusinessApiException.class)
    public void perform(ODataId composedNodeODataId, DetachResourceRequest request) throws BusinessApiException {
        validateRequest(request);
        ComposedNode composedNode = traverser.traverseComposedNode(composedNodeODataId);
        composedNodeActionsValidator.validateIfActionCanBePerformedOnNode(composedNode);

        ODataId resourceToDetach = request.getResourceODataId();
        Detacher resourceDetacher = resourceDetacherFactory.getDetacherForResource(resourceToDetach);
        DiscoverableEntity resource = traverser.traverseDiscoverableEntity(resourceToDetach);
        resourceDetacher.detach(composedNode, resource);
    }

    @Transactional(REQUIRES_NEW)
    public ActionInfoDto getActionInfo(ODataId target) throws BusinessApiException {
        ComposedNode composedNode = traverser.traverseComposedNode(target);
        return detachResourceInfoService.getActionInfo(composedNode);
    }

    private void validateRequest(DetachResourceRequest request) throws RequestValidationException {
        if (request.getResourceODataId() == null) {
            throw new RequestValidationException(createWithViolations("Resource needs to be specified"));
        }
    }
}
