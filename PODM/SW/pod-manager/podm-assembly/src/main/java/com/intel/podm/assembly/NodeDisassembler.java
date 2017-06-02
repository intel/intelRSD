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

package com.intel.podm.assembly;

import com.intel.podm.assembly.tasks.ComposedNodeDeletionTask;
import com.intel.podm.assembly.tasks.ComputerSystemDisassembleTask;
import com.intel.podm.assembly.tasks.DeallocatePcieDriveTask;
import com.intel.podm.assembly.tasks.DeallocateRemoteTarget;
import com.intel.podm.assembly.tasks.DetachPcieDriveTask;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.assembly.tasks.SecureErasePcieDriveTask;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static java.lang.Boolean.TRUE;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class NodeDisassembler {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private GenericDao genericDao;

    @Transactional(REQUIRES_NEW)
    public Collection<NodeAssemblyTask> getDisassemblyTasks(Id composedNodeId) throws ContextResolvingException {
        ComposedNode composedNode;
        try {
            composedNode = genericDao.find(ComposedNode.class, composedNodeId);
        } catch (EntityNotFoundException e) {
            throw new ContextResolvingException(e.getMessage(), contextOf(composedNodeId, COMPOSED_NODE), e);
        }

        Collection<NodeAssemblyTask> tasks = new ArrayList<>();
        tasks.addAll(prepareRemoteTargetDeallocationTasks(composedNode));
        tasks.addAll(preparePcieDriveDetachTasks(composedNode.getDrives()));
        tasks.addAll(preparePcieDriveSecureEraseTasks(composedNode));
        tasks.addAll(preparePcieDriveDeallocationTasks(composedNode));
        tasks.addAll(prepareComputerSystemDeallocationTasks(composedNode));
        tasks.addAll(prepareComposeNodeDeletionTasks());

        tasks.forEach(t -> t.setNodeId(composedNode.getId()));
        return tasks;
    }

    private Collection<? extends NodeAssemblyTask> prepareComposeNodeDeletionTasks() {
        return singletonList(beanFactory.create(ComposedNodeDeletionTask.class));
    }

    private Collection<? extends NodeAssemblyTask> prepareComputerSystemDeallocationTasks(ComposedNode composedNode) {
        UUID computerSystemServiceUuid = getComputerSystemServiceUuid(composedNode);
        if (computerSystemServiceUuid != null) {
            return singletonList(beanFactory.create(ComputerSystemDisassembleTask.class));
        } else {
            return emptyList();
        }
    }

    private Collection<? extends NodeAssemblyTask> prepareRemoteTargetDeallocationTasks(ComposedNode composedNode) {
        return composedNode.getRemoteTargets().stream()
            .map(remoteTarget -> beanFactory.create(DeallocateRemoteTarget.class).setRemoteTargetId(remoteTarget.getId()))
            .collect(toList());
    }

    private Collection<? extends NodeAssemblyTask> preparePcieDriveSecureEraseTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .filter(drive -> TRUE.equals(drive.getEraseOnDetach()))
            .map(drive -> beanFactory.create(SecureErasePcieDriveTask.class).setPcieDriveId(drive.getId()))
            .collect(toList());
    }

    private Collection<? extends NodeAssemblyTask> preparePcieDriveDeallocationTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .map(pcieDrive -> beanFactory.create(DeallocatePcieDriveTask.class).setDriveId(pcieDrive.getId()))
            .collect(toList());
    }

    public Collection<NodeAssemblyTask> preparePcieDriveDetachTasks(Collection<Drive> drives) {
        return drives.stream()
            .filter(drive -> drive.getConnectedEntity().getEndpoint() != null)
            .filter(drive -> drive.getConnectedEntity().getEndpoint().getZone() != null)
            .map(this::createDetachPcieDriveTask).collect(toList());
    }

    private DetachPcieDriveTask createDetachPcieDriveTask(Drive drive) {
        DetachPcieDriveTask detachPcieDriveTask = beanFactory.create(DetachPcieDriveTask.class);
        detachPcieDriveTask.setZoneId(drive.getConnectedEntity().getEndpoint().getZone().getId());
        detachPcieDriveTask.setEndpointId(drive.getConnectedEntity().getEndpoint().getId());
        return detachPcieDriveTask;
    }

    private UUID getComputerSystemServiceUuid(ComposedNode composedNode) {
        UUID uuid = null;
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem != null) {
            uuid = computerSystem.getService().getUuid();
        }
        return uuid;
    }
}
