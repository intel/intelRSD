/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.dao.ProcessorDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.types.Connectivity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.collections.IterableHelper.any;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper.isRemoteFpga;
import static com.intel.rsd.nodecomposer.types.Connectivity.ETHERNET;
import static com.intel.rsd.nodecomposer.types.Connectivity.REMOTE_PCI;
import static com.intel.rsd.nodecomposer.types.ProcessorType.FPGA;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static java.util.Collections.emptyList;
import static java.util.Collections.emptySet;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ProcessorsAllocator {
    private final ProcessorTaskFactory processorTaskFactory;
    private final GenericDao genericDao;
    private final ProcessorDao processorDao;

    @Autowired
    public ProcessorsAllocator(ProcessorTaskFactory processorTaskFactory, GenericDao genericDao, ProcessorDao processorDao) {
        this.processorTaskFactory = processorTaskFactory;
        this.genericDao = genericDao;
        this.processorDao = processorDao;
    }

    @Transactional(REQUIRED)
    public List<NodeTask> allocate(Collection<RequestedNode.Processor> requestedProcessors, ComputerSystem computerSystem, ComposedNode composedNode) {
        allocateProcessorsUnderComputerSystem(computerSystem);
        Collection<RequestedNode.Processor> requestedFpgaProcessors = getFpgaRequestedProcessors(requestedProcessors);
        if (!requestedFpgaProcessors.isEmpty()) {
            return getAssemblyTasksForRequestedFpgaProcessors(requestedFpgaProcessors, computerSystem, composedNode);
        }

        return emptyList();
    }

    private List<NodeTask> getAssemblyTasksForRequestedFpgaProcessors(Collection<RequestedNode.Processor> requestedProcessors, ComputerSystem computerSystem,
                                                                      ComposedNode composedNode) {
        List<NodeTask> tasks = new ArrayList<>();
        for (RequestedNode.Processor requestedProcessor : requestedProcessors) {
            List<Connectivity> requestedConnectivities = requestedProcessor.getConnectivity();
            if (nullOrEmpty(requestedConnectivities) || requestedRemotelyConnectedFpga(requestedConnectivities)) {
                Processor processor = tryRetrieveAchievableProcessor(requestedProcessor.getResourceODataId(), computerSystem);
                createAssemblyTaskForRemoteFpga(processor).ifPresent(task -> {
                        tasks.add(task);
                        composedNode.attachAsset(processor);
                    });
            }
        }

        return tasks;
    }

    private boolean requestedRemotelyConnectedFpga(List<Connectivity> connectivities) {
        return connectivities.contains(REMOTE_PCI) || connectivities.contains(ETHERNET);
    }

    private Collection<RequestedNode.Processor> getFpgaRequestedProcessors(Collection<RequestedNode.Processor> requestedProcessors) {
        return requestedProcessors == null ? emptySet() : requestedProcessors.stream()
            .filter(this::isProcessorTypeFpga)
            .collect(toSet());
    }

    private Optional<NodeTask> createAssemblyTaskForRemoteFpga(Processor processor) {
        if (isRemoteFpga(processor)) {
            return of(processorTaskFactory.createFpgaProcessorAssemblyTask(processor.getUri()));
        }

        return empty();
    }

    private void allocateProcessorsUnderComputerSystem(ComputerSystem computerSystem) {
        computerSystem.getProcessors()
            .forEach(processor -> processor.setAllocated(true));
    }

    private Processor tryRetrieveAchievableProcessor(ODataId requestedProcessorUri, ComputerSystem computerSystem) {
        if (requestedProcessorUri != null) {
            return genericDao.find(Processor.class, requestedProcessorUri);
        }

        Collection<Processor> remoteFpgaProcessors = getRemoteFpgaProcessors(computerSystem).stream()
            .filter(Processor::isAchievable)
            .collect(toSet());

        return !remoteFpgaProcessors.isEmpty() ? any(remoteFpgaProcessors)
            : getFirstLocalFpgaProcessor(computerSystem);
    }

    private Collection<Processor> getRemoteFpgaProcessors(ComputerSystem computerSystem) {
        Collection<Processor> remoteFpgaProcessors = getConnectedAndAvailablePcieProcessors(computerSystem);
        remoteFpgaProcessors.addAll(processorDao.getAvailableFpgaOverFabricsProcessors());

        return remoteFpgaProcessors;
    }

    private Set<Processor> getConnectedAndAvailablePcieProcessors(ComputerSystem computerSystem) {
        return processorDao.getAchievablePcieProcessors(computerSystem).stream()
            .filter(processor -> !processor.isAllocated())
            .collect(toSet());
    }

    private Processor getFirstLocalFpgaProcessor(ComputerSystem computerSystem) {
        return any(computerSystem.getProcessors().stream()
            .filter(processor -> FPGA.equals(processor.getProcessorType()))
            .collect(toSet()));
    }

    private boolean isProcessorTypeFpga(RequestedNode.Processor requestedProcessor) {
        if (requestedProcessor.getResourceODataId() != null) {
            Processor processor = genericDao.find(Processor.class, requestedProcessor.getResourceODataId());

            return FPGA.equals(processor.getProcessorType());
        }

        return FPGA.equals(requestedProcessor.getProcessorType());
    }
}
