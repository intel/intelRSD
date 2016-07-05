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

package com.intel.podm.allocation.mappers.memory;

import com.intel.podm.allocation.mappers.Sorter;
import com.intel.podm.business.dto.redfish.RequestedMemory;
import com.intel.podm.business.entities.redfish.base.MemoryModule;

import java.math.BigDecimal;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.stream.Collector;

import static com.google.common.collect.Maps.newHashMap;
import static com.intel.podm.allocation.mappers.memory.Predicates.byAtLeastDataWidthBits;
import static com.intel.podm.allocation.mappers.memory.Predicates.byAtLeastSpeedMhz;
import static com.intel.podm.allocation.mappers.memory.Predicates.byExactMemoryDeviceType;
import static com.intel.podm.allocation.mappers.memory.Predicates.byExactId;
import static com.intel.podm.allocation.mappers.memory.Predicates.byExactManufacturer;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

public class MemoryModulesAllocationMapper {
    public Map<RequestedMemory, List<MemoryModule>> map(List<RequestedMemory> requestedMemoryModules, List<MemoryModule> availableMemoryModules) {
        Map<RequestedMemory, List<MemoryModule>> mappedMemoryModules = newHashMap();

        List<RequestedMemory> requestedMemoryModulesQueue =
                Sorter.sort(requestedMemoryModules, Sorter.contextsAscendingWhereNullsLast());
        List<MemoryModule> availableMemoryModulesQueue =
                Sorter.sort(availableMemoryModules, Sorter.ascendingWhereNullsLast(MemoryModule::getId));

        for (RequestedMemory requestedMemoryModule : requestedMemoryModulesQueue) {
            List<MemoryModule> matched = findMatchedMemoryModules(requestedMemoryModule, availableMemoryModulesQueue);
            mappedMemoryModules.put(requestedMemoryModule, matched);
            availableMemoryModulesQueue.removeAll(matched);
        }

        return mappedMemoryModules;
    }

    private static List<MemoryModule> findMatchedMemoryModules(RequestedMemory requestedMemoryModule, List<MemoryModule> availableMemoryModules) {
        return availableMemoryModules.stream()
                .filter(byExactId(requestedMemoryModule.getResourceContext()))
                .filter(byExactMemoryDeviceType(requestedMemoryModule.getMemoryDeviceType()))
                .filter(byAtLeastSpeedMhz(requestedMemoryModule.getSpeedMhz()))
                .filter(byExactManufacturer(requestedMemoryModule.getManufacturer()))
                .filter(byAtLeastDataWidthBits(requestedMemoryModule.getDataWidthBits()))
                .sorted(withMemoryComparator())
                .collect(toMinimalSizeMibCollection(requestedMemoryModule.getCapacityMib()));
    }

    private static Collector<? super MemoryModule, List<MemoryModule>, List<MemoryModule>> toMinimalSizeMibCollection(BigDecimal requestedSizeMib) {
        Optional<BigDecimal> sizeMib = ofNullable(requestedSizeMib);
        return sizeMib.isPresent() ? new MinimalSizeMibSubsetCollector(sizeMib.get().doubleValue()) : (Collector) toList();
    }

    private static Comparator<? super MemoryModule> withMemoryComparator() {
        return comparing(MemoryModule::getMemoryDeviceType, nullsFirst(naturalOrder()))
                .thenComparing(MemoryModule::getOperatingSpeedMhz, nullsFirst(naturalOrder()))
                .thenComparing(MemoryModule::getManufacturer, nullsFirst(naturalOrder()))
                .thenComparing(MemoryModule::getDataWidthBits, nullsFirst(naturalOrder()))
                .thenComparing(MemoryModule::getCapacityMib, nullsFirst(naturalOrder()));
    }
}
