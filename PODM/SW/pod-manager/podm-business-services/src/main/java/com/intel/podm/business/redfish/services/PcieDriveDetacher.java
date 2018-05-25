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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.PcieDriveActionsInvoker;
import com.intel.podm.business.redfish.services.actions.PcieZoneActionsInvoker;
import com.intel.podm.business.redfish.services.helpers.NodeActionsValidator;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static java.lang.Boolean.TRUE;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PcieDriveDetacher {
    @Inject
    private PcieZoneActionsInvoker pcieZoneActionsInvoker;

    @Inject
    private PcieDriveActionsInvoker pcieDriveActionsInvoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NodeActionsValidator nodeActionHelper;

    @Transactional(REQUIRES_NEW)
    public void detachDriveFromZone(Context composedNodeContext, Context driveContext) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        Drive drive = (Drive) traverser.traverse(driveContext);

        nodeActionHelper.validateIfActionCanBePerformedOnNode(composedNode);
        validate(composedNode, drive);

        Set<Endpoint> endpoints = drive.getEntityConnections().stream()
            .map(ConnectedEntity::getEndpoint)
            .filter(Objects::nonNull)
            // TODO: Currently this class handles all Endpoint protocols. Determine if there should be separate class for each protocol.
            .collect(toSet());
        detachEndpoints(endpoints);

        if (TRUE.equals(drive.getEraseOnDetach())) {
            pcieDriveActionsInvoker.secureErase(drive);
        }
    }

    private void validate(ComposedNode composedNode, Drive drive) throws RequestValidationException {
        if (!composedNode.getDrives().contains(drive)) {
            throw new RequestValidationException(createWithViolations("Provided drive is not attached to this Node."));
        }
        if (drive.getEntityConnections().isEmpty()) {
            throw new RequestValidationException(createWithViolations("Provided drive is not attached to any Endpoint."));
        }
    }

    private void detachEndpoints(Set<Endpoint> endpoints) throws EntityOperationException {
        for (Endpoint endpoint : endpoints) {
            Zone zone = endpoint.getZone();
            if (zone != null) {
                pcieZoneActionsInvoker.detachEndpoint(zone, endpoint);
            }
        }
    }

    @Transactional(REQUIRES_NEW)
    public void detachDriveFromNode(Context composedNodeContext, Context driveContext) throws ContextResolvingException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        Drive drive = (Drive) traverser.traverse(driveContext);

        composedNode.unlinkDrive(drive);
        composedNode.decrementNumberOfRequestedDrives();
        drive.getMetadata().setAllocated(false);
    }
}
