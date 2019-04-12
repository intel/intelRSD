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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.ResourceFinderException;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ComputerSystemCollector;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Objects;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static java.util.Objects.isNull;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class ComputerSystemMatcher {
    private final ProcessorMatcher processorMatcher;
    private final MemoryMatcher memoryMatcher;
    private final LocalStorageMatcher localDriveMatcher;
    private final LocalStorageCollector localStorageCollector;
    private final EthernetInterfaceMatcher ethernetInterfaceMatcher;
    private final ComputerSystemAttributesMatcher computerSystemAttributesMatcher;
    private final ComputerSystemCollector computerSystemCollector;
    private final ComputerSystemSecurityAttributesMatcher computerSystemSecurityAttributesMatcher;

    @Autowired
    @SuppressWarnings({"checkstyle:ParameterNumber"})
    public ComputerSystemMatcher(ProcessorMatcher processorMatcher, MemoryMatcher memoryMatcher, LocalStorageMatcher localDriveMatcher,
                                 LocalStorageCollector localStorageCollector, EthernetInterfaceMatcher ethernetInterfaceMatcher,
                                 ComputerSystemAttributesMatcher computerSystemAttributesMatcher, ComputerSystemCollector computerSystemCollector,
                                 ComputerSystemSecurityAttributesMatcher computerSystemSecurityAttributesMatcher) {
        this.processorMatcher = processorMatcher;
        this.memoryMatcher = memoryMatcher;
        this.localDriveMatcher = localDriveMatcher;
        this.localStorageCollector = localStorageCollector;
        this.ethernetInterfaceMatcher = ethernetInterfaceMatcher;
        this.computerSystemAttributesMatcher = computerSystemAttributesMatcher;
        this.computerSystemCollector = computerSystemCollector;
        this.computerSystemSecurityAttributesMatcher = computerSystemSecurityAttributesMatcher;
    }

    public Collection<ComputerSystem> matches(RequestedNode requestedNode, Collection<ComputerSystem> computerSystems) throws ResourceFinderException {
        FilteringCollection<ComputerSystem> computerSystemList = new FilteringCollection<>(computerSystems);
        computerSystemList
            .filter(byAchievability(), "online systems")
            .filter(byEnabledAndHealthy(), "status")
            .filter(byComputerSystem(computerSystemCollector.getComputerSystemByResourceUris(requestedNode)), "resource")
            .filter(byComputerSystemsFromChassis(getCommonComputerSystemsByChassisUris(requestedNode)), "chassis")
            .filter(byProcessors(requestedNode), "processors")
            .filter(byMemoryModules(requestedNode), "memory")
            .filter(byLocalDrives(requestedNode), "local drives")
            .filter(byEthernetInterfaces(requestedNode), "ethernet interfaces")
            .filter(byComputerSystemAttributes(requestedNode), "computer system attributes")
            .filter(byComputerSystemSecurityAttributes(requestedNode), "security");

        String msg = computerSystemList.getFilterStatistics();
        log.debug(msg);

        Collection<ComputerSystem> systems = computerSystemList.getCollection();
        if (systems.isEmpty()) {
            throw new ResourceFinderException(createWithViolations("There are no computer systems available for this allocation request.", msg));
        }

        return systems;
    }

    private Predicate<ComputerSystem> byAchievability() {
        return ComputerSystem::isAchievable;
    }

    private Predicate<ComputerSystem> byEnabledAndHealthy() {
        return computerSystem -> statusOf(computerSystem).isEnabled().isHealthy().verify();
    }

    private Predicate<ComputerSystem> byComputerSystem(ComputerSystem requestedComputerSystem) {
        return computerSystem -> isNull(requestedComputerSystem) || Objects.equals(requestedComputerSystem, computerSystem);
    }

    private Predicate<ComputerSystem> byComputerSystemsFromChassis(Set<ComputerSystem> requestedComputerSystems) {
        return computerSystem -> isEmpty(requestedComputerSystems) || requestedComputerSystems.contains(computerSystem);
    }

    private Predicate<ComputerSystem> byProcessors(RequestedNode requestedNode) {
        return computerSystem -> processorMatcher.matches(requestedNode, computerSystem);
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

    private Predicate<ComputerSystem> byComputerSystemSecurityAttributes(RequestedNode requestedNode) {
        return computerSystem -> computerSystemSecurityAttributesMatcher.matches(requestedNode.getSecurity(), computerSystem);
    }

    private Set<ComputerSystem> getCommonComputerSystemsByChassisUris(RequestedNode requestedNode) {
        try {
            return computerSystemCollector.collectCommonComputerSystemsFromChassisODataIds(requestedNode);
        } catch (AllocationRequestProcessingException e) {
            throw new IllegalStateException("Some of provided resources are not valid", e);
        }
    }
}
