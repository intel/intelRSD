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

package com.intel.podm.business.redfish.services.assembly;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.business.redfish.services.allocation.strategy.ChangeTpmStateTaskFactory;
import com.intel.podm.business.redfish.services.assembly.tasks.CleanUpVlanDisassembleTask;
import com.intel.podm.business.redfish.services.assembly.tasks.ComposedNodeDeletionTask;
import com.intel.podm.business.redfish.services.assembly.tasks.ComputerSystemDisassembleTask;
import com.intel.podm.business.redfish.services.assembly.tasks.DeallocatePcieDriveTask;
import com.intel.podm.business.redfish.services.assembly.tasks.DetachPcieDriveTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeRemovalTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.PowerOffTask;
import com.intel.podm.business.redfish.services.assembly.tasks.SecureErasePcieDriveTask;
import com.intel.podm.business.redfish.services.assembly.tasks.StorageServiceDisassemblyTask;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;
import java.util.stream.Stream;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.Boolean.TRUE;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Stream.concat;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class NodeDisassembler {

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private ChangeTpmStateTaskFactory changeTpmStateTaskFactory;

    @Inject
    private Logger logger;

    @Transactional(REQUIRES_NEW)
    public Collection<NodeTask> getDisassemblyTasks(Id composedNodeId) throws ContextResolvingException {
        ComposedNode composedNode = resolveComposedNode(composedNodeId);

        Collection<NodeTask> tasks = new ArrayList<>();
        tasks.add(beanFactory.create(PowerOffTask.class));
        tasks.addAll(prepareStorageServiceDeallocationTasks(composedNode));
        tasks.addAll(preparePcieDriveDetachTasks(composedNode.getDrives()));
        tasks.addAll(preparePcieDriveSecureEraseTasks(composedNode));
        tasks.addAll(preparePcieDriveDeallocationTasks(composedNode));
        if (composedNode.getComputerSystem() != null) {
            tasks.addAll(addChangeTpmStateTask(composedNode.getComputerSystem(), composedNode.getClearTpmOnDelete()));
            tasks.addAll(prepareComputerSystemDeallocationTasks(composedNode.getComputerSystem()));
            tasks.addAll(prepareCleanVlansTasks(composedNode.getEthernetInterfaces()));
        }
        tasks.addAll(prepareComposeNodeDeletionTasks());
        tasks.forEach(t -> t.setNodeId(composedNode.getId()));
        return tasks;
    }

    private ComposedNode resolveComposedNode(Id composedNodeId) throws ContextResolvingException {
        try {
            return genericDao.find(ComposedNode.class, composedNodeId);
        } catch (EntityNotFoundException e) {
            throw new ContextResolvingException(e.getMessage(), contextOf(composedNodeId, COMPOSED_NODE), e);
        }
    }

    private Collection<? extends NodeTask> prepareStorageServiceDeallocationTasks(ComposedNode composedNode) {
        return determineRelatedStorageServices(composedNode)
            .map(storageServiceUuid -> beanFactory.create(StorageServiceDisassemblyTask.class).init(storageServiceUuid))
            .collect(toList());
    }

    private Stream<UUID> determineRelatedStorageServices(ComposedNode composedNode) {
        return concat(
            composedNode.getEndpoints().stream().map(endpoint -> endpoint.getService().getUuid()),
            composedNode.getVolumes().stream().map(volume -> volume.getService().getUuid())
        ).distinct();
    }

    private Collection<? extends NodeRemovalTask> prepareComposeNodeDeletionTasks() {
        return singletonList(beanFactory.create(ComposedNodeDeletionTask.class));
    }

    private Collection<? extends NodeRemovalTask> prepareComputerSystemDeallocationTasks(ComputerSystem computerSystem) {
        if (computerSystem.getService() != null) {
            return singletonList(beanFactory.create(ComputerSystemDisassembleTask.class));
        }
        return emptyList();
    }

    private Collection<? extends NodeRemovalTask> prepareCleanVlansTasks(Set<EthernetInterface> ethernetInterfaces) {
        return ethernetInterfaces.stream()
            .map(EthernetInterface::getMacAddress)
            .map(this::getEthernetSwitchPortFromMac)
            .filter(Objects::nonNull)
            .map(EthernetSwitchPort::getId)
            .map(id -> beanFactory.create(CleanUpVlanDisassembleTask.class).init(id))
            .collect(toList());
    }

    private Collection<? extends NodeTask> addChangeTpmStateTask(ComputerSystem computerSystem, boolean clearTpm) {
        if (isChangeTpmStateRequired(computerSystem, clearTpm)) {
            return singletonList(changeTpmStateTaskFactory.createChangeTpmStateTask(computerSystem, new ChangeTpmStatusUpdateDefinition(TRUE)));
        }
        return emptyList();
    }

    private boolean isChangeTpmStateRequired(ComputerSystem computerSystem, boolean clearTpm) {
        return clearTpm && !nullOrEmpty(computerSystem.getTrustedModules()) && areTrustedModulesEnabled(computerSystem.getTrustedModules());
    }

    private boolean areTrustedModulesEnabled(List<TrustedModule> trustedModules) {
        return trustedModules.stream().anyMatch(trustedModule -> ENABLED.equals(trustedModule.getStatus().getState()));
    }

    private Collection<? extends NodeTask> preparePcieDriveSecureEraseTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .filter(drive -> TRUE.equals(drive.getEraseOnDetach()))
            .map(drive -> beanFactory.create(SecureErasePcieDriveTask.class).setPcieDriveId(drive.getId()))
            .collect(toList());
    }

    private Collection<? extends NodeRemovalTask> preparePcieDriveDeallocationTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .map(pcieDrive -> beanFactory.create(DeallocatePcieDriveTask.class).setDriveId(pcieDrive.getId()))
            .collect(toList());
    }

    public Collection<NodeTask> preparePcieDriveDetachTasks(Collection<Drive> drives) {
        Collection<NodeTask> nodeTasks = new ArrayList<>();
        for (Drive drive : drives) {
            ConnectedEntity connectedEntity = single(drive.getEntityConnections());
            Endpoint endpoint = connectedEntity.getEndpoint();
            if (endpoint != null && endpoint.getZone() != null) {
                nodeTasks.add(createDetachPcieDriveTask(drive));
            }
        }
        return nodeTasks;
    }

    private DetachPcieDriveTask createDetachPcieDriveTask(Drive drive) {
        DetachPcieDriveTask detachPcieDriveTask = beanFactory.create(DetachPcieDriveTask.class);
        Endpoint endpoint = single(drive.getEntityConnections()).getEndpoint();
        detachPcieDriveTask.setZoneId(endpoint.getZone().getId());
        detachPcieDriveTask.setEndpointId(endpoint.getId());
        return detachPcieDriveTask;
    }

    private EthernetSwitchPort getEthernetSwitchPortFromMac(MacAddress mac) {
        try {
            return ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(mac);
        } catch (NonUniqueResultException e) {
            logger.e(format("Could not get neighbor switch port from mac address '%s'.", mac), e);
            return null;
        }
    }
}
