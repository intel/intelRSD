/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.PcieZoneActionsInvoker;
import com.intel.podm.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.podm.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.InvalidPayloadException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.redfish.services.helpers.NodeActionsValidator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.AttachEndpointRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.Dependent;
import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static java.lang.String.format;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static javax.transaction.Transactional.TxType.NEVER;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class AttachEndpointActionServiceImpl implements ActionService<AttachEndpointRequest> {
    @Inject
    private ServiceTraverser traverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Inject
    private PcieZoneAttacher zoneAttacher;

    @Inject
    private PcieDriveAttacher driveAttacher;

    @Override
    @Transactional(NEVER)
    public void perform(Context target, AttachEndpointRequest request) throws BusinessApiException, TimeoutException {
        validateRequest(request);

        final Context pcieDriveContext = getPcieDriveContext(target, request);
        taskCoordinator.runThrowing(traverser.traverseServiceUuid(pcieDriveContext),
            () -> zoneAttacher.attachEndpointToZone(target, pcieDriveContext));

        taskCoordinator.runThrowing(serviceConfigHolder.get().getUuid(),
            () -> driveAttacher.attachPcieDriveToNode(target, pcieDriveContext));
    }

    private void validateRequest(AttachEndpointRequest request) throws InvalidPayloadException {
        if (request.getCapacityGiB() == null && request.getDriveContext() == null) {
            throw new InvalidPayloadException("CapacityGiB or Resource need to be specified");
        }

        if (request.getCapacityGiB() != null && request.getDriveContext() != null) {
            throw new InvalidPayloadException("CapacityGiB and Resource can not be specified at the same time");
        }
    }

    private Context getPcieDriveContext(Context target, AttachEndpointRequest request) throws BusinessApiException {
        return (request.getDriveContext() != null)
            ? request.getDriveContext()
            : driveAttacher.determinePcieDriveContext(target, request.getCapacityGiB());
    }

    @Dependent
    public static class PcieDriveAttacher {
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

    @Dependent
    @SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
    public static class PcieZoneAttacher {
        @Inject
        private EntityTreeTraverser traverser;

        @Inject
        private LocalStorageCollector localStorageCollector;

        @Inject
        private PcieZoneActionsInvoker pcieZoneActionsInvoker;

        @Inject
        private NodeActionsValidator nodeActionHelper;

        @Transactional(REQUIRES_NEW)
        public void attachEndpointToZone(Context composedNodeContext, Context pcieDriveContext) throws BusinessApiException {
            ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
            nodeActionHelper.validateIfActionCanBePerformedOnNode(composedNode);

            Drive pcieDrive = (Drive) traverser.traverse(pcieDriveContext);
            Optional<PcieLocalStorage> pcieLocalStorage = findPcieLocalStorageByResource(composedNode.getComputerSystem(), pcieDrive);
            PcieLocalStorage localStorage = pcieLocalStorage.orElseThrow(() -> new EntityOperationException("PCIeDrive cannot be attached to this Node."));

            try {
                pcieZoneActionsInvoker.attachEndpoint(localStorage.getZone(), localStorage.getEndpoint());
            } catch (ActionException e) {
                throw new EntityOperationException(format("PCIeDrive attach action failed: %s.", e.getMessage()), e);
            }
        }

        private Optional<PcieLocalStorage> findPcieLocalStorageByResource(ComputerSystem computerSystem, Drive drive) {
            return localStorageCollector.getPcieStorage(computerSystem).stream()
                .filter(pcieLocalStorage -> Objects.equals(pcieLocalStorage.getDrive(), drive))
                .findFirst();
        }
    }
}
