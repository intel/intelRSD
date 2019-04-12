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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationStrategy;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ExistingRemoteDriveValidator;
import com.intel.rsd.nodecomposer.composition.allocation.validation.RequestedRemoteDriveValidator;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static com.intel.rsd.collections.IterableHelper.single;
import static com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.ResourceAllocationStrategyMapping.getStrategyForResource;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class RemoteDriveAllocationStrategyFactory {
    private final NodeComposerBeanFactory beanFactory;
    private final ExistingRemoteDriveValidator existingRemoteDriveValidator;
    private final RequestedRemoteDriveValidator requestedRemoteDriveValidator;

    @Autowired
    public RemoteDriveAllocationStrategyFactory(NodeComposerBeanFactory beanFactory, ExistingRemoteDriveValidator existingRemoteDriveValidator,
                                                RequestedRemoteDriveValidator requestedRemoteDriveValidator) {
        this.beanFactory = beanFactory;
        this.existingRemoteDriveValidator = existingRemoteDriveValidator;
        this.requestedRemoteDriveValidator = requestedRemoteDriveValidator;
    }


    public AllocationStrategy create(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        if (!containsRemoteDrives(requestedNode)) {
            return beanFactory.create(NoRemoteDriveStrategy.class);
        }

        if (requestedNode.getRemoteDrives().size() != 1) {
            throw new AllocationRequestProcessingException(new Violations().addViolation("Creation of Systems with multiple RemoteDrives is not supported"));
        }

        RemoteDrive drive = single(requestedNode.getRemoteDrives());
        validateRemoteDriveRequest(drive);

        return isDefiningExistingTarget(drive)
            ? existingRemoteDrive(drive)
            : newRemoteDrive(drive);
    }

    private void validateRemoteDriveRequest(RemoteDrive requestedDrive) throws AllocationRequestProcessingException {
        Violations violations = requestedRemoteDriveValidator.validate(requestedDrive);
        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }
    }

    private boolean containsRemoteDrives(RequestedNode requestedNode) {
        return isNotEmpty(requestedNode.getRemoteDrives());
    }

    private ExistingRemoteDriveAllocationStrategy existingRemoteDrive(RemoteDrive drive) throws AllocationRequestProcessingException {
        existingRemoteDriveValidator.validateRemoteDrive(drive);
        ExistingRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(ExistingRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);

        ODataId driveODataId = drive.getResourceODataId();

        ResourceAllocationStrategy resourceAllocationStrategy = beanFactory.create(getStrategyForResource(driveODataId));
        resourceAllocationStrategy.setODataId(driveODataId);
        allocationStrategy.setResourceAllocationStrategy(resourceAllocationStrategy);

        return allocationStrategy;
    }

    private NewRemoteDriveAllocationStrategy newRemoteDrive(RemoteDrive drive) {
        NewRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(NewRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);

        return allocationStrategy;
    }

    private boolean isDefiningExistingTarget(RemoteDrive drive) {
        return drive.getResourceODataId() != null;
    }
}
