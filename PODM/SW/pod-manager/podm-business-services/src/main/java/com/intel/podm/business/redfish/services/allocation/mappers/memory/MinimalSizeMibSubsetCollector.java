/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.mappers.memory;

import com.intel.podm.business.entities.redfish.base.MemoryModule;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import java.util.Set;
import java.util.function.BiConsumer;
import java.util.function.BinaryOperator;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.function.Supplier;
import java.util.stream.Collector;

import static java.util.Collections.emptyList;
import static java.util.stream.Collector.Characteristics.UNORDERED;

class MinimalSizeMibSubsetCollector implements Collector<MemoryModule, List<MemoryModule>, List<MemoryModule>> {
    private double sizeMib;

    MinimalSizeMibSubsetCollector(double sizeMib) {
        this.sizeMib = sizeMib;
    }

    @Override
    public Supplier<List<MemoryModule>> supplier() {
        return ArrayList::new;
    }

    @Override
    public BiConsumer<List<MemoryModule>, MemoryModule> accumulator() {
        return List::add;
    }

    @Override
    public BinaryOperator<List<MemoryModule>> combiner() {
        return (memoryModules1, memoryModules2) -> {
            memoryModules1.addAll(memoryModules2);
            return memoryModules1;
        };
    }

    @Override
    public Function<List<MemoryModule>, List<MemoryModule>> finisher() {
        return memoryModules -> {
            Predicate<List<MemoryModule>> satisfiesCapacityConstraint = memoryList ->
                    memoryList.stream()
                            .filter(memoryModule -> memoryModule.getCapacityMib() != null)
                            .mapToInt(MemoryModule::getCapacityMib).sum() >= sizeMib;

            List<MemoryModule> minimalSubset = new ArrayList<>();
            for (MemoryModule availableMemoryModule : memoryModules) {
                minimalSubset.add(availableMemoryModule);
                if (satisfiesCapacityConstraint.test(minimalSubset)) {
                    return minimalSubset;
                }
            }

            return emptyList();
        };
    }

    @Override
    public Set<Characteristics> characteristics() {
        return EnumSet.of(UNORDERED);
    }
}
