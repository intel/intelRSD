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

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.DetachEndpointFromZoneTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PcieLocalStorageAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.collections.IterableHelper.single;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static java.util.Arrays.asList;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Transactional(REQUIRED)
public class PcieLocalStorageAllocator {
    private final LocalStorageAllocationMapper mapper;
    private final PcieLocalStorageAssemblyTaskFactory taskFactory;
    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public PcieLocalStorageAllocator(LocalStorageAllocationMapper mapper,
                                     PcieLocalStorageAssemblyTaskFactory taskFactory,
                                     NodeComposerBeanFactory beanFactory) {
        this.mapper = mapper;
        this.taskFactory = taskFactory;
        this.beanFactory = beanFactory;
    }

    public Set<PcieLocalStorage> selectResources(List<RequestedNode.LocalDrive> requestedLocalDrives, Set<LocalStorage> availableStorage) {
        Map<RequestedNode.LocalDrive, LocalStorage> mappedInterfaces = mapper.map(requestedLocalDrives, availableStorage);
        return filterPcieLocalStorage(mappedInterfaces.values());
    }

    public List<NodeTask> prepareNodeAssemblyTasks(Set<PcieLocalStorage> selectedPcieLocalStorages, Set<LocalStorage> availableStorage) {
        Set<Drive> availableButNotSelected = filterPcieLocalStorage(availableStorage).stream()
            .filter(localStorage -> !selectedPcieLocalStorages.contains(localStorage))
            .map(PcieLocalStorage::getDrive)
            .collect(toSet());

        List<NodeTask> assemblyTasks = new ArrayList<>();
        assemblyTasks.addAll(prepareNvmeOverPcieDriveDetachTasks(availableButNotSelected));
        selectedPcieLocalStorages.forEach(localStorage -> assemblyTasks.add(taskFactory.createLocalStorageTask(localStorage.getUri())));
        return assemblyTasks;
    }

    private Set<PcieLocalStorage> filterPcieLocalStorage(Collection<LocalStorage> availableStorage) {
        return availableStorage.stream()
            .filter(localStorage -> localStorage instanceof PcieLocalStorage)
            .map(PcieLocalStorage.class::cast)
            .collect(toSet());
    }

    private Collection<NodeTask> prepareNvmeOverPcieDriveDetachTasks(Collection<Drive> drives) {
        Collection<NodeTask> nodeTasks = new ArrayList<>();
        for (Drive drive : drives) {
            Optional<Endpoint> endpointCandidate = tryGetAssociatedEndpoint(drive);
            if (endpointCandidate.isPresent() && endpointCandidate.get().getZone() != null) {
                nodeTasks.add(createDetachEndpointFromZoneTask(drive));
            }
        }
        return nodeTasks;
    }

    private Optional<Endpoint> tryGetAssociatedEndpoint(Drive drive) {
        return drive.getEntityConnections().stream()
            .map(ConnectedEntity::getEndpoint)
            .filter(Objects::nonNull)
            .filter(e -> asList(PCIE, NVME).contains(e.getProtocol()))
            .findFirst();
    }

    private DetachEndpointFromZoneTask createDetachEndpointFromZoneTask(Drive drive) {
        DetachEndpointFromZoneTask detachEndpointFromZoneTask = beanFactory.create(DetachEndpointFromZoneTask.class);
        Endpoint endpoint = single(drive.getEntityConnections()).getEndpoint();
        detachEndpointFromZoneTask.setZoneODataId(endpoint.getZone().getUri());
        detachEndpointFromZoneTask.setEndpointODataId(endpoint.getUri());
        return detachEndpointFromZoneTask;
    }
}
