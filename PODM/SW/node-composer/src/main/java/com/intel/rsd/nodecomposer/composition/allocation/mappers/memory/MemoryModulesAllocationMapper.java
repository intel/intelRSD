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

package com.intel.rsd.nodecomposer.composition.allocation.mappers.memory;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.MemoryModule;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.ascendingWhereNullsLast;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.sort;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.urisAscendingWhereNullsLast;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byAtLeastCapacityGib;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byAtLeastDataWidthBits;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byAtLeastSpeedMhz;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byExactManufacturer;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byExactMemoryDeviceType;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byExactMemoryType;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.memory.Predicates.byExactUri;
import static com.intel.rsd.nodecomposer.types.EnumeratedType.byPriority;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class MemoryModulesAllocationMapper {
    private static Comparator<? super MemoryModule> withMemoryComparator() {
        return comparing(MemoryModule::getMemoryDeviceType, nullsFirst(byPriority()))
            .thenComparing(MemoryModule::getMemoryType, nullsFirst(byPriority()))
            .thenComparing(MemoryModule::getOperatingSpeedMhz, nullsFirst(naturalOrder()))
            .thenComparing(MemoryModule::getManufacturer, nullsFirst(naturalOrder()))
            .thenComparing(MemoryModule::getDataWidthBits, nullsFirst(naturalOrder()))
            .thenComparing(MemoryModule::getCapacityMib, nullsFirst(naturalOrder()));
    }

    public Map<RequestedNode.Memory, MemoryModule> map(Collection<RequestedNode.Memory> requestedMemoryModules,
                                                       Collection<MemoryModule> availableMemoryModules) {
        Map<RequestedNode.Memory, MemoryModule> mappedMemoryModule = new HashMap<>();

        Collection<RequestedNode.Memory> requestedMemoryModulesQueue = sort(requestedMemoryModules, urisAscendingWhereNullsLast());
        Collection<MemoryModule> availableMemoryModulesQueue = sort(availableMemoryModules, ascendingWhereNullsLast(MemoryModule::getUri));

        for (RequestedNode.Memory requestedMemoryModule : requestedMemoryModulesQueue) {
            findMatchingMemoryModule(requestedMemoryModule, availableMemoryModulesQueue)
                .ifPresent(matchingModule -> {
                    mappedMemoryModule.put(requestedMemoryModule, matchingModule);
                    availableMemoryModulesQueue.remove(matchingModule);
                });
        }

        return mappedMemoryModule;
    }

    private Optional<MemoryModule> findMatchingMemoryModule(RequestedNode.Memory requestedMemoryModule,
                                                            Collection<MemoryModule> availableMemoryModules) {
        return availableMemoryModules.stream()
            .filter(byExactUri(requestedMemoryModule.getResourceODataId()))
            .filter(byExactMemoryType(requestedMemoryModule.getMemoryType()))
            .filter(byExactMemoryDeviceType(requestedMemoryModule.getMemoryDeviceType()))
            .filter(byAtLeastSpeedMhz(requestedMemoryModule.getSpeedMhz()))
            .filter(byExactManufacturer(requestedMemoryModule.getManufacturer()))
            .filter(byAtLeastDataWidthBits(requestedMemoryModule.getDataWidthBits()))
            .filter(byAtLeastCapacityGib(requestedMemoryModule.getCapacityMib()))
            .min(withMemoryComparator());
    }
}
