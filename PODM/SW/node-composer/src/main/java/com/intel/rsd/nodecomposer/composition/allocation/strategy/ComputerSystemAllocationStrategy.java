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

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.Security;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationStrategy;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.ComputerSystemMatcher;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ComputerSystemAllocationValidator;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.ProcessorType.FPGA;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Transactional(MANDATORY)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemAllocationStrategy implements AllocationStrategy {
    private final ComputerSystemDao computerSystemDao;
    private final ComputerSystemAllocationValidator validator;
    private final ComputerSystemMatcher computerSystemMatcher;
    private final EthernetInterfacesAllocator ethernetInterfacesAllocator;
    private final ComputerSystemConfigurationTaskFactory computerSystemConfigurationTaskFactory;
    private final PcieLocalStorageAllocator pcieLocalStorageAllocator;
    private final LocalStorageCollector localStorageCollector;
    private final ProcessorsAllocator processorsAllocator;

    private RequestedNode requestedNode;
    private List<NodeTask> tasks = new ArrayList<>();

    @Autowired
    @SuppressWarnings({"checkstyle:ParameterNumber"})
    public ComputerSystemAllocationStrategy(ComputerSystemDao computerSystemDao, ComputerSystemAllocationValidator validator,
                                            ComputerSystemMatcher computerSystemMatcher, EthernetInterfacesAllocator ethernetInterfacesAllocator,
                                            ComputerSystemConfigurationTaskFactory computerSystemConfigurationTaskFactory,
                                            PcieLocalStorageAllocator pcieLocalStorageAllocator, LocalStorageCollector localStorageCollector,
                                            ProcessorsAllocator processorsAllocator) {
        this.computerSystemDao = computerSystemDao;
        this.validator = validator;
        this.computerSystemMatcher = computerSystemMatcher;
        this.ethernetInterfacesAllocator = ethernetInterfacesAllocator;
        this.computerSystemConfigurationTaskFactory = computerSystemConfigurationTaskFactory;
        this.pcieLocalStorageAllocator = pcieLocalStorageAllocator;
        this.localStorageCollector = localStorageCollector;
        this.processorsAllocator = processorsAllocator;
    }

    public void setRequestedNode(RequestedNode requestedNode) {
        this.requestedNode = requestedNode;
    }

    @Override
    public Violations validate() {
        return validator.validate(requestedNode);
    }

    private ComputerSystem findResources() throws ResourceFinderException {
        Collection<ComputerSystem> computerSystems = computerSystemDao.getComputerSystemsPossibleToAllocate();
        computerSystems = computerSystemMatcher.matches(requestedNode, computerSystems);

        return getAppropriateComputerSystem(computerSystems);
    }

    private ComputerSystem getAppropriateComputerSystem(Collection<ComputerSystem> computerSystems) {
        ComputerSystem firstComputerSystem = computerSystems.iterator().next();
        List<RequestedNode.Processor> requestedProcessors = requestedNode.getProcessors();
        if (requestedProcessors == null) {
            return firstComputerSystem;
        }

        boolean isFpgaRequestWithoutConnectivity = requestedNode.getProcessors().stream()
            .allMatch(requestedProcessor -> nullOrEmpty(requestedProcessor.getConnectivity()))
            && requestedNode.getProcessors().stream()
            .anyMatch(requestedProcessor -> FPGA.equals(requestedProcessor.getProcessorType()));

        return isFpgaRequestWithoutConnectivity ? computerSystems.stream()
            .filter(computerSystem -> computerSystem.getProcessors().stream().anyMatch(processor -> FPGA.equals(processor.getProcessorType())))
            .findFirst()
            .orElse(firstComputerSystem) : firstComputerSystem;
    }

    @Override
    public void allocate(ComposedNode composedNode) throws ResourceFinderException {
        val computerSystem = findResources();
        attachSystemToNode(composedNode, computerSystem);

        Set<LocalStorage> storageUnderComputerSystem = localStorageCollector.getStorageUnderComputerSystem(computerSystem);

        Set<PcieLocalStorage> selectedPcieLocalStorages = pcieLocalStorageAllocator.selectResources(
            requestedNode.getLocalDrives(),
            storageUnderComputerSystem
        );

        tasks.addAll(pcieLocalStorageAllocator.prepareNodeAssemblyTasks(selectedPcieLocalStorages, storageUnderComputerSystem));
        for (PcieLocalStorage localStorage : selectedPcieLocalStorages) {
            composedNode.attachAsset(localStorage.getDrive());
        }

        tasks.addAll(processorsAllocator.allocate(requestedNode.getProcessors(), computerSystem, composedNode));
        tasks.addAll(ethernetInterfacesAllocator.createVlanAssemblyTasks(requestedNode.getEthernetInterfaces(), computerSystem.getEthernetInterfaces()));
        tasks.addAll(computerSystemConfigurationTaskFactory.createComputerSystemConfigurationTask(requestedNode.getSecurity(), computerSystem));

        composedNode.setClearTpmOnDelete(retrieveClearTpmOnDelete());
        composedNode.setClearOptanePersistentMemoryOnDelete(retrieveClearOptanePersistentMemoryOnDelete());
    }

    private boolean retrieveClearOptanePersistentMemoryOnDelete() {
        Security security = requestedNode.getSecurity();
        if (security != null && security.getClearOptaneMemoryOnDelete() != null) {
            return security.getClearOptaneMemoryOnDelete();
        }

        return true;
    }

    private boolean retrieveClearTpmOnDelete() {
        Security security = requestedNode.getSecurity();
        if (security != null && security.getClearTpmOnDelete() != null) {
            return security.getClearTpmOnDelete();
        }

        return true;
    }

    private void attachSystemToNode(ComposedNode composedNode, ComputerSystem computerSystem) {
        composedNode.setComputerSystem(computerSystem);
        composedNode.setAssociatedComputerSystemUuid(computerSystem.getUuid());
        computerSystem.getMetadata().setAllocated(true);
    }

    public List<NodeTask> getTasks() {
        return tasks;
    }
}
