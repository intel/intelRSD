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
import com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.MemoryModulesAllocationMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.base.MemoryModule;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.function.Predicate;

import static java.util.Collections.unmodifiableList;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class MemoryMatcher {
    private final MemoryModulesAllocationMapper mapper;

    @Autowired
    public MemoryMatcher(MemoryModulesAllocationMapper mapper) {
        this.mapper = mapper;
    }

    public boolean matches(RequestedNode requestedNode, ComputerSystem computerSystem) {
        List<RequestedNode.Memory> requestedMemoryModules = requestedNode.getMemoryModules();
        List<MemoryModule> availableMemoryModules = getMemoryModules(computerSystem);
        return isNotEmpty(requestedMemoryModules)
            ? areMatched(requestedMemoryModules, availableMemoryModules)
            : true;
    }

    private List<MemoryModule> getMemoryModules(ComputerSystem computerSystem) {
        List<MemoryModule> availableMemoryModules = new ArrayList<>(computerSystem.getMemoryModules());

        if (availableMemoryModules.isEmpty()) {
            availableMemoryModules.add(computerSystem.getMemoryModuleFromMemorySummary());
        }
        return availableMemoryModules;
    }

    private boolean areMatched(List<RequestedNode.Memory> requestedMemoryModules, List<MemoryModule> availableMemoryModules) {
        if (requestedCapacityExceedAvailable(requestedMemoryModules, availableMemoryModules)) {
            return false;
        }

        Map<RequestedNode.Memory, MemoryModule> mappedMemoryModules = mapper.map(requestedMemoryModules, unmodifiableList(availableMemoryModules));
        return Objects.equals(mappedMemoryModules.entrySet().size(), requestedMemoryModules.size());
    }

    private boolean requestedCapacityExceedAvailable(List<RequestedNode.Memory> requestedMemoryModules, List<MemoryModule> availableMemoryModules) {
        Integer requestedSizeMib = getRequestedSizeMib(requestedMemoryModules);
        Predicate<List<MemoryModule>> availableCapacityPredicate = byAvailableCapacity(requestedSizeMib).negate();

        return availableCapacityPredicate.test(availableMemoryModules);
    }

    private Integer getRequestedSizeMib(List<RequestedNode.Memory> requestedMemoryModules) {
        return requestedMemoryModules.stream()
            .filter(memoryModule -> memoryModule.getCapacityMib() != null)
            .mapToInt(memoryModule -> memoryModule.getCapacityMib().intValue())
            .sum();
    }

    private Predicate<List<MemoryModule>> byAvailableCapacity(Integer requestedSizeMib) {
        return memoryList ->
            memoryList.stream()
                .filter(memoryModule -> memoryModule.getCapacityMib() != null)
                .mapToInt(MemoryModule::getCapacityMib)
                .sum() >= requestedSizeMib;
    }
}
