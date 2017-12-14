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
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.podm.business.redfish.services.helpers.NodeActionsValidator;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.util.Optional;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class PcieDriveAttacher {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private LocalStorageCollector localStorageCollector;

    @Inject
    private NodeActionsValidator nodeActionHelper;

    @Transactional(REQUIRES_NEW)
    public Context determinePcieDriveContext(Context composedNodeContext, BigDecimal capacityGiB) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        nodeActionHelper.validateIfActionCanBePerformedOnNode(composedNode);

        Optional<PcieLocalStorage> determinedStorage = localStorageCollector.getPcieStorage(composedNode.getComputerSystem()).stream()
            .filter(pcieLocalStorage -> !pcieLocalStorage.needsToBeExplicitlySelected())
            .filter(pcieLocalStorage -> capacityGiB.compareTo(pcieLocalStorage.getCapacityGib()) <= 0)
            .sorted(comparing(LocalStorage::getCapacityGib, naturalOrder()))
            .findFirst();

        return contextOf(
            determinedStorage.orElseThrow(() -> new ResourceStateMismatchException("Matched 0 PCIeDrives to attach.")).getId(),
            DRIVE);
    }

    @Transactional(REQUIRES_NEW)
    public void attachPcieDriveToNode(Context composedNodeContext, Context pcieDriveContext) throws ContextResolvingException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        Drive drive = (Drive) traverser.traverse(pcieDriveContext);

        composedNode.addDrive(drive);
        composedNode.incrementNumberOfRequestedDrives();
        drive.getMetadata().setAllocated(true);
    }
}
