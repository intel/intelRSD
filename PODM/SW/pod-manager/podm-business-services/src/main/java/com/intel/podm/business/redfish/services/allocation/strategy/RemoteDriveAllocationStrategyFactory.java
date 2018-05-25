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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.Violations;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.redfish.services.allocation.validation.ExistingRemoteDriveValidator;
import com.intel.podm.business.redfish.services.allocation.validation.RequestedRemoteDriveValidator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.redfish.services.allocation.strategy.matcher.ResourceAllocationStrategyMapping.getStrategyForResource;
import static com.intel.podm.common.utils.IterableHelper.single;
import static org.apache.commons.collections4.CollectionUtils.isNotEmpty;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class RemoteDriveAllocationStrategyFactory {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private ExistingRemoteDriveValidator existingRemoteDriveValidator;

    @Inject
    private RequestedRemoteDriveValidator requestedRemoteDriveValidator;


    public RemoteDriveAllocationStrategy create(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        if (!containsRemoteDrives(requestedNode)) {
            return beanFactory.create(NoRemoteDriveStrategy.class);
        }

        if (requestedNode.getRemoteDrives().size() != 1) {
            throw new AllocationRequestProcessingException(new Violations().addViolation("Creation of Systems with multiple RemoteDrives is not supported"));
        }

        RequestedNode.RemoteDrive drive = single(requestedNode.getRemoteDrives());
        validateRemoteDriveRequest(drive);

        return isDefiningExistingTarget(drive)
            ? existingRemoteDrive(drive)
            : newRemoteDrive(drive);
    }

    private void validateRemoteDriveRequest(RequestedNode.RemoteDrive requestedDrive) throws AllocationRequestProcessingException {
        Violations violations = requestedRemoteDriveValidator.validate(requestedDrive);
        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }
    }

    private boolean containsRemoteDrives(RequestedNode requestedNode) {
        return isNotEmpty(requestedNode.getRemoteDrives());
    }

    private ExistingRemoteDriveAllocationStrategy existingRemoteDrive(RequestedNode.RemoteDrive drive) throws AllocationRequestProcessingException {
        existingRemoteDriveValidator.validateRemoteDrive(drive);
        ExistingRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(ExistingRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);

        Context context = drive.getResourceContext();

        ResourceAllocationStrategy resourceAllocationStrategy = beanFactory.create(getStrategyForResource(context.getType()));
        resourceAllocationStrategy.setContext(context);
        allocationStrategy.setResourceAllocationStrategy(resourceAllocationStrategy);

        return allocationStrategy;
    }

    private NewRemoteDriveAllocationStrategy newRemoteDrive(RequestedNode.RemoteDrive drive) throws AllocationRequestProcessingException {
        NewRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(NewRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);

        return allocationStrategy;
    }

    private boolean isDefiningExistingTarget(RequestedNode.RemoteDrive drive) {
        return drive.getResourceContext() != null;
    }
}
