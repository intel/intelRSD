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
import com.intel.rsd.nodecomposer.business.redfish.services.actions.AttachResourceInfoService;
import com.intel.rsd.nodecomposer.business.redfish.services.attach.Attacher;
import com.intel.rsd.nodecomposer.business.redfish.services.attach.ResourceAttacherFactory;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ComposedNodeActionsValidator;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.AttachResourceRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;

@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class AttachResourceActionService {
    private final ResourceAttacherFactory resourceAttacherFactory;
    private final ComposedNodeActionsValidator nodeActionValidator;
    private final EntityTreeTraverser traverser;
    private final AttachResourceInfoService attachResourceInfoService;

    @Autowired
    AttachResourceActionService(ResourceAttacherFactory resourceAttacherFactory, ComposedNodeActionsValidator nodeActionValidator,
                                EntityTreeTraverser traverser, AttachResourceInfoService attachResourceInfoService) {
        this.resourceAttacherFactory = resourceAttacherFactory;
        this.nodeActionValidator = nodeActionValidator;
        this.traverser = traverser;
        this.attachResourceInfoService = attachResourceInfoService;
    }

    public void perform(ODataId target, AttachResourceRequest request) throws BusinessApiException {
        validateRequest(request);
        validateComposedNode(target);

        ComposedNode composedNode = traverser.traverseComposedNode(target);
        ODataId resourceContext = request.getResourceODataId();
        Attacher attacher = resourceAttacherFactory.getAttacherForResource(resourceContext);
        DiscoverableEntity entity = traverser.tryGetDiscoverableEntity(resourceContext);

        attacher.validate(composedNode, entity, request);
        attacher.attach(composedNode, entity);
    }

    public ActionInfoDto getActionInfo(ODataId target) throws BusinessApiException {
        ComposedNode composedNode = traverser.traverseComposedNode(target);
        return attachResourceInfoService.getActionInfo(composedNode);
    }

    private void validateComposedNode(ODataId target) throws BusinessApiException {
        ComposedNode composedNode = traverser.traverseComposedNode(target);
        nodeActionValidator.validateIfActionCanBePerformedOnNode(composedNode);
    }

    private void validateRequest(AttachResourceRequest request) throws RequestValidationException {
        if (request.getResourceODataId() == null) {
            throw new RequestValidationException(createWithViolations("Resource needs to be specified"));
        }
    }
}
