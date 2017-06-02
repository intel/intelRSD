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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.AllocationRequestProcessingException;
import com.intel.podm.allocation.strategy.ResourceFinderException;
import com.intel.podm.allocation.validation.ComputerSystemCollector;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.logger.Logger;

import javax.inject.Inject;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.podm.business.Violations.createWithViolations;
import static java.util.Objects.isNull;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemMatcher {
    @Inject
    protected ProcessorMatcher processorMatcher;
    @Inject
    protected MemoryMatcher memoryMatcher;
    @Inject
    protected LocalStorageMatcher localDriveMatcher;
    @Inject
    protected LocalStorageCollector localStorageCollector;
    @Inject
    protected EthernetInterfaceMatcher ethernetInterfaceMatcher;
    @Inject
    protected ComputerSystemAttributesMatcher computerSystemAttributesMatcher;
    @Inject
    protected ComputerSystemCollector computerSystemCollector;
    @Inject
    private Logger logger;

    public List<ComputerSystem> matches(RequestedNode requestedNode, List<ComputerSystem> computerSystems) throws ResourceFinderException {
        FilteringList computerSystemList = new FilteringList(computerSystems);

        computerSystemList.filter(byEnabledAndHealthy(), "status")
            .filter(byComputerSystem(getComputerSystemByResourceContexts(requestedNode)), "resource")
            .filter(byComputerSystemsFromChassis(getCommonComputerSystemsByChassisContexts(requestedNode)), "chassis")
            .filter(byProcessors(requestedNode), "processors")
            .filter(byMemoryModules(requestedNode), "memory")
            .filter(byLocalDrives(requestedNode), "local drives")
            .filter(byEthernetInterfaces(requestedNode), "ethernet interfaces")
            .filter(byComputerSystemAttributes(requestedNode), "computer system attributes");

        String msg = computerSystemList.getFilterStatistics();
        logger.d(msg);

        List<ComputerSystem> systems = computerSystemList.getComputerSystems();
        if (systems.isEmpty()) {
            throw new ResourceFinderException(createWithViolations("There are no computer systems available for this allocation request.", msg));
        }

        return systems;
    }

    private Predicate<ComputerSystem> byEnabledAndHealthy() {
        return ComputerSystem::isEnabledAndHealthy;
    }

    private Predicate<ComputerSystem> byComputerSystem(ComputerSystem requestedComputerSystem) {
        return computerSystem -> isNull(requestedComputerSystem) || Objects.equals(requestedComputerSystem, computerSystem);
    }

    private Predicate<ComputerSystem> byComputerSystemsFromChassis(Set<ComputerSystem> requestedComputerSystems) {
        return computerSystem -> isEmpty(requestedComputerSystems) || requestedComputerSystems.contains(computerSystem);
    }

    private Predicate<ComputerSystem> byProcessors(RequestedNode requestedNode) {
        return computerSystem -> processorMatcher.matches(requestedNode, computerSystem.getProcessors());
    }

    private Predicate<ComputerSystem> byMemoryModules(RequestedNode requestedNode) {
        return computerSystem -> memoryMatcher.matches(requestedNode, computerSystem);
    }

    private Predicate<ComputerSystem> byLocalDrives(RequestedNode requestedNode) {
        return computerSystem -> localDriveMatcher.matches(requestedNode, localStorageCollector.getStorageUnderComputerSystem(computerSystem));
    }

    private Predicate<ComputerSystem> byEthernetInterfaces(RequestedNode requestedNode) {
        return computerSystem -> ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces());
    }

    private Predicate<ComputerSystem> byComputerSystemAttributes(RequestedNode requestedNode) {
        return computerSystem -> computerSystemAttributesMatcher.matches(requestedNode, computerSystem);
    }

    private ComputerSystem getComputerSystemByResourceContexts(RequestedNode requestedNode) {
        try {
            Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(requestedNode);
            if (computerSystems.size() <= 1) {
                return computerSystems.isEmpty() ? null : computerSystems.iterator().next();
            }
        } catch (AllocationRequestProcessingException e) {
            throw new IllegalStateException("Some of provided resources are not valid", e);
        }

        throw new IllegalStateException("Allocation of assets on multiple computer systems is not supported");
    }

    private Set<ComputerSystem> getCommonComputerSystemsByChassisContexts(RequestedNode requestedNode) {
        try {
            return computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
        } catch (AllocationRequestProcessingException e) {
            throw new IllegalStateException("Some of provided resources are not valid", e);
        }
    }
}
