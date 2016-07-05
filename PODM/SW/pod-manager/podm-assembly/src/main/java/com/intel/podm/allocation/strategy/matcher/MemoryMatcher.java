/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.allocation.mappers.memory.MemoryModulesAllocationMapper;
import com.intel.podm.business.dto.redfish.RequestedMemory;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.base.MemoryModule;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static java.util.Collections.unmodifiableList;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

public class MemoryMatcher {
    public boolean matches(RequestedNode requestedNode, ComputerSystem computerSystem) {
        List<RequestedMemory> requestedMemoryModules = requestedNode.getMemoryModules();
        List<MemoryModule> availableMemoryModules = getMemoryModules(computerSystem);
        return isNotEmpty(requestedMemoryModules)
                ? areMatched(requestedMemoryModules, availableMemoryModules)
                : true;
    }

    private List<MemoryModule> getMemoryModules(ComputerSystem computerSystem) {
        List<MemoryModule> availableMemoryModules = new ArrayList<>();
        availableMemoryModules.addAll(computerSystem.getMemoryModules());

        if (availableMemoryModules.isEmpty()) {
            availableMemoryModules.add(computerSystem.getMemoryModuleFromMemorySummary());
        }
        return availableMemoryModules;
    }

    private static boolean areMatched(List<RequestedMemory> requestedMemoryModules, List<MemoryModule> availableMemoryModules) {
        MemoryModulesAllocationMapper mapper = new MemoryModulesAllocationMapper();
        Map<RequestedMemory, List<MemoryModule>> mappedMemoryModules = mapper.map(requestedMemoryModules, unmodifiableList(availableMemoryModules));
        return mappedMemoryModules.entrySet().stream().allMatch(entry -> !entry.getValue().isEmpty());
    }
}
