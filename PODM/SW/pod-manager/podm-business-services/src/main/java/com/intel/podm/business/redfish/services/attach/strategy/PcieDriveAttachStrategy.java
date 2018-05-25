/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.attach.strategy;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.dao.DriveDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.redfish.services.PcieDriveAttacher;
import com.intel.podm.business.redfish.services.PcieZoneAttacher;
import com.intel.podm.business.redfish.services.attach.AttachResourceStrategy;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.requests.AttachResourceRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PcieDriveAttachStrategy implements AttachResourceStrategy {
    @Inject
    private ServiceTraverser traverser;

    @Inject
    private EntityTreeTraverser entityTraverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Inject
    private PcieZoneAttacher zoneAttacher;

    @Inject
    private PcieDriveAttacher driveAttacher;

    @Inject
    private DriveDao driveDao;

    @Override
    public ContextType supportedType() {
        return DRIVE;
    }

    @Override
    @Transactional(MANDATORY)
    public void attach(Context target, Context resourceContext) throws BusinessApiException, TimeoutException {
        taskCoordinator.run(traverser.traverseServiceUuid(resourceContext), () -> zoneAttacher.attachEndpointToZone(target, resourceContext));
        taskCoordinator.run(serviceConfigHolder.get().getUuid(), () -> driveAttacher.attachPcieDriveToNode(target, resourceContext));
    }

    @Override
    @Transactional(MANDATORY)
    public void validate(Context target, AttachResourceRequest request) throws BusinessApiException {
        Drive drive = (Drive) entityTraverser.traverse(request.getResourceContext());
        ComposedNode composedNode = (ComposedNode) entityTraverser.traverse(target);
        validateDriveAchievable(drive, composedNode);
        validateDriveAttachAbility(drive);
    }

    private void validateDriveAchievable(Drive drive, ComposedNode composedNode) throws RequestValidationException {
        Set<Drive> achievableDrives = driveDao.getAchievablePcieDrives(composedNode.getComputerSystem());
        if (!achievableDrives.contains(drive)) {
            throw new RequestValidationException(createWithViolations("Selected Drive is not achievable for this node!"));
        }
    }

    private void validateDriveAttachAbility(Drive drive) throws RequestValidationException {
        if (drive.getMetadata().isAllocated()) {
            throw new RequestValidationException(createWithViolations("Selected Drive is currently in use!"));
        }
    }
}
