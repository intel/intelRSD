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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.services.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.podm.business.redfish.services.assembly.NodeDisassembler;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.PcieLocalStorageAssemblyTaskFactory;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class PcieLocalStorageAllocator {
    @Inject
    private LocalStorageAllocationMapper mapper;

    @Inject
    private PcieLocalStorageAssemblyTaskFactory taskFactory;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private NodeDisassembler nodeDisassembler;

    public Set<PcieLocalStorage> selectResources(List<RequestedNode.LocalDrive> requestedLocalDrives, Set<LocalStorage> availableStorage) {
        Map<RequestedNode.LocalDrive, LocalStorage> mappedInterfaces = mapper.map(requestedLocalDrives, availableStorage);
        return filterPcieLocalStorage(mappedInterfaces.values());
    }

    public List<NodeTask> prepareNodeAssemblyTasks(Set<PcieLocalStorage> selectedPcieLocalStorages, Set<LocalStorage> availableStorage) {
        Set<Drive> availableButNotSelected = filterPcieLocalStorage(availableStorage).stream()
            .filter(localStorage -> !selectedPcieLocalStorages.contains(localStorage))
            .map(PcieLocalStorage::getDrive)
            .collect(toSet());

        List<NodeTask> assemblyTask = nodeDisassembler.preparePcieDriveDetachTasks(availableButNotSelected).stream().collect(toList());

        getZoneToEndpointsMapping(selectedPcieLocalStorages).entrySet().stream()
            .map(entry -> taskFactory.createLocalStorageTask(entry.getKey(), entry.getValue()))
            .forEach(assemblyTask::add);

        return assemblyTask;
    }

    private Set<PcieLocalStorage> filterPcieLocalStorage(Collection<LocalStorage> availableStorage) {
        return availableStorage.stream()
            .filter(localStorage -> localStorage instanceof PcieLocalStorage)
            .map(PcieLocalStorage.class::cast)
            .collect(toSet());
    }

    private Map<Id, Id> getZoneToEndpointsMapping(Set<PcieLocalStorage> pcieLocalStorages) {
        Map<Id, Id> zoneToEndpointsMapping = new HashMap<>();
        for (PcieLocalStorage pcieLocalStorage : pcieLocalStorages) {
            Id zoneId = pcieLocalStorage.getZone().getId();
            Id endpointId = zoneToEndpointsMapping.get(zoneId);
            if (endpointId == null) {
                endpointId = pcieLocalStorage.getEndpoint().getId();
            }
            zoneToEndpointsMapping.put(zoneId, endpointId);
        }
        return zoneToEndpointsMapping;
    }
}
