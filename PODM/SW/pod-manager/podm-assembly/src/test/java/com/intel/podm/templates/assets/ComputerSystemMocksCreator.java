/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.templates.assets;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.Processor;

import java.util.HashSet;
import java.util.Set;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public final class ComputerSystemMocksCreator {
    private ComputerSystemMocksCreator() {

    }

    public static ComputerSystem createComputerSystemWithProcessors(Integer numberOfProcessors, Integer numberOfCoresPerProcessor) {
        ComputerSystem mock = mock(ComputerSystem.class);
        Set<Processor> processors = new HashSet<>();
        for (int i = 0; i < numberOfProcessors; i++) {
            processors.add(createProcessorWithTotalCores("Processor #" + i, numberOfCoresPerProcessor));
        }
        when(mock.getProcessors()).thenReturn(processors);

        return mock;
    }

    public static ComputerSystem createComputerSystemWithMemory(Integer numberOfMemory, Integer capacityPerMemory) {
        ComputerSystem mock = mock(ComputerSystem.class);
        Set<Memory> memory = new HashSet<>();
        for (int i = 0; i < numberOfMemory; i++) {
            memory.add(createMemoryWithCapacity("Memory #" + i, capacityPerMemory));
        }
        when(mock.getMemoryModules()).thenReturn(memory);

        return mock;
    }

    public static ComputerSystem createComputerSystemWithMemoryAndProcessors(Integer numberOfMemory, Integer capacityPerMemory,
                                                                             Integer numberOfProcessors, Integer numberOfCoresPerProcessor) {
        ComputerSystem mock = mock(ComputerSystem.class);
        Set<Memory> memory = new HashSet<>();
        for (int i = 0; i < numberOfMemory; i++) {
            memory.add(createMemoryWithCapacity("Memory #" + i, capacityPerMemory));
        }
        when(mock.getMemoryModules()).thenReturn(memory);

        Set<Processor> processors = new HashSet<>();
        for (int i = 0; i < numberOfProcessors; i++) {
            processors.add(createProcessorWithTotalCores("Processor #" + i, numberOfCoresPerProcessor));
        }
        when(mock.getProcessors()).thenReturn(processors);

        return mock;
    }



    private static Processor createProcessorWithTotalCores(String name, Integer numberOfCores) {
        Processor mock = mock(Processor.class);
        when(mock.getName()).thenReturn(name);
        when(mock.getTotalCores()).thenReturn(numberOfCores);
        return mock;
    }

    private static Memory createMemoryWithCapacity(String name, Integer capacity) {
        Memory mock = mock(Memory.class);
        when(mock.getName()).thenReturn(name);
        when(mock.getCapacityMib()).thenReturn(capacity);
        return mock;
    }
}
