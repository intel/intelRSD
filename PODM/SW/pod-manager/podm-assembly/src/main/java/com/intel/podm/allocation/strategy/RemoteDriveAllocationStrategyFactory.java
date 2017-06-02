/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.allocation.strategy;

import com.intel.podm.allocation.validation.RemoteDriveValidationException;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.common.utils.IterableHelper.single;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

@Dependent
public class RemoteDriveAllocationStrategyFactory {
    @Inject
    private BeanFactory beanFactory;

    public RemoteDriveAllocationStrategy create(RequestedNode requestedNode) throws RemoteDriveValidationException {
        if (!containsRemoteDrives(requestedNode)) {
            return beanFactory.create(NoRemoteDriveStrategy.class);
        }

        if (requestedNode.getRemoteDrives().size() != 1) {
            throw new RemoteDriveValidationException("Creation of Systems with multiple RemoteDrives is not supported");
        }

        RequestedNode.RemoteDrive drive = single(requestedNode.getRemoteDrives());

        return isDefiningExistingTarget(drive)
                ? existingRemoteDrive(drive)
                : newRemoteDrive(drive);
    }

    private boolean containsRemoteDrives(RequestedNode requestedNode) {
        return isNotEmpty(requestedNode.getRemoteDrives());
    }

    private ExistingRemoteDriveAllocationStrategy existingRemoteDrive(RequestedNode.RemoteDrive drive) {
        ExistingRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(ExistingRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);
        return allocationStrategy;
    }

    private NewRemoteDriveAllocationStrategy newRemoteDrive(RequestedNode.RemoteDrive drive) {
        NewRemoteDriveAllocationStrategy allocationStrategy = beanFactory.create(NewRemoteDriveAllocationStrategy.class);
        allocationStrategy.setDrive(drive);
        return allocationStrategy;
    }

    private boolean isDefiningExistingTarget(RequestedNode.RemoteDrive drive) {
        return drive.getMaster() == null;
    }
}
