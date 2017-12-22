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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.ComputerSystemMocksCreator.createComputerSystemWithMemory;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.ComputerSystemMocksCreator.createComputerSystemWithMemoryAndProcessors;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.ComputerSystemMocksCreator.createComputerSystemWithProcessors;
import static com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeBuilder.newRequestedNode;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class ComputerSystemAttributesMatcherTest {
    private ComputerSystemAttributesMatcher computerSystemAttributesMatcher;

    @BeforeClass
    public void setup() {
        this.computerSystemAttributesMatcher = new ComputerSystemAttributesMatcher();
    }

    @Test
    public void whenRequestingWithinSummaryTotalCores_shouldMatch() {
        int requiredNumberOfCores = 6;
        int numberOfProcessors = 2;
        int coresPerProcessor = 4;
        RequestedNode requestedNode = newRequestedNode()
            .totalSystemCoreCount(requiredNumberOfCores)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithProcessors(numberOfProcessors, coresPerProcessor);
        assertTrue(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithinMemoryCapacity_shouldMatch() {
        int requiredCapacity = 6;
        int numberOfMemory = 2;
        int capacityPerMemory = 4;
        RequestedNode requestedNode = newRequestedNode().totalSystemMemoryMib(requiredCapacity).build();

        ComputerSystem computerSystem = createComputerSystemWithMemory(numberOfMemory, capacityPerMemory);
        assertTrue(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithinMemoryCapacityAnTotalCores_shouldMatch() {
        int requiredNumberOfCores = 6;
        int numberOfProcessors = 2;
        int coresPerProcessor = 4;

        int requiredCapacity = 6;
        int numberOfMemory = 2;
        int capacityPerMemory = 4;
        RequestedNode requestedNode = newRequestedNode()
            .totalSystemMemoryMib(requiredCapacity)
            .totalSystemCoreCount(requiredNumberOfCores)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithMemoryAndProcessors(numberOfMemory, capacityPerMemory, numberOfProcessors, coresPerProcessor);
        assertTrue(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithMoreCoresThanAvailable_shouldNotMatch() {
        int requiredNumberOfCores = 10;
        int numberOfProcessors = 2;
        int coresPerProcessor = 4;

        RequestedNode requestedNode = newRequestedNode()
            .totalSystemCoreCount(requiredNumberOfCores)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithProcessors(numberOfProcessors, coresPerProcessor);
        assertFalse(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithMoreCapacityThanAvailable_shouldNotMatch() {
        int requiredCapacity = 600;
        int numberOfMemory = 2;
        int capacityPerMemory = 4;
        RequestedNode requestedNode = newRequestedNode()
            .totalSystemMemoryMib(requiredCapacity)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithMemory(numberOfMemory, capacityPerMemory);
        assertFalse(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithMoreCapacityThanAvailableButTotalCoreIsInRange_shouldNotMatch() {
        int requiredNumberOfCores = 6;
        int numberOfProcessors = 2;
        int coresPerProcessor = 4;

        int requiredCapacity = 600;
        int numberOfMemory = 2;
        int capacityPerMemory = 4;
        RequestedNode requestedNode = newRequestedNode()
            .totalSystemMemoryMib(requiredCapacity)
            .totalSystemCoreCount(requiredNumberOfCores)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithMemoryAndProcessors(numberOfMemory, capacityPerMemory, numberOfProcessors, coresPerProcessor);
        assertFalse(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }

    @Test
    public void whenRequestingWithMoreTotalCoresThanAvailableButCapacityIsInRange_shouldNotMatch() {
        int requiredNumberOfCores = 10;
        int numberOfProcessors = 2;
        int coresPerProcessor = 4;

        int requiredCapacity = 6;
        int numberOfMemory = 2;
        int capacityPerMemory = 4;
        RequestedNode requestedNode = newRequestedNode()
            .totalSystemMemoryMib(requiredCapacity)
            .totalSystemCoreCount(requiredNumberOfCores)
            .build();

        ComputerSystem computerSystem = createComputerSystemWithMemoryAndProcessors(numberOfMemory, capacityPerMemory, numberOfProcessors, coresPerProcessor);
        assertFalse(computerSystemAttributesMatcher.matches(requestedNode, computerSystem));
    }
}
