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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.MemoryModuleFromMemorySummary;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeWithMemoryModules;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.MemoryModulesCreation.createAvailableMemory;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.MemoryModulesCreation.createRequestedMemory;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.common.enterprise.utils.measures.AmountOfInformation.createAmountOfInformation;
import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.GIBI;
import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.MEBI;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.MemoryDeviceType.DDR3;
import static com.intel.podm.common.types.MemoryDeviceType.SDRAM;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;
import static org.testng.AssertJUnit.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodCount"})
public class MemoryMatcherTest {
    private static final BigDecimal TOTAL_SYSTEM_MEMORY_GIB = new BigDecimal(8);
    private MemoryMatcher memoryMatcher;

    @BeforeClass
    public void setup() {
        this.memoryMatcher = new MemoryMatcher();
    }

    @Test
    public void whenRequestingMoreThanAvailable_shouldNotMatch() {
        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(
            createRequestedMemory(DDR3, 500, 200, new BigDecimal(10), "Intel", contextOf(id(1), MEMORY)),
            createRequestedMemory(SDRAM, 500, 200, new BigDecimal(10), "Intel", contextOf(id(2), MEMORY))
        ));

        Set<Memory> availableMemoryModules = new HashSet<>(singletonList(
            createAvailableMemory(DDR3, 500, 200, 10, "Intel", id(1))
        ));

        assertFalse(memoryMatcher.matches(requestedNode, createComputerSystemWithMemoryModules(availableMemoryModules)));
    }

    @Test
    public void whenRequestingMultipleMemoryModulesWithSatisfiedAvailable_shouldMatch() {
        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(
            createRequestedMemory(DDR3, 400, 200, new BigDecimal(300), "Intel", contextOf(id(4), MEMORY)),
            createRequestedMemory(SDRAM, 400, 200, new BigDecimal(400), "Intel", contextOf(id(1), MEMORY))
        ));

        Set<Memory> availableMemoryModules = new HashSet<>(asList(
            createAvailableMemory(SDRAM, 400, 200, 400, "Intel", id(1)),
            createAvailableMemory(DDR3, 400, 200, 150, "Intel", id(2)),
            createAvailableMemory(DDR3, 400, 200, 350, "Intel", id(3)),
            createAvailableMemory(DDR3, 400, 200, 300, "Intel", id(4))
        ));

        assertTrue(memoryMatcher.matches(requestedNode, createComputerSystemWithMemoryModules(availableMemoryModules)));
    }


    @Test
    public void whenExactMemoryTypeRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(SDRAM, null, null, null, null),
            createAvailableMemory(DDR3, null, null, null, null)
        );
    }

    @Test
    public void whenExactMemoryTypeRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(SDRAM, null, null, null, null),
            createAvailableMemory(SDRAM, null, null, null, null)
        );
    }

    @Test
    public void whenAvailableSpeedMhzIsNull_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, 120, null, null, null),
            createAvailableMemory(null, null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastSpeedMhzRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(null, 120, null, null, null),
            createAvailableMemory(null, 121, null, null, null)
        );
    }

    @Test
    public void whenAtLeastSpeedMhzRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, 120, null, null, null),
            createAvailableMemory(null, 119, null, null, null)
        );
    }

    @Test
    public void whenAvailableDataWidthBitsIsNull_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, null, 120, null, null),
            createAvailableMemory(null, null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastDataWidthBitsRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(null, null, 120, null, null),
            createAvailableMemory(null, null, 121, null, null)
        );
    }

    @Test
    public void whenAtLeastDataWidthBitsRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, null, 120, null, null),
            createAvailableMemory(null, null, 119, null, null)
        );
    }

    @Test
    public void whenAtLeastSizeGibRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(null, null, null, new BigDecimal(100), null),
            createAvailableMemory(null, null, null, 101, null)
        );
    }

    @Test
    public void whenAtLeastSizeGibRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, null, null, new BigDecimal(100), null),
            createAvailableMemory(null, null, null, 99, null)
        );
    }

    @Test
    public void whenExactManufacturerRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(null, null, null, null, "Kingston"),
            createAvailableMemory(null, null, null, null, "Kingston")
        );
    }

    @Test
    public void whenExactManufacturerRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, null, null, null, "Kingston"),
            createAvailableMemory(null, null, null, null, "Intel")
        );
    }

    @Test
    public void whenExactIdRequestIsSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedMemory(null, null, null, null, null, contextOf(id(1), MEMORY)),
            createAvailableMemory(null, null, null, null, null, id(1))
        );
    }

    @Test
    public void whenExactIdRequestIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedMemory(null, null, null, null, null, contextOf(id(2), MEMORY)),
            createAvailableMemory(null, null, null, null, null, id(1))
        );
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndIsSatisfied_shouldMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);
        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, null, null,
            createAmountOfInformation(TOTAL_SYSTEM_MEMORY_GIB, GIBI).getQuantityOf(MEBI), null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), true);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndNotSatisfied_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, null, null,
            createAmountOfInformation(TOTAL_SYSTEM_MEMORY_GIB, GIBI).getQuantityOf(MEBI).add(new BigDecimal(1)), null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndMemoryDeviceTypeRequested_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(DDR3, null, null, null, null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndAchievableSpeedMhzRequested_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, 100, null, null, null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndDataWidthBitsRequested_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, null, 100, null, null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsPresentAndManufacturerRequested_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(TOTAL_SYSTEM_MEMORY_GIB);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, null, null, null, "Kingston")));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }

    @Test
    public void whenNoMemoryModulesAndTotalSystemMemoryIsNull_shouldNotMatch() {
        ComputerSystem computerSystemWithMemoryModules = createComputerSystemWithTotalSystemMemory(null);

        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(createRequestedMemory(null, null, null,
            createAmountOfInformation(TOTAL_SYSTEM_MEMORY_GIB, GIBI).getQuantityOf(MEBI), null)));

        assertEquals(memoryMatcher.matches(requestedNode, computerSystemWithMemoryModules), false);
    }


    private void shouldMatch(RequestedNode.Memory requestedMemory, Memory availableMemory) {
        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(requestedMemory));
        Set<Memory> availableMemoryModules = new HashSet<>(singletonList(availableMemory));

        assertEquals(memoryMatcher.matches(requestedNode, createComputerSystemWithMemoryModules(availableMemoryModules)), true);
    }

    private void shouldNotMatch(RequestedNode.Memory requestedMemory, Memory availableMemory) {
        RequestedNode requestedNode = new RequestedNodeWithMemoryModules(asList(requestedMemory));
        Set<Memory> availableMemoryModules = new HashSet<>(singletonList(availableMemory));

        assertEquals(memoryMatcher.matches(requestedNode, createComputerSystemWithMemoryModules(availableMemoryModules)), false);
    }

    private static ComputerSystem createComputerSystemWithMemoryModules(Set<Memory> memoryModules) {
        ComputerSystem computerSystemMock = mock(ComputerSystem.class);
        when(computerSystemMock.getMemoryModules()).thenReturn(memoryModules);

        return computerSystemMock;
    }

    private static ComputerSystem createComputerSystemWithTotalSystemMemory(BigDecimal totalSystemMemory) {
        ComputerSystem computerSystemMock = createComputerSystemWithMemoryModules(new HashSet<>());
        when(computerSystemMock.getTotalSystemMemoryGiB()).thenReturn(totalSystemMemory);

        MemoryModuleFromMemorySummary memoryModuleFromMemorySummary = new MemoryModuleFromMemorySummary(computerSystemMock);
        when(computerSystemMock.getMemoryModuleFromMemorySummary()).thenReturn(memoryModuleFromMemorySummary);

        return computerSystemMock;
    }
}
