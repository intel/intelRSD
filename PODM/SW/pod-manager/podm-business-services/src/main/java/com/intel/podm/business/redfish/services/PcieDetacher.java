/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.PcieDriveActionsInvoker;
import com.intel.podm.business.redfish.services.actions.PcieZoneActionsInvoker;
import com.intel.podm.business.redfish.services.helpers.NodeActionsValidator;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.Violations.createWithViolations;
import static java.lang.Boolean.TRUE;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PcieDetacher {
    @Inject
    private PcieZoneActionsInvoker pcieZoneActionsInvoker;

    @Inject
    private PcieDriveActionsInvoker pcieDriveActionsInvoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NodeActionsValidator nodeActionHelper;

    @Transactional(REQUIRES_NEW)
    @SuppressWarnings({"checkstyle:NPathComplexity"})
    public void detachPcieDriveFromZone(Context composedNodeContext, Context pcieDriveContext) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        nodeActionHelper.validateIfActionCanBePerformedOnNode(composedNode);
        Drive drive = (Drive) traverser.traverse(pcieDriveContext);

        if (!composedNode.getDrives().contains(drive)) {
            throw new RequestValidationException(createWithViolations("Provided drive is not attached to this Node."));
        }
        if (drive.getConnectedEntity() == null) {
            throw new EntityOperationException("Provided drive is not attached to any Endpoint.");
        }

        Zone zone = drive.getConnectedEntity().getEndpoint().getZone();
        pcieZoneActionsInvoker.detachEndpoint(zone, drive.getConnectedEntity().getEndpoint());
        if (TRUE.equals(drive.getEraseOnDetach())) {
            pcieDriveActionsInvoker.secureErase(drive);
        }
    }

    @Transactional(REQUIRES_NEW)
    public void detachPcieDriveFromNode(Context composedNodeContext, Context pcieDriveContext) throws ContextResolvingException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        Drive drive = (Drive) traverser.traverse(pcieDriveContext);

        composedNode.unlinkDrive(drive);
        composedNode.decrementNumberOfRequestedDrives();
        drive.getMetadata().setAllocated(false);
    }
}
